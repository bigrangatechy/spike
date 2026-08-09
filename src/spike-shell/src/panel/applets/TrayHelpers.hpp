#pragma once

#include <QPoint>
#include <QString>
#include <QWidget>

class QPushButton;

namespace spike {
namespace tray {

/** Place a Qt::Popup widget above (or below) the tray button. */
void placePopupAbove(QWidget *anchor, QWidget *popup);

/** Walk to SpikePanel and open Settings (optional page id). */
void openPanelSettings(QWidget *from, const QString &pageId = QString());

/** Theme icon on a flat tray button, with text fallback. */
void setTrayIcon(QPushButton *btn, const QString &themeName, const QString &textFallback,
                 const QSize &iconSize = QSize(20, 20));

} // namespace tray
} // namespace spike
