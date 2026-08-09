#include "panel/applets/KeyboardLayoutApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

KeyboardLayoutApplet::KeyboardLayoutApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("KeyboardLayoutApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  connect(this, &QPushButton::clicked, this, &KeyboardLayoutApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(240);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Keyboard layout"), m_popup));
  m_detail = new QLabel(m_popup);
  lay->addWidget(m_detail);
  auto *cycleBtn = new QPushButton(QStringLiteral("Next layout"), m_popup);
  auto *settings = new QPushButton(QStringLiteral("Keyboard Layout Settings"), m_popup);
  lay->addWidget(cycleBtn);
  lay->addWidget(settings);
  connect(cycleBtn, &QPushButton::clicked, this, &KeyboardLayoutApplet::cycle);
  connect(settings, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("keyboard-layout"));
  });

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &KeyboardLayoutApplet::refresh);
  timer->start(10000);
  refresh();
}

void KeyboardLayoutApplet::refresh()
{
  m_layouts.clear();
  QSettings kx(QStringLiteral("kxkbrc"), QSettings::IniFormat);
  kx.beginGroup(QStringLiteral("Layout"));
  const QString layout = kx.value(QStringLiteral("LayoutList")).toString();
  if (!layout.isEmpty()) {
    m_layouts = layout.split(QLatin1Char(','), Qt::SkipEmptyParts);
  }
  if (m_layouts.isEmpty()) {
    QProcess proc;
    proc.start(QStringLiteral("setxkbmap"), {QStringLiteral("-query")});
    if (proc.waitForFinished(2000)) {
      const QString out = QString::fromUtf8(proc.readAllStandardOutput());
      for (const QString &line : out.split(QLatin1Char('\n'))) {
        if (line.startsWith(QLatin1String("layout:"))) {
          const QString v = line.section(QLatin1Char(':'), 1).trimmed();
          m_layouts = v.split(QLatin1Char(','), Qt::SkipEmptyParts);
        }
      }
    }
  }
  if (m_layouts.size() < 2) {
    hide();
    return;
  }
  show();
  if (m_index < 0 || m_index >= m_layouts.size()) {
    m_index = 0;
  }
  const QString cur = m_layouts.at(m_index).trimmed().toUpper();
  setText(cur.left(3));
  setIcon(QIcon());
  setToolTip(QStringLiteral("Layout: %1").arg(m_layouts.join(QStringLiteral(", "))));
  if (m_detail) {
    m_detail->setText(QStringLiteral("Current: %1\nConfigured: %2")
                          .arg(m_layouts.at(m_index), m_layouts.join(QStringLiteral(", "))));
  }
}

void KeyboardLayoutApplet::cycle()
{
  if (m_layouts.size() < 2) {
    return;
  }
  m_index = (m_index + 1) % m_layouts.size();
  QProcess::execute(QStringLiteral("setxkbmap"), {m_layouts.at(m_index).trimmed()});
  refresh();
}

void KeyboardLayoutApplet::togglePopup()
{
  if (m_layouts.size() < 2 || !m_popup) {
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
