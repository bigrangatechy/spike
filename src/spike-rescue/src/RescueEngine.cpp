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
      dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable,
                        QDir::Name);
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

RescueEngine::RescueEngine(QObject *parent)
  : QObject(parent)
{
}

RescueEngine::~RescueEngine()
{
  cleanupMounts();
}

bool RescueEngine::runPrivileged(const QStringList &args, QString *error) const
{
  QProcess proc;
  QStringList full = {QStringLiteral("-n")};
  full.append(args);
  proc.start(QStringLiteral("sudo"), full);
  if (!proc.waitForStarted(3000)) {
    if (error) {
      *error = QStringLiteral("sudo not available");
    }
    return false;
  }
  if (!proc.waitForFinished(120000)) {
    proc.kill();
    if (error) {
      *error = QStringLiteral("command timed out");
    }
    return false;
  }
  if (proc.exitCode() != 0) {
    if (error) {
      *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
      if (error->isEmpty()) {
        *error = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
      }
    }
    return false;
  }
  return true;
}

bool RescueEngine::mountRo(const QString &device, const QString &mountPoint, const QString &fstype,
                           QString *error)
{
  QDir().mkpath(mountPoint);
  QStringList args{QStringLiteral("mount"), QStringLiteral("-o"), QStringLiteral("ro,ro")};
  if (!fstype.isEmpty() && fstype != QLatin1String("crypto_LUKS")) {
    args << QStringLiteral("-t") << fstype;
  }
  args << device << mountPoint;
  if (!runPrivileged(args, error)) {
    // NTFS often needs ntfs-3g explicitly.
    if (fstype.contains(QLatin1String("ntfs"), Qt::CaseInsensitive)) {
      QStringList ntfs{QStringLiteral("mount"), QStringLiteral("-t"), QStringLiteral("ntfs-3g"),
                       QStringLiteral("-o"), QStringLiteral("ro"), device, mountPoint};
      if (runPrivileged(ntfs, error)) {
        m_ourMounts.append(mountPoint);
        return true;
      }
    }
    return false;
  }
  m_ourMounts.append(mountPoint);
  return true;
}

bool RescueEngine::umountPath(const QString &mountPoint)
{
  QString err;
  const bool ok =
      runPrivileged({QStringLiteral("umount"), mountPoint}, &err) ||
      runPrivileged({QStringLiteral("umount"), QStringLiteral("-l"), mountPoint}, &err);
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
    QDir home(mnt + QStringLiteral("/home"));
    for (const QString &u :
         home.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      if (!u.startsWith(QLatin1Char('.'))) {
        sys.users.append(u);
      }
    }
  } else if (QFile::exists(mnt + QStringLiteral("/Windows/System32")) ||
             QFile::exists(mnt + QStringLiteral("/windows/system32"))) {
    sys.os = OsKind::Windows;
    sys.osLabel = QStringLiteral("Windows");
    QDir users(mnt + QStringLiteral("/Users"));
    for (const QString &u : users.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      if (u != QLatin1String("Public") && u != QLatin1String("Default") &&
          u != QLatin1String("Default User") && u != QLatin1String("All Users") &&
          !u.startsWith(QLatin1Char('.'))) {
        sys.users.append(u);
      }
    }
  } else if (QFile::exists(mnt + QStringLiteral("/System/Library/CoreServices"))) {
    sys.os = OsKind::MacOS;
    sys.osLabel = QStringLiteral("macOS");
    QDir users(mnt + QStringLiteral("/Users"));
    for (const QString &u : users.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      if (u != QLatin1String("Shared") && u != QLatin1String("Guest") &&
          !u.startsWith(QLatin1Char('.'))) {
        sys.users.append(u);
      }
    }
  } else if (QDir(mnt + QStringLiteral("/home")).exists() ||
             QDir(mnt + QStringLiteral("/Users")).exists()) {
    sys.os = OsKind::Linux;
    sys.osLabel = QStringLiteral("Linux (home found)");
    QDir home(mnt + QStringLiteral("/home"));
    if (home.exists()) {
      for (const QString &u : home.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (!u.startsWith(QLatin1Char('.'))) {
          sys.users.append(u);
        }
      }
    }
  } else {
    sys.os = OsKind::Unknown;
    return sys;
  }
  return sys;
}

