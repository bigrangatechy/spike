#include "shortcuts/ShellShortcuts.hpp"

#include "audio/VolumeClient.hpp"
#include "power/BrightnessClient.hpp"
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
{
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
  QDBusMessage msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.Notifications"), QStringLiteral("/org/freedesktop/Notifications"),
      QStringLiteral("org.freedesktop.Notifications"), QStringLiteral("Notify"));
  msg << QStringLiteral("Spike") << uint(0) << QString() << summary << body << QStringList()
      << QVariantMap{{QStringLiteral("transient"), true}, {QStringLiteral("urgency"), uchar(0)}}
      << 1500;
  QDBusConnection::sessionBus().asyncCall(msg);
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
    return;
  }
  const int next = qMin(100, m_brightness->percentage() + 5);
  if (m_brightness->setPercentage(next)) {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("%1%").arg(m_brightness->percentage()));
  }
}

void ShellShortcuts::brightnessDown()
{
  if (!m_brightness || !m_brightness->hasBacklight()) {
    return;
  }
  const int next = qMax(1, m_brightness->percentage() - 5);
  if (m_brightness->setPercentage(next)) {
    notifyOsd(QStringLiteral("Brightness"), QStringLiteral("%1%").arg(m_brightness->percentage()));
  }
}

void ShellShortcuts::mediaPlayPause()
{
  playerctl(QStringLiteral("play-pause"));
}

void ShellShortcuts::mediaNext()
{
  playerctl(QStringLiteral("next"));
}

void ShellShortcuts::mediaPrevious()
{
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

  // KWin scripting comes up with the compositor; retry like the task-list applet.
  QTimer::singleShot(800, this, [this]() { ensureKwinScript(); });
  QTimer::singleShot(2500, this, [this]() { ensureKwinScript(); });
  QTimer::singleShot(5000, this, [this]() { ensureKwinScript(); });
}

} // namespace spike
