#include "desktop/DesktopBackground.hpp"

#include "desktop/DesktopIcons.hpp"

#include <LayerShellQt/Window>

#include <QGuiApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QScreen>
#include <QShowEvent>
#include <QWindow>

namespace spike {

DesktopBackground *DesktopBackground::s_instance = nullptr;

DesktopBackground::DesktopBackground(QWidget *parent)
  : QWidget(parent)
{
  s_instance = this;
  setObjectName(QStringLiteral("SpikeDesktopBackground"));
  setWindowTitle(QStringLiteral("Spike Desktop"));
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
  setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);

  m_icons = new DesktopIcons(this);
  layoutIcons();
}

DesktopBackground *DesktopBackground::instance()
{
  return s_instance;
}

void DesktopBackground::applyLayerShell()
{
  winId();
  QWindow *win = windowHandle();
  if (!win) {
    return;
  }
  LayerShellQt::Window *layer = LayerShellQt::Window::get(win);
  if (!layer) {
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
      setGeometry(screen->geometry());
    }
    layoutIcons();
    return;
  }
  using LS = LayerShellQt::Window;
  layer->setScope(QStringLiteral("spike-desktop"));
  // Bottom (above wallpaper-only surfaces) so icons can receive clicks.
  layer->setLayer(LS::LayerBottom);
  LS::Anchors anchors = LS::Anchors(LS::AnchorTop) | LS::AnchorBottom | LS::AnchorLeft |
                        LS::AnchorRight;
  layer->setAnchors(anchors);
  layer->setExclusiveZone(-1);
  // Need pointer input for desktop icons (wallpaper alone used None).
  layer->setKeyboardInteractivity(LS::KeyboardInteractivityOnDemand);
  if (QScreen *screen = QGuiApplication::primaryScreen()) {
    layer->setScreen(screen);
    layer->setDesiredSize(screen->geometry().size());
  }
  layoutIcons();
}

bool DesktopBackground::setImagePath(const QString &path)
{
  m_path = path;
  reloadPixmap();
  update();
  return path.isEmpty() || !m_pixmap.isNull();
}

void DesktopBackground::reloadIcons()
{
  if (m_icons) {
    m_icons->reload();
  }
}

void DesktopBackground::reloadPixmap()
{
  m_pixmap = QPixmap();
  if (m_path.isEmpty()) {
    return;
  }
  m_pixmap.load(m_path);
}

void DesktopBackground::layoutIcons()
{
  if (!m_icons) {
    return;
  }
  m_icons->setGeometry(rect());
  m_icons->raise();
  m_icons->show();
}

void DesktopBackground::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.fillRect(rect(), QColor(QStringLiteral("#1a1a2e")));
  if (m_pixmap.isNull()) {
    return;
  }
  const QPixmap scaled =
      m_pixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  p.drawPixmap(0, 0, scaled);
}

void DesktopBackground::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  applyLayerShell();
  layoutIcons();
}

void DesktopBackground::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  layoutIcons();
}

} // namespace spike
