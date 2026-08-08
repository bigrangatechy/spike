#pragma once

#include <QString>
#include <QStringList>

namespace spike {

/** Answers collected across the 10-step installer wizard (INSTALLER.md). */
struct InstallState {
  QString language = QStringLiteral("en_US");
  QString timezone = QStringLiteral("UTC");
  QString wifiSsid;
  QString wifiConnected; // display only for now
  QString username = QStringLiteral("spike");
  QString password; // kept in memory only; never logged
  QString hostname = QStringLiteral("spike");
  QString variant = QStringLiteral("standard"); // standard | plus
  bool doBackup = false;
  QString backupDestMount; // USB mount for Step 7
  bool restoreAfterInstall = false;
  QString restoreSessionPath; // SpikeBackup/<stamp>/<label>
  QString targetDisk;         // e.g. /dev/sda — confirmed on storage step
  QString detectSummary;
  QStringList backupSessionsFound;
};

} // namespace spike
