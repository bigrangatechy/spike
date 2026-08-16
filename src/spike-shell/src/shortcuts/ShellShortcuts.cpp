#include "shortcuts/ShellShortcuts.hpp"

#include "audio/VolumeClient.hpp"
#include "power/BrightnessClient.hpp"
#include "shortcuts/EvdevMediaKeys.hpp"
#include "shortcuts/OsdPopup.hpp"
#include "shortcuts/ShortcutsAdaptor.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QStringList>
#include <QTimer>
#include <QVariantMap>
#include <QtGlobal>

namespace spike {

namespace {

void startKGlobalAccelDaemon()
{
  QProcess::startDetached(QStringLiteral("systemctl"),
                          {QStringLiteral("--user"), QStringLiteral("start"),
                           QStringLiteral("--no-block"), QStringLiteral("plasma-kglobalaccel.service")});
  QDBusInterface check(QStringLiteral("org.kde.kglobalaccel"), QStringLiteral("/kglobalaccel"),
                       QStringLiteral("org.kde.KGlobalAccel"), QDBusConnection::sessionBus());
  if (!check.isValid() &&
      QFile::exists(QStringLiteral("/usr/lib/x86_64-linux-gnu/libexec/kglobalacceld"))) {
    QProcess::startDetached(QStringLiteral("/usr/lib/x86_64-linux-gnu/libexec/kglobalacceld"), {});
  }
}

void playerctl(const QString &cmd)
{
  if (QStandardPaths::findExecutable(QStringLiteral("playerctl")).isEmpty()) {
    return;
  }
  QProcess::startDetached(QStringLiteral("playerctl"), {cmd});
}

bool playerctlAvailable()
{
  return !QStandardPaths::findExecutable(QStringLiteral("playerctl")).isEmpty();
}

QString kwinScriptMainPath()
{
  const QString packaged =
      QStringLiteral("/usr/share/kwin/scripts/spike-shortcuts/contents/code/main.js");
  if (QFile::exists(packaged)) {
    return packaged;
  }
  return {};
}

} // namespace

ShellShortcuts::ShellShortcuts(QObject *parent)
  : QObject(parent)
  , m_volume(new VolumeClient(this))
  , m_brightness(new BrightnessClient(this))
  , m_evdev(new EvdevMediaKeys(this))
{
  // Evdev is the reliable path for XF86 volume/brightness on Spike's KWin-only
  // session (kglobalaccel often never binds those keys). Meta+L / Meta+Space stay
  // on the KWin script.
  connect(m_evdev, &EvdevMediaKeys::volumeUp, this, &ShellShortcuts::volumeUp);
  connect(m_evdev, &EvdevMediaKeys::volumeDown, this, &ShellShortcuts::volumeDown);
  connect(m_evdev, &EvdevMediaKeys::volumeMute, this, &ShellShortcuts::volumeMute);
  connect(m_evdev, &EvdevMediaKeys::brightnessUp, this, &ShellShortcuts::brightnessUp);
  connect(m_evdev, &EvdevMediaKeys::brightnessDown, this, &ShellShortcuts::brightnessDown);
  connect(m_evdev, &EvdevMediaKeys::mediaPlayPause, this, &ShellShortcuts::mediaPlayPause);
  connect(m_evdev, &EvdevMediaKeys::mediaNext, this, &ShellShortcuts::mediaNext);
  connect(m_evdev, &EvdevMediaKeys::mediaPrevious, this, &ShellShortcuts::mediaPrevious);
}

void ShellShortcuts::setLockHandler(VoidHandler handler)
{
  m_lock = std::move(handler);
}

void ShellShortcuts::setLauncherHandler(VoidHandler handler)
{
  m_launcher = std::move(handler);
}

void ShellShortcuts::notifyOsd(const QString &summary, const QString &body)
{
  // Tray history (Notifications daemon has no toast UI).
  QDBusMessage msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.Notifications"), QStringLiteral("/org/freedesktop/Notifications"),
      QStringLiteral("org.freedesktop.Notifications"), QStringLiteral("Notify"));
  msg << QStringLiteral("Spike") << uint(0) << QString() << summary << body << QStringList()
      << QVariantMap{{QStringLiteral("transient"), true}, {QStringLiteral("urgency"), uchar(0)}}
      << 1500;
  QDBusConnection::sessionBus().asyncCall(msg);

  // Real on-screen OSD via layer-shell overlay (plain ToolTip windows are invisible on Wayland).
  if (!m_osd) {
    m_osd = new OsdPopup(nullptr);
  }
  m_osd->showMessage(summary, body);
}

void ShellShortcuts::volumeUp()
{
  if (!m_volume) {
    return;
  }
  m_volume->refresh();
  m_volume->adjustBy(5);
  notifyOsd(QStringLiteral("Volume"),
            m_volume->muted() ? QStringLiteral("Muted")
                              : QStringLiteral("%1%").arg(m_volume->volumePercent()));
}

