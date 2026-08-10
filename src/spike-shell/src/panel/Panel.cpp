#include "panel/Panel.hpp"

#include "desktop/DesktopBackground.hpp"
#include "launcher/Launcher.hpp"
#include "notify/NotificationDaemon.hpp"
#include "panel/applets/AirplaneModeApplet.hpp"
#include "panel/applets/BatteryApplet.hpp"
#include "panel/applets/BluetoothApplet.hpp"
#include "panel/applets/BrightnessApplet.hpp"
#include "panel/applets/ClockApplet.hpp"
#include "panel/applets/KeyboardLayoutApplet.hpp"
#include "panel/applets/NetworkApplet.hpp"
#include "panel/applets/NightLightApplet.hpp"
#include "panel/applets/NotificationsApplet.hpp"
#include "panel/applets/RemovableDevicesApplet.hpp"
#include "panel/applets/SessionMenuApplet.hpp"
#include "panel/applets/UpdateNotifierApplet.hpp"
#include "panel/applets/VolumeApplet.hpp"
#include "panel/applets/WindowListApplet.hpp"
#include "settings/AppearanceLive.hpp"
#include "settings/ConfigClient.hpp"
#include "settings/SettingsWindow.hpp"

#include <LayerShellQt/Window>

#include <QApplication>
#include <QCursor>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QTimer>
#include <QWindow>

namespace spike {

namespace {

constexpr int kTriggerPx = 2;

bool runWait(const QString &program, const QStringList &args, int timeoutMs = 8000)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000)) {
    return false;
  }
  if (!proc.waitForFinished(timeoutMs)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

void runPowerAction(const QString &logindMethod, const QString &systemctlVerb)
{
  if (runWait(QStringLiteral("sudo"),
              {QStringLiteral("-n"), QStringLiteral("systemctl"), systemctlVerb})) {
    return;
  }
  if (runWait(QStringLiteral("busctl"),
              {QStringLiteral("call"), QStringLiteral("org.freedesktop.login1"),
               QStringLiteral("/org/freedesktop/login1"),
               QStringLiteral("org.freedesktop.login1.Manager"), logindMethod, QStringLiteral("b"),
               QStringLiteral("false")})) {
    return;
  }
  QProcess::startDetached(QStringLiteral("systemctl"), {systemctlVerb});
}

} // namespace

Panel::Panel(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("SpikePanel"));
  setFixedHeight(m_panelHeight);
  setWindowTitle(QStringLiteral("Spike Panel"));
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

  m_config = new ConfigClient(this);
  connect(m_config, &ConfigClient::stateChanged, this, &Panel::onConfigStateChanged);

  m_autoHideTimer = new QTimer(this);
  m_autoHideTimer->setInterval(200);
  connect(m_autoHideTimer, &QTimer::timeout, this, &Panel::onAutoHideTick);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 2, 8, 2);
  layout->setSpacing(8);

  auto *spikeBtn = new QPushButton(QStringLiteral("Spike"), this);
  spikeBtn->setObjectName(QStringLiteral("SpikeButton"));
  spikeBtn->setFixedHeight(26);
  spikeBtn->setToolTip(QStringLiteral("Open start menu"));
  const QIcon spikeIcon = QIcon::fromTheme(QStringLiteral("start-here-kde"),
                                           QIcon::fromTheme(QStringLiteral("application-menu")));
  if (!spikeIcon.isNull()) {
    spikeBtn->setIcon(spikeIcon);
  }
  connect(spikeBtn, &QPushButton::clicked, this, &Panel::toggleLauncher);
  layout->addWidget(spikeBtn);

  m_windowList = new WindowListApplet(this);
  layout->addWidget(m_windowList);

  layout->addStretch(1);

  auto *network = new NetworkApplet(this);
  layout->addWidget(network);

  auto *volume = new VolumeApplet(this);
  layout->addWidget(volume);

  auto *battery = new BatteryApplet(this);
  layout->addWidget(battery);
  if (!battery->hasBattery()) {
    battery->hide();
  }

  auto *brightness = new BrightnessApplet(this);
  layout->addWidget(brightness);

  m_nightLight = new NightLightApplet(this);
  layout->addWidget(m_nightLight);

  auto *bluetooth = new BluetoothApplet(this);
  layout->addWidget(bluetooth);

  auto *airplane = new AirplaneModeApplet(this);
  layout->addWidget(airplane);

  auto *kbd = new KeyboardLayoutApplet(this);
  layout->addWidget(kbd);

  auto *removable = new RemovableDevicesApplet(this);
  layout->addWidget(removable);

  m_updates = new UpdateNotifierApplet(this);
  layout->addWidget(m_updates);

  m_notify = new NotificationDaemon(this);
  m_notify->registerService();
  m_notifications = new NotificationsApplet(m_notify, this);
  layout->addWidget(m_notifications);

  auto *clock = new ClockApplet(this);
  layout->addWidget(clock);

  auto *session = new SessionMenuApplet(this);
  layout->addWidget(session);

  m_launcher = new Launcher(this);
  m_launcher->hide();
  connect(m_launcher, &Launcher::openSettingsRequested, this, [this]() { openSettings(); });
  connect(m_launcher, &Launcher::logoutRequested, this, &Panel::onLogout);
  connect(m_launcher, &Launcher::rebootRequested, this, &Panel::onReboot);
  connect(m_launcher, &Launcher::shutdownRequested, this, &Panel::onShutdown);

  // Defer config load until after winId / layer-shell exist (main calls applyLayerShell).
  QTimer::singleShot(0, this, &Panel::reloadDesktopSettings);
}

