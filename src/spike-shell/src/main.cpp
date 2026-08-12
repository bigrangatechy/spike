#include "desktop/DesktopBackground.hpp"
#include "firstrun/FirstRunHooks.hpp"
#include "firstrun/FirstRunWizard.hpp"
#include "lock/LockController.hpp"
#include "panel/Panel.hpp"
#include "settings/InputConfig.hpp"
#include "shortcuts/ShellShortcuts.hpp"

#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QPalette>
#include <QProcess>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

namespace {

void loadStyleSheet(QApplication &app)
{
  QFile file(QStringLiteral(":/styles/spike.qss"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    app.setStyleSheet(QString::fromUtf8(file.readAll()));
  }
}

void applyDarkPalette(QApplication &app)
{
  // Fallback when stylesheets don't cover a widget (menus, etc.).
  QPalette pal = app.palette();
  const QColor bg(0x1a, 0x1a, 0x2e);
  const QColor panel(0x22, 0x22, 0x36);
  const QColor text(0xff, 0xff, 0xff);
  const QColor disabled(0xa0, 0xa0, 0xb8);
  const QColor accent(0x6d, 0x4a, 0xff);

  pal.setColor(QPalette::Window, bg);
  pal.setColor(QPalette::WindowText, text);
  pal.setColor(QPalette::Base, panel);
  pal.setColor(QPalette::AlternateBase, bg);
  pal.setColor(QPalette::Text, text);
  pal.setColor(QPalette::Button, panel);
  pal.setColor(QPalette::ButtonText, text);
  pal.setColor(QPalette::BrightText, text);
  pal.setColor(QPalette::ToolTipBase, panel);
  pal.setColor(QPalette::ToolTipText, text);
  pal.setColor(QPalette::PlaceholderText, disabled);
  pal.setColor(QPalette::Highlight, accent);
  pal.setColor(QPalette::HighlightedText, text);
  pal.setColor(QPalette::Disabled, QPalette::Text, disabled);
  pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
  pal.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
  app.setPalette(pal);
}

} // namespace

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-shell"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.56"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));

  // Breeze SVG icons need qt6-svg-plugins on the live image.
  // Prefer breeze-dark directly; spike-icons inherits it for future overrides.
  QStringList iconPaths = QIcon::themeSearchPaths();
  iconPaths.prepend(QStringLiteral("/usr/share/icons"));
  const QString localIcons =
      QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
      QStringLiteral("/icons");
  if (!localIcons.isEmpty()) {
    iconPaths.prepend(localIcons);
  }
  QIcon::setThemeSearchPaths(iconPaths);
  QIcon::setThemeName(QStringLiteral("breeze-dark"));
  QIcon::setFallbackThemeName(QStringLiteral("breeze"));
  if (!QIcon::hasThemeIcon(QStringLiteral("network-wireless"))) {
    QIcon::setThemeName(QStringLiteral("breeze"));
  }

  applyDarkPalette(app);
  loadStyleSheet(app);

  spike::DesktopBackground desktop;
  desktop.applyLayerShell();
  desktop.show();

  spike::Panel panel;
  panel.applyLayerShell();
  panel.show();

  // Restore sleep/lock inhibit + watch PrepareForSleep / Session.Lock.
  (void)spike::LockController::instance();

  // Fn / media keys + Meta+L / Meta+Space (KWin script → D-Bus; needs kglobalacceld).
  auto *shortcuts = new spike::ShellShortcuts(&app);
  shortcuts->setLockHandler([]() { spike::LockController::instance().lockScreen(); });
  shortcuts->setLauncherHandler([&panel]() { panel.toggleLauncher(); });
  shortcuts->start();

  // Re-apply saved pointer/keyboard prefs once KWin InputDevice nodes exist.
  QTimer::singleShot(1500, []() {
    const QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
                         QStringLiteral("/kcminputrc");
    QSettings s(path, QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Mouse"));
    const double accel = s.value(QStringLiteral("PointerAcceleration"), 0.0).toDouble();
    s.endGroup();
    s.beginGroup(QStringLiteral("Libinput"));
    const bool tap = s.value(QStringLiteral("TapToClick"), true).toBool();
    s.endGroup();
    s.beginGroup(QStringLiteral("Keyboard"));
    const int delay = s.value(QStringLiteral("RepeatDelay"), 600).toInt();
    const int rate = s.value(QStringLiteral("RepeatRate"), 25).toInt();
    s.endGroup();
    spike::applyPointerSettings(accel, tap, nullptr);
    spike::applyKeyboardRepeat(delay, rate, nullptr);
  });

  // Portals need WAYLAND_DISPLAY (set by KWin for --exit-with-session).
  // Never start these from spike-session before KWin — that blocks ~90s then ABRTs.
  QTimer::singleShot(2000, []() {
    QProcess::startDetached(
        QStringLiteral("systemctl"),
        {QStringLiteral("--user"), QStringLiteral("start"), QStringLiteral("--no-block"),
         QStringLiteral("xdg-desktop-portal.service"),
         QStringLiteral("plasma-xdg-desktop-portal-kde.service")});
  });

  // Post-install first-run wizard (INSTALLER.md / AGENTS.md) — installed only.
  QTimer::singleShot(800, []() {
    if (!spike::firstrun::shouldShowFirstRunWizard()) {
      return;
    }
    auto *wiz = new spike::FirstRunWizard(nullptr);
    wiz->setAttribute(Qt::WA_DeleteOnClose);
    wiz->show();
    wiz->raise();
    wiz->activateWindow();
  });

  return app.exec();
}
