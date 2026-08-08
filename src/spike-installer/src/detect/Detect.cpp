#include "Detect.hpp"

#include "SpikeBackupLayout.hpp"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QStorageInfo>

namespace spike {
namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

} // namespace

QString HardwareProbe::summary()
{
  QStringList lines;
  const QString uname = runCapture(QStringLiteral("uname"), {QStringLiteral("-mr")});
  if (!uname.isEmpty()) {
    lines << QStringLiteral("Kernel: %1").arg(uname);
  }
  QFile meminfo(QStringLiteral("/proc/meminfo"));
  if (meminfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString first = QString::fromUtf8(meminfo.readLine());
    lines << first.trimmed();
  }
  QFile cpuinfo(QStringLiteral("/proc/cpuinfo"));
  if (cpuinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    while (!cpuinfo.atEnd()) {
      const QString line = QString::fromUtf8(cpuinfo.readLine());
      if (line.startsWith(QLatin1String("model name"))) {
        lines << line.trimmed();
        break;
      }
    }
  }
  const QStringList disks = blockDevicesHuman();
  if (!disks.isEmpty()) {
    lines << QStringLiteral("Disks:");
    for (const QString &d : disks) {
      lines << QStringLiteral("  %1").arg(d);
    }
  }
  // Prefer spike-config detect summary when present.
  const QString cfg =
      runCapture(QStringLiteral("spike-config"), {QStringLiteral("--version")});
  if (!cfg.isEmpty()) {
    lines << QStringLiteral("spike-config: %1").arg(cfg);
  }
  return lines.join(QLatin1Char('\n'));
}

QStringList HardwareProbe::blockDevicesHuman()
{
  QStringList out;
  const QString raw = runCapture(
      QStringLiteral("lsblk"),
      {QStringLiteral("-dn"), QStringLiteral("-o"), QStringLiteral("NAME,SIZE,TYPE,MODEL")});
  for (const QString &line : raw.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    if (line.contains(QLatin1String("disk"))) {
      out << line.simplified();
    }
  }
  return out;
}

QString HardwareProbe::recommendVariant()
{
  // INSTALLER.md: auto-recommend from RAM; Plus when enough memory.
  qint64 memKb = 0;
  QFile meminfo(QStringLiteral("/proc/meminfo"));
  if (meminfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString line = QString::fromUtf8(meminfo.readLine());
    const QStringList parts = line.split(QRegularExpression(QStringLiteral("\\s+")),
                                         Qt::SkipEmptyParts);
    if (parts.size() >= 2) {
      memKb = parts.at(1).toLongLong();
    }
  }
  const double gib = memKb / (1024.0 * 1024.0);
  // Rough: Standard for <=4GiB class, Plus when more (matches product intent).
  return gib > 4.5 ? QStringLiteral("plus") : QStringLiteral("standard");
}

QStringList BackupScanner::volumeRoots()
{
  QStringList roots;
  for (const QString &base : {QStringLiteral("/run/media"), QStringLiteral("/media")}) {
    QDir root(base);
    if (!root.exists()) {
      continue;
    }
    for (const QString &user : root.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QDir userDir(root.filePath(user));
      for (const QString &vol : userDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        roots << userDir.filePath(vol);
      }
    }
  }
  const QString writable =
      runCapture(QStringLiteral("findmnt"),
                 {QStringLiteral("-n"), QStringLiteral("-o"), QStringLiteral("TARGET"),
                  QStringLiteral("-S"), QStringLiteral("LABEL=writable")});
  for (QString t : writable.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    t = t.trimmed();
    if (!t.isEmpty() && !roots.contains(t)) {
      roots << t;
      QDir d(t);
      if (d.cdUp() && d.absolutePath().contains(QLatin1String("install-logs"))) {
        QDir up(d.absolutePath());
        if (up.cdUp() && !roots.contains(up.absolutePath())) {
          roots << up.absolutePath();
        }
      }
    }
  }
  return roots;
}

QStringList BackupScanner::sessionLabels()
{
  QStringList labels;
  for (const BackupSession &s : discoverAllBackupSessions(volumeRoots())) {
    labels << QStringLiteral("%1 / %2 — %3 files%4")
                 .arg(s.stamp, s.osLabel)
                 .arg(s.fileCount)
                 .arg(s.legacyPath ? QStringLiteral(" [legacy]") : QString());
  }
  return labels;
}

QStringList BackupScanner::sessionPaths()
{
  QStringList paths;
  for (const BackupSession &s : discoverAllBackupSessions(volumeRoots())) {
    paths << s.sessionPath;
  }
  return paths;
}

} // namespace spike