void Panel::applyLayerShell()
{
  createWinId();
  QWindow *win = windowHandle();
  if (!win) {
    return;
  }
  LayerShellQt::Window *layer = LayerShellQt::Window::get(win);
  if (!layer) {
    // X11 / nested smoke — geometry fallback.
    if (QScreen *screen = this->screen() ? this->screen() : QGuiApplication::primaryScreen()) {
      const QRect geo = screen->geometry();
      const int h = m_panelRevealed ? m_panelHeight : kTriggerPx;
      if (m_panelOnTop) {
        setGeometry(geo.left(), geo.top(), geo.width(), h);
      } else {
        setGeometry(geo.left(), geo.bottom() - h + 1, geo.width(), h);
      }
    }
    return;
  }

  using LS = LayerShellQt::Window;
  const int visibleH = m_panelRevealed ? m_panelHeight : kTriggerPx;
  layer->setScope(QStringLiteral("spike-panel"));
  layer->setLayer(LS::LayerTop);
  LS::Anchors anchors = LS::Anchors(LS::AnchorLeft) | LS::AnchorRight;
  if (m_panelOnTop) {
    anchors |= LS::AnchorTop;
    layer->setExclusiveEdge(LS::AnchorTop);
  } else {
    anchors |= LS::AnchorBottom;
    layer->setExclusiveEdge(LS::AnchorBottom);
  }
  layer->setAnchors(anchors);
  layer->setExclusiveZone(visibleH);
  layer->setKeyboardInteractivity(LS::KeyboardInteractivityOnDemand);
  layer->setActivateOnShow(true);
  if (QScreen *screen = this->screen() ? this->screen() : QGuiApplication::primaryScreen()) {
    layer->setScreen(screen);
    layer->setDesiredSize(QSize(screen->geometry().width(), visibleH));
  }
  setFixedHeight(visibleH);
}

void Panel::applyPanelChrome()
{
  const int h = m_panelRevealed ? m_panelHeight : kTriggerPx;
  setFixedHeight(h);
  applyLayerShell();
}

void Panel::reloadDesktopSettings()
{
  if (!m_config) {
    return;
  }
  QString err;
  const QString json = m_config->getModuleState(QStringLiteral("desktop"), &err);
  if (json.isEmpty()) {
    applyPanelChrome();
    return;
  }
  const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
  m_panelHeight = o.value(QStringLiteral("panel_height")).toInt(32);
  if (m_panelHeight < 24) {
    m_panelHeight = 24;
  }
  if (m_panelHeight > 48) {
    m_panelHeight = 48;
  }
  const QString pos = o.value(QStringLiteral("panel_position")).toString(QStringLiteral("bottom"));
  m_panelOnTop = (pos == QLatin1String("top"));
  m_autoHide = o.value(QStringLiteral("panel_auto_hide")).toBool(false);
  m_panelRevealed = true;
  if (m_autoHide) {
    m_autoHideTimer->start();
  } else {
    m_autoHideTimer->stop();
  }
  applyPanelChrome();

  // Live chrome for accent / font / wallpaper (also applied from Settings Apply).
  applyShellChromeLive(qApp, o.value(QStringLiteral("accent_color")).toString(QStringLiteral("#6d4aff")),
                       o.value(QStringLiteral("font_size_pt")).toInt(10), false);
  applyWallpaperLive(o.value(QStringLiteral("wallpaper")).toString());

  applyTrayVisibility(o);

  if (m_nightLight) {
    m_nightLight->applyFromConfig(o.value(QStringLiteral("night_light_enabled")).toBool(false),
                                  o.value(QStringLiteral("night_light_temperature")).toInt(4500));
  }

  // Sync DND from privacy prefs when available.
  if (m_notify && m_config) {
    QString perr;
    const QString privacy = m_config->getModuleState(QStringLiteral("privacy"), &perr);
    if (!privacy.isEmpty()) {
      const QJsonObject po = QJsonDocument::fromJson(privacy.toUtf8()).object();
      m_notify->setDoNotDisturb(po.value(QStringLiteral("do_not_disturb")).toBool(false));
    }
  }
}

