#include "panel/Panel.hpp"

#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QPalette>
#include <QProcess>
#include <QScreen>
#include <QStandardPaths>
#include <QTimer>

namespace {

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

} // namespace

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-shell"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.22"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));

  // Breeze SVG icons need qt6-svg-plugins on the live image.
  // Prefer breeze-dark directly; spike-icons inherits it for future overrides.
  QStringList iconPaths = QIcon::themeSearchPaths();
  iconPaths.prepend(QStringLiteral("/usr/share/icons"));
  const QString localIcons =
      QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
      QStringLiteral("/icons");
  if (!localIcons.isEmpty()) {
    iconPaths.prepend(localIcons);
  }
  QIcon::setThemeSearchPaths(iconPaths);
  QIcon::setThemeName(QStringLiteral("breeze-dark"));
  QIcon::setFallbackThemeName(QStringLiteral("breeze"));
  if (!QIcon::hasThemeIcon(QStringLiteral("network-wireless"))) {
    QIcon::setThemeName(QStringLiteral("breeze"));
  }

  applyDarkPalette(app);
  loadStyleSheet(app);

  spike::Panel panel;
  panel.applyLayerShell();
  panel.show();

  // Portals need WAYLAND_DISPLAY (set by KWin for --exit-with-session).
  // Never start these from spike-session before KWin — that blocks ~90s then ABRTs.
  QTimer::singleShot(2000, []() {
    QProcess::startDetached(
        QStringLiteral("systemctl"),
        {QStringLiteral("--user"), QStringLiteral("start"), QStringLiteral("--no-block"),
         QStringLiteral("xdg-desktop-portal.service"),
         QStringLiteral("plasma-xdg-desktop-portal-kde.service")});
  });

  return app.exec();
}
