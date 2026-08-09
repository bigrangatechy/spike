#pragma once

#include <QDBusAbstractAdaptor>
#include <QStringList>
#include <QVariantMap>

namespace spike {

class NotificationDaemon;

class NotificationAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
  explicit NotificationAdaptor(NotificationDaemon *parent);

public slots:
  QStringList GetCapabilities();
  QString GetServerInformation(QString &vendor, QString &version, QString &spec_version);
  uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon,
              const QString &summary, const QString &body, const QStringList &actions,
              const QVariantMap &hints, int expire_timeout);
  void CloseNotification(uint id);

signals:
  void NotificationClosed(uint id, uint reason);
  void ActionInvoked(uint id, const QString &action_key);

private:
  NotificationDaemon *m_daemon = nullptr;
};

} // namespace spike
