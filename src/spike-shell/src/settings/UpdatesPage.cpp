#include "settings/UpdatesPage.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

bool sudoWriteFile(const QString &path, const QByteArray &body, QString *error)
{
  QProcess tee;
  tee.start(QStringLiteral("sudo"),
            {QStringLiteral("-n"), QStringLiteral("tee"), path});
  if (!tee.waitForStarted(3000)) {
    if (error) {
      *error = QStringLiteral("sudo/tee not available");
    }
    return false;
  }
  tee.write(body);
  tee.closeWriteChannel();
  if (!tee.waitForFinished(8000) || tee.exitCode() != 0) {
    if (error) {
      *error = QStringLiteral("could not write %1").arg(path);
    }
    return false;
  }
  return true;
}

bool sudoSystemctl(const QStringList &args)
{
  QProcess proc;
  proc.start(QStringLiteral("sudo"),
             QStringList{QStringLiteral("-n"), QStringLiteral("systemctl")} + args);
  return proc.waitForFinished(12000) && proc.exitCode() == 0;
}

/** Apply Ubuntu security auto-install via unattended-upgrades (BOOT-PROCESS.md). */
bool applyUnattendedSecurity(bool enable, QString *error)
{
  const QByteArray periodic = enable ? QByteArrayLiteral(
      "// Written by Spike Settings → Updates\n"
      "APT::Periodic::Update-Package-Lists \"1\";\n"
      "APT::Periodic::Download-Upgradeable-Packages \"1\";\n"
      "APT::Periodic::Unattended-Upgrade \"1\";\n"
      "APT::Periodic::AutocleanInterval \"7\";\n")
                                     : QByteArrayLiteral(
                                           "// Written by Spike Settings → Updates\n"
                                           "APT::Periodic::Update-Package-Lists \"1\";\n"
                                           "APT::Periodic::Download-Upgradeable-Packages \"0\";\n"
                                           "APT::Periodic::Unattended-Upgrade \"0\";\n"
                                           "APT::Periodic::AutocleanInterval \"7\";\n");

  const QByteArray origins = QByteArrayLiteral(
      "// Written by Spike Settings → Updates — Ubuntu security only.\n"
      "// Spike component packages stay notify/Discover (not silent).\n"
      "Unattended-Upgrade::Allowed-Origins {\n"
      "        \"${distro_id}:${distro_codename}-security\";\n"
      "        \"${distro_id}ESMApps:${distro_codename}-apps-security\";\n"
      "        \"${distro_id}ESM:${distro_codename}-infra-security\";\n"
      "};\n"
      "Unattended-Upgrade::Package-Blacklist {\n"
      "        \"spike-shell\";\n"
      "        \"spike-config\";\n"
      "        \"spike-rescue\";\n"
      "        \"spike-installer\";\n"
      "        \"spike-migration\";\n"
      "};\n"
      "Unattended-Upgrade::Remove-Unused-Dependencies \"true\";\n"
      "Unattended-Upgrade::Automatic-Reboot \"false\";\n");

  if (!sudoWriteFile(QStringLiteral("/etc/apt/apt.conf.d/20auto-upgrades"), periodic, error)) {
    return false;
  }
  if (enable) {
    if (!sudoWriteFile(QStringLiteral("/etc/apt/apt.conf.d/51spike-unattended-upgrades"), origins,
                       error)) {
      return false;
    }
    sudoSystemctl({QStringLiteral("enable"), QStringLiteral("--now"),
                   QStringLiteral("unattended-upgrades.service")});
  } else {
    QProcess rm;
    rm.start(QStringLiteral("sudo"),
             {QStringLiteral("-n"), QStringLiteral("rm"), QStringLiteral("-f"),
              QStringLiteral("/etc/apt/apt.conf.d/51spike-unattended-upgrades")});
    rm.waitForFinished(5000);
  }
  return true;
}

} // namespace

QWidget *makeUpdatesPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Updates</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Ubuntu security updates can install automatically. Spike packages "
          "(shell, rescue, …) and other apps notify via Discover / the tray — "
          "see UPDATES.md. Spike APT stays Enabled: no until the package host is online."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *securityAuto = new QCheckBox(QStringLiteral("Automatically install security updates"), w);
  form->addRow(QString(), securityAuto);

  auto *nonSecNotify = new QCheckBox(QStringLiteral("Notify only for non-security updates"), w);
  form->addRow(QString(), nonSecNotify);

  auto *flatpakNotify = new QCheckBox(QStringLiteral("Notify only for Flatpak updates"), w);
  form->addRow(QString(), flatpakNotify);

  auto *interval = new QSpinBox(w);
  interval->setRange(1, 168);
  interval->setSuffix(QStringLiteral(" hours"));
  form->addRow(QStringLiteral("Check interval"), interval);

  auto *idleMins = new QSpinBox(w);
  idleMins->setRange(0, 120);
  idleMins->setSuffix(QStringLiteral(" min"));
  form->addRow(QStringLiteral("Require idle before install"), idleMins);

  auto *neverReboot = new QCheckBox(QStringLiteral("Never force reboot after updates"), w);
  form->addRow(QString(), neverReboot);

  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  auto load = [config, securityAuto, nonSecNotify, flatpakNotify, interval, idleMins, neverReboot,
               status, statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("updates"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    securityAuto->setChecked(o.value(QStringLiteral("security_auto_install")).toBool(true));
    nonSecNotify->setChecked(o.value(QStringLiteral("non_security_notify_only")).toBool(true));
    flatpakNotify->setChecked(o.value(QStringLiteral("flatpak_notify_only")).toBool(true));
    interval->setValue(o.value(QStringLiteral("check_interval_hours")).toInt(6));
    idleMins->setValue(o.value(QStringLiteral("idle_required_minutes")).toInt(10));
    neverReboot->setChecked(o.value(QStringLiteral("never_force_reboot")).toBool(true));
    status->setText(QStringLiteral("Loaded updates module"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Updates loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, securityAuto, nonSecNotify, flatpakNotify, interval, idleMins,
                    neverReboot, status, statusBar]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     auto set = [&](const QString &key, const QVariant &v) -> bool {
                       if (!config->setSetting(QStringLiteral("updates"), key, v, &err)) {
                         status->setText(
                             QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
                         return false;
                       }
                       return true;
                     };
                     if (!set(QStringLiteral("security_auto_install"), securityAuto->isChecked()) ||
                         !set(QStringLiteral("non_security_notify_only"),
                              nonSecNotify->isChecked()) ||
                         !set(QStringLiteral("flatpak_notify_only"), flatpakNotify->isChecked()) ||
                         !set(QStringLiteral("check_interval_hours"), interval->value()) ||
                         !set(QStringLiteral("idle_required_minutes"), idleMins->value()) ||
                         !set(QStringLiteral("never_force_reboot"), neverReboot->isChecked())) {
                       return;
                     }
                     QString applyErr;
                     if (!applyUnattendedSecurity(securityAuto->isChecked(), &applyErr)) {
                       status->setText(QStringLiteral(
                           "Saved preference, but could not configure unattended-upgrades: %1 "
                           "(need sudo -n)")
                                           .arg(applyErr));
                       return;
                     }
                     status->setText(
                         securityAuto->isChecked()
                             ? QStringLiteral(
                                   "Security auto-install enabled (Ubuntu -security only).")
                             : QStringLiteral("Security auto-install disabled."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Updates applied"));
                     }
                   });

  load();
  return w;
}

} // namespace spike
