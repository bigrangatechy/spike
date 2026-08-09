#pragma once

#include <QPushButton>

class QCheckBox;
class QListWidget;
class QWidget;

namespace spike {

class BluetoothApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit BluetoothApplet(QWidget *parent = nullptr);
  bool hasAdapter() const { return m_hasAdapter; }

private slots:
  void refresh();
  void togglePopup();
  void onPower(bool on);
  void onDeviceActivated();

private:
  QString adapterPath() const;

  bool m_hasAdapter = false;
  QWidget *m_popup = nullptr;
  QCheckBox *m_power = nullptr;
  QListWidget *m_list = nullptr;
};

} // namespace spike
