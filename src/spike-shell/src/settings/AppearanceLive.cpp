#include "settings/AppearanceLive.hpp"

#include "desktop/DesktopBackground.hpp"

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QFont>
#include <QPalette>
#include <QRegularExpression>

namespace spike {

namespace {

QString loadBaseStyleSheet()
{
  QFile file(QStringLiteral(":/styles/spike.qss"));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return {};
  }
  return QString::fromUtf8(file.readAll());
}

QString tintAccent(const QString &hex, double factor)
{
  QColor c(hex);
  if (!c.isValid()) {
    c = QColor(QStringLiteral("#6d4aff"));
  }
  c = c.lighter(static_cast<int>(100 + (factor - 1.0) * 100));
  return c.name(QColor::HexRgb);
}

} // namespace

void applyShellChromeLive(QApplication *app, const QString &accentHex, int fontSizePt,
                          bool highContrast)
{
  if (!app) {
    return;
  }
  QString accent = accentHex.trimmed();
  if (accent.isEmpty() || !QColor(accent).isValid()) {
    accent = QStringLiteral("#6d4aff");
  }
  if (fontSizePt < 8) {
    fontSizePt = 8;
  }
  if (fontSizePt > 18) {
    fontSizePt = 18;
  }

  QPalette pal = app->palette();
  if (highContrast) {
    pal.setColor(QPalette::Window, QColor(QStringLiteral("#000000")));
    pal.setColor(QPalette::WindowText, QColor(QStringLiteral("#ffffff")));
    pal.setColor(QPalette::Base, QColor(QStringLiteral("#000000")));
    pal.setColor(QPalette::Text, QColor(QStringLiteral("#ffffff")));
    pal.setColor(QPalette::Button, QColor(QStringLiteral("#111111")));
    pal.setColor(QPalette::ButtonText, QColor(QStringLiteral("#ffffff")));
    pal.setColor(QPalette::Highlight, QColor(QStringLiteral("#ffff00")));
    pal.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#000000")));
  } else {
    pal.setColor(QPalette::Window, QColor(QStringLiteral("#1a1a2e")));
    pal.setColor(QPalette::WindowText, QColor(QStringLiteral("#ffffff")));
    pal.setColor(QPalette::Base, QColor(QStringLiteral("#222236")));
    pal.setColor(QPalette::Text, QColor(QStringLiteral("#ffffff")));
    pal.setColor(QPalette::Button, QColor(QStringLiteral("#222236")));
    pal.setColor(QPalette::ButtonText, QColor(QStringLiteral("#ffffff")));
    pal.setColor(QPalette::Highlight, QColor(accent));
    pal.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#ffffff")));
  }
  app->setPalette(pal);

  QFont f = app->font();
  f.setPointSize(fontSizePt);
  app->setFont(f);

  QString qss = loadBaseStyleSheet();
  if (qss.isEmpty()) {
    return;
  }
  if (highContrast) {
    qss.replace(QStringLiteral("#1a1a2e"), QStringLiteral("#000000"));
    qss.replace(QStringLiteral("#222236"), QStringLiteral("#000000"));
    qss.replace(QStringLiteral("#2a2a4a"), QStringLiteral("#111111"));
    qss.replace(QStringLiteral("#33335a"), QStringLiteral("#ffffff"));
  }
  qss.replace(QStringLiteral("#6d4aff"), accent);
  qss.replace(QStringLiteral("#7e63ff"), tintAccent(accent, 1.15));
  qss.replace(QStringLiteral("#5a3ad9"), tintAccent(accent, 0.85));
  // Clock / body font size hint
  qss.replace(QRegularExpression(QStringLiteral("font-size:\\s*\\d+px")),
              QStringLiteral("font-size: %1px").arg(fontSizePt + 2));
  app->setStyleSheet(qss);
}

bool applyWallpaperLive(const QString &imagePath, QString *detail)
{
  DesktopBackground *bg = DesktopBackground::instance();
  if (!bg) {
    if (detail) {
      *detail = QStringLiteral("Desktop background layer not started");
    }
    return false;
  }
  if (!bg->setImagePath(imagePath)) {
    if (detail) {
      *detail = QStringLiteral("Could not load image: %1").arg(imagePath);
    }
    return false;
  }
  if (detail) {
    *detail = imagePath.isEmpty() ? QStringLiteral("Cleared wallpaper")
                                  : QStringLiteral("Wallpaper set");
  }
  return true;
}

} // namespace spike
