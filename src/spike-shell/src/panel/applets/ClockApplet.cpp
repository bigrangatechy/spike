#include "panel/applets/ClockApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QCalendarWidget>
#include <QDateTime>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

ClockApplet::ClockApplet(QWidget *parent)
  : QLabel(parent)
{
  setObjectName(QStringLiteral("ClockLabel"));
  setAlignment(Qt::AlignCenter);
  setCursor(Qt::PointingHandCursor);
  setToolTip(QStringLiteral("Clock / calendar"));

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setObjectName(QStringLiteral("ClockPopup"));
  m_popup->setMinimumWidth(280);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  m_largeTime = new QLabel(m_popup);
  m_largeTime->setAlignment(Qt::AlignCenter);
  m_largeTime->setStyleSheet(QStringLiteral("font-size: 22pt; font-weight: 600;"));
  lay->addWidget(m_largeTime);
  m_cal = new QCalendarWidget(m_popup);
  m_cal->setGridVisible(false);
  m_cal->setNavigationBarVisible(true);
  m_cal->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
  lay->addWidget(m_cal);
  auto *settings = new QPushButton(QStringLiteral("Date & Time Settings"), m_popup);
  lay->addWidget(settings);
  connect(settings, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("datetime"));
  });

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ClockApplet::tick);
  timer->start(1000);
  tick();
}

void ClockApplet::tick()
{
  const QDateTime now = QDateTime::currentDateTime();
  setText(now.toString(QStringLiteral("ddd HH:mm")));
  if (m_largeTime) {
    m_largeTime->setText(now.toString(QStringLiteral("dddd\nd MMM yyyy\nHH:mm:ss")));
  }
}

void ClockApplet::placePopup()
{
  tray::placePopupAbove(this, m_popup);
}

void ClockApplet::togglePopup()
{
  if (!m_popup) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  if (m_cal) {
    m_cal->setSelectedDate(QDate::currentDate());
  }
  tick();
  placePopup();
  m_popup->show();
  m_popup->raise();
  m_popup->activateWindow();
}

void ClockApplet::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    togglePopup();
  }
  QLabel::mouseReleaseEvent(event);
}

} // namespace spike
