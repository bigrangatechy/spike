#include "lock/LockController.hpp"

#include "lock/SpikeLockScreen.hpp"
#include "power/SleepInhibit.hpp"

#include <QDBusConnection>

namespace spike {

LockController &LockController::instance()
{
  static LockController s;
  return s;
}

LockController::LockController(QObject *parent)
  : QObject(parent)
{
  // Touch SleepInhibit so QSettings restore runs even without battery/settings.
  (void)SleepInhibit::instance();
  watchLogind();
}

void LockController::lockScreen()
{
  SpikeLockScreen::instance().lock();
}

void LockController::watchLogind()
{
  QDBusConnection bus = QDBusConnection::systemBus();
  bus.connect(QStringLiteral("org.freedesktop.login1"), QStringLiteral("/org/freedesktop/login1"),
              QStringLiteral("org.freedesktop.login1.Manager"), QStringLiteral("PrepareForSleep"),
              this, SLOT(onPrepareForSleep(bool)));

  bus.connect(QStringLiteral("org.freedesktop.login1"),
              QStringLiteral("/org/freedesktop/login1/session/auto"),
              QStringLiteral("org.freedesktop.login1.Session"), QStringLiteral("Lock"), this,
              SLOT(onSessionLock()));
}

void LockController::onPrepareForSleep(bool starting)
{
  if (!starting) {
    return;
  }
  lockScreen();
}

void LockController::onSessionLock()
{
  lockScreen();
}

} // namespace spike
