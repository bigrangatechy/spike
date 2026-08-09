#pragma once

#include <QPushButton>

class QLabel;
class QWidget;

namespace spike {

class UpdateNotifierApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit UpdateNotifierApplet(QWidget *parent = nullptr);
  void setEnabledVisible(bool on);

private slots:
  void refresh();
  void togglePopup();

private:
  int countUpgradable() const;

  QWidget *m_popup = nullptr;
  QLabel *m_detail = nullptr;
  int m_count = 0;
  bool m_userVisible = true;
};

} // namespace spike
