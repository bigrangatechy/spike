#include "panel/applets/NightLightApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString kwinrcPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/kwinrc");
}

void writeNightColorConfig(bool enabled, int temperature)
{
  QSettings s(kwinrcPath(), QSettings::IniFormat);
  s.beginGroup(QStringLiteral("NightColor"));
  s.setValue(QStringLiteral("Active"), enabled);
  // Constant = always use NightTemperature while Active (not schedule).
  s.setValue(QStringLiteral("Mode"), QStringLiteral("Constant"));
  s.setValue(QStringLiteral("NightTemperature"), temperature);
  s.setValue(QStringLiteral("DayTemperature"), 6500);
  s.endGroup();
  s.beginGroup(QStringLiteral("Plugins"));
  s.setValue(QStringLiteral("nightlightEnabled"), true);
  s.endGroup();
  s.sync();
}

bool reconfigureKwin()
{
  QDBusInterface kwin(QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
                      QStringLiteral("org.kde.KWin"), QDBusConnection::sessionBus());
  if (!kwin.isValid()) {
    return false;
  }
  kwin.call(QStringLiteral("reconfigure"));
  return true;
}

QDBusInterface nightLightIface()
{
  return QDBusInterface(QStringLiteral("org.kde.KWin"),
                        QStringLiteral("/org/kde/KWin/NightLight"),
                        QStringLiteral("org.kde.KWin.NightLight"),
                        QDBusConnection::sessionBus());
}

} // namespace

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
  // KWin 6 NightLight D-Bus is mostly read-only; enablement lives in kwinrc
  // [NightColor]. preview() applies temperature immediately while Active.
  writeNightColorConfig(enabled, temperature);
  reconfigureKwin();

  QDBusInterface night = nightLightIface();
  if (!night.isValid()) {
    // Plugin may need a moment after reconfigure; still wrote config.
    return false;
  }

  if (enabled) {
    QDBusMessage preview = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"), QStringLiteral("/org/kde/KWin/NightLight"),
        QStringLiteral("org.kde.KWin.NightLight"), QStringLiteral("preview"));
    preview << static_cast<uint>(temperature);
    QDBusConnection::sessionBus().call(preview);
  } else {
    QDBusMessage stop = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"), QStringLiteral("/org/kde/KWin/NightLight"),
        QStringLiteral("org.kde.KWin.NightLight"), QStringLiteral("stopPreview"));
    QDBusConnection::sessionBus().call(stop);
  }
  return true;
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
