#pragma once

#include <QObject>

namespace spike {

/**
 * Plasma-style “Manually block sleep and screen locking”:
 * holds a logind Inhibit("sleep:idle", …, "block") fd while enabled.
 */
class SleepInhibit : public QObject
{
  Q_OBJECT

public:
  static SleepInhibit &instance();

  bool isActive() const { return m_active; }
  /** Enable/disable; persists to QSettings. Returns false if inhibit failed to acquire. */
  bool setActive(bool on, QString *error = nullptr);

signals:
  void changed(bool active);

private:
  explicit SleepInhibit(QObject *parent = nullptr);
  ~SleepInhibit() override;

  bool acquire(QString *error);
  void release();

  bool m_active = false;
  int m_fd = -1;
};

} // namespace spike
