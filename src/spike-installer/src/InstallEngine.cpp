#include "InstallEngine.hpp"

#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QTemporaryFile>

namespace spike {

InstallEngine::InstallEngine(QObject *parent)
  : QObject(parent)
{
}

bool InstallEngine::isRunning() const
{
  return m_proc && m_proc->state() != QProcess::NotRunning;
}

void InstallEngine::start(const InstallState &state)
{
  if (isRunning()) {
    return;
  }

  // Password file for helper (shredded after use).
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

  m_proc = new QProcess(this);
  m_proc->setProcessChannelMode(QProcess::MergedChannels);
  connect(m_proc, &QProcess::readyRead, this, &InstallEngine::onReadyRead);
  connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          &InstallEngine::onProcessFinished);

  // Pass all params as argv — sudo env_reset strips SPIKE_INSTALL_* by default.
  const QString lang = state.language.contains(QLatin1Char('.'))
                           ? state.language
                           : state.language + QStringLiteral(".UTF-8");
  QStringList args = {
      QStringLiteral("-n"),
      QStringLiteral("/usr/lib/spike/spike-install-helper"),
      QStringLiteral("install-all"),
      QStringLiteral("--disk"),
      state.targetDisk,
      QStringLiteral("--user"),
      state.username,
      QStringLiteral("--hostname"),
      state.hostname,
      QStringLiteral("--lang"),
      lang,
      QStringLiteral("--timezone"),
      state.timezone,
      QStringLiteral("--variant"),
      state.variant,
      QStringLiteral("--password-file"),
      m_passwordFile,
      QStringLiteral("--confirm"),
      QStringLiteral("ERASE"),
  };
  if (state.restoreAfterInstall && !state.restoreSessionPath.isEmpty()) {
    args << QStringLiteral("--restore-session") << state.restoreSessionPath;
  }

  emit logLine(QStringLiteral("Starting privileged install on %1 …").arg(state.targetDisk));
  m_proc->start(QStringLiteral("sudo"), args);
  if (!m_proc->waitForStarted(5000)) {
    QFile::remove(m_passwordFile);
    m_passwordFile.clear();
    emit finished(false,
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
  }
}

void InstallEngine::onProcessFinished(int exitCode, int status)
{
  Q_UNUSED(status);
  if (!m_passwordFile.isEmpty()) {
    QFile::remove(m_passwordFile);
    m_passwordFile.clear();
  }
  const bool ok = (exitCode == 0);
  emit finished(ok, ok ? QStringLiteral("Install finished successfully.")
                       : QStringLiteral("Install failed (exit %1). See log above and "
                                        "/var/log/spike/install.log")
                             .arg(exitCode));
}

} // namespace spike