void Panel::persistNightLight(bool enabled, int temperature)
{
  if (!m_config) {
    return;
  }
  QString err;
  m_config->setSetting(QStringLiteral("desktop"), QStringLiteral("night_light_enabled"), enabled,
                       &err);
  m_config->setSetting(QStringLiteral("desktop"), QStringLiteral("night_light_temperature"),
                       temperature, &err);
}

void Panel::applyTrayVisibility(const QJsonObject &desktop)
{
  QJsonObject tray;
  const QJsonValue trayVal = desktop.value(QStringLiteral("tray_applets"));
  if (trayVal.isObject()) {
    tray = trayVal.toObject();
  } else if (trayVal.isString()) {
    tray = QJsonDocument::fromJson(trayVal.toString().toUtf8()).object();
  }
  const bool showNotifications = tray.value(QStringLiteral("notifications")).toBool(true);
  const bool showNight = tray.value(QStringLiteral("night_light")).toBool(true);
  const bool showUpdates = tray.value(QStringLiteral("updates")).toBool(true);
  const bool showWindows = tray.value(QStringLiteral("window_list")).toBool(true);
  if (m_notifications) {
    m_notifications->setEnabledVisible(showNotifications);
  }
  if (m_nightLight) {
    m_nightLight->setEnabledVisible(showNight);
  }
  if (m_updates) {
    m_updates->setEnabledVisible(showUpdates);
  }
  if (m_windowList) {
    m_windowList->setEnabledVisible(showWindows);
  }
}

void Panel::onConfigStateChanged(const QString &module, const QString &key, const QVariant &,
                                 const QVariant &)
{
  if (module != QLatin1String("desktop")) {
    return;
  }
  if (key == QLatin1String("panel_height") || key == QLatin1String("panel_position") ||
      key == QLatin1String("panel_auto_hide") || key == QLatin1String("accent_color") ||
      key == QLatin1String("font_size_pt") || key == QLatin1String("wallpaper") ||
      key == QLatin1String("tray_applets") || key == QLatin1String("night_light_enabled") ||
      key == QLatin1String("night_light_temperature")) {
    reloadDesktopSettings();
  }
}

bool Panel::cursorNearPanelEdge() const
{
  QScreen *screen = this->screen() ? this->screen() : QGuiApplication::primaryScreen();
  if (!screen) {
    return false;
  }
  const QRect geo = screen->geometry();
  const QPoint p = QCursor::pos();
  if (!geo.contains(p)) {
    return false;
  }
  const int margin = m_panelHeight + 4;
  if (m_panelOnTop) {
    return p.y() <= geo.top() + margin;
  }
  return p.y() >= geo.bottom() - margin;
}

void Panel::onAutoHideTick()
{
  if (!m_autoHide) {
    return;
  }
  const bool near = cursorNearPanelEdge() || underMouse() || (m_launcher && m_launcher->isVisible());
  if (near && !m_panelRevealed) {
    m_panelRevealed = true;
    applyPanelChrome();
  } else if (!near && m_panelRevealed) {
    m_panelRevealed = false;
    applyPanelChrome();
  }
}

void Panel::toggleLauncher()
{
  if (!m_launcher) {
    return;
  }
  if (m_launcher->isVisible()) {
    m_launcher->hide();
    return;
  }

  // Reveal panel if auto-hidden before placing the launcher.
  if (m_autoHide && !m_panelRevealed) {
    m_panelRevealed = true;
    applyPanelChrome();
  }

  const QPoint global = mapToGlobal(QPoint(8, 0));
  int y = global.y() - m_launcher->height() - 4;
  if (m_panelOnTop) {
    y = global.y() + height() + 4;
  }
  m_launcher->move(global.x(), y);
  m_launcher->show();
  m_launcher->raise();
  m_launcher->activateWindow();
}

void Panel::openSettings(const QString &pageId)
{
  if (!m_settings) {
    m_settings = new SettingsWindow(m_config, nullptr);
  }
  if (!pageId.isEmpty()) {
    m_settings->openPage(pageId);
  }
  m_settings->show();
  m_settings->raise();
  m_settings->activateWindow();
}

void Panel::onLogout()
{
  const QString sessionId = qEnvironmentVariable("XDG_SESSION_ID");
  if (!sessionId.isEmpty()) {
    QProcess::startDetached(QStringLiteral("loginctl"),
                            {QStringLiteral("terminate-session"), sessionId});
  } else {
    QProcess::startDetached(QStringLiteral("loginctl"),
                            {QStringLiteral("terminate-user"), qEnvironmentVariable("USER")});
  }
}

void Panel::onReboot()
{
  runPowerAction(QStringLiteral("Reboot"), QStringLiteral("reboot"));
}

void Panel::onShutdown()
{
  runPowerAction(QStringLiteral("PowerOff"), QStringLiteral("poweroff"));
}

} // namespace spike
