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

bool isCasperLogWritableBind(const QString &mp)
{
  // Casper often bind-mounts a writable *subdir* at /var/log — SpikeBackup must
  // not land there (ends up under install-logs-*/log/ after collect).
  return mp == QLatin1String("/var/log") || mp.startsWith(QLatin1String("/var/log/"));
}

QString BackupScanner::ensureLiveUsbWritableRoot()
{
  const QString mnt = QStringLiteral("/run/spike-rescue/dest-writable");
  QStorageInfo already(mnt);
  if (already.isValid() && already.isReady() && !already.isReadOnly()) {
    QDir().mkpath(mnt + QStringLiteral("/SpikeBackup"));
    return mnt;
  }

  QString dev = runCapture(QStringLiteral("blkid"),
                           {QStringLiteral("-L"), QStringLiteral("writable")});
  if (dev.isEmpty()) {
    // blkid -L prints the device path; some systems need -o device.
    const QString raw =
        runCapture(QStringLiteral("blkid"),
                   {QStringLiteral("-o"), QStringLiteral("device"), QStringLiteral("-t"),
                    QStringLiteral("LABEL=writable")});
    for (const QString &line : raw.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
      if (line.startsWith(QLatin1String("/dev/"))) {
        dev = line.trimmed();
        break;
      }
    }
  }
  if (dev.isEmpty()) {
    return {};
  }

  QProcess prep;
  prep.start(QStringLiteral("sudo"),
             {QStringLiteral("-n"), QStringLiteral("/usr/lib/spike/spike-rescue-mount"),
              QStringLiteral("prepare")});
  prep.waitForFinished(5000);

  QString fstype =
      runCapture(QStringLiteral("blkid"),
                 {QStringLiteral("-o"), QStringLiteral("value"), QStringLiteral("-s"),
                  QStringLiteral("TYPE"), dev});
  QStringList args = {QStringLiteral("-n"), QStringLiteral("/usr/lib/spike/spike-rescue-mount"),
                      QStringLiteral("mount-rw")};
  if (!fstype.isEmpty()) {
    args << QStringLiteral("-t") << fstype;
  }
  args << QStringLiteral("-o") << QStringLiteral("rw") << dev << mnt;

  QProcess mount;
  mount.start(QStringLiteral("sudo"), args);
  if (!mount.waitForFinished(25000) || mount.exitCode() != 0) {
    return {};
  }
  QDir().mkpath(mnt + QStringLiteral("/SpikeBackup"));
  return mnt;
}

QStringList BackupScanner::volumeRoots()
{
  QStringList roots;

  // Prefer a dedicated RW mount of the live USB writable *partition root*.
  const QString liveWritable = ensureLiveUsbWritableRoot();
  if (!liveWritable.isEmpty()) {
    roots << liveWritable;
  }

  for (const QString &base : {QStringLiteral("/run/media"), QStringLiteral("/media")}) {
    QDir root(base);
    if (!root.exists()) {
      continue;
    }
    for (const QString &user : root.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QDir userDir(root.filePath(user));
      for (const QString &vol : userDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString path = userDir.filePath(vol);
        if (isCasperLogWritableBind(path) || roots.contains(path)) {
          continue;
        }
        roots << path;
      }
    }
  }

  // Other findmnt LABEL=writable targets — skip casper /var/log binds.
  const QString writable =
      runCapture(QStringLiteral("findmnt"),
                 {QStringLiteral("-n"), QStringLiteral("-o"), QStringLiteral("TARGET"),
                  QStringLiteral("-S"), QStringLiteral("LABEL=writable")});
  for (QString t : writable.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    t = t.trimmed();
    if (t.isEmpty() || isCasperLogWritableBind(t) || roots.contains(t)) {
      continue;
    }
    // Skip nested install-logs mounts; climb to partition root when obvious.
    if (t.contains(QLatin1String("install-logs"))) {
      continue;
    }
    roots << t;
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
