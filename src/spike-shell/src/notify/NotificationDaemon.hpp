#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class QDBusConnection;

namespace spike {

struct NotificationItem {
  uint id = 0;
  QString appName;
  QString summary;
  QString body;
  qint64 timestampMs = 0;
};

/**
 * Minimal org.freedesktop.Notifications on the session bus (in-process).
 * Respects privacy.do_not_disturb via ConfigClient polling from the applet.
 */
class NotificationDaemon : public QObject
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
  explicit NotificationDaemon(QObject *parent = nullptr);

  bool registerService();
  void setDoNotDisturb(bool on) { m_dnd = on; }
  bool doNotDisturb() const { return m_dnd; }

  QVector<NotificationItem> history() const { return m_history; }
  int unreadCount() const { return m_unread; }
  void clearHistory();
  void markRead();

public slots:
  QStringList GetCapabilities();
  QString GetServerInformation(QString &vendor, QString &version, QString &specVersion);
  uint Notify(const QString &appName, uint replacesId, const QString &appIcon,
              const QString &summary, const QString &body, const QStringList &actions,
              const QVariantMap &hints, int expireTimeout);
  void CloseNotification(uint id);

signals:
  void NotificationClosed(uint id, uint reason);
  void ActionInvoked(uint id, const QString &actionKey);
  void changed();

private:
  uint m_nextId = 1;
  bool m_dnd = false;
  int m_unread = 0;
  QVector<NotificationItem> m_history;
  static constexpr int kMaxHistory = 100;
};

} // namespace spike
