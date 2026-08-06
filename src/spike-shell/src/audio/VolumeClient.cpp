#include "audio/VolumeClient.hpp"

#include <QProcess>
#include <QRegularExpression>
#include <QThread>
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

int parsePactlPercent(const QString &volOut)
{
  static const QRegularExpression re(QStringLiteral(R"((\d+)%)"));
  const auto match = re.match(volOut);
  if (match.hasMatch()) {
    return match.captured(1).toInt();
  }
  return -1;
}

int parseWpctlPercent(const QString &out)
{
  // wpctl get-volume @DEFAULT_AUDIO_SINK@ → "Volume: 0.50" or "Volume: 0.50 [MUTED]"
  static const QRegularExpression re(QStringLiteral(R"(Volume:\s*([0-9.]+))"));
  const auto match = re.match(out);
  if (match.hasMatch()) {
    return qBound(0, qRound(match.captured(1).toDouble() * 100.0), 150);
  }
  return -1;
}

} // namespace

VolumeClient::VolumeClient(QObject *parent)
  : QObject(parent)
{
  refresh();
}

void VolumeClient::refresh()
{
  QString volOut;
  QString muteOut;
  bool available = false;
  int pct = m_volume;
  bool muted = m_muted;

  // Retry briefly — PipeWire-Pulse may still be coming up at panel start.
  for (int attempt = 0; attempt < 5; ++attempt) {
    volOut = runCapture(QStringLiteral("pactl"),
                        {QStringLiteral("get-sink-volume"), QStringLiteral("@DEFAULT_SINK@")});
    muteOut = runCapture(QStringLiteral("pactl"),
                         {QStringLiteral("get-sink-mute"), QStringLiteral("@DEFAULT_SINK@")});
    const int parsed = parsePactlPercent(volOut);
    if (parsed >= 0 || !muteOut.isEmpty()) {
      available = true;
      if (parsed >= 0) {
        pct = parsed;
      }
      muted = muteOut.contains(QLatin1String("yes"), Qt::CaseInsensitive);
      break;
    }

    // Fallback: wpctl (WirePlumber)
    const QString wp = runCapture(QStringLiteral("wpctl"),
                                  {QStringLiteral("get-volume"), QStringLiteral("@DEFAULT_AUDIO_SINK@")});
    const int wpPct = parseWpctlPercent(wp);
    if (wpPct >= 0) {
      available = true;
      pct = wpPct;
      muted = wp.contains(QLatin1String("MUTED"), Qt::CaseInsensitive);
      break;
    }

    if (attempt < 4) {
      QThread::msleep(200);
    }
  }

  const bool dirty = (available != m_available) || (pct != m_volume) || (muted != m_muted);
  m_available = available;
  m_volume = qBound(0, pct, 150);
  m_muted = muted;
  if (dirty) {
    emit changed();
  }
}

void VolumeClient::setVolumePercent(int percent)
{
  percent = qBound(0, percent, 150);
  if (runOk(QStringLiteral("pactl"),
            {QStringLiteral("set-sink-volume"), QStringLiteral("@DEFAULT_SINK@"),
             QStringLiteral("%1%").arg(percent)})) {
    m_volume = percent;
    m_available = true;
    emit changed();
    return;
  }
  // wpctl set-volume @DEFAULT_AUDIO_SINK@ 0.5
  if (runOk(QStringLiteral("wpctl"),
            {QStringLiteral("set-volume"), QStringLiteral("@DEFAULT_AUDIO_SINK@"),
             QString::number(percent / 100.0, 'f', 2)})) {
    m_volume = percent;
    m_available = true;
    emit changed();
    return;
  }
  refresh();
}

void VolumeClient::setMuted(bool muted)
{
  if (runOk(QStringLiteral("pactl"),
            {QStringLiteral("set-sink-mute"), QStringLiteral("@DEFAULT_SINK@"),
             muted ? QStringLiteral("1") : QStringLiteral("0")})) {
    m_muted = muted;
    m_available = true;
    emit changed();
    return;
  }
  if (runOk(QStringLiteral("wpctl"),
            {QStringLiteral("set-mute"), QStringLiteral("@DEFAULT_AUDIO_SINK@"),
             muted ? QStringLiteral("1") : QStringLiteral("0")})) {
    m_muted = muted;
    m_available = true;
    emit changed();
    return;
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
