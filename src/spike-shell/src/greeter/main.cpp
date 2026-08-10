#include "lock/PamAuth.hpp"

#include <QApplication>
#include <QFile>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <unistd.h>

#include <cstdlib>
#include <cstring>

/**
 * Spike graphical login greeter (Alpha).
 * Runs on tty1 via spike-greeter.service (linuxfb/eglfs) when auto-login is off.
 * On success: exec login -f <user> so profile.d starts spike-session.
 */
namespace {

QString defaultUsername()
{
  QFile f(QStringLiteral("/etc/spike/installed"));
  Q_UNUSED(f);
  // Prefer the first non-system user with a home under /home.
  QProcess proc;
  proc.start(QStringLiteral("getent"), {QStringLiteral("passwd")});
  if (!proc.waitForFinished(5000)) {
    return QStringLiteral("spike");
  }
  const QString out = QString::fromUtf8(proc.readAllStandardOutput());
  for (const QString &line : out.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    const QStringList p = line.split(QLatin1Char(':'));
    if (p.size() < 6) {
      continue;
    }
    bool ok = false;
    const int uid = p.at(2).toInt(&ok);
    if (!ok || uid < 1000 || uid >= 65534) {
      continue;
    }
    if (p.at(5).startsWith(QLatin1String("/home/"))) {
      return p.at(0);
    }
  }
  return QStringLiteral("spike");
}

} // namespace

int main(int argc, char *argv[])
{
  // Prefer linuxfb on a bare VT; fall back to eglfs / wayland if set by env.
  if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
    qputenv("QT_QPA_PLATFORM", "linuxfb");
  }

  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-greeter"));
  QApplication::setOrganizationName(QStringLiteral("Spike"));

  auto *win = new QWidget;
  win->setObjectName(QStringLiteral("SpikeGreeter"));
  win->setStyleSheet(QStringLiteral(
      "#SpikeGreeter { background-color: #1a1a2e; color: #ffffff; }"
      "QLineEdit { background: #222236; color: #fff; padding: 8px; border: 1px solid #444; "
      "border-radius: 4px; min-height: 28px; }"
      "QPushButton { background: #6d4aff; color: #fff; padding: 10px 18px; border: none; "
      "border-radius: 4px; font-weight: bold; }"
      "QPushButton:hover { background: #7e5fff; }"
      "QLabel { color: #e8e8f0; }"));
  win->setWindowTitle(QStringLiteral("Spike Login"));

  auto *lay = new QVBoxLayout(win);
  lay->setContentsMargins(48, 48, 48, 48);
  lay->addStretch(1);
  auto *brand = new QLabel(QStringLiteral("<h1>Spike</h1>"), win);
  brand->setAlignment(Qt::AlignCenter);
  lay->addWidget(brand);
  auto *sub = new QLabel(QStringLiteral("Sign in to continue"), win);
  sub->setAlignment(Qt::AlignCenter);
  lay->addWidget(sub);
  lay->addSpacing(24);

  auto *form = new QFormLayout();
  auto *user = new QLineEdit(defaultUsername(), win);
  auto *pass = new QLineEdit(win);
  pass->setEchoMode(QLineEdit::Password);
  form->addRow(QStringLiteral("Username"), user);
  form->addRow(QStringLiteral("Password"), pass);
  lay->addLayout(form);

  auto *status = new QLabel(win);
  status->setWordWrap(true);
  status->setAlignment(Qt::AlignCenter);
  lay->addWidget(status);

  auto *loginBtn = new QPushButton(QStringLiteral("Log in"), win);
  lay->addWidget(loginBtn, 0, Qt::AlignHCenter);
  lay->addStretch(2);

  auto tryLogin = [user, pass, status, &app]() {
    const QString u = user->text().trimmed();
    const QString p = pass->text();
    if (u.isEmpty() || p.isEmpty()) {
      status->setText(QStringLiteral("Enter username and password."));
      return;
    }
    QString err;
    if (!spike::pamAuthenticateLogin(u, p, &err)) {
      status->setText(err.isEmpty() ? QStringLiteral("Login failed") : err);
      pass->clear();
      pass->setFocus();
      return;
    }
    status->setText(QStringLiteral("Welcome…"));
    app.processEvents();
    // Hand off to login(1) so pam/session + profile.d start spike-session.
    // Greeter must be root (systemd unit) for login -f.
    const QByteArray ub = u.toUtf8();
    execl("/bin/login", "login", "-f", ub.constData(), static_cast<char *>(nullptr));
    // If execl fails:
    status->setText(QStringLiteral("login(1) failed — is this greeter running as root?"));
  };

  QObject::connect(loginBtn, &QPushButton::clicked, win, tryLogin);
  QObject::connect(pass, &QLineEdit::returnPressed, win, tryLogin);
  QObject::connect(user, &QLineEdit::returnPressed, win, [pass]() { pass->setFocus(); });

  win->showFullScreen();
  pass->setFocus();
  return app.exec();
}
