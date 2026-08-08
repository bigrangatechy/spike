#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <cstdint>

namespace spike {

/**
 * Canonical SpikeBackup layout (Rescue / Installer / Migration):
 *
 *   SpikeBackup/<utc-stamp>/<os-label>/home/<user>/Documents/…
 *   SpikeBackup/<utc-stamp>/<os-label>/Users/<user>/…   (Windows/macOS)
 *   SpikeBackup/<utc-stamp>/<os-label>/REPORT.txt
 *
 * Legacy: also discover SpikeBackup under install-logs-* / log /
 * (casper often bind-mounts that path at /var/log).
 */

struct BackupSession {
  QString spikeBackupRoot; // …/SpikeBackup
  QString stamp;           // 20260808-113057
  QString osLabel;         // Linux, Windows, …
  QString sessionPath;     // …/SpikeBackup/<stamp>/<osLabel>
  QStringList users;       // home/ or Users/ names found
  qint64 fileCount = 0;
  qint64 byteTotal = 0;
  bool legacyPath = false; // under install-logs-*/log/
};

struct RestoreMapping {
  QString srcAbsolute;
  QString destAbsolute;
  QString relativeDisplay; // e.g. Documents/foo.odt
};

/** Find SpikeBackup directories on a volume root (and legacy install-logs paths). */
QStringList findSpikeBackupRoots(const QString &volumeRoot);

/** Enumerate stamp/os-label sessions under a SpikeBackup root. */
QVector<BackupSession> listBackupSessions(const QString &spikeBackupRoot, bool legacy = false);

/** Scan common mount roots for all sessions (newest stamp first). */
QVector<BackupSession> discoverAllBackupSessions(const QStringList &volumeRoots);

/** Map one file under sessionPath into targetHome (Documents/… etc.). */
QString mapBackupFileToHome(const QString &sessionPath, const QString &srcFile,
                            const QString &targetHome, QString *displayRel = nullptr);

/** Build full restore file list for a session (optional user filter; empty = all). */
QVector<RestoreMapping> buildRestoreMappings(const QString &sessionPath,
                                             const QString &targetHome,
                                             const QString &onlyUser = {});

/** Inventory helpers for UI. */
void summarizeSession(BackupSession *session);

} // namespace spike
