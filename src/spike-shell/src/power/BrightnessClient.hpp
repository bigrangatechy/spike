#pragma once

#include <QObject>
#include <QString>

namespace spike {

/** Backlight brightness via sysfs and/or logind SetBrightness (unprivileged). */
class BrightnessClient : public QObject
{
  Q_OBJECT

public:
  explicit BrightnessClient(QObject *parent = nullptr);

  bool hasBacklight() const { return m_hasBacklight; }
  QString deviceName() const { return m_deviceName; }

  int percentage() const;
  /** 1–100; returns false if no write path worked. */
  bool setPercentage(int pct);

private:
  bool discover();
  int readMax() const;
  int readBrightness() const;
  bool writeSysfs(int raw) const;
  bool writeLogind(int raw) const;
  bool writeBrightnessctl(int raw) const;

  bool m_hasBacklight = false;
  QString m_deviceName;
  QString m_brightnessPath;
  QString m_maxPath;
};

} // namespace spike
