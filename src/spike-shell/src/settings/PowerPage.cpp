#include "settings/PowerPage.hpp"

#include "power/BatteryClient.hpp"
#include "power/SleepInhibit.hpp"
#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
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

bool applyGovernorLive(const QString &governor)
{
  if (governor.isEmpty()) {
    return false;
  }
  if (runOk(QStringLiteral("cpupower"),
            {QStringLiteral("frequency-set"), QStringLiteral("-g"), governor})) {
    return true;
  }
  return runOk(QStringLiteral("sudo"),
               {QStringLiteral("-n"), QStringLiteral("cpupower"), QStringLiteral("frequency-set"),
                QStringLiteral("-g"), governor});
}

void fillActionCombo(QComboBox *box)
{
  box->clear();
  box->addItem(QStringLiteral("Suspend"), QStringLiteral("suspend"));
  box->addItem(QStringLiteral("Hibernate"), QStringLiteral("hibernate"));
  box->addItem(QStringLiteral("Hybrid sleep"), QStringLiteral("hybrid-sleep"));
  box->addItem(QStringLiteral("Power off"), QStringLiteral("poweroff"));
  box->addItem(QStringLiteral("Lock"), QStringLiteral("lock"));
  box->addItem(QStringLiteral("Do nothing"), QStringLiteral("ignore"));
}

void setComboData(QComboBox *box, const QString &data)
{
  const int idx = box->findData(data);
  box->setCurrentIndex(idx >= 0 ? idx : 0);
}

} // namespace

