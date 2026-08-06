#pragma once

#include <QLabel>

namespace spike {

class ClockApplet : public QLabel
{
  Q_OBJECT

public:
  explicit ClockApplet(QWidget *parent = nullptr);

private slots:
  void tick();
};

} // namespace spike
