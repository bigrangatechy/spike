#pragma once

#include <QWidget>

namespace spike {

class Launcher;

class Panel : public QWidget
{
  Q_OBJECT

public:
  explicit Panel(QWidget *parent = nullptr);

private slots:
  void toggleLauncher();

private:
  Launcher *m_launcher = nullptr;
};

} // namespace spike
