#include "RescueEngine.hpp"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QSet>
#include <QStorageInfo>
#include <QUuid>

#include <functional>

namespace spike {

namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(60000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput());
}

qint64 parseSize(const QString &s)
{
  bool ok = false;
  const qint64 v = s.toLongLong(&ok);
  return ok ? v : 0;
}

bool pathLooksHidden(const QString &rel)
{
  const QStringList parts = rel.split(QLatin1Char('/'), Qt::SkipEmptyParts);
  for (const QString &p : parts) {
    if (p.startsWith(QLatin1Char('.'))) {
      return true;
    }
  }
  return false;
}

void walkFiles(const QString &root, const QString &prefix, QStringList *outFiles, qint64 *bytes,
               qint64 *count, QStringList *unreadable)
{
  QDir dir(root);
  if (!dir.exists()) {
    return;
  }
  const QFileInfoList entries =
      dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
  for (const QFileInfo &fi : entries) {
    const QString rel = prefix.isEmpty() ? fi.fileName() : (prefix + QLatin1Char('/') + fi.fileName());
    if (pathLooksHidden(rel)) {
      continue;
    }
    if (fi.isSymLink()) {
      continue;
    }
    if (fi.isDir()) {
      walkFiles(fi.absoluteFilePath(), rel, outFiles, bytes, count, unreadable);
      continue;
    }
    if (!fi.isFile()) {
      continue;
    }
    if (!fi.isReadable()) {
      if (unreadable) {
        unreadable->append(rel);
      }
      continue;
    }
    outFiles->append(fi.absoluteFilePath());
    *bytes += fi.size();
    *count += 1;
  }
}

} // namespace

void RescueEngine::appendDebug(const QString &line)
{
  const QString stamped =
      QDateTime::currentDateTimeUtc().toString(QStringLiteral("hh:mm:ss.zzzZ ")) + line;
  m_debugLog.append(stamped);
  if (m_debugLog.size() > 500) {
    m_debugLog.removeFirst();
  }
}

RescueEngine::RescueEngine(QObject *parent)
  : QObject(parent)
{
  appendDebug(QStringLiteral("RescueEngine constructed (pre-alpha debug logging on)"));
}

RescueEngine::~RescueEngine()
{
  cleanupMounts();
}

bool RescueEngine::runHelper(const QStringList &args, QString *error) const
{
  QString local;
  QString *e = error ? error : &local;
  runHelperCapture(args, e);
  return e->isEmpty();
}

QString RescueEngine::runHelperCapture(const QStringList &args, QString *error) const
{
  if (error) {
    error->clear();
  }
  auto runSudo = [&](const QStringList &cmd, QByteArray *stdoutBytes) -> bool {
    QProcess proc;
    proc.start(QStringLiteral("sudo"), cmd);
    if (!proc.waitForStarted(3000)) {
      if (error) {
        *error = QStringLiteral("sudo not available");
      }
      return false;
    }
    if (!proc.waitForFinished(300000)) {
      proc.kill();
      if (error) {
        *error = QStringLiteral("command timed out");
      }
      return false;
    }
    if (stdoutBytes) {
      *stdoutBytes = proc.readAllStandardOutput();
    }
    if (proc.exitCode() != 0) {
      if (error) {
        *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
        if (error->isEmpty()) {
          *error = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
        }
        if (error->isEmpty()) {
          *error = QStringLiteral("exit %1").arg(proc.exitCode());
        }
      }
      return false;
    }
    return true;
  };

  QStringList viaHelper = {QStringLiteral("-n"), QStringLiteral("/usr/lib/spike/spike-rescue-mount")};
  viaHelper.append(args);
  QByteArray out;
  if (runSudo(viaHelper, &out)) {
    return QString::fromUtf8(out);
  }
  const QString helperErr = error ? *error : QString();

  if (args.isEmpty()) {
    return {};
  }
  // Dev fallback only for mount verbs — not for find/copy (needs root for 700 homes).
  if (args.first() == QLatin1String("prepare")) {
    QDir().mkpath(QStringLiteral("/run/spike-rescue"));
    if (error) {
      error->clear();
    }
    return QStringLiteral("ok");
  }
  QStringList plain = {QStringLiteral("-n")};
  if (args.first() == QLatin1String("mount") || args.first() == QLatin1String("mount-rw")) {
    plain << QStringLiteral("mount");
    plain.append(args.mid(1));
  } else if (args.first() == QLatin1String("umount")) {
    plain << QStringLiteral("umount") << args.value(1);
  } else {
    if (error && error->isEmpty()) {
      *error = helperErr;
    }
    return {};
  }
  out.clear();
  if (runSudo(plain, &out)) {
    if (error) {
      error->clear();
    }
    return QString::fromUtf8(out);
  }
  if (error && error->isEmpty()) {
    *error = helperErr;
  }
  return {};
}

QString RescueEngine::blkidType(const QString &device) const
{
  return runCapture(QStringLiteral("blkid"),
                    {QStringLiteral("-o"), QStringLiteral("value"), QStringLiteral("-s"),
                     QStringLiteral("TYPE"), device})
      .trimmed();
}

QString RescueEngine::mountpointsFromJson(const QJsonObject &obj)
{
  if (obj.contains(QStringLiteral("mountpoint"))) {
    const QJsonValue v = obj.value(QStringLiteral("mountpoint"));
    if (v.isString()) {
      return v.toString();
    }
  }
  if (obj.contains(QStringLiteral("mountpoints"))) {
    const QJsonArray arr = obj.value(QStringLiteral("mountpoints")).toArray();
    QStringList mps;
    for (const QJsonValue &v : arr) {
      if (v.isNull() || v.toString().isEmpty()) {
        continue;
      }
      mps.append(v.toString());
    }
    return mps.join(QLatin1Char(','));
  }
  return {};
}

