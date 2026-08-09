#include "settings/UpdatesPage.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

QWidget *makeUpdatesPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Updates</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Stores update policy in org.spike.Config updates. "
          "Unattended-upgrade / Flatpak hooks land later — prefs are saved for the installed system."),
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
                     status->setText(QStringLiteral(
                         "Saved update policy (no unattended-upgrades generator yet)."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Updates applied"));
                     }
                   });

  load();
  return w;
}

} // namespace spike
