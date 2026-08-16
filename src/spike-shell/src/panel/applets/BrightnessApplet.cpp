#include "panel/applets/BrightnessApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"
#include "power/BrightnessClient.hpp"

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

BrightnessApplet::BrightnessApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("BrightnessApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &BrightnessApplet::togglePopup);

  m_client = new BrightnessClient(this);
  connect(m_client, &BrightnessClient::changed, this, &BrightnessApplet::refresh);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(240);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Brightness"), m_popup));
  m_slider = new QSlider(Qt::Horizontal, m_popup);
  m_slider->setRange(1, 100);
  m_pct = new QLabel(m_popup);
  lay->addWidget(m_slider);
  lay->addWidget(m_pct);
  connect(m_slider, &QSlider::valueChanged, this, &BrightnessApplet::onSlider);

  if (!m_client->hasBacklight()) {
    hide();
    return;
  }
  refresh();
}

bool BrightnessApplet::hasBacklight() const
{
  return m_client && m_client->hasBacklight();
}

void BrightnessApplet::refresh()
{
  if (!m_client || !m_client->hasBacklight()) {
    hide();
    return;
  }
  show();
  const int pct = m_client->percentage();
  if (m_slider && !m_slider->isSliderDown()) {
    m_slider->blockSignals(true);
    m_slider->setValue(pct);
    m_slider->blockSignals(false);
  }
  if (m_pct) {
    m_pct->setText(QStringLiteral("%1%").arg(pct));
  }
  tray::setTrayIcon(this, QStringLiteral("display-brightness"), QStringLiteral("%1%").arg(pct));
  setToolTip(QStringLiteral("Brightness %1%").arg(pct));
}

void BrightnessApplet::onSlider(int value)
{
  if (!m_client) {
    return;
  }
  if (!m_client->setPercentage(value) && m_pct) {
    m_pct->setText(QStringLiteral("%1% (could not apply)").arg(value));
    return;
  }
  if (m_pct) {
    m_pct->setText(QStringLiteral("%1%").arg(value));
  }
  tray::setTrayIcon(this, QStringLiteral("display-brightness"), QStringLiteral("%1%").arg(value));
}

void BrightnessApplet::togglePopup()
{
  if (!m_client || !m_client->hasBacklight() || !m_popup) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  refresh();
  tray::placePopupAbove(this, m_popup);
  m_popup->show();
}

} // namespace spike
