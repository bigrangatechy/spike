#include "panel/applets/SessionMenuApplet.hpp"

#include <QMenu>
#include <QProcess>

namespace spike {

SessionMenuApplet::SessionMenuApplet(QWidget *parent)
  : QPushButton(QStringLiteral("Session"), parent)
{
  setObjectName(QStringLiteral("SessionButton"));
  setFlat(true);
  connect(this, &QPushButton::clicked, this, &SessionMenuApplet::showMenu);
}

void SessionMenuApplet::showMenu()
{
  QMenu menu(this);
  QAction *logout = menu.addAction(QStringLiteral("Log out"));
  QAction *reboot = menu.addAction(QStringLiteral("Restart"));
  QAction *poweroff = menu.addAction(QStringLiteral("Shut down"));

  QAction *chosen = menu.exec(mapToGlobal(QPoint(0, -menu.sizeHint().height())));
  if (chosen == logout) {
    QProcess::startDetached(QStringLiteral("loginctl"), {QStringLiteral("terminate-user"),
                                                         qEnvironmentVariable("USER")});
  } else if (chosen == reboot) {
    QProcess::startDetached(QStringLiteral("systemctl"), {QStringLiteral("reboot")});
  } else if (chosen == poweroff) {
    QProcess::startDetached(QStringLiteral("systemctl"), {QStringLiteral("poweroff")});
  }
}

} // namespace spike
