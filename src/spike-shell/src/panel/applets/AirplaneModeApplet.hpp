#pragma once

#include <QPushButton>

class QCheckBox;
class QLabel;
class QWidget;

namespace spike {

class AirplaneModeApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit AirplaneModeApplet(QWidget *parent = nullptr);
  bool hasRfkill() const { return m_hasRfkill; }

private slots:
  void refresh();
  void togglePopup();
  void onToggle(bool on);

private:
  bool m_hasRfkill = false;
  bool m_blocked = false;
  QWidget *m_popup = nullptr;
  QCheckBox *m_toggle = nullptr;
  QLabel *m_detail = nullptr;
};

} // namespace spike
