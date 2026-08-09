#pragma once

#include <QPushButton>
#include <QStringList>

class QLabel;
class QWidget;

namespace spike {

class KeyboardLayoutApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit KeyboardLayoutApplet(QWidget *parent = nullptr);
  bool multiLayout() const { return m_layouts.size() >= 2; }

private slots:
  void refresh();
  void togglePopup();
  void cycle();

private:
  QStringList m_layouts;
  int m_index = 0;
  QWidget *m_popup = nullptr;
  QLabel *m_detail = nullptr;
};

} // namespace spike
