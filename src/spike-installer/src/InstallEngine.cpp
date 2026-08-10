#include "InstallEngine.hpp"

#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QTemporaryFile>

namespace spike {

namespace {

bool partitionOnDisk(const QString &part, const QString &disk)
{
  if (disk.isEmpty() || part.isEmpty()) {
    return false;
  }
  if (part == disk) {
    return true;
  }
  if (part.startsWith(disk + QLatin1String("p")) && part.size() > disk.size() + 1) {
    return part.at(disk.size() + 1).isDigit();
  }
  if (part.startsWith(disk) && part.size() > disk.size()) {
    return part.at(disk.size()).isDigit();
  }
  return false;
}

/** Parent disk of the live ISO medium (never the wipe target). Empty if unknown. */
QString detectLiveInstallDisk()
{
  const QStringList mounts = {QStringLiteral("/cdrom"), QStringLiteral("/lib/live/mount/medium"),
                              QStringLiteral("/run/live/medium")};
  for (const QString &mp : mounts) {
    QProcess proc;
    proc.start(QStringLiteral("findmnt"),
               {QStringLiteral("-n"), QStringLiteral("-o"), QStringLiteral("SOURCE"),
                QStringLiteral("-T"), mp});
    if (!proc.waitForFinished(3000) || proc.exitCode() != 0) {
      continue;
    }
    QString src = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    // Strip partition → whole disk via lsblk PKNAME when possible.
    if (src.startsWith(QLatin1String("/dev/"))) {
      QProcess pk;
      pk.start(QStringLiteral("lsblk"),
               {QStringLiteral("-no"), QStringLiteral("PKNAME"), src});
      if (pk.waitForFinished(3000) && pk.exitCode() == 0) {
        const QString parent = QString::fromUtf8(pk.readAllStandardOutput()).trimmed();
        if (!parent.isEmpty()) {
          return QStringLiteral("/dev/") + parent;
        }
      }
      // Already a whole disk (rare) or fallback: strip trailing partition suffix.
      return src;
    }
  }
  return {};
}

struct ListedSystem {
  int rawIndex = 0;
  QString partition;
  QString label;
  QString line;
};

QList<ListedSystem> parseListSystems(const QString &text)
{
  QList<ListedSystem> out;
  for (const QString &line : text.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    if (line.startsWith(QLatin1String("systems=")) || line.startsWith(QLatin1Char('[')) ||
        line.startsWith(QLatin1String("ERROR")) || line.startsWith(QLatin1String("WARN"))) {
      continue;
    }
    const QStringList p = line.split(QLatin1Char('\t'));
    if (p.size() < 3) {
      continue;
    }
    bool ok = false;
    ListedSystem s;
    s.rawIndex = p.at(0).toInt(&ok);
    if (!ok) {
      continue;
    }
    s.partition = p.value(1);
    s.label = p.value(2);
    s.line = line;
    out.append(s);
  }
  return out;
}

} // namespace

InstallEngine::InstallEngine(QObject *parent)
  : QObject(parent)
{
}

bool InstallEngine::isRunning() const
{
  return m_proc && m_proc->state() != QProcess::NotRunning;
}

void InstallEngine::cleanupPasswordFile()
{
  if (!m_passwordFile.isEmpty()) {
    QFile::remove(m_passwordFile);
    m_passwordFile.clear();
  }
}

void InstallEngine::emitFinished(bool ok, const QString &message)
{
  cleanupPasswordFile();
  m_phase = Phase::Idle;
  emit finished(ok, message);
}

void InstallEngine::start(const InstallState &state)
{
  if (isRunning()) {
    return;
  }

  m_state = state;
  m_backupSkipped = false;
  if (m_state.backupStatus.isEmpty()) {
    m_state.backupStatus = m_state.doBackup ? QStringLiteral("pending") : QStringLiteral("skipped");
  }
  if (m_state.restoreStatus.isEmpty()) {
    m_state.restoreStatus = (m_state.restoreAfterInstall && !m_state.restoreSessionPath.isEmpty())
                                ? QStringLiteral("pending")
                                : QStringLiteral("skipped");
  }

  auto *tf = new QTemporaryFile(QStringLiteral("/tmp/spike-install-pw-XXXXXX"), this);
  tf->setAutoRemove(false);
  if (!tf->open()) {
    emit finished(false, QStringLiteral("Could not create password temp file"));
    return;
  }
  tf->write(state.password.toUtf8());
  tf->write("\n");
  tf->flush();
  tf->setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
  m_passwordFile = tf->fileName();
  tf->close();

  if (state.doBackup) {
    startBackupThenInstall();
  } else {
    startInstallAll();
  }
}

void InstallEngine::startBackupThenInstall()
{
  if (m_state.backupDestMount.isEmpty()) {
    emitFinished(false, QStringLiteral("Backup destination not set (Step 7)."));
    return;
  }

  emit logLine(QStringLiteral("Step 7: scanning systems (spike-rescue --list-systems)…"));
  QProcess listProc;
  listProc.setProcessChannelMode(QProcess::MergedChannels);
  listProc.start(QStringLiteral("spike-rescue"), {QStringLiteral("--list-systems")});
  if (!listProc.waitForFinished(180000)) {
    listProc.kill();
    emitFinished(false, QStringLiteral("Step 7: --list-systems timed out — install aborted."));
    return;
  }
  const QString listOut = QString::fromUtf8(listProc.readAll());
  for (const QString &line : listOut.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    emit logLine(line);
  }

  // INSTALLER.md: backup personal files from the OS that will be erased — usually
  // ON the wipe target. Do not exclude the install disk (that made reinstall
  // backups always SKIPPED). Only skip the live USB medium if we can detect it.
  const QString liveDisk = detectLiveInstallDisk();
  QList<ListedSystem> systems = parseListSystems(listOut);
  QList<ListedSystem> eligible;
  for (const ListedSystem &s : systems) {
    if (!liveDisk.isEmpty() && partitionOnDisk(s.partition, liveDisk)) {
      emit logLine(QStringLiteral("skip (live USB): %1").arg(s.partition));
      continue;
    }
    eligible.append(s);
  }

  if (eligible.isEmpty()) {
    emit logLine(QStringLiteral(
        "WARN: no recoverable systems found — skipping Step 7 backup "
        "(nothing will be restored from a new session)."));
    m_backupSkipped = true;
    m_state.backupStatus = QStringLiteral("skipped");
    // Do not invent a restore session from a backup that did not run.
    if (m_state.restoreAfterInstall && m_state.doBackup) {
      emit logLine(QStringLiteral(
          "WARN: clearing restore-after-install because backup was skipped."));
      m_state.restoreAfterInstall = false;
      m_state.restoreSessionPath.clear();
      m_state.restoreStatus = QStringLiteral("skipped");
    }
    startInstallAll();
    return;
  }

  int pick = 0; // index into eligible → maps to raw --list-systems index
  if (!m_state.backupSystemPartition.isEmpty()) {
    for (int i = 0; i < eligible.size(); ++i) {
      if (eligible.at(i).partition == m_state.backupSystemPartition) {
        pick = i;
        break;
      }
    }
  }
  const ListedSystem &chosen = eligible.at(pick);
  emit logLine(QStringLiteral("Step 7: backing up [%1] %2 (%3) → %4")
                   .arg(chosen.rawIndex)
                   .arg(chosen.label, chosen.partition, m_state.backupDestMount));

  m_phase = Phase::Backup;
  if (m_proc) {
    m_proc->deleteLater();
  }
  m_proc = new QProcess(this);
  m_proc->setProcessChannelMode(QProcess::MergedChannels);
  connect(m_proc, &QProcess::readyRead, this, &InstallEngine::onReadyRead);
  connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          &InstallEngine::onProcessFinished);

