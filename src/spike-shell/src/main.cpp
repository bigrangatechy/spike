#include "panel/Panel.hpp"

#include <LayerShellQt/Window>

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QScreen>
#include <QWindow>

namespace {

constexpr int kPanelHeight = 32;

void loadStyleSheet(QApplication &app)
{
  QFile file(QStringLiteral(":/styles/spike.qss"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    app.setStyleSheet(QString::fromUtf8(file.readAll()));
  }
}

void applyDarkPalette(QApplication &app)
{
  // Fallback when stylesheets don't cover a widget (menus, etc.).
  QPalette pal = app.palette();
  const QColor bg(0x1a, 0x1a, 0x2e);
  const QColor panel(0x22, 0x22, 0x36);
  const QColor text(0xff, 0xff, 0xff);
  const QColor disabled(0xa0, 0xa0, 0xb8);
  const QColor accent(0x6d, 0x4a, 0xff);

  pal.setColor(QPalette::Window, bg);
  pal.setColor(QPalette::WindowText, text);
  pal.setColor(QPalette::Base, panel);
  pal.setColor(QPalette::AlternateBase, bg);
  pal.setColor(QPalette::Text, text);
  pal.setColor(QPalette::Button, panel);
  pal.setColor(QPalette::ButtonText, text);
  pal.setColor(QPalette::BrightText, text);
  pal.setColor(QPalette::ToolTipBase, panel);
  pal.setColor(QPalette::ToolTipText, text);
  pal.setColor(QPalette::PlaceholderText, disabled);
  pal.setColor(QPalette::Highlight, accent);
  pal.setColor(QPalette::HighlightedText, text);
  pal.setColor(QPalette::Disabled, QPalette::Text, disabled);
  pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
  pal.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
  app.setPalette(pal);
}

bool anchorPanelBottom(spike::Panel &panel, QScreen *screen)
{
  // Wayland clients cannot place themselves with setGeometry(); KWin will
  // centre a normal xdg-shell window. Use wlr-layer-shell via LayerShellQt.
  panel.createWinId();
  QWindow *win = panel.windowHandle();
  if (!win) {
    return false;
  }

  LayerShellQt::Window *layer = LayerShellQt::Window::get(win);
  if (!layer) {
    return false;
  }

  using LS = LayerShellQt::Window;
  layer->setScope(QStringLiteral("spike-panel"));
  layer->setLayer(LS::LayerTop);
  layer->setAnchors(LS::Anchors(LS::AnchorLeft) | LS::AnchorRight | LS::AnchorBottom);
  layer->setExclusiveZone(kPanelHeight);
  layer->setExclusiveEdge(LS::AnchorBottom);
  layer->setKeyboardInteractivity(LS::KeyboardInteractivityOnDemand);
  layer->setActivateOnShow(true);
  if (screen) {
    layer->setScreen(screen);
    layer->setDesiredSize(QSize(screen->geometry().width(), kPanelHeight));
  }
  return true;
}

void placePanelFallback(spike::Panel &panel, QScreen *screen)
{
  // X11 / nested smoke tests only — ignored by Wayland compositors.
  if (!screen) {
    return;
  }
  const QRect geo = screen->geometry();
  panel.setGeometry(geo.left(), geo.bottom() - kPanelHeight + 1, geo.width(),
                    kPanelHeight);
}

} // namespace

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-shell"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.9"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));

  applyDarkPalette(app);
  loadStyleSheet(app);

  spike::Panel panel;
  QScreen *screen = app.primaryScreen();
  if (!anchorPanelBottom(panel, screen)) {
    placePanelFallback(panel, screen);
  }
  panel.show();

  return app.exec();
}
