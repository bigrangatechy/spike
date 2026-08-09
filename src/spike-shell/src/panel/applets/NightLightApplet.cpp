#include "panel/applets/NightLightApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

NightLightApplet::NightLightApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("NightLightApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &NightLightApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(260);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Night Light"), m_popup));
  m_toggle = new QCheckBox(QStringLiteral("Enable Night Light"), m_popup);
  lay->addWidget(m_toggle);
  m_temp = new QSlider(Qt::Horizontal, m_popup);
  m_temp->setRange(3000, 6500);
  m_temp->setValue(m_temperature);
  m_tempLabel = new QLabel(m_popup);
  lay->addWidget(m_temp);
  lay->addWidget(m_tempLabel);
  auto *settings = new QPushButton(QStringLiteral("Night Light Settings"), m_popup);
  lay->addWidget(settings);
  connect(m_toggle, &QCheckBox::toggled, this, &NightLightApplet::onToggle);
  connect(m_temp, &QSlider::valueChanged, this, &NightLightApplet::onTemp);
  connect(settings, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("night-light"));
  });
  refresh();
}

void NightLightApplet::setEnabledVisible(bool on)
{
  m_userVisible = on;
  setVisible(on);
}

void NightLightApplet::applyFromConfig(bool enabled, int temperature)
{
  m_enabled = enabled;
  m_temperature = qBound(3000, temperature, 6500);
  tryKwinSet(m_enabled, m_temperature);
  refresh();
}

bool NightLightApplet::tryKwinSet(bool enabled, int temperature)
{
  // KWin Night Color manager (best-effort; may no-op on older builds).
  QDBusInterface night(QStringLiteral("org.kde.KWin"), QStringLiteral("/ColorCorrect"),
                       QStringLiteral("org.kde.kwin.ColorCorrect"),
                       QDBusConnection::sessionBus());
  if (night.isValid()) {
    night.setProperty("enabled", enabled);
    night.setProperty("nightTemperature", temperature);
    return true;
  }
  QDBusMessage msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.kde.KWin.NightLight"), QStringLiteral("/org/kde/KWin/NightLight"),
      QStringLiteral("org.kde.KWin.NightLight"), QStringLiteral("setEnabled"));
  msg << enabled;
  QDBusConnection::sessionBus().call(msg);
  Q_UNUSED(temperature);
  return false;
}

void NightLightApplet::refresh()
{
  if (!m_userVisible) {
    hide();
    return;
  }
  show();
  if (m_toggle) {
    m_toggle->blockSignals(true);
    m_toggle->setChecked(m_enabled);
    m_toggle->blockSignals(false);
  }
  if (m_temp) {
    m_temp->blockSignals(true);
    m_temp->setValue(m_temperature);
    m_temp->blockSignals(false);
  }
  if (m_tempLabel) {
    m_tempLabel->setText(QStringLiteral("%1 K").arg(m_temperature));
  }
  tray::setTrayIcon(this,
                    m_enabled ? QStringLiteral("redshift-status-on")
                              : QStringLiteral("redshift-status-off"),
                    m_enabled ? QStringLiteral("NL") : QStringLiteral("nl"));
  setToolTip(m_enabled ? QStringLiteral("Night Light on (%1 K)").arg(m_temperature)
                       : QStringLiteral("Night Light off"));
}

void NightLightApplet::onToggle(bool on)
{
  m_enabled = on;
  tryKwinSet(m_enabled, m_temperature);
  refresh();
}

void NightLightApplet::onTemp(int kelvin)
{
  m_temperature = kelvin;
  if (m_tempLabel) {
    m_tempLabel->setText(QStringLiteral("%1 K").arg(kelvin));
  }
  if (m_enabled) {
    tryKwinSet(true, kelvin);
  }
}

void NightLightApplet::togglePopup()
{
  if (!m_popup) {
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