bool RescueEngine::mountRo(const QString &device, const QString &mountPoint, const QString &fstype,
                           const QString &extraOpts, QString *error)
{
  runHelper({QStringLiteral("prepare")}, nullptr);
  QString opts = QStringLiteral("ro");
  if (!extraOpts.isEmpty()) {
    opts += QLatin1Char(',') + extraOpts;
  }

  QStringList args{QStringLiteral("mount")};
  QString type = fstype;
  if (type.isEmpty()) {
    type = blkidType(device);
  }
  if (!type.isEmpty() && type != QLatin1String("crypto_LUKS")) {
    args << QStringLiteral("-t") << type;
  }
  args << QStringLiteral("-o") << opts << device << mountPoint;

  if (runHelper(args, error)) {
    m_ourMounts.append(mountPoint);
    return true;
  }
  // NTFS fallback via ntfs-3g
  if (type.contains(QLatin1String("ntfs"), Qt::CaseInsensitive)) {
    QStringList ntfs{QStringLiteral("mount"), QStringLiteral("-t"), QStringLiteral("ntfs-3g"),
                     QStringLiteral("-o"), QStringLiteral("ro"), device, mountPoint};
    if (runHelper(ntfs, error)) {
      m_ourMounts.append(mountPoint);
      return true;
    }
  }
  return false;
}

bool RescueEngine::mountRwWritable(const QString &device, const QString &mountPoint, QString *error)
{
  runHelper({QStringLiteral("prepare")}, nullptr);
  const QString type = blkidType(device);
  QStringList args{QStringLiteral("mount-rw")};
  if (!type.isEmpty()) {
    args << QStringLiteral("-t") << type;
  }
  args << QStringLiteral("-o") << QStringLiteral("rw") << device << mountPoint;
  if (runHelper(args, error)) {
    if (!m_ourMounts.contains(mountPoint)) {
      m_ourMounts.append(mountPoint);
    }
    return true;
  }
  return false;
}

QString RescueEngine::blkidLabel(const QString &device) const
{
  return runCapture(QStringLiteral("blkid"),
                    {QStringLiteral("-o"), QStringLiteral("value"), QStringLiteral("-s"),
                     QStringLiteral("LABEL"), device})
      .trimmed();
}

QString RescueEngine::deviceByLabel(const QString &label) const
{
  return runCapture(QStringLiteral("blkid"), {QStringLiteral("-L"), label}).trimmed();
}

bool RescueEngine::appendDestIfUsable(QVector<DestVolume> *vols, const QSet<QString> &seenPaths,
                                     const QSet<QString> &sourceDevs, const QString &mp,
                                     const QString &displayLabel) const
{
  if (!vols || mp.isEmpty() || seenPaths.contains(mp)) {
    return false;
  }
  if (mp.contains(QLatin1String("/cdrom")) ||
      mp.startsWith(QLatin1String("/run/live")) ||
      mp.startsWith(QLatin1String("/lib/live"))) {
    return false;
  }
  QStorageInfo info(mp);
  if (!info.isValid() || !info.isReady() || info.isReadOnly()) {
    return false;
  }
  const QString dev = QString::fromUtf8(info.device());
  if (sourceDevs.contains(dev)) {
    return false;
  }
  // Never use the ISO9660 live volume as a destination.
  if (blkidType(dev).contains(QLatin1String("iso9660"), Qt::CaseInsensitive) ||
      blkidLabel(dev).contains(QLatin1String("Spike Live"), Qt::CaseInsensitive)) {
    return false;
  }
  DestVolume d;
  d.path = mp;
  d.device = dev;
  d.freeBytes = info.bytesAvailable();
  if (!displayLabel.isEmpty()) {
    d.label = displayLabel;
  } else {
    const QString lab = blkidLabel(dev);
    d.label = lab.isEmpty() ? QFileInfo(mp).fileName() : lab;
  }
  vols->append(d);
  return true;
}

QString RescueEngine::ensureLiveUsbWritableDest()
{
  // Prefer an already-mounted writable partition (persistence / capture paths).
  // Casper commonly mounts LABEL=writable at /var/log on the live USB.
  const QStringList known = {
      QStringLiteral("/var/log"),
      QStringLiteral("/media/writable"),
      QStringLiteral("/log-persistence"),
      QStringLiteral("/mnt/writable"),
  };
  for (const QString &p : known) {
    QStorageInfo info(p);
    if (!info.isValid() || !info.isReady() || info.isReadOnly() || !QDir(p).exists()) {
      continue;
    }
    const QString dev = QString::fromUtf8(info.device());
    const QString lab = blkidLabel(dev);
    if (lab.compare(QLatin1String("writable"), Qt::CaseInsensitive) == 0 ||
        p == QLatin1String("/media/writable") || p == QLatin1String("/log-persistence") ||
        p == QLatin1String("/mnt/writable")) {
      // Ensure SpikeBackup is creatable by the live user.
      QDir().mkpath(p + QStringLiteral("/SpikeBackup"));
      return p;
    }
  }

  const QString existing =
      runCapture(QStringLiteral("findmnt"),
                 {QStringLiteral("-n"), QStringLiteral("-o"), QStringLiteral("TARGET"),
                  QStringLiteral("-S"), QStringLiteral("LABEL=writable")})
          .trimmed()
          .split(QLatin1Char('\n'))
          .value(0)
          .trimmed();
  if (!existing.isEmpty()) {
    QStorageInfo info(existing);
    if (info.isValid() && info.isReady() && !info.isReadOnly()) {
      QDir().mkpath(existing + QStringLiteral("/SpikeBackup"));
      return existing;
    }
  }

  // Unmounted leftover space on the Spike live USB (LABEL=writable).
  const QString dev = deviceByLabel(QStringLiteral("writable"));
  if (dev.isEmpty()) {
    return {};
  }
  const QString mnt = QStringLiteral("/run/spike-rescue/dest-writable");
  if (QStorageInfo(mnt).isValid() && QStorageInfo(mnt).isReady() && !QStorageInfo(mnt).isReadOnly()) {
    return mnt;
  }
  QString err;
  if (!mountRwWritable(dev, mnt, &err)) {
    return {};
  }
  return mnt;
}

bool RescueEngine::umountPath(const QString &mountPoint)
{
  if (mountPoint.isEmpty()) {
    return true;
  }
  // Only unmount mounts we created under /run/spike-rescue
  if (!mountPoint.startsWith(QLatin1String("/run/spike-rescue/"))) {
    m_ourMounts.removeAll(mountPoint);
    return true;
  }
  QString err;
  const bool ok = runHelper({QStringLiteral("umount"), mountPoint}, &err);
  m_ourMounts.removeAll(mountPoint);
  return ok;
}

void RescueEngine::cleanupMounts()
{
  const QStringList mounts = m_ourMounts;
  for (const QString &m : mounts) {
    umountPath(m);
  }
  m_ourMounts.clear();
}

