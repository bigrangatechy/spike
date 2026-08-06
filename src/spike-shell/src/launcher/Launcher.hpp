#pragma once

#include <QWidget>

class QKeyEvent;
class QLineEdit;
class QListWidget;
class QShowEvent;

namespace spike {

class Launcher : public QWidget
{
  Q_OBJECT

public:
  explicit Launcher(QWidget *parent = nullptr);

protected:
  void showEvent(QShowEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void filterChanged(const QString &text);
  void activateCurrent();

private:
  void populateStubEntries();

  QLineEdit *m_search = nullptr;
  QListWidget *m_list = nullptr;
};

} // namespace spike
