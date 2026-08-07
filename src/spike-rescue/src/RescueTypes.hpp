#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <cstdint>

namespace spike {

enum class OsKind {
  Unknown,
  SpikeLinux,
  Linux,
  Windows,
  MacOS,
};

struct BlockPartition {
  QString name;       // sda2
  QString path;       // /dev/sda2
  QString fstype;
  QString label;
  QString sizeHuman;
  qint64 sizeBytes = 0;
  bool removable = false;
  bool isLiveMedium = false;
};

struct DetectedSystem {
  BlockPartition partition;
  OsKind os = OsKind::Unknown;
  QString osLabel;    // "Spike Linux", "Windows 10", …
  QString mountPoint; // temporary RO mount during scan/copy
  QStringList users;  // home usernames found
  qint64 fileCount = 0;
  qint64 byteTotal = 0;
};

struct CategorySummary {
  QString name; // Documents, Pictures, …
  qint64 files = 0;
  qint64 bytes = 0;
};

struct Inventory {
  QVector<CategorySummary> categories;
  qint64 fileCount = 0;
  qint64 byteTotal = 0;
  QStringList unreadable; // relative paths that failed during inventory peek
  /** Absolute source paths to copy (files only). */
  QStringList files;
};

struct DestVolume {
  QString path;       // mount point
  QString label;
  QString device;
  qint64 freeBytes = 0;
};

struct CopyResult {
  qint64 copied = 0;
  qint64 failedRead = 0;
  qint64 failedVerify = 0;
  qint64 bytesCopied = 0;
  QStringList unreadables;
  QStringList verifyFails;
  bool cancelled = false;
  QString destRoot; // …/SpikeBackup
};

inline QString osKindLabel(OsKind k)
{
  switch (k) {
  case OsKind::SpikeLinux:
    return QStringLiteral("Spike Linux");
  case OsKind::Linux:
    return QStringLiteral("Linux");
  case OsKind::Windows:
    return QStringLiteral("Windows");
  case OsKind::MacOS:
    return QStringLiteral("macOS");
  default:
    return QStringLiteral("Unknown");
  }
}

} // namespace spike
