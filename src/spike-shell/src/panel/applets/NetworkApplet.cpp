#include "panel/applets/NetworkApplet.hpp"

#include "network/NetworkPanelWidget.hpp"
#include "network/NmClient.hpp"

#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString barsForStrength(int strength)
{
  if (strength < 0) {
    return QStringLiteral("Net");
  }
  if (strength >= 80) {
    return QStringLiteral("WiFi ▮▮▮▮");
  }
  if (strength >= 55) {
    return QStringLiteral("WiFi ▮▮▮▯");
  }
  if (strength >= 30) {
    return QStringLiteral("WiFi ▮▮▯▯");
  }
  if (strength >= 10) {
    return QStringLiteral("WiFi ▮▯▯▯");
  }
  return QStringLiteral("WiFi ▯▯▯▯");
}

} // namespace

NetworkApplet::NetworkApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("NetworkApplet"));
  setFlat(true);
  setFixedHeight(26);
  setCursor(Qt::PointingHandCursor);

  m_nm = new NmClient(this);
  connect(m_nm, &NmClient::changed, this, &NetworkApplet::refresh);
  connect(this, &QPushButton::clicked, this, &NetworkApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setObjectName(QStringLiteral("NetworkPopup"));
  m_popup->setMinimumWidth(320);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(0, 0, 0, 0);
  auto *panel = new NetworkPanelWidget(m_nm, m_popup);
  lay->addWidget(panel);
  connect(panel, &NetworkPanelWidget::openFullSettings, this,
          &NetworkApplet::openNetworkSettings);

  refresh();
}

void NetworkApplet::refresh()
{
  if (!m_nm) {
    setText(QStringLiteral("Net"));
    setToolTip(QStringLiteral("NetworkManager unavailable"));
    return;
  }
  QString err;
  const QString kind = m_nm->statusKind(&err);
  const QString label = m_nm->statusLabel(&err);
  setToolTip(label);

  if (kind == QLatin1String("ethernet")) {
    setText(QStringLiteral("Eth"));
  } else if (kind == QLatin1String("wifi")) {
    setText(barsForStrength(m_nm->wifiStrength()));
  } else if (kind == QLatin1String("disabled")) {
    setText(QStringLiteral("WiFi ✕"));
  } else {
    setText(QStringLiteral("Net"));
  }
}

void NetworkApplet::placePopup()
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

void NetworkApplet::togglePopup()
{
  if (!m_popup) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  placePopup();
  m_popup->show();
  m_popup->raise();
  m_popup->activateWindow();
}

void NetworkApplet::openNetworkSettings()
{
  if (m_popup) {
    m_popup->hide();
  }
  QWidget *w = parentWidget();
  while (w) {
    if (w->objectName() == QLatin1String("SpikePanel")) {
      QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection,
                                Q_ARG(QString, QStringLiteral("network")));
      break;
    }
    w = w->parentWidget();
  }
}

} // namespace spike
