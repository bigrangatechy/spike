#pragma once

#include <QPushButton>
#include <QVector>

class QListWidget;
class QWidget;

namespace spike {

class RemovableDevicesApplet : public QPushButton
{
  Q_OBJECT

public:
  explicit RemovableDevicesApplet(QWidget *parent = nullptr);

private slots:
  void refresh();
  void togglePopup();
  void onEject();

private:
  struct Vol {
    QString path;
    QString label;
    QString mount;
  };
  QVector<Vol> listVolumes() const;

  QWidget *m_popup = nullptr;
  QListWidget *m_list = nullptr;
};

} // namespace spike
