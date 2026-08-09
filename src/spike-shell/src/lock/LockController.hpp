#pragma once

#include <QObject>

namespace spike {

/**
 * Owns lock triggers: logind Session.Lock, PrepareForSleep, and manual API.
 * Call instance() early from main so inhibit restore + D-Bus watches are active.
 */
class LockController : public QObject
{
  Q_OBJECT

public:
  static LockController &instance();

  void lockScreen();

private slots:
  void onPrepareForSleep(bool starting);
  void onSessionLock();

private:
  explicit LockController(QObject *parent = nullptr);
  void watchLogind();
};

} // namespace spike