QWidget *makePowerPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *outer = new QWidget(parent);
  auto *outerLay = new QVBoxLayout(outer);
  outerLay->setContentsMargins(0, 0, 0, 0);

  auto *scroll = new QScrollArea(outer);
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  auto *w = new QWidget(scroll);
  auto *lay = new QVBoxLayout(w);

  lay->addWidget(new QLabel(QStringLiteral("<h2>Power</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Writes org.spike.Config power + /etc/systemd/logind.conf.d/99-spike-power.conf. "
          "Lid / power button / idle actions apply on next reboot (do not restart logind "
          "while a desktop session is running). "
          "Screen dimming and charge limits land later (POWER-MANAGEMENT.md)."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *blockSleep = new QCheckBox(
      QStringLiteral("Manually block sleep and screen locking (this session)"), w);
  blockSleep->setToolTip(
      QStringLiteral("Plasma-equivalent: logind inhibit for idle sleep and idle lock. "
                     "Also available in the battery tray popup."));
  blockSleep->setChecked(SleepInhibit::instance().isActive());
  lay->addWidget(blockSleep);
  QObject::connect(blockSleep, &QCheckBox::toggled, outer, [blockSleep, statusBar](bool on) {
    QString err;
    if (!SleepInhibit::instance().setActive(on, &err)) {
      blockSleep->blockSignals(true);
      blockSleep->setChecked(false);
      blockSleep->blockSignals(false);
      QMessageBox::warning(blockSleep, QStringLiteral("Power"),
                           QStringLiteral("Could not block sleep/locking:\n%1").arg(err));
      return;
    }
    if (statusBar) {
      statusBar->setText(on ? QStringLiteral("Blocking sleep and screen locking")
                            : QStringLiteral("Idle sleep/lock allowed"));
    }
  });
  QObject::connect(&SleepInhibit::instance(), &SleepInhibit::changed, outer,
                   [blockSleep](bool active) {
                     blockSleep->blockSignals(true);
                     blockSleep->setChecked(active);
                     blockSleep->blockSignals(false);
                   });

  auto *statusHead = new QLabel(QStringLiteral("<b>Current status</b>"), w);
  lay->addWidget(statusHead);
  auto *liveStatus = new QLabel(w);
  liveStatus->setWordWrap(true);
  liveStatus->setObjectName(QStringLiteral("PowerLiveStatus"));
  lay->addWidget(liveStatus);

  auto *form = new QFormLayout();

  auto *profile = new QComboBox(w);
  profile->addItem(QStringLiteral("Adaptive (default)"), QStringLiteral("adaptive"));
  profile->addItem(QStringLiteral("Performance"), QStringLiteral("performance"));
  profile->addItem(QStringLiteral("Battery Saver"), QStringLiteral("battery_saver"));
  profile->addItem(QStringLiteral("Critical"), QStringLiteral("critical"));
  form->addRow(QStringLiteral("Profile"), profile);

  auto *blank = new QComboBox(w);
  blank->addItem(QStringLiteral("2 minutes"), 2);
  blank->addItem(QStringLiteral("5 minutes"), 5);
  blank->addItem(QStringLiteral("10 minutes"), 10);
  blank->addItem(QStringLiteral("15 minutes"), 15);
  blank->addItem(QStringLiteral("30 minutes"), 30);
  blank->addItem(QStringLiteral("1 hour"), 60);
  blank->addItem(QStringLiteral("Never (idle suspend off)"), 0);
  form->addRow(QStringLiteral("Screen blank / idle suspend"), blank);

  auto *dimBefore = new QCheckBox(QStringLiteral("Dim before blank (saved; compositor later)"), w);
  form->addRow(QString(), dimBefore);

  auto *allowSuspend = new QCheckBox(QStringLiteral("Allow suspend"), w);
  auto *allowHibernate = new QCheckBox(QStringLiteral("Allow hibernate"), w);
  auto *hybrid = new QCheckBox(QStringLiteral("Prefer hybrid sleep when available"), w);
  form->addRow(QString(), allowSuspend);
  form->addRow(QString(), allowHibernate);
  form->addRow(QString(), hybrid);

  auto *lidBat = new QComboBox(w);
  fillActionCombo(lidBat);
  form->addRow(QStringLiteral("Lid close (on battery)"), lidBat);

  auto *lidAc = new QComboBox(w);
  fillActionCombo(lidAc);
  form->addRow(QStringLiteral("Lid close (on AC)"), lidAc);

  auto *powerBtn = new QComboBox(w);
  fillActionCombo(powerBtn);
  form->addRow(QStringLiteral("Power button"), powerBtn);

  auto *governor = new QComboBox(w);
  governor->addItem(QStringLiteral("powersave"), QStringLiteral("powersave"));
  governor->addItem(QStringLiteral("schedutil"), QStringLiteral("schedutil"));
  governor->addItem(QStringLiteral("performance"), QStringLiteral("performance"));
  form->addRow(QStringLiteral("CPU governor"), governor);

  auto *wifiSave = new QComboBox(w);
  wifiSave->addItem(QStringLiteral("Adaptive"), QStringLiteral("adaptive"));
  wifiSave->addItem(QStringLiteral("On"), QStringLiteral("on"));
  wifiSave->addItem(QStringLiteral("Off"), QStringLiteral("off"));
  form->addRow(QStringLiteral("Wi‑Fi power saving"), wifiSave);

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

  scroll->setWidget(w);
  outerLay->addWidget(scroll, 1);

  auto *battery = new BatteryClient(outer);
  auto refreshLive = [battery, liveStatus]() {
    battery->refresh();
    if (!battery->hasBattery()) {
      liveStatus->setText(QStringLiteral("Power source: AC (no battery detected via UPower)"));
      return;
    }
    liveStatus->setText(
        QStringLiteral("Battery: %1% — %2")
            .arg(battery->percentage())
            .arg(battery->stateText()));
  };
  refreshLive();
  auto *timer = new QTimer(outer);
  timer->setInterval(5000);
  QObject::connect(timer, &QTimer::timeout, outer, refreshLive);
  timer->start();
  QObject::connect(battery, &BatteryClient::changed, outer, refreshLive);

  auto load = [config, profile, blank, dimBefore, allowSuspend, allowHibernate, hybrid, lidBat,
               lidAc, powerBtn, governor, wifiSave, status, statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("power"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    setComboData(profile, o.value(QStringLiteral("profile")).toString(QStringLiteral("adaptive")));
    const int mins = o.value(QStringLiteral("screen_blank_minutes")).toInt(15);
    const int bIdx = blank->findData(mins);
    blank->setCurrentIndex(bIdx >= 0 ? bIdx : blank->findData(15));
    dimBefore->setChecked(o.value(QStringLiteral("dim_before_blank")).toBool(true));
    allowSuspend->setChecked(o.value(QStringLiteral("allow_suspend")).toBool(true));
    allowHibernate->setChecked(o.value(QStringLiteral("allow_hibernate")).toBool(false));
    hybrid->setChecked(o.value(QStringLiteral("hybrid_sleep")).toBool(true));
    setComboData(lidBat,
                 o.value(QStringLiteral("lid_close_action")).toString(QStringLiteral("suspend")));
    setComboData(lidAc, o.value(QStringLiteral("lid_close_action_ac"))
                            .toString(o.value(QStringLiteral("lid_close_action"))
                                          .toString(QStringLiteral("suspend"))));
    setComboData(powerBtn, o.value(QStringLiteral("power_button_action"))
                               .toString(QStringLiteral("suspend")));
    setComboData(governor,
                 o.value(QStringLiteral("cpu_governor")).toString(QStringLiteral("powersave")));
    setComboData(wifiSave, o.value(QStringLiteral("wifi_power_saving"))
                               .toString(QStringLiteral("adaptive")));
    status->setText(QStringLiteral("Loaded power module"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Power loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, outer, load);
  // Profile drives live governor / Wi‑Fi powersave defaults (POWER-MANAGEMENT.md).
  QObject::connect(profile, &QComboBox::currentIndexChanged, outer, [profile, governor, wifiSave]() {
    const QString p = profile->currentData().toString();
    if (p == QLatin1String("performance")) {
      setComboData(governor, QStringLiteral("performance"));
      setComboData(wifiSave, QStringLiteral("off"));
    } else if (p == QLatin1String("battery_saver") || p == QLatin1String("critical")) {
      setComboData(governor, QStringLiteral("powersave"));
      setComboData(wifiSave, QStringLiteral("on"));
    } else {
      setComboData(governor, QStringLiteral("powersave"));
      setComboData(wifiSave, QStringLiteral("adaptive"));
    }
  });
  QObject::connect(
      apply, &QPushButton::clicked, outer,
      [config, profile, blank, dimBefore, allowSuspend, allowHibernate, hybrid, lidBat, lidAc,
       powerBtn, governor, wifiSave, status, statusBar]() {
        if (!config) {
          return;
        }
        QString err;
        auto set = [&](const QString &key, const QVariant &v) -> bool {
          if (!config->setSetting(QStringLiteral("power"), key, v, &err)) {
            status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
            return false;
          }
          return true;
        };
        if (!set(QStringLiteral("profile"), profile->currentData()) ||
            !set(QStringLiteral("screen_blank_minutes"), blank->currentData()) ||
            !set(QStringLiteral("dim_before_blank"), dimBefore->isChecked()) ||
            !set(QStringLiteral("allow_suspend"), allowSuspend->isChecked()) ||
            !set(QStringLiteral("allow_hibernate"), allowHibernate->isChecked()) ||
            !set(QStringLiteral("hybrid_sleep"), hybrid->isChecked()) ||
            !set(QStringLiteral("lid_close_action"), lidBat->currentData()) ||
            !set(QStringLiteral("lid_close_action_ac"), lidAc->currentData()) ||
            !set(QStringLiteral("power_button_action"), powerBtn->currentData()) ||
            !set(QStringLiteral("cpu_governor"), governor->currentData()) ||
            !set(QStringLiteral("wifi_power_saving"), wifiSave->currentData())) {
          return;
        }

        // spike-config-dbus (root) applies governor + Wi‑Fi powersave on SetSetting.
        const QString gov = governor->currentData().toString();
        (void)applyGovernorLive(gov); // optional; root DBus path is authoritative
        status->setText(QStringLiteral(
            "Saved. Profile/CPU/Wi‑Fi applied live via spike-config. "
            "Lid/power-button/idle suspend still need a reboot if those changed (logind)."));
        if (statusBar) {
          statusBar->setText(QStringLiteral("Power applied live"));
        }
      });

  load();
  return outer;
}

} // namespace spike
