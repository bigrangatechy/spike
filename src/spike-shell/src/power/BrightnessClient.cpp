#include "power/BrightnessClient.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDir>
#include <QFile>
#include <QProcess>

namespace spike {

BrightnessClient::BrightnessClient(QObject *parent)
  : QObject(parent)
{
  discover();
}

bool BrightnessClient::discover()
{
  const QDir dir(QStringLiteral("/sys/class/backlight"));
  if (!dir.exists()) {
    return false;
  }
  const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &e : entries) {
    const QString base = dir.absoluteFilePath(e);
    const QString bright = base + QStringLiteral("/brightness");
    const QString maxb = base + QStringLiteral("/max_brightness");
    if (QFile::exists(bright) && QFile::exists(maxb)) {
      m_brightnessPath = bright;
      m_maxPath = maxb;
      m_deviceName = e;
      m_hasBacklight = true;
      return true;
    }
  }
  return false;
}

int BrightnessClient::readMax() const
{
  QFile f(m_maxPath);
  if (!f.open(QIODevice::ReadOnly)) {
    return 100;
  }
  return qMax(1, QString::fromUtf8(f.readAll()).trimmed().toInt());
}

int BrightnessClient::readBrightness() const
{
  QFile f(m_brightnessPath);
  if (!f.open(QIODevice::ReadOnly)) {
    return 0;
  }
  return QString::fromUtf8(f.readAll()).trimmed().toInt();
}

int BrightnessClient::percentage() const
{
  if (!m_hasBacklight) {
    return 0;
  }
  const int maxv = readMax();
  return qBound(1, qRound(100.0 * readBrightness() / maxv), 100);
}

bool BrightnessClient::writeSysfs(int raw) const
{
  QFile f(m_brightnessPath);
  if (!f.open(QIODevice::WriteOnly)) {
    return false;
  }
  return f.write(QByteArray::number(raw)) > 0;
}

bool BrightnessClient::writeLogind(int raw) const
{
  // session/auto resolves to the caller's session; works without root when seat permits.
  QDBusMessage msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.login1"), QStringLiteral("/org/freedesktop/login1/session/auto"),
      QStringLiteral("org.freedesktop.login1.Session"), QStringLiteral("SetBrightness"));
  msg << QStringLiteral("backlight") << m_deviceName << quint32(raw);
  QDBusMessage reply = QDBusConnection::systemBus().call(msg, QDBus::Block, 3000);
  return reply.type() != QDBusMessage::ErrorMessage;
}

bool BrightnessClient::writeBrightnessctl(int raw) const
{
  QProcess proc;
  proc.start(QStringLiteral("brightnessctl"),
             {QStringLiteral("-d"), m_deviceName, QStringLiteral("set"), QString::number(raw)});
  if (!proc.waitForStarted(2000) || !proc.waitForFinished(3000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool BrightnessClient::setPercentage(int pct)
{
  if (!m_hasBacklight) {
    return false;
  }
  const int maxv = readMax();
  const int raw = qBound(1, qRound(maxv * (qBound(1, pct, 100) / 100.0)), maxv);
  if (writeSysfs(raw)) {
    return true;
  }
  if (writeLogind(raw)) {
    return true;
  }
  return writeBrightnessctl(raw);
}

} // namespace spike
