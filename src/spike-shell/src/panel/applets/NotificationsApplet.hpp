#pragma once

#include <QPushButton>

class QListWidget;
class QWidget;

namespace spike {

class NotificationDaemon;

class NotificationsApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit NotificationsApplet(NotificationDaemon *daemon, QWidget *parent = nullptr);

public slots:
  void setEnabledVisible(bool on);

private slots:
  void refresh();
  void togglePopup();

private:
  NotificationDaemon *m_daemon = nullptr;
  QWidget *m_popup = nullptr;
  QListWidget *m_list = nullptr;
  bool m_userVisible = true;
};

} // namespace spike
