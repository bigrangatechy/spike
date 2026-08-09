#include "panel/applets/BatteryApplet.hpp"

#include "power/BatteryClient.hpp"

#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

BatteryApplet::BatteryApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("BatteryApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));

  m_client = new BatteryClient(this);
  connect(m_client, &BatteryClient::changed, this, &BatteryApplet::refresh);
  connect(this, &QPushButton::clicked, this, &BatteryApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setObjectName(QStringLiteral("BatteryPopup"));
  m_popup->setMinimumWidth(220);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  auto *title = new QLabel(QStringLiteral("Battery"), m_popup);
  title->setObjectName(QStringLiteral("BatteryTitle"));
  lay->addWidget(title);
  auto *detail = new QLabel(m_popup);
  detail->setObjectName(QStringLiteral("BatteryDetail"));
  detail->setWordWrap(true);
  lay->addWidget(detail);
  auto *powerSettings = new QPushButton(QStringLiteral("Power Settings"), m_popup);
  lay->addWidget(powerSettings);
  connect(powerSettings, &QPushButton::clicked, this, &BatteryApplet::openPowerSettings);

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, m_client, &BatteryClient::refresh);
  timer->start(5000);

  refresh();
}

bool BatteryApplet::hasBattery() const
{
  return m_client && m_client->hasBattery();
}

void BatteryApplet::updateIcon()
{
  if (!m_client) {
    return;
  }
  const QIcon icon = QIcon::fromTheme(m_client->iconName());
  if (!icon.isNull()) {
    setIcon(icon);
    setText(QStringLiteral("%1%").arg(m_client->percentage()));
  } else {
    setIcon(QIcon());
    setText(QStringLiteral("Bat %1%").arg(m_client->percentage()));
  }
}

void BatteryApplet::refresh()
{
  if (!m_client || !m_client->hasBattery()) {
    hide();
    return;
  }
  show();
  QString tip = QStringLiteral("%1 — %2%").arg(m_client->stateText()).arg(m_client->percentage());
  if (!m_client->timeRemainingText().isEmpty()) {
    tip += QStringLiteral("\n%1").arg(m_client->timeRemainingText());
  }
  setToolTip(tip);
  updateIcon();
  if (m_popup) {
    if (auto *detail = m_popup->findChild<QLabel *>(QStringLiteral("BatteryDetail"))) {
      QString body = QStringLiteral("%1\n%2%").arg(m_client->stateText()).arg(m_client->percentage());
      if (!m_client->timeRemainingText().isEmpty()) {
        body += QStringLiteral("\n%1").arg(m_client->timeRemainingText());
      }
      detail->setText(body);
    }
  }
}

void BatteryApplet::placePopup()
{
  if (!m_popup) {
    return;
  }
  m_popup->adjustSize();
  const QPoint global = mapToGlobal(QPoint(width() - m_popup->width(), 0));
  int x = global.x();
  int y = global.y() - m_popup->height() - 4;
  if (QScreen *screen = QApplication::screenAt(mapToGlobal(rect().center()))) {
    const QRect geo = screen->availableGeometry();
    x = qBound(geo.left(), x, geo.right() - m_popup->width());
    if (y < geo.top()) {
      y = mapToGlobal(QPoint(0, height())).y() + 4;
    }
  }
  m_popup->move(x, y);
}

void BatteryApplet::togglePopup()
{
  if (!m_popup || !hasBattery()) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  if (m_client) {
    m_client->refresh();
  }
  placePopup();
  m_popup->show();
  m_popup->raise();
  m_popup->activateWindow();
}

void BatteryApplet::openPowerSettings()
{
  if (m_popup) {
    m_popup->hide();
  }
  QWidget *w = parentWidget();
  while (w) {
    if (w->objectName() == QLatin1String("SpikePanel")) {
      QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection,
                                Q_ARG(QString, QStringLiteral("power")));
      break;
    }
    w = w->parentWidget();
  }
}

} // namespace spike
