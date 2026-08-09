#include "panel/applets/SessionMenuApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QStringList>

namespace spike {

namespace {

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
  if (runWait(QStringLiteral("sudo"),
              {QStringLiteral("-n"), QStringLiteral("systemctl"), systemctlVerb})) {
    return;
  }
  if (runWait(QStringLiteral("busctl"),
              {QStringLiteral("call"), QStringLiteral("org.freedesktop.login1"),
               QStringLiteral("/org/freedesktop/login1"),
               QStringLiteral("org.freedesktop.login1.Manager"), logindMethod, QStringLiteral("b"),
               QStringLiteral("false")})) {
    return;
  }
  runDetached(QStringLiteral("systemctl"), {systemctlVerb});
}

bool confirm(QWidget *parent, const QString &title, const QString &text)
{
  return QMessageBox::question(parent, title, text, QMessageBox::Yes | QMessageBox::No,
                               QMessageBox::No) == QMessageBox::Yes;
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
  QAction *lock = menu.addAction(QStringLiteral("Lock Screen"));
  QAction *logout = menu.addAction(QStringLiteral("Log out"));
  QAction *suspend = menu.addAction(QStringLiteral("Suspend"));
  QAction *reboot = menu.addAction(QStringLiteral("Restart"));
  QAction *poweroff = menu.addAction(QStringLiteral("Shut down"));

  QAction *chosen = menu.exec(mapToGlobal(QPoint(0, -menu.sizeHint().height())));
  if (!chosen) {
    return;
  }

  if (chosen == settings) {
    tray::openPanelSettings(this);
  } else if (chosen == lock) {
    const QString sessionId = qEnvironmentVariable("XDG_SESSION_ID");
    if (!sessionId.isEmpty() &&
        runDetached(QStringLiteral("loginctl"), {QStringLiteral("lock-session"), sessionId})) {
      return;
    }
    runWait(QStringLiteral("busctl"),
            {QStringLiteral("call"), QStringLiteral("org.freedesktop.login1"),
             QStringLiteral("/org/freedesktop/login1"),
             QStringLiteral("org.freedesktop.login1.Manager"), QStringLiteral("LockSessions")});
  } else if (chosen == logout) {
    if (!confirm(this, QStringLiteral("Log out"), QStringLiteral("Log out of this session?"))) {
      return;
    }
    const QString sessionId = qEnvironmentVariable("XDG_SESSION_ID");
    if (!sessionId.isEmpty()) {
      runDetached(QStringLiteral("loginctl"),
                  {QStringLiteral("terminate-session"), sessionId});
    } else {
      runDetached(QStringLiteral("loginctl"),
                  {QStringLiteral("terminate-user"), qEnvironmentVariable("USER")});
    }
  } else if (chosen == suspend) {
    runPowerAction(QStringLiteral("Suspend"), QStringLiteral("suspend"));
  } else if (chosen == reboot) {
    if (!confirm(this, QStringLiteral("Restart"), QStringLiteral("Restart the computer now?"))) {
      return;
    }
    runPowerAction(QStringLiteral("Reboot"), QStringLiteral("reboot"));
  } else if (chosen == poweroff) {
    if (!confirm(this, QStringLiteral("Shut down"),
                 QStringLiteral("Shut down the computer now?"))) {
      return;
    }
    runPowerAction(QStringLiteral("PowerOff"), QStringLiteral("poweroff"));
  }
}

} // namespace spike
