#pragma once

#include <QPushButton>

class QCheckBox;
class QLabel;
class QSlider;
class QWidget;

namespace spike {

class NightLightApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit NightLightApplet(QWidget *parent = nullptr);
  void setEnabledVisible(bool on);
  void applyFromConfig(bool enabled, int temperature);

private slots:
  void refresh();
  void togglePopup();
  void onToggle(bool on);
  void onTemp(int kelvin);

private:
  bool tryKwinSet(bool enabled, int temperature);
  bool m_enabled = false;
  int m_temperature = 4500;
  bool m_userVisible = true;
  QWidget *m_popup = nullptr;
  QCheckBox *m_toggle = nullptr;
  QSlider *m_temp = nullptr;
  QLabel *m_tempLabel = nullptr;
};

} // namespace spike