  // --system indexes rescue's eligible list after --exclude-disk (live USB only).
  QStringList args = {QStringLiteral("--batch-recover"), QStringLiteral("--dest"),
                      m_state.backupDestMount, QStringLiteral("--system"),
                      QString::number(liveDisk.isEmpty() ? chosen.rawIndex : pick)};
  if (!liveDisk.isEmpty()) {
    args << QStringLiteral("--exclude-disk") << liveDisk;
  }
  m_proc->start(QStringLiteral("spike-rescue"), args);
  if (!m_proc->waitForStarted(5000)) {
    emitFinished(false, QStringLiteral("spike-rescue failed to start (is spike-rescue installed?)"));
  }
}

void InstallEngine::startInstallAll()
{
  m_phase = Phase::Install;
  if (m_proc) {
    m_proc->deleteLater();
  }
  m_proc = new QProcess(this);
  m_proc->setProcessChannelMode(QProcess::MergedChannels);
  connect(m_proc, &QProcess::readyRead, this, &InstallEngine::onReadyRead);
  connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          &InstallEngine::onProcessFinished);

  const QString lang = m_state.language.contains(QLatin1Char('.'))
                           ? m_state.language
                           : m_state.language + QStringLiteral(".UTF-8");
  QStringList args = {
      QStringLiteral("-n"),
      QStringLiteral("/usr/lib/spike/spike-install-helper"),
      QStringLiteral("install-all"),
      QStringLiteral("--disk"),
      m_state.targetDisk,
      QStringLiteral("--user"),
      m_state.username,
      QStringLiteral("--hostname"),
      m_state.hostname,
      QStringLiteral("--lang"),
      lang,
      QStringLiteral("--timezone"),
      m_state.timezone,
      QStringLiteral("--keyboard"),
      m_state.keyboard.isEmpty() ? QStringLiteral("us") : m_state.keyboard,
      QStringLiteral("--variant"),
      m_state.variant,
      QStringLiteral("--password-file"),
      m_passwordFile,
      QStringLiteral("--confirm"),
      QStringLiteral("ERASE"),
  };
  if (m_state.restoreAfterInstall && !m_state.restoreSessionPath.isEmpty()) {
    args << QStringLiteral("--restore-session") << m_state.restoreSessionPath;
    m_state.restoreStatus = QStringLiteral("pending");
  } else {
    m_state.restoreStatus = QStringLiteral("skipped");
  }
  if (m_state.autoLogin) {
    args << QStringLiteral("--auto-login");
  }

  emit logLine(QStringLiteral("Starting privileged install on %1 …").arg(m_state.targetDisk));
  m_proc->start(QStringLiteral("sudo"), args);
  if (!m_proc->waitForStarted(5000)) {
    emitFinished(false,
                 QStringLiteral("sudo/helper failed to start (is spike-installer sudoers installed?)"));
  }
}