bool RescueEngine::isLiveDevice(const QString &devName, const QString &label,
                                const QString &mountpoints) const
{
  const QString lower = label.toLower();
  if (lower.contains(QLatin1String("spike live")) || lower == QLatin1String("writable") ||
      lower == QLatin1String("esp")) {
    return true;
  }
  if (mountpoints.contains(QLatin1String("/cdrom")) ||
      mountpoints.contains(QLatin1String("/run/live")) ||
      mountpoints.contains(QLatin1String("/lib/live"))) {
    return true;
  }
  // Casper often uses the USB that also has "writable".
  Q_UNUSED(devName);
  return false;
}

QStringList RescueEngine::categoryDirs(OsKind os) const
{
  if (os == OsKind::MacOS) {
    return {QStringLiteral("Documents"), QStringLiteral("Pictures"), QStringLiteral("Movies"),
            QStringLiteral("Music"),     QStringLiteral("Downloads"), QStringLiteral("Desktop")};
  }
  return {QStringLiteral("Documents"), QStringLiteral("Pictures"), QStringLiteral("Videos"),
          QStringLiteral("Music"),     QStringLiteral("Downloads"), QStringLiteral("Desktop")};
}

void RescueEngine::listUsersUnder(const QString &homeRoot, QStringList *users) const
{
  if (!users) {
    return;
  }
  QStringList names;
  if (homeRoot.startsWith(QLatin1String("/run/spike-rescue/"))) {
    names = privilegedListDirs(homeRoot);
  }
  if (names.isEmpty()) {
    QDir home(homeRoot);
    if (home.exists()) {
      names = home.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }
  }
  for (const QString &u : names) {
    if (u.startsWith(QLatin1Char('.'))) {
      continue;
    }
    if (u == QLatin1String("lost+found") || u == QLatin1String("Public") ||
        u == QLatin1String("Default") || u == QLatin1String("Default User") ||
        u == QLatin1String("All Users") || u == QLatin1String("Shared") ||
        u == QLatin1String("Guest")) {
      continue;
    }
    if (!users->contains(u)) {
      users->append(u);
    }
  }
}

QStringList RescueEngine::privilegedListDirs(const QString &dir) const
{
  QString err;
  const QString out =
      runHelperCapture({QStringLiteral("list-dirs"), dir}, &err);
  if (!err.isEmpty()) {
    return {};
  }
  QStringList names;
  for (const QString &line : out.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    names.append(line.trimmed());
  }
  return names;
}

QStringList RescueEngine::privilegedFindFiles(const QString &dir) const
{
  return privilegedFindFiles(dir, -1);
}

QStringList RescueEngine::privilegedFindFiles(const QString &dir, int maxDepth) const
{
  QString err;
  QStringList args{QStringLiteral("find-files"), dir};
  if (maxDepth >= 0) {
    args << QString::number(maxDepth);
  }
  const QString out = runHelperCapture(args, &err);
  if (!err.isEmpty()) {
    return {};
  }
  QStringList files;
  for (const QString &line : out.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    const QString p = line.trimmed();
    if (!p.isEmpty()) {
      files.append(p);
    }
  }
  return files;
}

bool RescueEngine::privilegedCopyFile(const QString &src, const QString &dst, QString *error) const
{
  return runHelper({QStringLiteral("copy-file"), src, dst}, error);
}

QByteArray RescueEngine::privilegedSha256(const QString &path, bool *ok, QString *error) const
{
  QString err;
  const QString hex =
      runHelperCapture({QStringLiteral("sha256"), path}, error ? error : &err).trimmed();
  if ((error && !error->isEmpty()) || (!error && !err.isEmpty()) || hex.size() != 64) {
    if (ok) {
      *ok = false;
    }
    if (error && error->isEmpty() && !err.isEmpty()) {
      *error = err;
    }
    if (error && error->isEmpty()) {
      *error = QStringLiteral("sha256 helper returned no digest");
    }
    return {};
  }
  if (ok) {
    *ok = true;
  }
  return QByteArray::fromHex(hex.toLatin1());
}

QByteArray RescueEngine::sha256Path(const QString &path, bool *ok, QString *errDetail) const
{
  QByteArray direct = sha256File(path, ok);
  if (ok && *ok) {
    return direct;
  }
  if (path.startsWith(QLatin1String("/run/spike-rescue/"))) {
    return privilegedSha256(path, ok, errDetail);
  }
  if (errDetail) {
    *errDetail = QStringLiteral("cannot open source for hashing");
  }
  return {};
}

void RescueEngine::enrichBtrfsHome(DetectedSystem *sys)
{
  if (!sys || sys->partition.fstype != QLatin1String("btrfs")) {
    return;
  }
  if (!sys->users.isEmpty()) {
    return;
  }
  // Fedora/Ubuntu-style layouts: personal files often on subvol=home or @home.
  const QStringList subvols = {QStringLiteral("home"), QStringLiteral("@home"),
                               QStringLiteral("Home"), QStringLiteral("var/home")};
  for (const QString &sub : subvols) {
    const QString mnt = QStringLiteral("/run/spike-rescue/home-%1")
                            .arg(QUuid::createUuid().toString(QUuid::Id128));
    QString err;
    if (!mountRo(sys->partition.path, mnt, QStringLiteral("btrfs"),
                 QStringLiteral("subvol=%1").arg(sub), &err)) {
      continue;
    }
    QStringList users;
    listUsersUnder(mnt, &users);
    // Sometimes the subvol root *is* a single user home (rare) — treat dirs as users.
    if (users.isEmpty()) {
      // If Documents exists at subvol root, invent a synthetic user "."
      if (QDir(mnt + QStringLiteral("/Documents")).exists() ||
          QDir(mnt + QStringLiteral("/Pictures")).exists()) {
        users.append(QStringLiteral("."));
      }
    }
    if (!users.isEmpty()) {
      sys->homeMountPoint = mnt;
      sys->users = users;
      return;
    }
    umountPath(mnt);
  }
}

