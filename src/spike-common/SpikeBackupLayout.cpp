#include "SpikeBackupLayout.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

#include <algorithm>

namespace spike {
namespace {

const QStringList kCategoryNames = {
    QStringLiteral("Documents"), QStringLiteral("Pictures"), QStringLiteral("Videos"),
    QStringLiteral("Movies"),    QStringLiteral("Music"),    QStringLiteral("Downloads"),
    QStringLiteral("Desktop"),
};

bool isCategoryDir(const QString &name)
{
  return kCategoryNames.contains(name, Qt::CaseInsensitive);
}

QString stripHomePrefix(const QString &rel)
{
  // home/<user>/Documents/… or Users/<user>/Documents/…
  QStringList parts = rel.split(QLatin1Char('/'), Qt::SkipEmptyParts);
  if (parts.size() >= 3 &&
      (parts.at(0).compare(QLatin1String("home"), Qt::CaseInsensitive) == 0 ||
       parts.at(0).compare(QLatin1String("Users"), Qt::CaseInsensitive) == 0)) {
    parts.removeFirst(); // home|Users
    parts.removeFirst(); // username
    return parts.join(QLatin1Char('/'));
  }
  // Flat Documents/… under session
  if (!parts.isEmpty() && isCategoryDir(parts.at(0))) {
    return rel;
  }
  return rel;
}

QStringList listUsersUnder(const QString &sessionPath, const QString &kind)
{
  QStringList users;
  QDir dir(sessionPath + QLatin1Char('/') + kind);
  if (!dir.exists()) {
    return users;
  }
  for (const QString &n : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
    if (n.startsWith(QLatin1Char('.'))) {
      continue;
    }
    users.append(n);
  }
  return users;
}

} // namespace

QStringList findSpikeBackupRoots(const QString &volumeRoot)
{
  QStringList roots;
  if (volumeRoot.isEmpty()) {
    return roots;
  }
  const QString primary = volumeRoot + QStringLiteral("/SpikeBackup");
  if (QDir(primary).exists()) {
    roots.append(primary);
  }
  // Legacy: casper wrote under install-logs-*/log/SpikeBackup when dest was /var/log.
  QDir vol(volumeRoot);
  for (const QString &name : vol.entryList(QStringList{QStringLiteral("install-logs-*")},
                                           QDir::Dirs | QDir::NoDotAndDotDot)) {
    const QString legacy = vol.filePath(name) + QStringLiteral("/log/SpikeBackup");
    if (QDir(legacy).exists() && !roots.contains(legacy)) {
      roots.append(legacy);
    }
  }
  return roots;
}

void summarizeSession(BackupSession *session)
{
  if (!session || session->sessionPath.isEmpty()) {
    return;
  }
  session->users = listUsersUnder(session->sessionPath, QStringLiteral("home"));
  if (session->users.isEmpty()) {
    session->users = listUsersUnder(session->sessionPath, QStringLiteral("Users"));
  }
  session->fileCount = 0;
  session->byteTotal = 0;
  QDirIterator it(session->sessionPath, QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    if (it.fileName() == QLatin1String("REPORT.txt")) {
      continue;
    }
    session->fileCount++;
    session->byteTotal += it.fileInfo().size();
  }
}

QVector<BackupSession> listBackupSessions(const QString &spikeBackupRoot, bool legacy)
{
  QVector<BackupSession> out;
  QDir root(spikeBackupRoot);
  if (!root.exists()) {
    return out;
  }
  QStringList stamps = root.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  std::sort(stamps.begin(), stamps.end(), std::greater<QString>());
  for (const QString &stamp : stamps) {
    QDir stampDir(root.filePath(stamp));
    for (const QString &osLabel : stampDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      BackupSession s;
      s.spikeBackupRoot = spikeBackupRoot;
      s.stamp = stamp;
      s.osLabel = osLabel;
      s.sessionPath = stampDir.filePath(osLabel);
      s.legacyPath = legacy;
      summarizeSession(&s);
      out.append(s);
    }
  }
  return out;
}

QVector<BackupSession> discoverAllBackupSessions(const QStringList &volumeRoots)
{
  QVector<BackupSession> all;
  QSet<QString> seenSessionPaths;
  for (const QString &vol : volumeRoots) {
    for (const QString &backupRoot : findSpikeBackupRoots(vol)) {
      const bool legacy = backupRoot.contains(QLatin1String("install-logs"));
      for (const BackupSession &s : listBackupSessions(backupRoot, legacy)) {
        if (seenSessionPaths.contains(s.sessionPath)) {
          continue;
        }
        seenSessionPaths.insert(s.sessionPath);
        all.append(s);
      }
    }
  }
  std::sort(all.begin(), all.end(), [](const BackupSession &a, const BackupSession &b) {
    if (a.stamp != b.stamp) {
      return a.stamp > b.stamp;
    }
    return a.sessionPath > b.sessionPath;
  });
  return all;
}

QString mapBackupFileToHome(const QString &sessionPath, const QString &srcFile,
                            const QString &targetHome, QString *displayRel)
{
  QString rel = srcFile;
  if (rel.startsWith(sessionPath)) {
    rel = rel.mid(sessionPath.size());
  }
  if (rel.startsWith(QLatin1Char('/'))) {
    rel.remove(0, 1);
  }
  if (rel == QLatin1String("REPORT.txt") || rel.endsWith(QLatin1String("/REPORT.txt"))) {
    return {};
  }
  const QString mapped = stripHomePrefix(rel);
  if (mapped.isEmpty()) {
    return {};
  }
  if (displayRel) {
    *displayRel = mapped;
  }
  return targetHome + QLatin1Char('/') + mapped;
}

QVector<RestoreMapping> buildRestoreMappings(const QString &sessionPath,
                                             const QString &targetHome,
                                             const QString &onlyUser)
{
  QVector<RestoreMapping> maps;
  if (sessionPath.isEmpty() || targetHome.isEmpty()) {
    return maps;
  }
  QDirIterator it(sessionPath, QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    const QString src = it.next();
    if (it.fileName() == QLatin1String("REPORT.txt")) {
      continue;
    }
    if (!onlyUser.isEmpty()) {
      const QString needleHome = QStringLiteral("/home/%1/").arg(onlyUser);
      const QString needleUsers = QStringLiteral("/Users/%1/").arg(onlyUser);
      if (!src.contains(needleHome) && !src.contains(needleUsers)) {
        // Also allow flat category layout without user segment.
        QString rel = src.mid(sessionPath.size());
        if (rel.startsWith(QLatin1Char('/'))) {
          rel.remove(0, 1);
        }
        const QStringList parts = rel.split(QLatin1Char('/'), Qt::SkipEmptyParts);
        if (!parts.isEmpty() &&
            (parts.at(0).compare(QLatin1String("home"), Qt::CaseInsensitive) == 0 ||
             parts.at(0).compare(QLatin1String("Users"), Qt::CaseInsensitive) == 0)) {
          continue;
        }
      }
    }
    QString display;
    const QString dst = mapBackupFileToHome(sessionPath, src, targetHome, &display);
    if (dst.isEmpty()) {
      continue;
    }
    maps.append({src, dst, display});
  }
  return maps;
}

} // namespace spike
