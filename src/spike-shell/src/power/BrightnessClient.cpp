#include "power/BrightnessClient.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>

#include <QtGlobal>

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
    m_hasBacklight = false;
    return false;
  }
  const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  // Prefer real panel backlights over ACPI/vendor proxies.
  const QStringList prefer = {QStringLiteral("intel_backlight"), QStringLiteral("amdgpu_bl0"),
                              QStringLiteral("amdgpu_bl1"), QStringLiteral("nvidia_0"),
                              QStringLiteral("apple_panel_bl")};
  QStringList ordered = prefer;
  for (const QString &e : entries) {
    if (!ordered.contains(e)) {
      ordered << e;
    }
  }
  for (const QString &e : ordered) {
    if (!entries.contains(e)) {
      continue;
    }
    if (selectDevice(e)) {
      return true;
    }
  }
  m_hasBacklight = false;
  return false;
}

bool BrightnessClient::selectDevice(const QString &name)
{
  const QString base = QStringLiteral("/sys/class/backlight/") + name;
  const QString bright = base + QStringLiteral("/brightness");
  const QString maxb = base + QStringLiteral("/max_brightness");
  if (!QFile::exists(bright) || !QFile::exists(maxb)) {
    return false;
  }
  m_brightnessPath = bright;
  m_maxPath = maxb;
  m_deviceName = name;
  m_hasBacklight = true;
  return true;
}

QStringList BrightnessClient::allDevices() const
{
  const QDir dir(QStringLiteral("/sys/class/backlight"));
  if (!dir.exists()) {
    return {};
  }
  const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  const QStringList prefer = {QStringLiteral("intel_backlight"), QStringLiteral("amdgpu_bl0"),
                              QStringLiteral("amdgpu_bl1"), QStringLiteral("nvidia_0"),
                              QStringLiteral("apple_panel_bl")};
  QStringList ordered = prefer;
  for (const QString &e : entries) {
    if (!ordered.contains(e)) {
      ordered << e;
    }
  }
  QStringList out;
  for (const QString &e : ordered) {
    if (entries.contains(e)) {
      out << e;
    }
  }
  return out;
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
  // session/self and XDG_SESSION_ID are more reliable than "auto" under seatd.
  QStringList paths;
  const QString sid = qEnvironmentVariable("XDG_SESSION_ID");
  if (!sid.isEmpty()) {
    paths << QStringLiteral("/org/freedesktop/login1/session/") + sid;
  }
  paths << QStringLiteral("/org/freedesktop/login1/session/self");
  paths << QStringLiteral("/org/freedesktop/login1/session/auto");

  for (const QString &path : paths) {
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"), path,
        QStringLiteral("org.freedesktop.login1.Session"), QStringLiteral("SetBrightness"));
    msg << QStringLiteral("backlight") << m_deviceName << quint32(raw);
    QDBusMessage reply = QDBusConnection::systemBus().call(msg, QDBus::Block, 3000);
    if (reply.type() != QDBusMessage::ErrorMessage) {
      return true;
    }
  }
  return false;
}

bool BrightnessClient::writeBrightnessctl(int raw) const
{
  if (QStandardPaths::findExecutable(QStringLiteral("brightnessctl")).isEmpty()) {
    return false;
  }
  QProcess proc;
  proc.start(QStringLiteral("brightnessctl"),
             {QStringLiteral("-d"), m_deviceName, QStringLiteral("set"), QString::number(raw)});
  if (!proc.waitForStarted(2000) || !proc.waitForFinished(3000)) {
    proc.kill();
    return false;
  }
  if (proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0) {
    return true;
  }
  // Device flag sometimes rejects; try default device.
  proc.start(QStringLiteral("brightnessctl"),
             {QStringLiteral("set"), QString::number(raw)});
  if (!proc.waitForStarted(2000) || !proc.waitForFinished(3000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool BrightnessClient::appliedNear(int raw) const
{
  // Some drivers round; accept within 1 step or exact.
  const int now = readBrightness();
  return now == raw || qAbs(now - raw) <= 1;
}

bool BrightnessClient::trySetRaw(int raw)
{
  if (writeBrightnessctl(raw) && appliedNear(raw)) {
    return true;
  }
  if (writeSysfs(raw) && appliedNear(raw)) {
    return true;
  }
  if (writeLogind(raw) && appliedNear(raw)) {
    return true;
  }
  // Permission path may report success without changing hardware — last chance
  // accept any method that at least claimed success after a re-read stall.
  if (writeBrightnessctl(raw) || writeSysfs(raw) || writeLogind(raw)) {
    return appliedNear(raw) || readBrightness() == raw;
  }
  return false;
}

bool BrightnessClient::writeBrightnessctlRelative(int deltaPct) const
{
  if (QStandardPaths::findExecutable(QStringLiteral("brightnessctl")).isEmpty()) {
    return false;
  }
  const QString spec = (deltaPct >= 0) ? QStringLiteral("+%1%").arg(deltaPct)
                                       : QStringLiteral("%1%").arg(deltaPct);
  QProcess proc;
  // Prefer named device; fall back to default.
  QStringList args;
  if (!m_deviceName.isEmpty()) {
    args << QStringLiteral("-d") << m_deviceName;
  }
  args << QStringLiteral("set") << spec;
  const int before = readBrightness();
  proc.start(QStringLiteral("brightnessctl"), args);
  if (!proc.waitForStarted(2000) || !proc.waitForFinished(3000)) {
    proc.kill();
    return false;
  }
  if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
    if (!m_deviceName.isEmpty()) {
      proc.start(QStringLiteral("brightnessctl"), {QStringLiteral("set"), spec});
      if (!proc.waitForStarted(2000) || !proc.waitForFinished(3000)) {
        proc.kill();
        return false;
      }
      if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        return false;
      }
    } else {
      return false;
    }
  }
  return readBrightness() != before;
}

bool BrightnessClient::adjustBy(int deltaPct)
{
  if (!m_hasBacklight && !discover()) {
    return false;
  }
  if (deltaPct == 0) {
    return true;
  }
  if (writeBrightnessctlRelative(deltaPct)) {
    emit changed();
    return true;
  }
  const int next = qBound(1, percentage() + deltaPct, 100);
  if (setPercentage(next)) {
    return true;
  }
  return false;
}

bool BrightnessClient::setPercentage(int pct)
{
  if (!m_hasBacklight && !discover()) {
    return false;
  }
  const int wantPct = qBound(1, pct, 100);
  const QString preferred = m_deviceName;
  const QStringList devices = allDevices();
  QStringList order;
  if (!preferred.isEmpty()) {
    order << preferred;
  }
  for (const QString &d : devices) {
    if (!order.contains(d)) {
      order << d;
    }
  }

  for (const QString &d : order) {
    if (!selectDevice(d)) {
      continue;
    }
    const int maxv = readMax();
    const int raw = qBound(1, qRound(maxv * (wantPct / 100.0)), maxv);
    if (trySetRaw(raw)) {
      emit changed();
      return true;
    }
  }
  // Restore preferred device for percentage() reads even if write failed.
  if (!preferred.isEmpty()) {
    selectDevice(preferred);
  }
  return false;
}

} // namespace spike
