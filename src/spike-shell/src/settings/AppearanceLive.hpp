#pragma once

#include <QString>

class QApplication;

namespace spike {

/** Reload spike.qss with accent/font substitutions + palette Highlight. */
void applyShellChromeLive(QApplication *app, const QString &accentHex, int fontSizePt,
                          bool highContrast);

/** Best-effort set desktop wallpaper image for the Spike background layer. */
bool applyWallpaperLive(const QString &imagePath, QString *detail = nullptr);

} // namespace spike
