#pragma once

#include <QLabel>

class QCalendarWidget;
class QWidget;

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
  void togglePopup();

private:
  void placePopup();

  QWidget *m_popup = nullptr;
  QLabel *m_largeTime = nullptr;
  QCalendarWidget *m_cal = nullptr;
};

} // namespace spike