DetectedSystem RescueEngine::probeMounted(const BlockPartition &part, const QString &mnt)
{
  DetectedSystem sys;
  sys.partition = part;
  sys.mountPoint = mnt;

  if (QFile::exists(mnt + QStringLiteral("/etc/os-release"))) {
    QFile release(mnt + QStringLiteral("/etc/os-release"));
    QString text;
    if (release.open(QIODevice::ReadOnly | QIODevice::Text)) {
      text = QString::fromUtf8(release.readAll());
    }
    if (text.contains(QLatin1String("Spike"), Qt::CaseInsensitive) ||
        text.contains(QLatin1String("ID=spike"))) {
      sys.os = OsKind::SpikeLinux;
    } else {
      sys.os = OsKind::Linux;
    }
    for (const QString &line : text.split(QLatin1Char('\n'))) {
      if (line.startsWith(QLatin1String("PRETTY_NAME="))) {
        sys.osLabel = line.mid(12).trimmed();
        if (sys.osLabel.startsWith(QLatin1Char('"')) && sys.osLabel.endsWith(QLatin1Char('"'))) {
          sys.osLabel = sys.osLabel.mid(1, sys.osLabel.size() - 2);
        }
        break;
      }
    }
    if (sys.osLabel.isEmpty()) {
      sys.osLabel = osKindLabel(sys.os);
    }
    listUsersUnder(mnt + QStringLiteral("/home"), &sys.users);
  } else if (QFile::exists(mnt + QStringLiteral("/Windows/System32")) ||
             QFile::exists(mnt + QStringLiteral("/windows/system32"))) {
    sys.os = OsKind::Windows;
    sys.osLabel = QStringLiteral("Windows");
    listUsersUnder(mnt + QStringLiteral("/Users"), &sys.users);
  } else if (QFile::exists(mnt + QStringLiteral("/System/Library/CoreServices"))) {
    sys.os = OsKind::MacOS;
    sys.osLabel = QStringLiteral("macOS");
    listUsersUnder(mnt + QStringLiteral("/Users"), &sys.users);
  } else if (QDir(mnt + QStringLiteral("/home")).exists() ||
             QDir(mnt + QStringLiteral("/Users")).exists()) {
    sys.os = OsKind::Linux;
    sys.osLabel = QStringLiteral("Linux (home found)");
    listUsersUnder(mnt + QStringLiteral("/home"), &sys.users);
    if (sys.users.isEmpty()) {
      listUsersUnder(mnt + QStringLiteral("/Users"), &sys.users);
    }
  } else {
    // btrfs default subvol may be empty top-level — try common root subvols next in scan.
    sys.os = OsKind::Unknown;
    return sys;
  }
  return sys;
}

