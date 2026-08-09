#include "power/SleepInhibit.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>
#include <QSettings>
#include <unistd.h>

namespace spike {

namespace {

constexpr const char *kSettingsOrg = "BigRangaTech";
constexpr const char *kSettingsApp = "spike-shell";
constexpr const char *kSettingsKey = "power/blockSleepAndLocking";

} // namespace

SleepInhibit &SleepInhibit::instance()
{
  static SleepInhibit s;
  return s;
}

SleepInhibit::SleepInhibit(QObject *parent)
  : QObject(parent)
{
  QSettings s(QString::fromUtf8(kSettingsOrg), QString::fromUtf8(kSettingsApp));
  if (s.value(QString::fromUtf8(kSettingsKey), false).toBool()) {
    QString err;
    if (!setActive(true, &err)) {
      // Leave setting on so UI can retry; stay inactive until then.
      m_active = false;
    }
  }
}

SleepInhibit::~SleepInhibit()
{
  release();
}

bool SleepInhibit::acquire(QString *error)
{
  if (m_fd >= 0) {
    return true;
  }
  QDBusMessage msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.login1"), QStringLiteral("/org/freedesktop/login1"),
      QStringLiteral("org.freedesktop.login1.Manager"), QStringLiteral("Inhibit"));
  msg << QStringLiteral("sleep:idle") << QStringLiteral("Spike Shell")
      << QStringLiteral("Manually block sleep and screen locking") << QStringLiteral("block");

  QDBusMessage reply = QDBusConnection::systemBus().call(msg, QDBus::Block, 5000);
  if (reply.type() == QDBusMessage::ErrorMessage) {
    if (error) {
      *error = reply.errorMessage();
    }
    return false;
  }
  const QList<QVariant> args = reply.arguments();
  if (args.isEmpty()) {
    if (error) {
      *error = QStringLiteral("Inhibit returned no file descriptor");
    }
    return false;
  }
  const QDBusUnixFileDescriptor ufd = qvariant_cast<QDBusUnixFileDescriptor>(args.at(0));
  if (!ufd.isValid()) {
    if (error) {
      *error = QStringLiteral("Invalid inhibit file descriptor");
    }
    return false;
  }
  m_fd = ::dup(ufd.fileDescriptor());
  if (m_fd < 0) {
    if (error) {
      *error = QStringLiteral("dup(inhibit fd) failed");
    }
    return false;
  }
  return true;
}

void SleepInhibit::release()
{
  if (m_fd >= 0) {
    ::close(m_fd);
    m_fd = -1;
  }
}

bool SleepInhibit::setActive(bool on, QString *error)
{
  if (on == m_active && ((on && m_fd >= 0) || (!on && m_fd < 0))) {
    return true;
  }
  if (on) {
    if (!acquire(error)) {
      return false;
    }
    m_active = true;
  } else {
    release();
    m_active = false;
  }
  QSettings s(QString::fromUtf8(kSettingsOrg), QString::fromUtf8(kSettingsApp));
  s.setValue(QString::fromUtf8(kSettingsKey), m_active);
  emit changed(m_active);
  return true;
}

} // namespace spike
