#include "shortcuts/OsdPopup.hpp"

#include <LayerShellQt/Window>

#include <QGuiApplication>
#include <QLabel>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>

namespace spike {

OsdPopup::OsdPopup(QWidget *parent)
  : QWidget(parent, Qt::FramelessWindowHint | Qt::Window | Qt::WindowDoesNotAcceptFocus)
{
  setObjectName(QStringLiteral("SpikeOsdPopup"));
  setAttribute(Qt::WA_ShowWithoutActivating);
  setAttribute(Qt::WA_TranslucentBackground);
  setFocusPolicy(Qt::NoFocus);

  auto *lay = new QVBoxLayout(this);
  lay->setContentsMargins(0, 12, 0, 12);
  lay->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  m_label = new QLabel(this);
  m_label->setAlignment(Qt::AlignCenter);
  m_label->setMargin(16);
  m_label->setStyleSheet(
      QStringLiteral("QLabel { background: rgba(20,20,28,220); color: #f2f2f7; "
                     "border-radius: 12px; font-size: 18px; padding: 14px 22px; }"));
  lay->addWidget(m_label, 0, Qt::AlignHCenter);

  m_hide = new QTimer(this);
  m_hide->setSingleShot(true);
  connect(m_hide, &QTimer::timeout, this, &QWidget::hide);
}

void OsdPopup::applyLayerShell()
{
  createWinId();
  QWindow *win = windowHandle();
  if (!win) {
    return;
  }
  QScreen *screen = QGuiApplication::primaryScreen();
  const int barH = qMax(96, sizeHint().height());
  LayerShellQt::Window *layer = LayerShellQt::Window::get(win);
  if (!layer) {
    if (screen) {
      const QRect geo = screen->geometry();
      setGeometry(geo.left(), geo.top() + 48, geo.width(), barH);
    }
    return;
  }
  using LS = LayerShellQt::Window;
  layer->setScope(QStringLiteral("spike-osd"));
  layer->setLayer(LS::LayerOverlay);
  // Full-width top strip; label is centred inside (layer-shell has no “centre” anchor).
  LS::Anchors anchors = LS::Anchors(LS::AnchorTop) | LS::AnchorLeft | LS::AnchorRight;
  layer->setAnchors(anchors);
  layer->setMargins(QMargins(0, 48, 0, 0));
  layer->setExclusiveZone(0);
  layer->setKeyboardInteractivity(LS::KeyboardInteractivityNone);
  layer->setActivateOnShow(false);
  if (screen) {
    layer->setScreen(screen);
    layer->setDesiredSize(QSize(screen->geometry().width(), barH));
    setFixedHeight(barH);
  }
}

void OsdPopup::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  applyLayerShell();
}

void OsdPopup::showMessage(const QString &summary, const QString &body, int ms)
{
  m_label->setText(QStringLiteral("%1\n%2").arg(summary, body));
  m_label->adjustSize();
  adjustSize();
  applyLayerShell();
  show();
  raise();
  m_hide->start(ms);
}

} // namespace spike