void RescueEngine::scanSystems()
{
  m_cancel = false;
  m_lastError.clear();
  m_scanSummary.clear();
  m_debugLog.clear();
  appendDebug(QStringLiteral("scanSystems: start"));
  cleanupMounts();
  m_systems.clear();
  emit scanProgress(QStringLiteral("Listing block devices…"), 5);

  const QString json = runCapture(
      QStringLiteral("lsblk"),
      {QStringLiteral("-J"), QStringLiteral("-b"), QStringLiteral("-o"),
       QStringLiteral("NAME,PATH,TYPE,FSTYPE,LABEL,SIZE,MOUNTPOINT,MOUNTPOINTS,RM")});
  if (json.isEmpty()) {
    m_lastError = QStringLiteral("lsblk failed");
    m_scanSummary = m_lastError;
    emit scanFinished(false);
    return;
  }

  const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QVector<BlockPartition> parts;

  std::function<void(const QJsonObject &, bool)> walk;
  walk = [&](const QJsonObject &obj, bool parentRm) {
    const QString type = obj.value(QStringLiteral("type")).toString();
    const bool rm = obj.value(QStringLiteral("rm")).toBool(false) || parentRm;
    if (type == QLatin1String("part") || type == QLatin1String("crypt") ||
        type == QLatin1String("lvm")) {
      BlockPartition p;
      p.name = obj.value(QStringLiteral("name")).toString();
      p.path = obj.value(QStringLiteral("path")).toString();
      if (p.path.isEmpty()) {
        p.path = QStringLiteral("/dev/") + p.name;
      }
      p.fstype = obj.value(QStringLiteral("fstype")).toString();
      if (p.fstype.isEmpty()) {
        p.fstype = blkidType(p.path);
      }
      p.label = obj.value(QStringLiteral("label")).toString();
      p.sizeBytes = parseSize(obj.value(QStringLiteral("size")).toVariant().toString());
      if (p.sizeBytes == 0) {
        p.sizeBytes = obj.value(QStringLiteral("size")).toVariant().toLongLong();
      }
      p.sizeHuman = QStringLiteral("%1 GB").arg(p.sizeBytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
      p.removable = rm;
      const QString mp = mountpointsFromJson(obj);
      p.isLiveMedium = isLiveDevice(p.name, p.label, mp);
      if (p.fstype != QLatin1String("swap") && p.fstype != QLatin1String("crypto_LUKS") &&
          !p.isLiveMedium && !p.path.isEmpty()) {
        // Allow empty fstype — mount will blkid.
        parts.append(p);
      }
    }
    const QJsonArray children = obj.value(QStringLiteral("children")).toArray();
    for (const QJsonValue &c : children) {
      walk(c.toObject(), rm);
    }
  };

  const QJsonArray blockdevices = doc.object().value(QStringLiteral("blockdevices")).toArray();
  for (const QJsonValue &v : blockdevices) {
    walk(v.toObject(), false);
  }

  int mountFails = 0;
  int probed = 0;
  QStringList failHints;
  const int total = parts.size();
  int i = 0;
  for (const BlockPartition &part : parts) {
    if (m_cancel) {
      break;
    }
    ++i;
    emit scanProgress(QStringLiteral("Checking %1…").arg(part.path),
                      total ? (10 + (80 * i) / total) : 90);

    // Reuse an existing mount if the partition is already mounted (and not live).
    const QString existingMp = runCapture(QStringLiteral("findmnt"),
                                          {QStringLiteral("-n"), QStringLiteral("-o"),
                                           QStringLiteral("TARGET"), QStringLiteral("-S"),
                                           part.path})
                                   .trimmed()
                                   .split(QLatin1Char('\n'))
                                   .value(0)
                                   .trimmed();

    QString mnt;
    bool weMounted = false;
    QStringList tryExtra = {QString()};
    if (part.fstype == QLatin1String("btrfs")) {
      tryExtra << QStringLiteral("subvol=root") << QStringLiteral("subvol=@")
               << QStringLiteral("subvol=@root");
    }

    DetectedSystem sys;
    sys.os = OsKind::Unknown;

    if (!existingMp.isEmpty() && QDir(existingMp).exists() &&
        !existingMp.startsWith(QLatin1String("/cdrom")) &&
        !existingMp.startsWith(QLatin1String("/run/live"))) {
      sys = probeMounted(part, existingMp);
      mnt = existingMp;
    }

    if (sys.os == OsKind::Unknown) {
      for (const QString &extra : tryExtra) {
        mnt = QStringLiteral("/run/spike-rescue/src-%1")
                  .arg(QUuid::createUuid().toString(QUuid::Id128));
        QString err;
        if (!mountRo(part.path, mnt, part.fstype, extra, &err)) {
          if (failHints.size() < 5) {
            failHints.append(QStringLiteral("%1: %2").arg(part.path, err));
          }
          ++mountFails;
          continue;
        }
        weMounted = true;
        ++probed;
        sys = probeMounted(part, mnt);
        if (sys.os != OsKind::Unknown) {
          break;
        }
        umountPath(mnt);
        weMounted = false;
        mnt.clear();
      }
    } else {
      ++probed;
    }

    if (sys.os == OsKind::Unknown) {
      if (weMounted) {
        umountPath(mnt);
      }
      continue;
    }

    enrichBtrfsHome(&sys);
    // Keep systems even with zero users (still recoverable after manual browse later).
    Inventory inv;
    if (!sys.users.isEmpty() || !sys.homeMountPoint.isEmpty() ||
        QDir(sys.mountPoint + QStringLiteral("/home")).exists()) {
      inv = buildInventory(sys);
    }
    sys.fileCount = inv.fileCount;
    sys.byteTotal = inv.byteTotal;

    if (weMounted) {
      // Drop temporary mounts; remount on inventory/copy.
      if (!sys.homeMountPoint.isEmpty() &&
          sys.homeMountPoint.startsWith(QLatin1String("/run/spike-rescue/"))) {
        umountPath(sys.homeMountPoint);
        sys.homeMountPoint.clear();
      }
      umountPath(mnt);
      sys.mountPoint.clear();
    } else {
      // Existing system mount — do not unmount; clear so copy remounts RO privately.
      sys.mountPoint.clear();
      sys.homeMountPoint.clear();
    }

    m_systems.append(sys);
  }

  m_scanSummary =
      QStringLiteral("Partitions considered: %1. Mounted/probed: %2. Systems found: %3. "
                     "Mount failures: %4.")
          .arg(total)
          .arg(probed)
          .arg(m_systems.size())
          .arg(mountFails);
  if (!failHints.isEmpty() && m_systems.isEmpty()) {
    m_scanSummary += QStringLiteral("\nExamples:\n  ") + failHints.join(QStringLiteral("\n  "));
    m_scanSummary +=
        QStringLiteral("\n(Need passwordless /usr/lib/spike/spike-rescue-mount via sudoers.)");
  }
  appendDebug(m_scanSummary);
  for (const DetectedSystem &s : m_systems) {
    appendDebug(QStringLiteral("found: %1 on %2 (%3) users=%4")
                    .arg(s.osLabel, s.partition.path, s.partition.fstype,
                         s.users.join(QLatin1Char(','))));
  }

  emit scanProgress(QStringLiteral("Scan complete"), 100);
  emit scanFinished(true);
}

Inventory RescueEngine::buildInventory(const DetectedSystem &sys)
{
  Inventory inv;
  QHash<QString, CategorySummary> cats;
  for (const QString &name : categoryDirs(sys.os)) {
    CategorySummary c;
    c.name = name;
    cats.insert(name, c);
  }

  QString homeRoot;
  if (!sys.homeMountPoint.isEmpty()) {
    homeRoot = sys.homeMountPoint;
  } else if (sys.os == OsKind::Windows || sys.os == OsKind::MacOS) {
    homeRoot = sys.mountPoint + QStringLiteral("/Users");
  } else {
    homeRoot = sys.mountPoint + QStringLiteral("/home");
  }

  const bool usePriv = homeRoot.startsWith(QLatin1String("/run/spike-rescue/")) ||
                       sys.mountPoint.startsWith(QLatin1String("/run/spike-rescue/"));

  for (const QString &user : sys.users) {
    const QString userHome = (user == QLatin1String("."))
                                 ? homeRoot
                                 : (homeRoot + QLatin1Char('/') + user);
    for (const QString &cat : categoryDirs(sys.os)) {
      const QString dir = userHome + QLatin1Char('/') + cat;
      QStringList files;
      qint64 bytes = 0;
      qint64 count = 0;
      if (usePriv) {
        for (const QString &path : privilegedFindFiles(dir)) {
          QFileInfo fi(path);
          if (!fi.isFile() || pathLooksHidden(fi.fileName())) {
            continue;
          }
          files.append(path);
          bytes += fi.size();
          count += 1;
        }
      } else {
        walkFiles(dir, cat + QLatin1Char('/') + user, &files, &bytes, &count, &inv.unreadable);
      }
      CategorySummary &cs = cats[cat];
      cs.files += count;
      cs.bytes += bytes;
      inv.files.append(files);
      inv.fileCount += count;
      inv.byteTotal += bytes;
    }
    // Root-of-home office docs (Linux/Spike) — shallow.
    if (sys.os == OsKind::SpikeLinux || sys.os == OsKind::Linux) {
      const QStringList globs = {QStringLiteral("*.odt"), QStringLiteral("*.ods"),
                                 QStringLiteral("*.odp"), QStringLiteral("*.pdf"),
                                 QStringLiteral("*.docx"), QStringLiteral("*.xlsx")};
      if (usePriv) {
        for (const QString &path : privilegedFindFiles(userHome, 1)) {
          QFileInfo fi(path);
          const QString name = fi.fileName();
          bool match = false;
          for (const QString &g : globs) {
            if (QDir::match(g, name)) {
              match = true;
              break;
            }
          }
          if (!match || name.startsWith(QLatin1Char('.'))) {
            continue;
          }
          inv.files.append(path);
          inv.fileCount += 1;
          inv.byteTotal += fi.size();
        }
      } else {
        QDir home(userHome);
        for (const QFileInfo &fi : home.entryInfoList(globs, QDir::Files)) {
          if (fi.fileName().startsWith(QLatin1Char('.'))) {
            continue;
          }
          inv.files.append(fi.absoluteFilePath());
          inv.fileCount += 1;
          inv.byteTotal += fi.size();
        }
      }
    }
  }

  for (const QString &name : categoryDirs(sys.os)) {
    inv.categories.append(cats.value(name));
  }
  return inv;
}

void RescueEngine::inventorySystem(int systemIndex)
{
  m_lastError.clear();
  m_inventory = {};
  if (systemIndex < 0 || systemIndex >= m_systems.size()) {
    m_lastError = QStringLiteral("invalid system index");
    emit inventoryFinished(false);
    return;
  }
  DetectedSystem sys = m_systems.at(systemIndex);
  QStringList tryExtra = {QString()};
  if (sys.partition.fstype == QLatin1String("btrfs")) {
    tryExtra << QStringLiteral("subvol=root") << QStringLiteral("subvol=@")
             << QStringLiteral("subvol=@root");
  }
  QString err;
  bool mounted = false;
  for (const QString &extra : tryExtra) {
    const QString mnt =
        QStringLiteral("/run/spike-rescue/src-%1").arg(QUuid::createUuid().toString(QUuid::Id128));
    if (!mountRo(sys.partition.path, mnt, sys.partition.fstype, extra, &err)) {
      continue;
    }
    DetectedSystem probed = probeMounted(sys.partition, mnt);
    if (probed.os == OsKind::Unknown) {
      umountPath(mnt);
      continue;
    }
    probed.osLabel = sys.osLabel.isEmpty() ? probed.osLabel : sys.osLabel;
    enrichBtrfsHome(&probed);
    sys = probed;
    mounted = true;
    break;
  }
  if (!mounted) {
    m_lastError = QStringLiteral("mount failed: %1").arg(err);
    emit inventoryFinished(false);
    return;
  }
  m_systems[systemIndex] = sys;
  m_inventory = buildInventory(sys);
  m_systems[systemIndex].fileCount = m_inventory.fileCount;
  m_systems[systemIndex].byteTotal = m_inventory.byteTotal;
  emit inventoryFinished(true);
}

void RescueEngine::refreshDestinations()
{
  QVector<DestVolume> vols;
  QSet<QString> seenPaths;
  QSet<QString> sourceDevs;
  for (const DetectedSystem &s : m_systems) {
    sourceDevs.insert(s.partition.path);
    QString disk = s.partition.path;
    disk.remove(QRegularExpression(QStringLiteral("[0-9]+$")));
    sourceDevs.insert(disk);
  }

  // Spike live USB leftover partition (LABEL=writable) — often unmounted until we attach it.
  const QString liveWritable = ensureLiveUsbWritableDest();
  if (!liveWritable.isEmpty()) {
    appendDebug(QStringLiteral("live writable dest resolved: %1").arg(liveWritable));
    if (appendDestIfUsable(&vols, seenPaths, sourceDevs, liveWritable,
                           QStringLiteral("This Spike USB (writable)"))) {
      seenPaths.insert(liveWritable);
    }
  } else {
    appendDebug(QStringLiteral("live writable dest: (none)"));
  }

  QStringList roots;
  roots << QStringLiteral("/run/media") << QStringLiteral("/media");

  for (const QString &root : roots) {
    QDir base(root);
    if (!base.exists()) {
      continue;
    }
    for (const QString &user : base.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QDir userDir(base.filePath(user));
      for (const QString &vol : userDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString mp = userDir.filePath(vol);
        if (mp.contains(QLatin1String("spike-rescue"))) {
          continue;
        }
        // Skip ISO9660 live volume by name; writable on the same stick is allowed above.
        if (vol.contains(QLatin1String("Spike Live"), Qt::CaseInsensitive)) {
          continue;
        }
        QString label;
        if (vol.compare(QLatin1String("writable"), Qt::CaseInsensitive) == 0 ||
            mp == QLatin1String("/var/log")) {
          label = QStringLiteral("This Spike USB (writable)");
        }
        QStorageInfo info(mp);
        appendDebug(QStringLiteral("dest candidate %1 -> %2 free=%3")
                        .arg(vol, mp, QString::number(info.bytesAvailable())));
        if (appendDestIfUsable(&vols, seenPaths, sourceDevs, mp, label)) {
          seenPaths.insert(mp);
        }
      }
    }
  }
  emit destinationsChanged(vols);
  appendDebug(QStringLiteral("refreshDestinations: %1 usable volume(s)").arg(vols.size()));
}

QByteArray RescueEngine::sha256File(const QString &path, bool *ok)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
    // Instance method needed for privilege — static path kept for dst hashes.
    if (ok) {
      *ok = false;
    }
    return {};
  }
  QCryptographicHash hash(QCryptographicHash::Sha256);
  while (!f.atEnd()) {
    const QByteArray chunk = f.read(65536);
    if (chunk.isEmpty() && !f.atEnd()) {
      if (ok) {
        *ok = false;
      }
      return {};
    }
    hash.addData(chunk);
  }
  if (ok) {
    *ok = true;
  }
  return hash.result();
}

