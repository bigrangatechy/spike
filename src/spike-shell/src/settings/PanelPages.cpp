#include "settings/PanelPages.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QJsonObject parseDesktop(ConfigClient *config, QString *err)
{
  if (!config) {
    if (err) {
      *err = QStringLiteral("no config");
    }
    return {};
  }
  const QString json = config->getModuleState(QStringLiteral("desktop"), err);
  if (json.isEmpty()) {
    return {};
  }
  return QJsonDocument::fromJson(json.toUtf8()).object();
}

} // namespace

QWidget *makePanelGeometryPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Panel</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Panel position, height, and auto-hide (org.spike.Config desktop). "
                     "Live-applied by Spike Shell."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *panelPos = new QComboBox(w);
  panelPos->addItem(QStringLiteral("Bottom"), QStringLiteral("bottom"));
  panelPos->addItem(QStringLiteral("Top"), QStringLiteral("top"));
  form->addRow(QStringLiteral("Panel position"), panelPos);

  auto *panelHeight = new QSpinBox(w);
  panelHeight->setRange(24, 48);
  panelHeight->setSuffix(QStringLiteral(" px"));
  form->addRow(QStringLiteral("Panel height"), panelHeight);

  auto *autoHide = new QCheckBox(QStringLiteral("Auto-hide panel"), w);
  form->addRow(QString(), autoHide);

  auto *anim = new QCheckBox(QStringLiteral("Window animations (saved; shell has no anim yet)"), w);
  form->addRow(QString(), anim);
  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto load = [config, panelPos, panelHeight, autoHide, anim, status, statusBar]() {
    QString err;
    const QJsonObject o = parseDesktop(config, &err);
    if (o.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const int idx =
        panelPos->findData(o.value(QStringLiteral("panel_position")).toString(QStringLiteral("bottom")));
    if (idx >= 0) {
      panelPos->setCurrentIndex(idx);
    }
    panelHeight->setValue(o.value(QStringLiteral("panel_height")).toInt(32));
    autoHide->setChecked(o.value(QStringLiteral("panel_auto_hide")).toBool(false));
    anim->setChecked(o.value(QStringLiteral("animations_enabled")).toBool(false));
    status->setText(QStringLiteral("Loaded panel settings"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Panel loaded"));
    }
  };

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, panelPos, panelHeight, autoHide, anim, status, statusBar]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     auto set = [&](const QString &key, const QVariant &v) -> bool {
                       if (!config->setSetting(QStringLiteral("desktop"), key, v, &err)) {
                         status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
                         return false;
                       }
                       return true;
                     };
                     if (!set(QStringLiteral("panel_position"), panelPos->currentData()) ||
                         !set(QStringLiteral("panel_height"), panelHeight->value()) ||
                         !set(QStringLiteral("panel_auto_hide"), autoHide->isChecked()) ||
                         !set(QStringLiteral("animations_enabled"), anim->isChecked())) {
                       return;
                     }
                     status->setText(QStringLiteral("Panel settings saved + applied live."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Panel applied live"));
                     }
                   });
  load();
  return w;
}

QWidget *makeTrayAppletsPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Tray Applets</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Show or hide optional tray applets. Hardware-conditional applets still hide "
          "when the device is absent. Jump: Network, Sound, Power, Bluetooth, Notifications, "
          "Keyboard Layout, Date & Time, Updates."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *showNotifications = new QCheckBox(QStringLiteral("Notifications"), w);
  auto *showNightLight = new QCheckBox(QStringLiteral("Night Light"), w);
  auto *showUpdates = new QCheckBox(QStringLiteral("Update Notifier"), w);
  auto *showWindowList = new QCheckBox(QStringLiteral("Window list"), w);
  lay->addWidget(showNotifications);
  lay->addWidget(showNightLight);
  lay->addWidget(showUpdates);
  lay->addWidget(showWindowList);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto load = [config, showNotifications, showNightLight, showUpdates, showWindowList, status,
               statusBar]() {
    QString err;
    const QJsonObject o = parseDesktop(config, &err);
    if (o.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonValue trayVal = o.value(QStringLiteral("tray_applets"));
    QJsonObject tray;
    if (trayVal.isObject()) {
      tray = trayVal.toObject();
    } else if (trayVal.isString()) {
      tray = QJsonDocument::fromJson(trayVal.toString().toUtf8()).object();
    }
    showNotifications->setChecked(tray.value(QStringLiteral("notifications")).toBool(true));
    showNightLight->setChecked(tray.value(QStringLiteral("night_light")).toBool(true));
    showUpdates->setChecked(tray.value(QStringLiteral("updates")).toBool(true));
    showWindowList->setChecked(tray.value(QStringLiteral("window_list")).toBool(true));
    status->setText(QStringLiteral("Loaded tray applet visibility"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Tray Applets loaded"));
    }
  };

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(
      apply, &QPushButton::clicked, w,
      [config, showNotifications, showNightLight, showUpdates, showWindowList, status, statusBar]() {
        if (!config) {
          return;
        }
        QJsonObject tray;
        tray.insert(QStringLiteral("notifications"), showNotifications->isChecked());
        tray.insert(QStringLiteral("night_light"), showNightLight->isChecked());
        tray.insert(QStringLiteral("updates"), showUpdates->isChecked());
        tray.insert(QStringLiteral("window_list"), showWindowList->isChecked());
        QString err;
        const QString json = QString::fromUtf8(QJsonDocument(tray).toJson(QJsonDocument::Compact));
        if (!config->setSetting(QStringLiteral("desktop"), QStringLiteral("tray_applets"), json,
                                &err)) {
          status->setText(QStringLiteral("Save failed: %1").arg(err));
          return;
        }
        status->setText(QStringLiteral("Tray applet visibility saved + applied live."));
        if (statusBar) {
          statusBar->setText(QStringLiteral("Tray Applets applied live"));
        }
      });
  load();
  return w;
}

QWidget *makeNightLightSettingsPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Night Light</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Enable Night Light and color temperature. Applied live when KWin "
                     "ColorCorrect / NightLight is available."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *enabled = new QCheckBox(QStringLiteral("Enable Night Light"), w);
  form->addRow(QString(), enabled);
  auto *temp = new QSpinBox(w);
  temp->setRange(3000, 6500);
  temp->setSuffix(QStringLiteral(" K"));
  form->addRow(QStringLiteral("Temperature"), temp);
  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto load = [config, enabled, temp, status, statusBar]() {
    QString err;
    const QJsonObject o = parseDesktop(config, &err);
    if (o.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    enabled->setChecked(o.value(QStringLiteral("night_light_enabled")).toBool(false));
    temp->setValue(o.value(QStringLiteral("night_light_temperature")).toInt(4500));
    status->setText(QStringLiteral("Loaded Night Light settings"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Night Light loaded"));
    }
  };

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, enabled, temp, status, statusBar]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     if (!config->setSetting(QStringLiteral("desktop"),
                                             QStringLiteral("night_light_enabled"),
                                             enabled->isChecked(), &err) ||
                         !config->setSetting(QStringLiteral("desktop"),
                                             QStringLiteral("night_light_temperature"),
                                             temp->value(), &err)) {
                       status->setText(QStringLiteral("Save failed: %1").arg(err));
                       return;
                     }
                     status->setText(QStringLiteral("Night Light saved."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Night Light applied"));
                     }
                   });
  load();
  return w;
}

} // namespace spike
