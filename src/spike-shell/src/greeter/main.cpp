#include "lock/PamAuth.hpp"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/kd.h>

#include <cstdlib>
#include <cstring>

/**
 * Spike graphical login greeter (Alpha).
 * Runs on tty1 via spike-greeter.service (linuxfb/eglfs) when auto-login is off.
 * On success: exec login -f <user> so profile.d starts spike-session.
 */
namespace {

// Keep the VT in KD_GRAPHICS so fbcon/console-setup cannot redraw over linuxfb.
int g_ttyFd = -1;

void claimGraphicsVt()
{
  if (g_ttyFd < 0) {
    // Prefer the already-open controlling tty from systemd StandardInput=tty.
    if (isatty(STDIN_FILENO)) {
      g_ttyFd = dup(STDIN_FILENO);
    }
    if (g_ttyFd < 0) {
      g_ttyFd = open("/dev/tty1", O_RDWR | O_NOCTTY | O_CLOEXEC);
    }
    if (g_ttyFd < 0) {
      g_ttyFd = open("/dev/tty", O_RDWR | O_NOCTTY | O_CLOEXEC);
    }
  }
  if (g_ttyFd < 0) {
    return;
  }
  if (ioctl(g_ttyFd, KDSETMODE, KD_GRAPHICS) != 0) {
    // best-effort — still try cursor/blanking control below
  }
  // No blanking, no blink, hide cursor (same sequence Qt linuxfb uses).
  static const char kTermCtl[] = "\033[9;0]\033[?33l\033[?25l\033[?1c";
  if (write(g_ttyFd, kTermCtl, sizeof(kTermCtl) - 1) < 0) {
    // best-effort
  }
  // Keep fbcon unbound so late setfont cannot blank the greeter.
  QDir vtcon(QStringLiteral("/sys/class/vtconsole"));
  for (const QString &e : vtcon.entryList({QStringLiteral("vtcon*")}, QDir::Dirs | QDir::NoDotAndDotDot)) {
    QFile bind(vtcon.absoluteFilePath(e + QStringLiteral("/bind")));
    if (bind.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      bind.write("0\n");
      bind.close();
    }
  }
}

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
  // Bare VT after Plymouth: linuxfb. Allow override (eglfs/kms) via env for debug.
  if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
    qputenv("QT_QPA_PLATFORM", "linuxfb");
  }
  if (qEnvironmentVariableIsEmpty("QT_QPA_FB") &&
      qgetenv("QT_QPA_PLATFORM").startsWith("linuxfb")) {
    qputenv("QT_QPA_FB", "/dev/fb0");
  }

  // Claim graphics mode before Qt opens fb — and keep the fd open for the
  // process lifetime so a late getty/console reset cannot flip us back to text.
  claimGraphicsVt();

  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-greeter"));
  QApplication::setOrganizationName(QStringLiteral("Spike"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.55"));

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
  auto *brand = new QLabel(win);
  brand->setAlignment(Qt::AlignCenter);
  bool haveEmblem = false;
  QPixmap emblem;
  for (const QString &path :
       {QStringLiteral("/usr/share/spike/branding/logo/spike-emblem-256.png"),
        QStringLiteral("/usr/share/plymouth/themes/spike-minimal/logo.png"),
        QStringLiteral("/usr/share/spike/branding/logo/spike-emblem-128.png")}) {
    if (emblem.load(path) && !emblem.isNull()) {
      brand->setPixmap(emblem.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      haveEmblem = true;
      break;
    }
  }
  if (!haveEmblem) {
    brand->setText(QStringLiteral("<h1>Spike</h1>"));
  }
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

  // Re-assert KD_GRAPHICS + unbind fbcon + repaint. Installed boots often get a
  // late console-setup/setfont or fbcon redraw ~1–2s after first paint.
  auto *keepAlive = new QTimer(win);
  keepAlive->setInterval(250);
  QObject::connect(keepAlive, &QTimer::timeout, win, [win, keepAlive]() {
    claimGraphicsVt();
    win->update();
    win->repaint();
    static int ticks = 0;
    if (++ticks >= 60) { // ~15s
      keepAlive->stop();
    }
  });
  keepAlive->start();

  return app.exec();
}
