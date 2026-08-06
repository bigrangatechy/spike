#include "panel/applets/SessionMenuApplet.hpp"

#include <QIcon>
#include <QMenu>
#include <QProcess>
#include <QStringList>

namespace spike {

namespace {

// Run and wait — startDetached cannot tell success from "Access denied".
bool runWait(const QString &program, const QStringList &args, int timeoutMs = 8000)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000)) {
    return false;
  }
  if (!proc.waitForFinished(timeoutMs)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool runDetached(const QString &program, const QStringList &args)
{
  return QProcess::startDetached(program, args);
}

void runPowerAction(const QString &logindMethod, const QString &systemctlVerb)
{
  // 1) Passwordless sudo (Ubuntu live user) — most reliable without a greeter/polkit agent.
  if (runWait(QStringLiteral("sudo"),
              {QStringLiteral("-n"), QStringLiteral("systemctl"), systemctlVerb})) {
    return;
  }

  // 2) logind D-Bus (needs polkitd + allow rule for local active seats).
  if (runWait(QStringLiteral("busctl"),
              {QStringLiteral("call"), QStringLiteral("org.freedesktop.login1"),
               QStringLiteral("/org/freedesktop/login1"),
               QStringLiteral("org.freedesktop.login1.Manager"), logindMethod, QStringLiteral("b"),
               QStringLiteral("false")})) {
    return;
  }

  // 3) Last resort (may fail without auth).
  runDetached(QStringLiteral("systemctl"), {systemctlVerb});
}

} // namespace

SessionMenuApplet::SessionMenuApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("SessionButton"));
  setFlat(true);
  setFixedHeight(26);
  const QIcon icon = QIcon::fromTheme(QStringLiteral("system-shutdown"));
  if (!icon.isNull()) {
    setIcon(icon);
    setIconSize(QSize(20, 20));
    setText(QString());
  } else {
    setText(QStringLiteral("Session"));
  }
  setToolTip(QStringLiteral("Session"));
  connect(this, &QPushButton::clicked, this, &SessionMenuApplet::showMenu);
}

void SessionMenuApplet::showMenu()
{
  QMenu menu(this);
  QAction *settings = menu.addAction(QStringLiteral("Settings"));
  menu.addSeparator();
  QAction *logout = menu.addAction(QStringLiteral("Log out"));
  QAction *reboot = menu.addAction(QStringLiteral("Restart"));
  QAction *poweroff = menu.addAction(QStringLiteral("Shut down"));

  QAction *chosen = menu.exec(mapToGlobal(QPoint(0, -menu.sizeHint().height())));
  if (!chosen) {
    return;
  }

  if (chosen == settings) {
    // Walk up to Panel and open Settings (same process).
    QWidget *w = parentWidget();
    while (w) {
      if (w->objectName() == QLatin1String("SpikePanel")) {
        QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection);
        break;
      }
      w = w->parentWidget();
    }
  } else if (chosen == logout) {
    const QString sessionId = qEnvironmentVariable("XDG_SESSION_ID");
    if (!sessionId.isEmpty()) {
      runDetached(QStringLiteral("loginctl"),
                  {QStringLiteral("terminate-session"), sessionId});
    } else {
      runDetached(QStringLiteral("loginctl"),
                  {QStringLiteral("terminate-user"), qEnvironmentVariable("USER")});
    }
  } else if (chosen == reboot) {
    runPowerAction(QStringLiteral("Reboot"), QStringLiteral("reboot"));
  } else if (chosen == poweroff) {
    runPowerAction(QStringLiteral("PowerOff"), QStringLiteral("poweroff"));
  }
}

} // namespace spike
