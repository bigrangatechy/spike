#pragma once

#include <QObject>
#include <QString>

class QDBusInterface;

namespace spike {

/** UPower battery status for the tray applet (DESKTOP.md). */
class BatteryClient : public QObject
{
  Q_OBJECT

public:
  explicit BatteryClient(QObject *parent = nullptr);

  bool hasBattery() const { return m_hasBattery; }
  int percentage() const { return m_percent; }
  bool charging() const { return m_charging; }
  QString stateText() const { return m_stateText; }
  /** Human ETA when UPower reports TimeToEmpty / TimeToFull (seconds); empty if unknown. */
  QString timeRemainingText() const { return m_timeText; }
  QString iconName() const;

  void refresh();

signals:
  void changed();

private:
  bool m_hasBattery = false;
  int m_percent = 0;
  bool m_charging = false;
  QString m_stateText;
  QString m_timeText;
  QString m_devicePath;
};

} // namespace spike