static QString classifyHelperError(const QString &err)
{
  const QString e = err.toLower();
  if (e.contains(QLatin1String("destination not allowed")) ||
      e.contains(QLatin1String("refusing write"))) {
    return QStringLiteral("write");
  }
  if (e.contains(QLatin1String("not a file")) || e.contains(QLatin1String("must be under"))) {
    return QStringLiteral("read");
  }
  if (e.contains(QLatin1String("no space")) || e.contains(QLatin1String("read-only")) ||
      e.contains(QLatin1String("permission denied")) || e.contains(QLatin1String("mkdir"))) {
    return QStringLiteral("write");
  }
  return {};
}

bool RescueEngine::copyOneFile(const QString &src, const QString &dst, QString *errKind,
                               QString *errDetail)
{
  auto setErr = [&](const QString &kind, const QString &detail) {
    if (errKind) {
      *errKind = kind;
    }
    if (errDetail) {
      *errDetail = detail;
    }
  };

  for (int attempt = 0; attempt < 3; ++attempt) {
    bool okSrc = false;
    QString hashDetail;
    const QByteArray srcHash = sha256Path(src, &okSrc, &hashDetail);
    if (!okSrc) {
      setErr(QStringLiteral("read"),
             hashDetail.isEmpty() ? QStringLiteral("cannot hash source") : hashDetail);
      continue;
    }

    QFile in(src);
    const bool srcOpen = in.open(QIODevice::ReadOnly);
    if (srcOpen) {
      QDir().mkpath(QFileInfo(dst).absolutePath());
      QFile out(dst);
      if (out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        bool writeOk = true;
        QString writeDetail;
        while (!in.atEnd()) {
          const QByteArray chunk = in.read(65536);
          if (chunk.isEmpty() && !in.atEnd()) {
            writeOk = false;
            writeDetail = QStringLiteral("read truncated while copying");
            break;
          }
          if (out.write(chunk) != chunk.size()) {
            writeOk = false;
            writeDetail = QStringLiteral("write incomplete (disk full?)");
            break;
          }
        }
        out.close();
        in.close();
        if (writeOk) {
          bool okDst = false;
          const QByteArray dstHash = sha256File(dst, &okDst);
          if (!okDst || dstHash != srcHash) {
            setErr(QStringLiteral("verify"),
                   okDst ? QStringLiteral("SHA256 mismatch after copy")
                         : QStringLiteral("cannot hash destination"));
            continue;
          }
          return true;
        }
        setErr(QStringLiteral("write"), writeDetail);
        // Fall through to privileged retry.
      } else {
        in.close();
        setErr(QStringLiteral("write"),
               QStringLiteral("cannot create destination file (permission/path)"));
      }
    } else {
      setErr(QStringLiteral("read"), QStringLiteral("cannot open source as user"));
    }

    QString helperErr;
    if (privilegedCopyFile(src, dst, &helperErr)) {
      bool okDst = false;
      const QByteArray dstHash = sha256File(dst, &okDst);
      if (!okDst || dstHash != srcHash) {
        setErr(QStringLiteral("verify"),
               okDst ? QStringLiteral("SHA256 mismatch after privileged copy")
                     : QStringLiteral("cannot hash destination after privileged copy"));
        continue;
      }
      return true;
    }
    const QString classified = classifyHelperError(helperErr);
    if (!classified.isEmpty()) {
      setErr(classified, helperErr);
    } else if (!helperErr.isEmpty()) {
      // Keep prior kind if set; attach helper detail.
      if (errDetail) {
        *errDetail = helperErr;
      }
      if (errKind && errKind->isEmpty()) {
        *errKind = QStringLiteral("write");
      }
    } else {
      setErr(QStringLiteral("write"), QStringLiteral("privileged copy failed (no detail)"));
    }
  }
  return false;
}

