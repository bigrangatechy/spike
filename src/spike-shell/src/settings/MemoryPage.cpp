#include "settings/MemoryPage.hpp"

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
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

bool runOk(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool applySysctlLive()
{
  if (runOk(QStringLiteral("sysctl"),
            {QStringLiteral("--system")})) {
    return true;
  }
  return runOk(QStringLiteral("sudo"),
               {QStringLiteral("-n"), QStringLiteral("sysctl"), QStringLiteral("--system")});
}

} // namespace

QWidget *makeMemoryPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Memory</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Writes org.spike.Config memory + regenerates sysctl/zram/earlyoom files. "
          "Swappiness can apply live via sysctl; ZRAM / earlyoom usually need a reboot or "
          "service restart."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *swappiness = new QSpinBox(w);
  swappiness->setRange(0, 100);
  form->addRow(QStringLiteral("Swappiness"), swappiness);

  auto *vfs = new QSpinBox(w);
  vfs->setRange(0, 200);
  form->addRow(QStringLiteral("VFS cache pressure"), vfs);

  auto *zramOn = new QCheckBox(QStringLiteral("Enable ZRAM"), w);
  form->addRow(QString(), zramOn);

  auto *zramMb = new QSpinBox(w);
  zramMb->setRange(256, 32768);
  zramMb->setSingleStep(256);
  zramMb->setSuffix(QStringLiteral(" MiB"));
  form->addRow(QStringLiteral("ZRAM size"), zramMb);

  auto *earlyoomOn = new QCheckBox(QStringLiteral("Enable earlyoom"), w);
  form->addRow(QString(), earlyoomOn);

  auto *earlyoomThresh = new QSpinBox(w);
  earlyoomThresh->setRange(2, 50);
  earlyoomThresh->setSuffix(QStringLiteral(" %"));
  form->addRow(QStringLiteral("earlyoom free threshold"), earlyoomThresh);

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

  auto load = [config, swappiness, vfs, zramOn, zramMb, earlyoomOn, earlyoomThresh, status,
               statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("memory"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    swappiness->setValue(o.value(QStringLiteral("swappiness")).toInt(15));
    vfs->setValue(o.value(QStringLiteral("vfs_cache_pressure")).toInt(100));
    zramOn->setChecked(o.value(QStringLiteral("zram_enabled")).toBool(true));
    zramMb->setValue(o.value(QStringLiteral("zram_size_mb")).toInt(4096));
    earlyoomOn->setChecked(o.value(QStringLiteral("earlyoom_enabled")).toBool(true));
    earlyoomThresh->setValue(o.value(QStringLiteral("earlyoom_threshold")).toInt(10));
    status->setText(QStringLiteral("Loaded memory module"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Memory loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, swappiness, vfs, zramOn, zramMb, earlyoomOn, earlyoomThresh, status,
                    statusBar, load]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     auto set = [&](const QString &key, const QVariant &v) -> bool {
                       if (!config->setSetting(QStringLiteral("memory"), key, v, &err)) {
                         status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
                         return false;
                       }
                       return true;
                     };
                     if (!set(QStringLiteral("swappiness"), swappiness->value()) ||
                         !set(QStringLiteral("vfs_cache_pressure"), vfs->value()) ||
                         !set(QStringLiteral("zram_enabled"), zramOn->isChecked()) ||
                         !set(QStringLiteral("zram_size_mb"), zramMb->value()) ||
                         !set(QStringLiteral("earlyoom_enabled"), earlyoomOn->isChecked()) ||
                         !set(QStringLiteral("earlyoom_threshold"), earlyoomThresh->value())) {
                       return;
                     }
                     const bool sysctlOk = applySysctlLive();
                     status->setText(
                         sysctlOk
                             ? QStringLiteral(
                                   "Saved + sysctl applied. Reboot/service restart may still be "
                                   "needed for ZRAM / earlyoom.")
                             : QStringLiteral(
                                   "Saved (configs regenerated). Could not run sysctl --system "
                                   "live — reboot or apply with privileges. ZRAM/earlyoom may need "
                                   "reboot."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Memory applied"));
                     }
                     load();
                   });

  load();
  return w;
}

} // namespace spike
