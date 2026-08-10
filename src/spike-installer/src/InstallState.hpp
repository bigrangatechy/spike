#pragma once

#include <QString>
#include <QStringList>

namespace spike {

/** Answers collected across the 10-step installer wizard (INSTALLER.md). */
struct InstallState {
  QString language = QStringLiteral("en_US");
  QString timezone = QStringLiteral("UTC");
  QString keyboard = QStringLiteral("us"); // XKBLAYOUT
  QString wifiSsid;
  QString wifiConnected; // installer UI + logged; NM profiles copied by helper

  QString username = QStringLiteral("spike");
  QString password; // kept in memory only; never logged
  QString hostname = QStringLiteral("spike");
  QString variant = QStringLiteral("standard"); // standard | plus
  /** Installed system only — live ISO always auto-logs in as spike. Default off (AGENTS.md). */
  bool autoLogin = false;
  bool doBackup = false;
  QString backupDestMount; // USB mount for Step 7
  QString backupSystemPartition; // e.g. /dev/sda2 from --list-systems
  QString backupStatus;          // ok | skipped | failed | ""
  bool restoreAfterInstall = false;
  QString restoreSessionPath; // SpikeBackup/<stamp>/<label>
  QString restoreStatus;      // ok | skipped | failed | ""
  QString targetDisk;         // e.g. /dev/sda — confirmed on storage step
  QString detectSummary;
  QStringList backupSessionsFound;
};

} // namespace spike
