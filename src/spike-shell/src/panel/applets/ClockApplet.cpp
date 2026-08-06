#include "panel/applets/ClockApplet.hpp"

#include <QDateTime>
#include <QTimer>

namespace spike {

ClockApplet::ClockApplet(QWidget *parent)
  : QLabel(parent)
{
  setObjectName(QStringLiteral("ClockLabel"));
  setAlignment(Qt::AlignCenter);

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ClockApplet::tick);
  timer->start(1000);
  tick();
}

void ClockApplet::tick()
{
  setText(QDateTime::currentDateTime().toString(QStringLiteral("ddd HH:mm")));
}

} // namespace spike
