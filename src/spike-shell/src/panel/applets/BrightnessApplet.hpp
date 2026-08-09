#pragma once

#include <QPushButton>

class QLabel;
class QSlider;
class QWidget;

namespace spike {

class BrightnessClient;

class BrightnessApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit BrightnessApplet(QWidget *parent = nullptr);
  bool hasBacklight() const;

private slots:
  void refresh();
  void togglePopup();
  void onSlider(int value);

private:
  BrightnessClient *m_client = nullptr;
  QWidget *m_popup = nullptr;
  QSlider *m_slider = nullptr;
  QLabel *m_pct = nullptr;
};

} // namespace spike
