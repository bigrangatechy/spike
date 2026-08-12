#pragma once

#include "RescueTypes.hpp"
#include "SpikeBackupLayout.hpp"

#include <QObject>
#include <QSet>
#include <QString>
#include <QVector>

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
  QString lastScanSummary() const { return m_scanSummary; }
  QStringList debugLog() const { return m_debugLog; }
  QVector<BackupSession> backupSessions() const { return m_backupSessions; }

  /**
   * When false, scanSystems skips per-user find-files inventory (fast --list-systems).
   * Full inventory still runs via inventorySystem() / recover path.
   */
  void setIncludeInventoryOnScan(bool include) { m_includeInventoryOnScan = include; }
  bool includeInventoryOnScan() const { return m_includeInventoryOnScan; }

  /** Unmount any RO source mounts we created. */
  Q_INVOKABLE void cleanupMounts();
  void appendDebug(const QString &line);

public slots:
  void scanSystems();
  void inventorySystem(int systemIndex);
  void refreshDestinations();
  void startCopy(int systemIndex, const QString &destMount);
  void scanBackups();
  void refreshRestoreTargets();
  void startRestore(int sessionIndex, const QString &targetHome);
  /** Restore from an absolute SpikeBackup session path (installer / migration batch). */
  void startRestoreFromPath(const QString &sessionPath, const QString &targetHome);
  void requestCancel();
  void cleanupMountsSlot() { cleanupMounts(); }

signals:
  void scanProgress(const QString &message, int percent);
  void scanFinished(bool ok);
  void inventoryFinished(bool ok);
  void destinationsChanged(const QVector<DestVolume> &vols);
  void backupScanFinished(bool ok);
  void restoreTargetsChanged(const QVector<DestVolume> &vols);
  void copyProgress(const QString &currentFile, qint64 doneFiles, qint64 totalFiles,
                    qint64 doneBytes, qint64 totalBytes);
  void copyFinished(bool ok);

private:
  bool runHelper(const QStringList &args, QString *error = nullptr) const;
  QString runHelperCapture(const QStringList &args, QString *error = nullptr) const;
  bool mountRo(const QString &device, const QString &mountPoint, const QString &fstype,
               const QString &extraOpts, QString *error);
  bool mountRwWritable(const QString &device, const QString &mountPoint, QString *error);
  bool umountPath(const QString &mountPoint);
  bool isLiveDevice(const QString &devName, const QString &label, const QString &mountpoints) const;
  QString blkidType(const QString &device) const;
  QString blkidLabel(const QString &device) const;
  QString deviceByLabel(const QString &label) const;
  QString ensureLiveUsbWritableDest();
  bool isWritablePartitionRoot(const QString &mp) const;
  void ensureSpikeBackupDir(const QString &mountRoot);
  bool appendDestIfUsable(QVector<DestVolume> *vols, const QSet<QString> &seenPaths,
                          const QSet<QString> &sourceDevs, const QString &mp,
                          const QString &displayLabel) const;
  QStringList privilegedListDirs(const QString &dir) const;
  QStringList privilegedFindFiles(const QString &dir) const;
  QStringList privilegedFindFiles(const QString &dir, int maxDepth) const;
  bool privilegedCopyFile(const QString &src, const QString &dst, QString *error) const;
  QByteArray privilegedSha256(const QString &path, bool *ok, QString *error = nullptr) const;
  DetectedSystem probeMounted(const BlockPartition &part, const QString &mnt);
  void enrichBtrfsHome(DetectedSystem *sys);
  void listUsersUnder(const QString &homeRoot, QStringList *users) const;
  Inventory buildInventory(const DetectedSystem &sys);
  QStringList categoryDirs(OsKind os) const;
  bool copyOneFile(const QString &src, const QString &dst, QString *errKind, QString *errDetail);
  void runRestoreMappings(const BackupSession &session, const QString &targetHome);
  QByteArray sha256Path(const QString &path, bool *ok, QString *errDetail = nullptr) const;
  static QByteArray sha256File(const QString &path, bool *ok);
  static QString mountpointsFromJson(const QJsonObject &obj);
  QStringList candidateBackupVolumeRoots();

  QVector<DetectedSystem> m_systems;
  Inventory m_inventory;
  CopyResult m_lastCopy;
  QString m_lastError;
  QString m_scanSummary;
  QStringList m_debugLog;
  QStringList m_ourMounts;
  QVector<BackupSession> m_backupSessions;
  bool m_cancel = false;
  bool m_includeInventoryOnScan = true;
  /** Parent disk of the live ISO (/cdrom), e.g. /dev/sda — skip all its partitions. */
  QString m_liveDisk;
};

} // namespace spike
