#include "settings/InputConfig.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

namespace spike {

namespace {

QString kcminputPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/kcminputrc");
}

bool tryKwinReconfigure()
{
  // Best-effort: KWin may reload kcminputrc; many builds only pick input up at start.
  QDBusInterface kwin(QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
                      QStringLiteral("org.kde.KWin"), QDBusConnection::sessionBus());
  if (kwin.isValid()) {
    const QDBusMessage reply = kwin.call(QStringLiteral("reconfigure"));
    if (reply.type() != QDBusMessage::ErrorMessage) {
      return true;
    }
  }

  QProcess proc;
  proc.start(QStringLiteral("qdbus6"),
             {QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
              QStringLiteral("org.kde.KWin.reconfigure")});
  if (proc.waitForFinished(3000) && proc.exitCode() == 0) {
    return true;
  }
  proc.start(QStringLiteral("qdbus"),
             {QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
              QStringLiteral("org.kde.KWin.reconfigure")});
  return proc.waitForFinished(3000) && proc.exitCode() == 0;
}

int hexIdToDecimal(const QByteArray &raw)
{
  bool ok = false;
  const int v = QString::fromUtf8(raw.trimmed()).toInt(&ok, 16);
  return ok ? v : 0;
}

} // namespace

QVector<PointerDevice> listPointerDevices()
{
  QVector<PointerDevice> out;
  const QDir inputDir(QStringLiteral("/sys/class/input"));
  const QStringList entries = inputDir.entryList(QStringList() << QStringLiteral("event*"),
                                                 QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &eventName : entries) {
    const QString base = inputDir.filePath(eventName + QStringLiteral("/device"));
    QFile nameFile(base + QStringLiteral("/name"));
    if (!nameFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      continue;
    }
    const QString name = QString::fromUtf8(nameFile.readAll()).trimmed();
    const QString lower = name.toLower();
    const bool isPad = lower.contains(QLatin1String("touchpad")) ||
                       lower.contains(QLatin1String("synaptics")) ||
                       lower.contains(QLatin1String("trackpad")) ||
                       lower.contains(QLatin1String("clickpad")) ||
                       lower.contains(QLatin1String("htix")) || // Gemini Lake laptop pads
                       lower.startsWith(QLatin1String("elan")) ||
                       lower.contains(QLatin1String("dll")); // Dell pads
    const bool isMouse = lower.contains(QLatin1String("mouse")) ||
                         lower.contains(QLatin1String("trackpoint")) ||
                         lower.contains(QLatin1String("pointer"));
    if (!isPad && !isMouse) {
      // Many laptop pads are named oddly (e.g. HTIX5288) — include if it has relative axes
      // via capabilities; fallback: skip non-obvious names except known HID pads.
      QFile caps(base + QStringLiteral("/capabilities/rel"));
      if (!caps.exists()) {
        continue;
      }
      // Still skip pure keyboards / switches by requiring "rel" non-empty and not "keyboard"
      if (lower.contains(QLatin1String("keyboard")) || lower.contains(QLatin1String("button")) ||
          lower.contains(QLatin1String("lid")) || lower.contains(QLatin1String("video bus"))) {
        continue;
      }
      // Accept devices with relative movement (mice/pads)
      if (!caps.open(QIODevice::ReadOnly | QIODevice::Text)) {
        continue;
      }
      const QByteArray rel = caps.readAll().trimmed();
      if (rel.isEmpty() || rel == QByteArrayLiteral("0")) {
        continue;
      }
    }

    PointerDevice d;
    d.name = name;
    d.touchpad = isPad;
    QFile vend(base + QStringLiteral("/id/vendor"));
    QFile prod(base + QStringLiteral("/id/product"));
    if (vend.open(QIODevice::ReadOnly | QIODevice::Text)) {
      d.vendorId = hexIdToDecimal(vend.readAll());
    }
    if (prod.open(QIODevice::ReadOnly | QIODevice::Text)) {
      d.productId = hexIdToDecimal(prod.readAll());
    }
    // Deduplicate by vid/pid
    bool dup = false;
    for (const PointerDevice &e : out) {
      if (e.vendorId == d.vendorId && e.productId == d.productId && e.name == d.name) {
        dup = true;
        break;
      }
    }
    if (!dup) {
      out.push_back(d);
    }
  }
  return out;
}

QString applyPointerSettings(double acceleration, bool tapToClick, bool *liveApplied)
{
  if (liveApplied) {
    *liveApplied = false;
  }
  acceleration = qBound(-1.0, acceleration, 1.0);

  QSettings s(kcminputPath(), QSettings::IniFormat);
  s.beginGroup(QStringLiteral("Mouse"));
  s.setValue(QStringLiteral("PointerAcceleration"), acceleration);
  s.endGroup();

  const QVector<PointerDevice> devices = listPointerDevices();
  int written = 0;
  for (const PointerDevice &d : devices) {
    // Plasma format: [Libinput][vendor][product]
    s.beginGroup(QStringLiteral("Libinput"));
    s.beginGroup(QString::number(d.vendorId));
    s.beginGroup(QString::number(d.productId));
    s.setValue(QStringLiteral("PointerAcceleration"), acceleration);
    s.setValue(QStringLiteral("PointerAccelerationProfile"), QStringLiteral("Adaptive"));
    if (d.touchpad || devices.size() == 1) {
      s.setValue(QStringLiteral("TapToClick"), tapToClick);
    }
    s.endGroup();
    s.endGroup();
    s.endGroup();
    ++written;
  }
  // Also keep a flat fallback key for readers that expect it.
  s.beginGroup(QStringLiteral("Libinput"));
  s.setValue(QStringLiteral("TapToClick"), tapToClick);
  s.endGroup();
  s.sync();

  const bool reconfigured = tryKwinReconfigure();
  if (liveApplied) {
    *liveApplied = reconfigured;
  }

  QString msg = QStringLiteral("Saved to %1 (%2 pointer device section(s)).")
                    .arg(kcminputPath())
                    .arg(written);
  if (reconfigured) {
    msg += QStringLiteral(" Asked KWin to reconfigure (may need a new session if feel unchanged).");
  } else {
    msg += QStringLiteral(" KWin did not reconfigure — log out/in or restart spike-session for "
                          "pointer changes to take effect.");
  }
  return msg;
}

QString applyKeyboardRepeat(int delayMs, int ratePerSec, bool *liveApplied)
{
  if (liveApplied) {
    *liveApplied = false;
  }
  delayMs = qBound(100, delayMs, 2000);
  ratePerSec = qBound(1, ratePerSec, 100);

  QSettings s(kcminputPath(), QSettings::IniFormat);
  s.beginGroup(QStringLiteral("Keyboard"));
  s.setValue(QStringLiteral("RepeatDelay"), delayMs);
  s.setValue(QStringLiteral("RepeatRate"), ratePerSec);
  s.endGroup();
  s.sync();

  // XWayland clients only — harmless if it fails under pure Wayland.
  QProcess::execute(QStringLiteral("xset"),
                    {QStringLiteral("r"), QStringLiteral("rate"), QString::number(delayMs),
                     QString::number(ratePerSec)});

  const bool reconfigured = tryKwinReconfigure();
  if (liveApplied) {
    *liveApplied = reconfigured;
  }

  QString msg = QStringLiteral("Saved keyboard repeat to %1.").arg(kcminputPath());
  if (reconfigured) {
    msg += QStringLiteral(" Asked KWin to reconfigure.");
  } else {
    msg += QStringLiteral(" Restart spike-session if repeat feel is unchanged.");
  }
  return msg;
}

} // namespace spike
