#include "panel/applets/TrayHelpers.hpp"

#include <QApplication>
#include <QIcon>
#include <QPushButton>
#include <QScreen>

namespace spike {
namespace tray {

void placePopupAbove(QWidget *anchor, QWidget *popup)
{
  if (!anchor || !popup) {
    return;
  }
  popup->adjustSize();
  const QPoint global = anchor->mapToGlobal(QPoint(anchor->width() - popup->width(), 0));
  int x = global.x();
  int y = global.y() - popup->height() - 4;
  if (QScreen *screen = QApplication::screenAt(anchor->mapToGlobal(anchor->rect().center()))) {
    const QRect geo = screen->availableGeometry();
    x = qBound(geo.left(), x, geo.right() - popup->width());
    if (y < geo.top()) {
      y = anchor->mapToGlobal(QPoint(0, anchor->height())).y() + 4;
    }
  }
  popup->move(x, y);
}

void openPanelSettings(QWidget *from, const QString &pageId)
{
  QWidget *w = from;
  while (w) {
    if (w->objectName() == QLatin1String("SpikePanel")) {
      if (pageId.isEmpty()) {
        QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection);
      } else {
        QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection, Q_ARG(QString, pageId));
      }
      break;
    }
    w = w->parentWidget();
  }
}

void setTrayIcon(QPushButton *btn, const QString &themeName, const QString &textFallback,
                 const QSize &iconSize)
{
  if (!btn) {
    return;
  }
  const QIcon icon = QIcon::fromTheme(themeName);
  if (QIcon::hasThemeIcon(themeName) || (!icon.isNull() && !icon.availableSizes().isEmpty())) {
    btn->setIcon(icon);
    btn->setIconSize(iconSize);
    btn->setText(QString());
  } else {
    btn->setIcon(QIcon());
    btn->setText(textFallback);
  }
}

} // namespace tray
} // namespace spike
