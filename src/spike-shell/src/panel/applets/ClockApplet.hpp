#pragma once

#include <QLabel>

namespace spike {

class ClockApplet : public QLabel
{
  Q_OBJECT

public:
  explicit ClockApplet(QWidget *parent = nullptr);

protected:
  void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
  void tick();
};

} // namespace spike
