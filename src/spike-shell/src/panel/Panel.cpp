#include "panel/Panel.hpp"

#include "launcher/Launcher.hpp"
#include "panel/applets/ClockApplet.hpp"
#include "panel/applets/SessionMenuApplet.hpp"

#include <QHBoxLayout>
#include <QPushButton>

namespace spike {

namespace {
constexpr int kPanelHeight = 32;
} // namespace

Panel::Panel(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("SpikePanel"));
  setFixedHeight(kPanelHeight);
  setWindowTitle(QStringLiteral("Spike Panel"));
  // Frameless toplevel — positioning is done via layer-shell in main.cpp.
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 2, 8, 2);
  layout->setSpacing(8);

  auto *spikeBtn = new QPushButton(QStringLiteral("Spike"), this);
  spikeBtn->setObjectName(QStringLiteral("SpikeButton"));
  spikeBtn->setFixedHeight(kPanelHeight - 6);
  connect(spikeBtn, &QPushButton::clicked, this, &Panel::toggleLauncher);
  layout->addWidget(spikeBtn);

  layout->addStretch(1);

  auto *clock = new ClockApplet(this);
  layout->addWidget(clock);

  auto *session = new SessionMenuApplet(this);
  layout->addWidget(session);

  m_launcher = new Launcher(this);
  m_launcher->hide();
}

void Panel::toggleLauncher()
{
  if (!m_launcher) {
    return;
  }
  if (m_launcher->isVisible()) {
    m_launcher->hide();
    return;
  }

  // Position launcher above the panel, left-aligned.
  const QPoint global = mapToGlobal(QPoint(8, 0));
  m_launcher->move(global.x(), global.y() - m_launcher->height() - 4);
  m_launcher->show();
  m_launcher->raise();
  m_launcher->activateWindow();
}

} // namespace spike
