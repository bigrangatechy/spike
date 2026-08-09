#include "notify/NotificationAdaptor.hpp"

#include "notify/NotificationDaemon.hpp"

namespace spike {

NotificationAdaptor::NotificationAdaptor(NotificationDaemon *parent)
  : QDBusAbstractAdaptor(parent)
  , m_daemon(parent)
{
  setAutoRelaySignals(true);
}

QStringList NotificationAdaptor::GetCapabilities()
{
  return m_daemon->GetCapabilities();
}

QString NotificationAdaptor::GetServerInformation(QString &vendor, QString &version,
                                                  QString &spec_version)
{
  return m_daemon->GetServerInformation(vendor, version, spec_version);
}

uint NotificationAdaptor::Notify(const QString &app_name, uint replaces_id,
                                 const QString &app_icon, const QString &summary,
                                 const QString &body, const QStringList &actions,
                                 const QVariantMap &hints, int expire_timeout)
{
  return m_daemon->Notify(app_name, replaces_id, app_icon, summary, body, actions, hints,
                          expire_timeout);
}

void NotificationAdaptor::CloseNotification(uint id)
{
  m_daemon->CloseNotification(id);
}

} // namespace spike
