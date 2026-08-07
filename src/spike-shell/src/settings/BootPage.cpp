#include "settings/BootPage.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QSpinBox>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

bool runOk(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(20000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

} // namespace

QWidget *makeBootPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Boot</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Writes org.spike.Config boot + regenerates /etc/default/grub. "
          "Run update-grub (needs privileges) and reboot for GRUB changes to take effect."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *countLabel = new QLabel(w);
  lay->addWidget(countLabel);

  auto *form = new QFormLayout();
  auto *timeout = new QSpinBox(w);
  timeout->setRange(0, 30);
  timeout->setSuffix(QStringLiteral(" s"));
  form->addRow(QStringLiteral("GRUB timeout"), timeout);

  auto *style = new QComboBox(w);
  style->addItem(QStringLiteral("Hidden"), QStringLiteral("hidden"));
  style->addItem(QStringLiteral("Menu"), QStringLiteral("menu"));
  style->addItem(QStringLiteral("Countdown"), QStringLiteral("countdown"));
  form->addRow(QStringLiteral("GRUB timeout style"), style);

  auto *showOnFail = new QCheckBox(QStringLiteral("Show menu on boot failure"), w);
  form->addRow(QString(), showOnFail);

  auto *failThresh = new QSpinBox(w);
  failThresh->setRange(1, 10);
  form->addRow(QStringLiteral("Failure threshold"), failThresh);

  auto *plymouth = new QComboBox(w);
  plymouth->addItem(QStringLiteral("spike-minimal"), QStringLiteral("spike-minimal"));
  plymouth->addItem(QStringLiteral("spike"), QStringLiteral("spike"));
  plymouth->setEditable(true);
  form->addRow(QStringLiteral("Plymouth theme"), plymouth);

  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *resetCount = new QPushButton(QStringLiteral("Reset boot counter"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  auto *updateGrub = new QPushButton(QStringLiteral("update-grub"), w);
  row->addWidget(reload);
  row->addWidget(resetCount);
  row->addWidget(apply);
  row->addWidget(updateGrub);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  auto load = [config, timeout, style, showOnFail, failThresh, plymouth, countLabel, status,
               statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("boot"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    timeout->setValue(o.value(QStringLiteral("grub_timeout")).toInt(0));
    const QString st = o.value(QStringLiteral("grub_timeout_style")).toString(QStringLiteral("hidden"));
    const int idx = style->findData(st);
    if (idx >= 0) {
      style->setCurrentIndex(idx);
    }
    showOnFail->setChecked(o.value(QStringLiteral("show_menu_on_failure")).toBool(true));
    failThresh->setValue(o.value(QStringLiteral("failure_threshold")).toInt(3));
    const QString theme =
        o.value(QStringLiteral("plymouth_theme")).toString(QStringLiteral("spike-minimal"));
    const int tidx = plymouth->findData(theme);
    if (tidx >= 0) {
      plymouth->setCurrentIndex(tidx);
    } else {
      plymouth->setEditText(theme);
    }
    const QString countOut =
        runCapture(QStringLiteral("spike-config"), {QStringLiteral("--boot-count"), QStringLiteral("get")});
    countLabel->setText(QStringLiteral("<b>Boot failure counter:</b> %1")
                            .arg(countOut.isEmpty()
                                     ? QString::number(o.value(QStringLiteral("boot_failure_count")).toInt(0))
                                     : countOut));
    status->setText(QStringLiteral("Loaded boot module"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Boot loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(resetCount, &QPushButton::clicked, w, [status, statusBar, load]() {
    if (!runOk(QStringLiteral("spike-config"),
               {QStringLiteral("--boot-count"), QStringLiteral("reset")})) {
      status->setText(QStringLiteral("spike-config --boot-count reset failed (need privileges?)."));
      return;
    }
    status->setText(QStringLiteral("Boot counter reset"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Boot counter reset"));
    }
    load();
  });
  QObject::connect(updateGrub, &QPushButton::clicked, w, [status, statusBar]() {
    if (runOk(QStringLiteral("update-grub"), {}) ||
        runOk(QStringLiteral("sudo"), {QStringLiteral("-n"), QStringLiteral("update-grub")})) {
      status->setText(QStringLiteral("update-grub OK — reboot to use new menu."));
      if (statusBar) {
        statusBar->setText(QStringLiteral("update-grub OK"));
      }
      return;
    }
    status->setText(QStringLiteral("update-grub failed (need privileges). Config files still written."));
  });
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, timeout, style, showOnFail, failThresh, plymouth, status, statusBar,
                    load]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     auto set = [&](const QString &key, const QVariant &v) -> bool {
                       if (!config->setSetting(QStringLiteral("boot"), key, v, &err)) {
                         status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
                         return false;
                       }
                       return true;
                     };
                     const QString theme = plymouth->currentData().isValid()
                                              ? plymouth->currentData().toString()
                                              : plymouth->currentText().trimmed();
                     if (!set(QStringLiteral("grub_timeout"), timeout->value()) ||
                         !set(QStringLiteral("grub_timeout_style"), style->currentData()) ||
                         !set(QStringLiteral("show_menu_on_failure"), showOnFail->isChecked()) ||
                         !set(QStringLiteral("failure_threshold"), failThresh->value()) ||
                         !set(QStringLiteral("plymouth_theme"), theme)) {
                       return;
                     }
                     status->setText(QStringLiteral(
                         "Boot config saved. Click update-grub (if allowed) then reboot."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Boot applied"));
                     }
                     load();
                   });

  load();
  return w;
}

} // namespace spike
