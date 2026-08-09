#pragma once

#include <QPushButton>

class QLabel;
class QSlider;
class QWidget;

namespace spike {

class BrightnessApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit BrightnessApplet(QWidget *parent = nullptr);
  bool hasBacklight() const { return m_hasBacklight; }

private slots:
  void refresh();
  void togglePopup();
  void onSlider(int value);

private:
  bool discover();
  int readBrightness() const;
  int readMax() const;
  bool writeBrightness(int value);

  QString m_brightnessPath;
  QString m_maxPath;
  bool m_hasBacklight = false;
  QWidget *m_popup = nullptr;
  QSlider *m_slider = nullptr;
  QLabel *m_pct = nullptr;
};

} // namespace spike
