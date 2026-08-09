#include "notify/NotificationDaemon.hpp"

#include "notify/NotificationAdaptor.hpp"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDateTime>
#include <QtDBus>

namespace spike {

NotificationDaemon::NotificationDaemon(QObject *parent)
  : QObject(parent)
{
  qDBusRegisterMetaType<QVariantMap>();
}

bool NotificationDaemon::registerService()
{
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) {
    return false;
  }
  if (!bus.registerService(QStringLiteral("org.freedesktop.Notifications"))) {
    return false;
  }
  new NotificationAdaptor(this);
  if (!bus.registerObject(QStringLiteral("/org/freedesktop/Notifications"), this)) {
    bus.unregisterService(QStringLiteral("org.freedesktop.Notifications"));
    return false;
  }
  return true;
}

QStringList NotificationDaemon::GetCapabilities()
{
  return {QStringLiteral("body"), QStringLiteral("actions"), QStringLiteral("persistence")};
}

QString NotificationDaemon::GetServerInformation(QString &vendor, QString &version,
                                                 QString &specVersion)
{
  vendor = QStringLiteral("Spike");
  version = QStringLiteral("0.0.27");
  specVersion = QStringLiteral("1.2");
  return QStringLiteral("spike-shell");
}

uint NotificationDaemon::Notify(const QString &appName, uint replacesId, const QString & /*appIcon*/,
                                const QString &summary, const QString &body,
                                const QStringList & /*actions*/, const QVariantMap & /*hints*/,
                                int /*expireTimeout*/)
{
  if (m_dnd) {
    // Still record for history when DND is on (DESKTOP.md).
  }
  uint id = replacesId ? replacesId : m_nextId++;
  if (!replacesId) {
    // keep m_nextId advancing
  } else if (id >= m_nextId) {
    m_nextId = id + 1;
  }

  NotificationItem item;
  item.id = id;
  item.appName = appName;
  item.summary = summary;
  item.body = body;
  item.timestampMs = QDateTime::currentMSecsSinceEpoch();

  // Replace existing
  for (int i = 0; i < m_history.size(); ++i) {
    if (m_history[i].id == id) {
      m_history[i] = item;
      if (!m_dnd) {
        ++m_unread;
      }
      emit changed();
      return id;
    }
  }
  m_history.prepend(item);
  while (m_history.size() > kMaxHistory) {
    m_history.removeLast();
  }
  if (!m_dnd) {
    ++m_unread;
  }
  emit changed();
  return id;
}

void NotificationDaemon::CloseNotification(uint id)
{
  for (int i = 0; i < m_history.size(); ++i) {
    if (m_history[i].id == id) {
      m_history.removeAt(i);
      emit NotificationClosed(id, 3);
      emit changed();
      return;
    }
  }
}

void NotificationDaemon::clearHistory()
{
  m_history.clear();
  m_unread = 0;
  emit changed();
}

void NotificationDaemon::markRead()
{
  m_unread = 0;
  emit changed();
}

} // namespace spike
