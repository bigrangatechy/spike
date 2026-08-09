#pragma once

#include "InstallState.hpp"

#include <QObject>
#include <QString>

class QProcess;

namespace spike {

/** Runs privileged spike-install-helper and streams log lines. */
class InstallEngine : public QObject
{
  Q_OBJECT

public:
  explicit InstallEngine(QObject *parent = nullptr);

  bool isRunning() const;
  void start(const InstallState &state);
  void cancel();

signals:
  void logLine(const QString &line);
  void finished(bool ok, const QString &message);

private slots:
  void onReadyRead();
  void onProcessFinished(int exitCode, int status);

private:
  QProcess *m_proc = nullptr;
  QString m_passwordFile;
};

} // namespace spike
