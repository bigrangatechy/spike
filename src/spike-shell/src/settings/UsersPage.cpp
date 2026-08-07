#include "settings/UsersPage.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <unistd.h>

namespace spike {

namespace {

bool isLiveSession()
{
  return QFile::exists(QStringLiteral("/cdrom/casper")) ||
         QFile::exists(QStringLiteral("/run/casper")) ||
         qEnvironmentVariableIsSet("SPIKE_LIVE");
}

bool changePassword(const QString &user, const QString &password, QString *error)
{
  // Non-interactive: chpasswd via sudo -n (same pattern as power/timedatectl).
  QProcess proc;
  proc.start(QStringLiteral("sudo"),
             {QStringLiteral("-n"), QStringLiteral("chpasswd")});
  if (!proc.waitForStarted(3000)) {
    if (error) {
      *error = QStringLiteral("sudo/chpasswd not available");
    }
    return false;
  }
  const QByteArray payload = (user + QLatin1Char(':') + password + QLatin1Char('\n')).toUtf8();
  proc.write(payload);
  proc.closeWriteChannel();
  if (!proc.waitForFinished(15000)) {
    proc.kill();
    if (error) {
      *error = QStringLiteral("chpasswd timed out");
    }
    return false;
  }
  if (proc.exitCode() != 0) {
    if (error) {
      *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
      if (error->isEmpty()) {
        *error = QStringLiteral("chpasswd failed");
      }
    }
    return false;
  }
  return true;
}

} // namespace

QWidget *makeUsersPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Users</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Current session account only (not kcm_users). Password change needs sudo -n "
          "chpasswd. Auto-login is stored in spike-config for the installer / installed system."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  const QString user = qEnvironmentVariable("USER");
  char host[256] = {};
  gethostname(host, sizeof(host) - 1);
  auto *who = new QLabel(
      QStringLiteral("<b>User:</b> %1<br/><b>Host:</b> %2%3")
          .arg(user.isEmpty() ? QStringLiteral("(unknown)") : user, QString::fromUtf8(host),
               isLiveSession() ? QStringLiteral("<br/><i>Live session</i>") : QString()),
      w);
  who->setTextFormat(Qt::RichText);
  lay->addWidget(who);

  auto *autoLogin = new QCheckBox(QStringLiteral("Enable auto-login (installed system)"), w);
  lay->addWidget(autoLogin);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *passwdBtn = new QPushButton(QStringLiteral("Change password…"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(passwdBtn);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  auto load = [config, autoLogin, status, statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("installer"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    autoLogin->setChecked(o.value(QStringLiteral("auto_login")).toBool(false));
    status->setText(isLiveSession()
                        ? QStringLiteral("Loaded. Auto-login applies after install.")
                        : QStringLiteral("Loaded installer.auto_login from spike-config"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Users loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, autoLogin, status, statusBar, load]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     if (!config->setSetting(QStringLiteral("installer"), QStringLiteral("auto_login"),
                                             autoLogin->isChecked(), &err)) {
                       status->setText(QStringLiteral("SetSetting failed: %1").arg(err));
                       return;
                     }
                     status->setText(isLiveSession()
                                         ? QStringLiteral(
                                               "auto_login saved for installed system (live session "
                                               "already auto-logs in).")
                                         : QStringLiteral(
                                               "auto_login saved — session manager must honour it "
                                               "(installer / display manager)."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Users applied"));
                     }
                     load();
                   });
  QObject::connect(passwdBtn, &QPushButton::clicked, w, [w, user, status, statusBar]() {
    if (user.isEmpty()) {
      status->setText(QStringLiteral("No USER in environment"));
      return;
    }
    QDialog dlg(w);
    dlg.setWindowTitle(QStringLiteral("Change password"));
    auto *form = new QFormLayout(&dlg);
    auto *p1 = new QLineEdit(&dlg);
    p1->setEchoMode(QLineEdit::Password);
    auto *p2 = new QLineEdit(&dlg);
    p2->setEchoMode(QLineEdit::Password);
    form->addRow(QStringLiteral("New password"), p1);
    form->addRow(QStringLiteral("Confirm"), p2);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted) {
      return;
    }
    if (p1->text().isEmpty() || p1->text() != p2->text()) {
      status->setText(QStringLiteral("Passwords empty or do not match"));
      return;
    }
    if (p1->text().size() < 6) {
      status->setText(QStringLiteral("Password must be at least 6 characters"));
      return;
    }
    QString err;
    if (!changePassword(user, p1->text(), &err)) {
      status->setText(QStringLiteral("Password change failed: %1").arg(err));
      return;
    }
    status->setText(QStringLiteral("Password updated for %1").arg(user));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Password updated"));
    }
  });

  load();
  return w;
}

} // namespace spike
