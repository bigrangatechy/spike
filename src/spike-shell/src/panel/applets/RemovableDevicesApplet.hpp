#pragma once

#include <QDBusObjectPath>
#include <QPushButton>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariantMap>
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
  void onOpen();
  void onMount();
  void onEject();
  void onUdisksChanged();
  void onInterfacesAdded(const QDBusObjectPath &object, const QVariantMap &interfaces);
  void onInterfacesRemoved(const QDBusObjectPath &object, const QStringList &interfaces);

private:
  struct Vol {
    QString path;
    QString label;
    QString mount;
    qulonglong size = 0;
  };
  QVector<Vol> listVolumes() const;
  bool mountVolume(const QString &path, QString *error = nullptr);
  void maybeAutomount(const QString &path);

  QWidget *m_popup = nullptr;
  QListWidget *m_list = nullptr;
  QSet<QString> m_knownPaths;
  bool m_seenInitialScan = false;
};

} // namespace spike
