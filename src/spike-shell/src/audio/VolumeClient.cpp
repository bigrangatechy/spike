#include "audio/VolumeClient.hpp"

#include <QProcess>
#include <QRegularExpression>
#include <QtGlobal>

namespace spike {

namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(2000) || !proc.waitForFinished(4000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput());
}

bool runOk(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(2000) || !proc.waitForFinished(4000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

} // namespace

VolumeClient::VolumeClient(QObject *parent)
  : QObject(parent)
{
  refresh();
}

void VolumeClient::refresh()
{
  const QString volOut =
      runCapture(QStringLiteral("pactl"),
                 {QStringLiteral("get-sink-volume"), QStringLiteral("@DEFAULT_SINK@")});
  const QString muteOut =
      runCapture(QStringLiteral("pactl"),
                 {QStringLiteral("get-sink-mute"), QStringLiteral("@DEFAULT_SINK@")});

  m_available = !volOut.isEmpty() || !muteOut.isEmpty();
  int pct = m_volume;
  static const QRegularExpression re(QStringLiteral(R"((\d+)%)"));
  const auto match = re.match(volOut);
  if (match.hasMatch()) {
    pct = match.captured(1).toInt();
  }
  const bool muted = muteOut.contains(QLatin1String("yes"), Qt::CaseInsensitive);
  if (pct != m_volume || muted != m_muted || !m_available) {
    m_volume = qBound(0, pct, 150);
    m_muted = muted;
    emit changed();
  } else {
    m_volume = qBound(0, pct, 150);
    m_muted = muted;
  }
}

void VolumeClient::setVolumePercent(int percent)
{
  percent = qBound(0, percent, 150);
  if (runOk(QStringLiteral("pactl"),
            {QStringLiteral("set-sink-volume"), QStringLiteral("@DEFAULT_SINK@"),
             QStringLiteral("%1%").arg(percent)})) {
    m_volume = percent;
    emit changed();
  }
  refresh();
}

void VolumeClient::setMuted(bool muted)
{
  if (runOk(QStringLiteral("pactl"),
            {QStringLiteral("set-sink-mute"), QStringLiteral("@DEFAULT_SINK@"),
             muted ? QStringLiteral("1") : QStringLiteral("0")})) {
    m_muted = muted;
    emit changed();
  }
  refresh();
}

void VolumeClient::toggleMute()
{
  setMuted(!m_muted);
}

void VolumeClient::adjustBy(int delta)
{
  setVolumePercent(m_volume + delta);
}

} // namespace spike
