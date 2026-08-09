#pragma once

#include <QWidget>

class QHBoxLayout;

namespace spike {

/** Left-zone window buttons (KWin D-Bus / best-effort). */
class WindowListApplet : public QWidget
{
  Q_OBJECT

public:
  explicit WindowListApplet(QWidget *parent = nullptr);
  void setEnabledVisible(bool on);

private slots:
  void refresh();

private:
  struct Win {
    QString uuid;
    QString caption;
  };
  QVector<Win> listWindows() const;

  QHBoxLayout *m_lay = nullptr;
  bool m_userVisible = true;
};

} // namespace spike
