#include "settings/NotificationsPage.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
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

QWidget *makeNotificationsPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Notifications</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Retention and Do Not Disturb preferences are saved in spike-config (privacy). "
          "The panel Notifications tray owns org.freedesktop.Notifications when no other "
          "daemon is present (history + badge)."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *dnd = new QCheckBox(QStringLiteral("Do Not Disturb"), w);
  form->addRow(QString(), dnd);

  auto *sound = new QCheckBox(QStringLiteral("Play notification sound"), w);
  form->addRow(QString(), sound);

  auto *retention = new QSpinBox(w);
  retention->setRange(1, 31);
  retention->setSuffix(QStringLiteral(" days"));
  form->addRow(QStringLiteral("History retention"), retention);

  auto *maxCount = new QSpinBox(w);
  maxCount->setRange(10, 1000);
  maxCount->setSingleStep(10);
  form->addRow(QStringLiteral("Max history entries"), maxCount);

  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  auto *test = new QPushButton(QStringLiteral("Send test notification"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addWidget(test);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  auto load = [config, dnd, sound, retention, maxCount, status, statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("privacy"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    dnd->setChecked(o.value(QStringLiteral("notification_dnd")).toBool(false));
    sound->setChecked(o.value(QStringLiteral("notification_sound")).toBool(true));
    retention->setValue(o.value(QStringLiteral("notification_retention_days")).toInt(3));
    maxCount->setValue(o.value(QStringLiteral("notification_max_count")).toInt(500));
    status->setText(QStringLiteral("Loaded privacy notification prefs"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Notifications loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w,
                   [config, dnd, sound, retention, maxCount, status, statusBar, load]() {
                     if (!config) {
                       return;
                     }
                     QString err;
                     auto set = [&](const QString &key, const QVariant &v) -> bool {
                       if (!config->setSetting(QStringLiteral("privacy"), key, v, &err)) {
                         status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
                         return false;
                       }
                       return true;
                     };
                     if (!set(QStringLiteral("notification_dnd"), dnd->isChecked()) ||
                         !set(QStringLiteral("notification_sound"), sound->isChecked()) ||
                         !set(QStringLiteral("notification_retention_days"), retention->value()) ||
                         !set(QStringLiteral("notification_max_count"), maxCount->value())) {
                       return;
                     }
                     status->setText(QStringLiteral(
                         "Saved. Daemon will honour these once shipped; DND/sound are prefs only "
                         "for now."));
                     if (statusBar) {
                       statusBar->setText(QStringLiteral("Notifications applied"));
                     }
                     load();
                   });
  QObject::connect(test, &QPushButton::clicked, w, [status, statusBar]() {
    QDBusInterface iface(QStringLiteral("org.freedesktop.Notifications"),
                         QStringLiteral("/org/freedesktop/Notifications"),
                         QStringLiteral("org.freedesktop.Notifications"),
                         QDBusConnection::sessionBus());
    if (!iface.isValid()) {
      status->setText(QStringLiteral(
          "No org.freedesktop.Notifications owner (Spike daemon not running). Prefs still save."));
      return;
    }
    // Notify(app_name, replaces_id, app_icon, summary, body, actions, hints, timeout)
    QDBusMessage reply = iface.call(
        QStringLiteral("Notify"), QStringLiteral("Spike"), uint(0),
        QStringLiteral("preferences-desktop-notification"), QStringLiteral("Spike test"),
        QStringLiteral("If you see this, a notification daemon is answering on the session bus."),
        QStringList{}, QVariantMap{}, int(5000));
    if (reply.type() == QDBusMessage::ErrorMessage) {
      status->setText(QStringLiteral("Notify failed: %1").arg(reply.errorMessage()));
      return;
    }
    status->setText(QStringLiteral("Test notification sent"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Test notification sent"));
    }
  });

  load();
  return w;
}

} // namespace spike