void RescueEngine::scanSystems()
{
  m_cancel = false;
  m_lastError.clear();
  cleanupMounts();
  m_systems.clear();
  emit scanProgress(QStringLiteral("Listing block devices…"), 5);

  const QString json = runCapture(QStringLiteral("lsblk"),
                                  {QStringLiteral("-J"), QStringLiteral("-b"), QStringLiteral("-o"),
                                   QStringLiteral("NAME,PATH,TYPE,FSTYPE,LABEL,SIZE,MOUNTPOINT,RM")});
  if (json.isEmpty()) {
    m_lastError = QStringLiteral("lsblk failed");
    emit scanFinished(false);
    return;
  }

  const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QVector<BlockPartition> parts;

  std::function<void(const QJsonObject &, bool)> walk;
  walk = [&](const QJsonObject &obj, bool parentRm) {
    const QString type = obj.value(QStringLiteral("type")).toString();
    const bool rm = obj.value(QStringLiteral("rm")).toBool(false) || parentRm;
    if (type == QLatin1String("part") || type == QLatin1String("crypt")) {
      BlockPartition p;
      p.name = obj.value(QStringLiteral("name")).toString();
      p.path = obj.value(QStringLiteral("path")).toString();
      if (p.path.isEmpty()) {
        p.path = QStringLiteral("/dev/") + p.name;
      }
      p.fstype = obj.value(QStringLiteral("fstype")).toString();
      p.label = obj.value(QStringLiteral("label")).toString();
      p.sizeBytes = parseSize(obj.value(QStringLiteral("size")).toVariant().toString());
      if (p.sizeBytes == 0) {
        p.sizeBytes = obj.value(QStringLiteral("size")).toVariant().toLongLong();
      }
      p.sizeHuman = QStringLiteral("%1 GB").arg(p.sizeBytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
      p.removable = rm;
      const QString mp = obj.value(QStringLiteral("mountpoint")).toString();
      p.isLiveMedium = isLiveDevice(p.name, p.label, mp);
      if (!p.fstype.isEmpty() && p.fstype != QLatin1String("swap") &&
          p.fstype != QLatin1String("crypto_LUKS") && !p.isLiveMedium) {
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

  const int total = parts.size();
  int i = 0;
  for (const BlockPartition &part : parts) {
    if (m_cancel) {
      break;
    }
    ++i;
    emit scanProgress(QStringLiteral("Checking %1…").arg(part.path),
                      total ? (10 + (80 * i) / total) : 90);
    const QString mnt =
        QStringLiteral("/run/spike-rescue/src-%1").arg(QUuid::createUuid().toString(QUuid::Id128));
    QString err;
    if (!mountRo(part.path, mnt, part.fstype, &err)) {
      continue;
    }
    DetectedSystem sys = probeMounted(part, mnt);
    if (sys.os == OsKind::Unknown || sys.users.isEmpty()) {
      umountPath(mnt);
      continue;
    }
    Inventory inv = buildInventory(sys);
    sys.fileCount = inv.fileCount;
    sys.byteTotal = inv.byteTotal;
    umountPath(mnt);
    sys.mountPoint.clear();
    m_systems.append(sys);
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

  const QString homeRoot = (sys.os == OsKind::Windows || sys.os == OsKind::MacOS)
                               ? (sys.mountPoint + QStringLiteral("/Users"))
                               : (sys.mountPoint + QStringLiteral("/home"));

  for (const QString &user : sys.users) {
    const QString userHome = homeRoot + QLatin1Char('/') + user;
    for (const QString &cat : categoryDirs(sys.os)) {
      const QString dir = userHome + QLatin1Char('/') + cat;
      QStringList files;
      qint64 bytes = 0;
      qint64 count = 0;
      walkFiles(dir, cat + QLatin1Char('/') + user, &files, &bytes, &count, &inv.unreadable);
      // Prefer relative path stored as absolute for copy; remap category key.
      CategorySummary &cs = cats[cat];
      cs.files += count;
      cs.bytes += bytes;
      inv.files.append(files);
      inv.fileCount += count;
      inv.byteTotal += bytes;
    }
    // Root-of-home office docs (Linux/Spike) — shallow.
    if (sys.os == OsKind::SpikeLinux || sys.os == OsKind::Linux) {
      QDir home(userHome);
      const QStringList globs = {QStringLiteral("*.odt"), QStringLiteral("*.ods"),
                                 QStringLiteral("*.odp"), QStringLiteral("*.pdf"),
                                 QStringLiteral("*.docx"), QStringLiteral("*.xlsx")};
      for (const QFileInfo &fi : home.entryInfoList(globs, QDir::Files | QDir::Readable)) {
        if (fi.fileName().startsWith(QLatin1Char('.'))) {
          continue;
        }
        inv.files.append(fi.absoluteFilePath());
        inv.fileCount += 1;
        inv.byteTotal += fi.size();
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
  const QString mnt =
      QStringLiteral("/run/spike-rescue/src-%1").arg(QUuid::createUuid().toString(QUuid::Id128));
  QString err;
  if (!mountRo(sys.partition.path, mnt, sys.partition.fstype, &err)) {
    m_lastError = QStringLiteral("mount failed: %1").arg(err);
    emit inventoryFinished(false);
    return;
  }
  sys.mountPoint = mnt;
  m_systems[systemIndex].mountPoint = mnt;
  m_inventory = buildInventory(sys);
  m_systems[systemIndex].fileCount = m_inventory.fileCount;
  m_systems[systemIndex].byteTotal = m_inventory.byteTotal;
  emit inventoryFinished(true);
}

void RescueEngine::refreshDestinations()
{
  QVector<DestVolume> vols;
  QStringList roots;
  roots << QStringLiteral("/run/media") << QStringLiteral("/media");

  QSet<QString> sourceDevs;
  for (const DetectedSystem &s : m_systems) {
    sourceDevs.insert(s.partition.path);
    // Also whole disk prefix
    QString disk = s.partition.path;
    disk.remove(QRegularExpression(QStringLiteral("[0-9]+$")));
    sourceDevs.insert(disk);
  }

  for (const QString &root : roots) {
    QDir base(root);
    if (!base.exists()) {
      continue;
    }
    // /run/media/USER/LABEL
    for (const QString &user : base.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QDir userDir(base.filePath(user));
      for (const QString &vol : userDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString mp = userDir.filePath(vol);
        if (mp.contains(QLatin1String("spike-rescue"))) {
          continue;
        }
        QStorageInfo info(mp);
        if (!info.isValid() || !info.isReady()) {
          continue;
        }
        const QString dev = QString::fromUtf8(info.device());
        if (sourceDevs.contains(dev)) {
          continue;
        }
        // Skip live casper mounts
        if (mp.contains(QLatin1String("/cdrom")) || vol.contains(QLatin1String("Spike Live"),
                                                                  Qt::CaseInsensitive)) {
          continue;
        }
        DestVolume d;
        d.path = mp;
        d.label = vol;
        d.device = dev;
        d.freeBytes = info.bytesAvailable();
        vols.append(d);
      }
    }
  }
  emit destinationsChanged(vols);
}

QByteArray RescueEngine::sha256File(const QString &path, bool *ok)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
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

bool RescueEngine::copyOneFile(const QString &src, const QString &dst, QString *errKind)
{
  for (int attempt = 0; attempt < 3; ++attempt) {
    bool okSrc = false;
    const QByteArray srcHash = sha256File(src, &okSrc);
    if (!okSrc) {
      if (errKind) {
        *errKind = QStringLiteral("read");
      }
      continue;
    }
    QDir().mkpath(QFileInfo(dst).absolutePath());
    QFile in(src);
    QFile out(dst);
    if (!in.open(QIODevice::ReadOnly) || !out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      if (errKind) {
        *errKind = QStringLiteral("read");
      }
      continue;
    }
    bool writeOk = true;
    while (!in.atEnd()) {
      const QByteArray chunk = in.read(65536);
      if (chunk.isEmpty() && !in.atEnd()) {
        writeOk = false;
        break;
      }
      if (out.write(chunk) != chunk.size()) {
        writeOk = false;
        break;
      }
    }
    out.close();
    in.close();
    if (!writeOk) {
      if (errKind) {
        *errKind = QStringLiteral("write");
      }
      continue;
    }
    bool okDst = false;
    const QByteArray dstHash = sha256File(dst, &okDst);
    if (!okDst || dstHash != srcHash) {
      if (errKind) {
        *errKind = QStringLiteral("verify");
      }
      continue;
    }
    return true;
  }
  return false;
}

void RescueEngine::startCopy(int systemIndex, const QString &destMount)
{
  m_cancel = false;
  m_lastCopy = {};
  m_lastError.clear();

  if (systemIndex < 0 || systemIndex >= m_systems.size()) {
    m_lastError = QStringLiteral("invalid system");
    emit copyFinished(false);
    return;
  }
  if (destMount.isEmpty() || !QDir(destMount).exists()) {
    m_lastError = QStringLiteral("destination not mounted");
    emit copyFinished(false);
    return;
  }

  DetectedSystem &sys = m_systems[systemIndex];
  if (sys.mountPoint.isEmpty() || !QDir(sys.mountPoint).exists()) {
    const QString mnt =
        QStringLiteral("/run/spike-rescue/src-%1").arg(QUuid::createUuid().toString(QUuid::Id128));
    QString err;
    if (!mountRo(sys.partition.path, mnt, sys.partition.fstype, &err)) {
      m_lastError = QStringLiteral("remount failed: %1").arg(err);
      emit copyFinished(false);
      return;
    }
    sys.mountPoint = mnt;
    m_inventory = buildInventory(sys);
  }

  const QString stamp =
      QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMdd-HHmmss"));
  const QString destRoot =
      destMount + QStringLiteral("/SpikeBackup/") + stamp + QLatin1Char('/') +
      sys.osLabel.replace(QLatin1Char('/'), QLatin1Char('-'));
  QDir().mkpath(destRoot);
  m_lastCopy.destRoot = destRoot;

  const qint64 totalFiles = m_inventory.files.size();
  const qint64 totalBytes = m_inventory.byteTotal;
  qint64 doneFiles = 0;
  qint64 doneBytes = 0;

  for (const QString &src : m_inventory.files) {
    if (m_cancel) {
      m_lastCopy.cancelled = true;
      break;
    }
    QString rel = src;
    if (rel.startsWith(sys.mountPoint)) {
      rel = rel.mid(sys.mountPoint.size());
    }
    if (rel.startsWith(QLatin1Char('/'))) {
      rel = rel.mid(1);
    }
    const QString dst = destRoot + QLatin1Char('/') + rel;
    emit copyProgress(rel, doneFiles, totalFiles, doneBytes, totalBytes);

    QString errKind;
    if (!copyOneFile(src, dst, &errKind)) {
      if (errKind == QLatin1String("verify")) {
        m_lastCopy.failedVerify++;
        m_lastCopy.verifyFails.append(rel);
      } else {
        m_lastCopy.failedRead++;
        m_lastCopy.unreadables.append(rel);
      }
    } else {
      m_lastCopy.copied++;
      doneBytes += QFileInfo(src).size();
    }
    doneFiles++;
    emit copyProgress(rel, doneFiles, totalFiles, doneBytes, totalBytes);
  }

  m_lastCopy.bytesCopied = doneBytes;
  // Leave source mounted until cleanup; unmount after copy for safety.
  if (!sys.mountPoint.isEmpty()) {
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
