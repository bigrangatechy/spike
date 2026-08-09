#include "panel/applets/AirplaneModeApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

AirplaneModeApplet::AirplaneModeApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("AirplaneModeApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &AirplaneModeApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(260);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Airplane Mode"), m_popup));
  m_toggle = new QCheckBox(QStringLiteral("Airplane mode"), m_popup);
  lay->addWidget(m_toggle);
  m_detail = new QLabel(QStringLiteral("Turns off Wi‑Fi and Bluetooth (rfkill)."), m_popup);
  m_detail->setWordWrap(true);
  lay->addWidget(m_detail);
  connect(m_toggle, &QCheckBox::toggled, this, &AirplaneModeApplet::onToggle);

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &AirplaneModeApplet::refresh);
  timer->start(5000);
  refresh();
}

void AirplaneModeApplet::refresh()
{
  QProcess proc;
  proc.start(QStringLiteral("rfkill"), {QStringLiteral("list")});
  if (!proc.waitForFinished(3000)) {
    m_hasRfkill = false;
    hide();
    return;
  }
  const QString out = QString::fromUtf8(proc.readAllStandardOutput());
  m_hasRfkill = out.contains(QLatin1String("Soft blocked:"));
  if (!m_hasRfkill) {
    hide();
    return;
  }
  show();
  // Airplane if any wifi/bluetooth soft-blocked
  m_blocked = out.contains(QLatin1String("Soft blocked: yes"));
  if (m_toggle) {
    m_toggle->blockSignals(true);
    m_toggle->setChecked(m_blocked);
    m_toggle->blockSignals(false);
  }
  tray::setTrayIcon(this,
                    m_blocked ? QStringLiteral("network-flightmode-on")
                              : QStringLiteral("network-flightmode-off"),
                    m_blocked ? QStringLiteral("Air") : QStringLiteral("air"));
  setToolTip(m_blocked ? QStringLiteral("Airplane mode on") : QStringLiteral("Airplane mode off"));
}

void AirplaneModeApplet::onToggle(bool on)
{
  QProcess::execute(QStringLiteral("rfkill"),
                    {on ? QStringLiteral("block") : QStringLiteral("unblock"),
                     QStringLiteral("all")});
  refresh();
}

void AirplaneModeApplet::togglePopup()
{
  if (!m_hasRfkill || !m_popup) {
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
