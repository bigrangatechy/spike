#include "panel/applets/NotificationsApplet.hpp"

#include "notify/NotificationDaemon.hpp"
#include "panel/applets/TrayHelpers.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

NotificationsApplet::NotificationsApplet(NotificationDaemon *daemon, QWidget *parent)
  : QPushButton(parent)
  , m_daemon(daemon)
{
  setObjectName(QStringLiteral("NotificationsApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &NotificationsApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(320);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Notifications"), m_popup));
  auto *dnd = new QCheckBox(QStringLiteral("Do Not Disturb"), m_popup);
  lay->addWidget(dnd);
  m_list = new QListWidget(m_popup);
  m_list->setMinimumHeight(180);
  lay->addWidget(m_list, 1);
  auto *clear = new QPushButton(QStringLiteral("Clear all"), m_popup);
  auto *settings = new QPushButton(QStringLiteral("Notification Settings"), m_popup);
  lay->addWidget(clear);
  lay->addWidget(settings);

  connect(dnd, &QCheckBox::toggled, this, [this](bool on) {
    if (m_daemon) {
      m_daemon->setDoNotDisturb(on);
    }
  });
  connect(clear, &QPushButton::clicked, this, [this]() {
    if (m_daemon) {
      m_daemon->clearHistory();
    }
  });
  connect(settings, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("notifications"));
  });
  if (m_daemon) {
    connect(m_daemon, &NotificationDaemon::changed, this, &NotificationsApplet::refresh);
    dnd->setChecked(m_daemon->doNotDisturb());
  }
  refresh();
}

void NotificationsApplet::setEnabledVisible(bool on)
{
  m_userVisible = on;
  setVisible(on);
}

void NotificationsApplet::refresh()
{
  if (!m_userVisible) {
    hide();
    return;
  }
  show();
  const int n = m_daemon ? m_daemon->unreadCount() : 0;
  tray::setTrayIcon(this, QStringLiteral("preferences-desktop-notification"),
                    n > 0 ? QStringLiteral("N%1").arg(n) : QStringLiteral("N"));
  setToolTip(n > 0 ? QStringLiteral("%1 unread notification(s)").arg(n)
                   : QStringLiteral("Notifications"));
  if (m_list && m_daemon) {
    m_list->clear();
    for (const NotificationItem &it : m_daemon->history()) {
      QString line = it.summary;
      if (!it.appName.isEmpty()) {
        line = QStringLiteral("[%1] %2").arg(it.appName, it.summary);
      }
      if (!it.body.isEmpty()) {
        line += QStringLiteral(" — %1").arg(it.body);
      }
      m_list->addItem(line);
    }
  }
}

void NotificationsApplet::togglePopup()
{
  if (!m_popup) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  if (m_daemon) {
    m_daemon->markRead();
  }
  refresh();
  tray::placePopupAbove(this, m_popup);
  m_popup->show();
  m_popup->raise();
}

} // namespace spike
