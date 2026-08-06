#pragma once

#include <QPushButton>

namespace spike {

class SessionMenuApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit SessionMenuApplet(QWidget *parent = nullptr);

private slots:
  void showMenu();
};

} // namespace spike
