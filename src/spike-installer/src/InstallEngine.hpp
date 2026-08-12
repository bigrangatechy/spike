#pragma once

#include "InstallState.hpp"

#include <QObject>
#include <QString>

class QProcess;

namespace spike {

/** Runs Step 7 batch-recover (optional) then privileged install-all; streams logs. */
class InstallEngine : public QObject
{
  Q_OBJECT

public:
  explicit InstallEngine(QObject *parent = nullptr);

  bool isRunning() const;
  void start(const InstallState &state);
  void cancel();
  InstallState lastState() const { return m_state; }

signals:
  void logLine(const QString &line);
  void finished(bool ok, const QString &message);

private slots:
  void onReadyRead();
  void onProcessFinished(int exitCode, int status);

private:
  void startInstallAll();
  void startBackupThenInstall();
  void ingestBackupOutputLine(const QString &line);
  void emitFinished(bool ok, const QString &message);
  void cleanupPasswordFile();

  enum class Phase { Idle, Backup, Install };

  QProcess *m_proc = nullptr;
  QString m_passwordFile;
  InstallState m_state;
  Phase m_phase = Phase::Idle;
  bool m_backupSkipped = false;
};

} // namespace spike
