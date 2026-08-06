#pragma once

#include <QPushButton>

class QLabel;
class QSlider;
class QWheelEvent;
class QWidget;

namespace spike {

class VolumeClient;

class VolumeApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit VolumeApplet(QWidget *parent = nullptr);

protected:
  void wheelEvent(QWheelEvent *event) override;

private slots:
  void refresh();
  void togglePopup();
  void openSoundSettings();

private:
  void placePopup();
  void updateIcon();

  VolumeClient *m_client = nullptr;
  QWidget *m_popup = nullptr;
  QSlider *m_slider = nullptr;
  QLabel *m_pctLabel = nullptr;
};

} // namespace spike
