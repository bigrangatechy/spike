#include "panel/applets/ClockApplet.hpp"

#include <QDateTime>
#include <QMouseEvent>
#include <QTimer>
#include <QWidget>

namespace spike {

ClockApplet::ClockApplet(QWidget *parent)
  : QLabel(parent)
{
  setObjectName(QStringLiteral("ClockLabel"));
  setAlignment(Qt::AlignCenter);
  setCursor(Qt::PointingHandCursor);
  setToolTip(QStringLiteral("Date & Time settings"));

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ClockApplet::tick);
  timer->start(1000);
  tick();
}

void ClockApplet::tick()
{
  setText(QDateTime::currentDateTime().toString(QStringLiteral("ddd HH:mm")));
}

void ClockApplet::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    QWidget *w = parentWidget();
    while (w) {
      if (w->objectName() == QLatin1String("SpikePanel")) {
        QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("datetime")));
        break;
      }
      w = w->parentWidget();
    }
  }
  QLabel::mouseReleaseEvent(event);
}

} // namespace spike
