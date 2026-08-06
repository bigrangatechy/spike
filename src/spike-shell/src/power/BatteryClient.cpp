#include "power/BatteryClient.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QVariant>

namespace spike {

BatteryClient::BatteryClient(QObject *parent)
  : QObject(parent)
{
  refresh();
}

QString BatteryClient::iconName() const
{
  if (!m_hasBattery) {
    return QStringLiteral("battery-missing");
  }
  if (m_charging) {
    if (m_percent >= 95) {
      return QStringLiteral("battery-full-charging");
    }
    if (m_percent >= 55) {
      return QStringLiteral("battery-good-charging");
    }
    if (m_percent >= 25) {
      return QStringLiteral("battery-low-charging");
    }
    return QStringLiteral("battery-caution-charging");
  }
  if (m_percent >= 95) {
    return QStringLiteral("battery-full");
  }
  if (m_percent >= 55) {
    return QStringLiteral("battery-good");
  }
  if (m_percent >= 25) {
    return QStringLiteral("battery-low");
  }
  if (m_percent >= 10) {
    return QStringLiteral("battery-caution");
  }
  return QStringLiteral("battery-empty");
}

void BatteryClient::refresh()
{
  QDBusInterface up(QStringLiteral("org.freedesktop.UPower"),
                    QStringLiteral("/org/freedesktop/UPower"),
                    QStringLiteral("org.freedesktop.UPower"), QDBusConnection::systemBus());
  if (!up.isValid()) {
    if (m_hasBattery) {
      m_hasBattery = false;
      emit changed();
    }
    return;
  }

  QDBusReply<QList<QDBusObjectPath>> devices = up.call(QStringLiteral("EnumerateDevices"));
  if (!devices.isValid()) {
    m_hasBattery = false;
    emit changed();
    return;
  }

  bool found = false;
  int percent = 0;
  bool charging = false;
  QString stateText;
  QString path;

  for (const QDBusObjectPath &op : devices.value()) {
    QDBusInterface dev(QStringLiteral("org.freedesktop.UPower"), op.path(),
                       QStringLiteral("org.freedesktop.UPower.Device"),
                       QDBusConnection::systemBus());
    if (!dev.isValid()) {
      continue;
    }
    // Type 2 = Battery
    const int type = dev.property("Type").toInt();
    if (type != 2) {
      continue;
    }
    found = true;
    path = op.path();
    percent = qBound(0, static_cast<int>(dev.property("Percentage").toDouble()), 100);
    const int state = dev.property("State").toInt();
    // 1 charging, 2 discharging, 3 empty, 4 fully charged, 5 pending charge, 6 pending discharge
    charging = (state == 1 || state == 5 || state == 4);
    switch (state) {
    case 1:
      stateText = QStringLiteral("Charging");
      break;
    case 2:
      stateText = QStringLiteral("Discharging");
      break;
    case 4:
      stateText = QStringLiteral("Fully charged");
      break;
    default:
      stateText = QStringLiteral("Battery");
      break;
    }
    break;
  }

  if (found != m_hasBattery || percent != m_percent || charging != m_charging ||
      stateText != m_stateText || path != m_devicePath) {
    m_hasBattery = found;
    m_percent = percent;
    m_charging = charging;
    m_stateText = stateText;
    m_devicePath = path;
    emit changed();
  }
}

} // namespace spike
