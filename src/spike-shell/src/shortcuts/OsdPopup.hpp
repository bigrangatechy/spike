#pragma once

#include <QWidget>

class QLabel;
class QTimer;

namespace spike {

/** Short-lived volume/brightness OSD on the Wayland overlay layer. */
class OsdPopup : public QWidget
{
  Q_OBJECT

public:
  explicit OsdPopup(QWidget *parent = nullptr);

  void showMessage(const QString &summary, const QString &body, int ms = 1500);

protected:
  void showEvent(QShowEvent *event) override;

private:
  void applyLayerShell();

  QLabel *m_label = nullptr;
  QTimer *m_hide = nullptr;
};

} // namespace spike