void ShellShortcuts::volumeDown()
{
  if (!m_volume) {
    return;
  }
  m_volume->refresh();
  m_volume->adjustBy(-5);
  notifyOsd(QStringLiteral("Volume"),
            m_volume->muted() ? QStringLiteral("Muted")
                              : QStringLiteral("%1%").arg(m_volume->volumePercent()));
}

void ShellShortcuts::volumeMute()
{
  if (!m_volume) {
    return;
  }
  m_volume->refresh();
  m_volume->toggleMute();
  notifyOsd(QStringLiteral("Volume"),
            m_volume->muted() ? QStringLiteral("Muted")
                              : QStringLiteral("%1%").arg(m_volume->volumePercent()));
}

void ShellShortcuts::brightnessUp()
{
  if (!m_brightness || !m_brightness->hasBacklight()) {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("No backlight control"));
    return;
  }
  if (m_brightness->adjustBy(5)) {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("%1%").arg(m_brightness->percentage()));
  } else {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("Adjust failed"));
  }
}

void ShellShortcuts::brightnessDown()
{
  if (!m_brightness || !m_brightness->hasBacklight()) {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("No backlight control"));
    return;
  }
  if (m_brightness->adjustBy(-5)) {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("%1%").arg(m_brightness->percentage()));
  } else {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("Adjust failed"));
  }
}

void ShellShortcuts::mediaPlayPause()
{
  if (!playerctlAvailable()) {
    notifyOsd(QStringLiteral("Media"), QStringLiteral("playerctl not installed"));
    return;
  }
  playerctl(QStringLiteral("play-pause"));
}

void ShellShortcuts::mediaNext()
{
  if (!playerctlAvailable()) {
    notifyOsd(QStringLiteral("Media"), QStringLiteral("playerctl not installed"));
    return;
  }
  playerctl(QStringLiteral("next"));
}

void ShellShortcuts::mediaPrevious()
{
  if (!playerctlAvailable()) {
    notifyOsd(QStringLiteral("Media"), QStringLiteral("playerctl not installed"));
    return;
  }
  playerctl(QStringLiteral("previous"));
}

void ShellShortcuts::lock()
{
  if (m_lock) {
    m_lock();
  }
}

void ShellShortcuts::launcher()
{
  if (m_launcher) {
    m_launcher();
  }
}

void ShellShortcuts::ensureKwinScript()
{
  if (m_scriptLoaded) {
    return;
  }
  const QString mainJs = kwinScriptMainPath();
  if (mainJs.isEmpty()) {
    return;
  }
  QDBusInterface scripting(QStringLiteral("org.kde.KWin"), QStringLiteral("/Scripting"),
                           QStringLiteral("org.kde.kwin.Scripting"),
                           QDBusConnection::sessionBus());
  if (!scripting.isValid()) {
    return;
  }
  QDBusReply<int> id =
      scripting.call(QStringLiteral("loadScript"), mainJs, QStringLiteral("spike-shortcuts"));
  if (!id.isValid() || id.value() < 0) {
    return;
  }
  QDBusInterface script(QStringLiteral("org.kde.KWin"),
                        QStringLiteral("/Scripting/Script%1").arg(id.value()),
                        QStringLiteral("org.kde.kwin.Script"), QDBusConnection::sessionBus());
  script.call(QStringLiteral("run"));
  m_scriptLoaded = true;
}

void ShellShortcuts::start()
{
  if (m_started) {
    return;
  }
  m_started = true;

  startKGlobalAccelDaemon();

  QDBusConnection bus = QDBusConnection::sessionBus();
  if (bus.isConnected()) {
    // Task list may already own org.spike.Shell — still register our path.
    bus.registerService(QStringLiteral("org.spike.Shell"));
    new ShortcutsAdaptor(this);
    bus.registerObject(QStringLiteral("/Shortcuts"), this);
  }

  // Primary: read KEY_VOLUME* / KEY_BRIGHTNESS* from /dev/input (needs `input` group).
  if (m_evdev) {
    m_evdev->start();
    // Hotplug USB keyboards: re-scan a few times after session start.
    QTimer::singleShot(2000, this, [this]() {
      if (m_evdev) {
        m_evdev->start();
      }
    });
    QTimer::singleShot(8000, this, [this]() {
      if (m_evdev) {
        m_evdev->start();
      }
    });
  }

  // Secondary: KWin registerShortcut for Meta+L / Meta+Space (and XF86 if it binds).
  auto tryLoad = [this]() {
    startKGlobalAccelDaemon();
    ensureKwinScript();
  };
  QTimer::singleShot(800, this, tryLoad);
  QTimer::singleShot(2500, this, tryLoad);
  QTimer::singleShot(5000, this, tryLoad);
  QTimer::singleShot(10000, this, tryLoad);
}

} // namespace spike