void RescueEngine::startCopy(int systemIndex, const QString &destMount)
{
  m_cancel = false;
  m_lastCopy = {};
  m_lastError.clear();
  appendDebug(QStringLiteral("startCopy: systemIndex=%1 destMount=%2")
                  .arg(systemIndex)
                  .arg(destMount));

  if (systemIndex < 0 || systemIndex >= m_systems.size()) {
    m_lastError = QStringLiteral("invalid system");
    appendDebug(QStringLiteral("startCopy: FAIL %1").arg(m_lastError));
    emit copyFinished(false);
    return;
  }
  if (destMount.isEmpty() || !QDir(destMount).exists()) {
    m_lastError = QStringLiteral("destination not mounted");
    appendDebug(QStringLiteral("startCopy: FAIL %1").arg(m_lastError));
    emit copyFinished(false);
    return;
  }

  {
    QStorageInfo info(destMount);
    appendDebug(QStringLiteral("dest device=%1 free=%2 label_guess=%3")
                    .arg(QString::fromUtf8(info.device()), QString::number(info.bytesAvailable()),
                         blkidLabel(QString::fromUtf8(info.device()))));
  }

  DetectedSystem &sys = m_systems[systemIndex];
  appendDebug(QStringLiteral("source part=%1 fstype=%2 os=%3")
                  .arg(sys.partition.path, sys.partition.fstype, sys.osLabel));
  if (sys.mountPoint.isEmpty() || !QDir(sys.mountPoint).exists()) {
    QStringList tryExtra = {QString()};
    if (sys.partition.fstype == QLatin1String("btrfs")) {
      tryExtra << QStringLiteral("subvol=root") << QStringLiteral("subvol=@")
               << QStringLiteral("subvol=@root");
    }
    QString err;
    bool mounted = false;
    for (const QString &extra : tryExtra) {
      const QString mnt =
          QStringLiteral("/run/spike-rescue/src-%1").arg(QUuid::createUuid().toString(QUuid::Id128));
      appendDebug(QStringLiteral("remount try extra='%1' -> %2").arg(extra, mnt));
      if (!mountRo(sys.partition.path, mnt, sys.partition.fstype, extra, &err)) {
        appendDebug(QStringLiteral("remount failed: %1").arg(err));
        continue;
      }
      DetectedSystem probed = probeMounted(sys.partition, mnt);
      if (probed.os == OsKind::Unknown) {
        umountPath(mnt);
        continue;
      }
      probed.osLabel = sys.osLabel.isEmpty() ? probed.osLabel : sys.osLabel;
      enrichBtrfsHome(&probed);
      sys = probed;
      mounted = true;
      appendDebug(QStringLiteral("remounted ok mount=%1 homeMount=%2 users=%3")
                      .arg(sys.mountPoint, sys.homeMountPoint, sys.users.join(QLatin1Char(','))));
      break;
    }
    if (!mounted) {
      m_lastError = QStringLiteral("remount failed: %1").arg(err);
      appendDebug(QStringLiteral("startCopy: FAIL %1").arg(m_lastError));
      m_lastCopy.debugLog = m_debugLog;
      emit copyFinished(false);
      return;
    }
    m_inventory = buildInventory(sys);
    appendDebug(QStringLiteral("inventory rebuilt: files=%1 bytes=%2")
                    .arg(m_inventory.fileCount)
                    .arg(m_inventory.byteTotal));
  } else {
    appendDebug(QStringLiteral("reusing mounts mount=%1 homeMount=%2")
                    .arg(sys.mountPoint, sys.homeMountPoint));
  }

  const QString stamp =
      QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMdd-HHmmss"));
  QString labelPart = sys.osLabel;
  labelPart.replace(QLatin1Char('/'), QLatin1Char('-'));
  labelPart.replace(QLatin1Char('\\'), QLatin1Char('-'));
  if (labelPart.isEmpty()) {
    labelPart = QStringLiteral("recovered");
  }
  const QString destRoot =
      destMount + QStringLiteral("/SpikeBackup/") + stamp + QLatin1Char('/') + labelPart;
  QString mkErr;
  if (!runHelper({QStringLiteral("mkdir-dest"), destRoot}, &mkErr)) {
    if (!QDir().mkpath(destRoot)) {
      m_lastError = QStringLiteral("cannot create destination: %1").arg(mkErr);
      appendDebug(QStringLiteral("startCopy: FAIL %1").arg(m_lastError));
      m_lastCopy.debugLog = m_debugLog;
      emit copyFinished(false);
      return;
    }
    appendDebug(QStringLiteral("mkdir-dest helper failed (%1); QDir::mkpath ok").arg(mkErr));
  } else {
    appendDebug(QStringLiteral("mkdir-dest ok: %1").arg(destRoot));
  }
  m_lastCopy.destRoot = destRoot;

  const qint64 totalFiles = m_inventory.files.size();
  const qint64 totalBytes = m_inventory.byteTotal;
  qint64 doneFiles = 0;
  qint64 doneBytes = 0;
  appendDebug(QStringLiteral("copy loop: totalFiles=%1 totalBytes=%2").arg(totalFiles).arg(totalBytes));

  for (const QString &src : m_inventory.files) {
    if (m_cancel) {
      m_lastCopy.cancelled = true;
      appendDebug(QStringLiteral("copy cancelled by user"));
      break;
    }
    QString rel = src;
    if (!sys.homeMountPoint.isEmpty() && rel.startsWith(sys.homeMountPoint)) {
      rel = QStringLiteral("home") + rel.mid(sys.homeMountPoint.size());
    } else if (rel.startsWith(sys.mountPoint)) {
      rel = rel.mid(sys.mountPoint.size());
    }
    if (rel.startsWith(QLatin1Char('/'))) {
      rel = rel.mid(1);
    }
    const QString dst = destRoot + QLatin1Char('/') + rel;
    emit copyProgress(rel, doneFiles, totalFiles, doneBytes, totalBytes);

    QString errKind;
    QString errDetail;
    if (!copyOneFile(src, dst, &errKind, &errDetail)) {
      const QString line =
          QStringLiteral("%1 — %2%3")
              .arg(rel, errKind.isEmpty() ? QStringLiteral("error") : errKind,
                   errDetail.isEmpty() ? QString()
                                       : (QStringLiteral(": ") + errDetail));
      if (m_lastCopy.failureDetails.size() < 40) {
        m_lastCopy.failureDetails.append(line);
      }
      appendDebug(QStringLiteral("FAIL %1 src=%2 dst=%3").arg(line, src, dst));
      if (errKind == QLatin1String("verify")) {
        m_lastCopy.failedVerify++;
        m_lastCopy.verifyFails.append(rel);
      } else if (errKind == QLatin1String("write")) {
        m_lastCopy.failedWrite++;
      } else {
        m_lastCopy.failedRead++;
      }
    } else {
      m_lastCopy.copied++;
      doneBytes += QFileInfo(src).size();
    }
    doneFiles++;
    emit copyProgress(rel, doneFiles, totalFiles, doneBytes, totalBytes);
  }

  m_lastCopy.bytesCopied = doneBytes;
  appendDebug(QStringLiteral("copy done: ok=%1 readFail=%2 writeFail=%3 verifyFail=%4")
                  .arg(m_lastCopy.copied)
                  .arg(m_lastCopy.failedRead)
                  .arg(m_lastCopy.failedWrite)
                  .arg(m_lastCopy.failedVerify));

  // Always write a debug report next to the backup (pre-alpha).
  {
    const QString reportPath = destRoot + QStringLiteral("/REPORT.txt");
    QFile report(reportPath);
    QString body;
    body += QStringLiteral("Spike Rescue REPORT (pre-alpha debug)\n");
    body += QStringLiteral("UTC: %1\n")
                .arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    body += QStringLiteral("destMount: %1\n").arg(destMount);
    body += QStringLiteral("destRoot: %1\n").arg(destRoot);
    body += QStringLiteral("source: %1 (%2) %3\n")
                .arg(sys.partition.path, sys.partition.fstype, sys.osLabel);
    body += QStringLiteral("mountPoint: %1\n").arg(sys.mountPoint);
    body += QStringLiteral("homeMountPoint: %1\n").arg(sys.homeMountPoint);
    body += QStringLiteral("copied=%1 failedRead=%2 failedWrite=%3 failedVerify=%4 bytes=%5\n")
                .arg(m_lastCopy.copied)
                .arg(m_lastCopy.failedRead)
                .arg(m_lastCopy.failedWrite)
                .arg(m_lastCopy.failedVerify)
                .arg(m_lastCopy.bytesCopied);
    body += QStringLiteral("\n--- failure details ---\n");
    for (const QString &f : m_lastCopy.failureDetails) {
      body += f + QLatin1Char('\n');
    }
    body += QStringLiteral("\n--- debug log ---\n");
    for (const QString &d : m_debugLog) {
      body += d + QLatin1Char('\n');
    }
    if (report.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
      report.write(body.toUtf8());
      report.close();
      appendDebug(QStringLiteral("wrote %1").arg(reportPath));
    } else {
      appendDebug(QStringLiteral("could not write REPORT.txt as user: %1").arg(report.errorString()));
      runHelper({QStringLiteral("prepare")}, nullptr);
      const QString tmp =
          QStringLiteral("/run/spike-rescue/report-%1.txt").arg(stamp);
      QFile tf(tmp);
      if (tf.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        tf.write(body.toUtf8());
        tf.close();
        QString err;
        if (privilegedCopyFile(tmp, reportPath, &err)) {
          appendDebug(QStringLiteral("wrote REPORT.txt via privileged copy"));
        } else {
          appendDebug(QStringLiteral("privileged REPORT copy failed: %1").arg(err));
        }
        QFile::remove(tmp);
      }
    }
  }

  m_lastCopy.debugLog = m_debugLog;

  if (!sys.homeMountPoint.isEmpty() &&
      sys.homeMountPoint.startsWith(QLatin1String("/run/spike-rescue/"))) {
    umountPath(sys.homeMountPoint);
    sys.homeMountPoint.clear();
  }
  if (!sys.mountPoint.isEmpty() &&
      sys.mountPoint.startsWith(QLatin1String("/run/spike-rescue/"))) {
    umountPath(sys.mountPoint);
    sys.mountPoint.clear();
  }
  emit copyFinished(true);
}

void RescueEngine::requestCancel()
{
  m_cancel = true;
}

} // namespace spike
