#pragma once

#include <QPushButton>

class QWidget;

namespace spike {

class NmClient;

/** Tray network applet — NetworkManager via D-Bus (DESKTOP.md). */
class NetworkApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit NetworkApplet(QWidget *parent = nullptr);

private slots:
  void refresh();
  void togglePopup();
  void openNetworkSettings();

private:
  void placePopup();

  NmClient *m_nm = nullptr;
  QWidget *m_popup = nullptr;
};

} // namespace spike
