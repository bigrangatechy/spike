#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

namespace spike {

/** Backlight brightness via brightnessctl / sysfs / logind SetBrightness. */
class BrightnessClient : public QObject
{
  Q_OBJECT

public:
  explicit BrightnessClient(QObject *parent = nullptr);

  bool hasBacklight() const { return m_hasBacklight; }
  QString deviceName() const { return m_deviceName; }

  int percentage() const;
  /** 1–100; returns false if no write path changed the backlight. */
  bool setPercentage(int pct);

private:
  bool discover();
  bool selectDevice(const QString &name);
  QStringList allDevices() const;
  int readMax() const;
  int readBrightness() const;
  bool writeSysfs(int raw) const;
  bool writeLogind(int raw) const;
  bool writeBrightnessctl(int raw) const;
  bool appliedNear(int raw) const;
  bool trySetRaw(int raw);

  bool m_hasBacklight = false;
  QString m_deviceName;
  QString m_brightnessPath;
  QString m_maxPath;
};

} // namespace spike
