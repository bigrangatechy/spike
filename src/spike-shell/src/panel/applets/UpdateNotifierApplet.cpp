#include "panel/applets/UpdateNotifierApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

UpdateNotifierApplet::UpdateNotifierApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("UpdateNotifierApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &UpdateNotifierApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(260);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Updates"), m_popup));
  m_detail = new QLabel(m_popup);
  m_detail->setWordWrap(true);
  lay->addWidget(m_detail);
  auto *open = new QPushButton(QStringLiteral("Open Updates"), m_popup);
  lay->addWidget(open);
  connect(open, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("updates"));
  });

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &UpdateNotifierApplet::refresh);
  timer->start(15 * 60 * 1000);
  QTimer::singleShot(8000, this, &UpdateNotifierApplet::refresh);
  hide();
}

void UpdateNotifierApplet::setEnabledVisible(bool on)
{
  m_userVisible = on;
  if (!on) {
    hide();
  } else {
    refresh();
  }
}

int UpdateNotifierApplet::countUpgradable() const
{
  // Simulate upgrade without root; count "Inst" lines from apt-get -s.
  QProcess proc;
  proc.start(QStringLiteral("apt-get"),
             {QStringLiteral("-s"), QStringLiteral("-o"), QStringLiteral("Debug::NoLocking=1"),
              QStringLiteral("upgrade")});
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(60000)) {
    proc.kill();
    return 0;
  }
  const QString out = QString::fromUtf8(proc.readAllStandardOutput());
  int n = 0;
  for (const QString &line : out.split(QLatin1Char('\n'))) {
    if (line.startsWith(QLatin1String("Inst "))) {
      ++n;
    }
  }
  return n;
}

void UpdateNotifierApplet::refresh()
{
  if (!m_userVisible) {
    hide();
    return;
  }
  m_count = countUpgradable();
  if (m_count <= 0) {
    hide();
    return;
  }
  show();
  tray::setTrayIcon(this, QStringLiteral("system-software-update"),
                    QStringLiteral("U%1").arg(m_count));
  setToolTip(QStringLiteral("%1 package update(s) available").arg(m_count));
  if (m_detail) {
    m_detail->setText(QStringLiteral("%1 upgradable package(s) (simulated apt-get -s).").arg(m_count));
  }
}

void UpdateNotifierApplet::togglePopup()
{
  if (!m_popup || isHidden()) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  tray::placePopupAbove(this, m_popup);
  m_popup->show();
}

} // namespace spike
