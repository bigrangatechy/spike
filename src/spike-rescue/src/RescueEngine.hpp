#pragma once

#include "RescueTypes.hpp"

#include <QObject>
#include <QString>

namespace spike {

/** Disk scan, RO mount, inventory, SHA256 copy — DISASTER-RECOVERY.md Layer 3. */
class RescueEngine : public QObject
{
  Q_OBJECT

public:
  explicit RescueEngine(QObject *parent = nullptr);
  ~RescueEngine() override;

  QVector<DetectedSystem> systems() const { return m_systems; }
  Inventory inventory() const { return m_inventory; }
  CopyResult lastCopy() const { return m_lastCopy; }
  QString lastError() const { return m_lastError; }

  /** Unmount any RO source mounts we created. */
  Q_INVOKABLE void cleanupMounts();

public slots:
  void scanSystems();
  void inventorySystem(int systemIndex);
  void refreshDestinations();
  void startCopy(int systemIndex, const QString &destMount);
  void requestCancel();
  void cleanupMountsSlot() { cleanupMounts(); }

signals:
  void scanProgress(const QString &message, int percent);
  void scanFinished(bool ok);
  void inventoryFinished(bool ok);
  void destinationsChanged(const QVector<DestVolume> &vols);
  void copyProgress(const QString &currentFile, qint64 doneFiles, qint64 totalFiles,
                    qint64 doneBytes, qint64 totalBytes);
  void copyFinished(bool ok);

private:
  bool runPrivileged(const QStringList &args, QString *error = nullptr) const;
  bool mountRo(const QString &device, const QString &mountPoint, const QString &fstype,
               QString *error);
  bool umountPath(const QString &mountPoint);
  bool isLiveDevice(const QString &devName, const QString &label, const QString &mountpoints) const;
  DetectedSystem probeMounted(const BlockPartition &part, const QString &mnt);
  Inventory buildInventory(const DetectedSystem &sys);
  QStringList categoryDirs(OsKind os) const;
  bool copyOneFile(const QString &src, const QString &dst, QString *errKind);
  static QByteArray sha256File(const QString &path, bool *ok);

  QVector<DetectedSystem> m_systems;
  Inventory m_inventory;
  CopyResult m_lastCopy;
  QString m_lastError;
  QStringList m_ourMounts;
  bool m_cancel = false;
};

} // namespace spike
