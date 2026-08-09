#pragma once

#include <QPushButton>

class QWidget;
class QCheckBox;

namespace spike {

class BatteryClient;

class BatteryApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit BatteryApplet(QWidget *parent = nullptr);

  /** False when UPower reports no battery — Panel should hide this applet. */
  bool hasBattery() const;

private slots:
  void refresh();
  void togglePopup();
  void openPowerSettings();

private:
  void placePopup();
  void updateIcon();

  BatteryClient *m_client = nullptr;
  QWidget *m_popup = nullptr;
  QCheckBox *m_blockSleep = nullptr;
};

} // namespace spike