void InstallEngine::cancel()
{
  if (m_proc && m_proc->state() != QProcess::NotRunning) {
    m_proc->kill();
  }
}

void InstallEngine::onReadyRead()
{
  if (!m_proc) {
    return;
  }
  const QByteArray data = m_proc->readAll();
  const QString text = QString::fromUtf8(data);
  const QStringList lines = text.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    emit logLine(line);
    if (m_phase == Phase::Backup) {
      if (line.startsWith(QLatin1String("SESSION_PATH="))) {
        const QString path = line.mid(QStringLiteral("SESSION_PATH=").size()).trimmed();
        if (!path.isEmpty()) {
          if (m_state.restoreAfterInstall &&
              (m_state.restoreSessionPath.isEmpty() || m_state.doBackup)) {
            m_state.restoreSessionPath = path;
            emit logLine(
                QStringLiteral("Using new backup session for Layer 4 restore: %1").arg(path));
          } else if (m_state.restoreSessionPath.isEmpty()) {
            m_state.restoreSessionPath = path;
          }
        }
      }
      if (line.startsWith(QLatin1String("SKIPPED="))) {
        m_backupSkipped = true;
        m_state.backupStatus = QStringLiteral("skipped");
      }
    }
    if (m_phase == Phase::Install && line.contains(QLatin1String("RESTORE_STATUS="))) {
      const int eq = line.indexOf(QLatin1String("RESTORE_STATUS="));
      m_state.restoreStatus = line.mid(eq + QStringLiteral("RESTORE_STATUS=").size()).trimmed();
    }
  }
}

void InstallEngine::onProcessFinished(int exitCode, int status)
{
  Q_UNUSED(status);
  if (m_phase == Phase::Backup) {
    if (exitCode != 0) {
      m_state.backupStatus = QStringLiteral("failed");
      emitFinished(false, QStringLiteral("Step 7 backup failed (exit %1) — install aborted "
                                         "(disk not wiped). See log above.")
                              .arg(exitCode));
      return;
    }
    if (m_backupSkipped) {
      emit logLine(QStringLiteral(
          "WARN: Step 7 reported SKIPPED — continuing install without a new backup session."));
      m_state.backupStatus = QStringLiteral("skipped");
      if (m_state.restoreAfterInstall && m_state.restoreSessionPath.isEmpty()) {
        m_state.restoreAfterInstall = false;
        m_state.restoreStatus = QStringLiteral("skipped");
      }
    } else {
      m_state.backupStatus = QStringLiteral("ok");
      emit logLine(QStringLiteral("Step 7 backup OK — starting wipe/install …"));
    }
    startInstallAll();
    return;
  }

  const bool ok = (exitCode == 0);
  QString msg;
  if (ok) {
    msg = QStringLiteral("Install finished successfully.\n"
                         "Backup: %1\n"
                         "Restore: %2")
              .arg(m_state.backupStatus.isEmpty() ? QStringLiteral("n/a") : m_state.backupStatus,
                   m_state.restoreStatus.isEmpty() ? QStringLiteral("n/a") : m_state.restoreStatus);
  } else {
    msg = QStringLiteral("Install failed (exit %1). See log above and "
                         "/var/log/spike/install.log")
              .arg(exitCode);
  }
  emitFinished(ok, msg);
}

} // namespace spike
