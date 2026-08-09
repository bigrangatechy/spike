#include "settings/InputConfig.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QVariant>

namespace spike {

namespace {

QString kcminputPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/kcminputrc");
}

bool setKwinProperty(const QString &objectPath, const char *property, const QVariant &value)
{
  QDBusInterface props(QStringLiteral("org.kde.KWin"), objectPath,
                       QStringLiteral("org.freedesktop.DBus.Properties"),
                       QDBusConnection::sessionBus());
  if (!props.isValid()) {
    return false;
  }
  const QDBusMessage reply =
      props.call(QStringLiteral("Set"), QStringLiteral("org.kde.KWin.InputDevice"),
                 QString::fromUtf8(property), QVariant::fromValue(QDBusVariant(value)));
  return reply.type() != QDBusMessage::ErrorMessage;
}

QVariant getKwinProperty(const QString &objectPath, const char *property)
{
  QDBusInterface props(QStringLiteral("org.kde.KWin"), objectPath,
                       QStringLiteral("org.freedesktop.DBus.Properties"),
                       QDBusConnection::sessionBus());
  if (!props.isValid()) {
    return {};
  }
  const QDBusReply<QVariant> reply =
      props.call(QStringLiteral("Get"), QStringLiteral("org.kde.KWin.InputDevice"),
                 QString::fromUtf8(property));
  return reply.isValid() ? reply.value() : QVariant();
}

/** Apply pointer accel / tap to every KWin pointer or touchpad device. */
int applyPointerViaKwin(double acceleration, bool tapToClick)
{
  int ok = 0;
  for (int i = 0; i < 64; ++i) {
    const QString path =
        QStringLiteral("/org/kde/KWin/InputDevice/event%1").arg(i);
    const QVariant supportsPointer = getKwinProperty(path, "supportsPointerAcceleration");
    const QVariant isPointer = getKwinProperty(path, "pointer");
    const QVariant isTouchpad = getKwinProperty(path, "touchpad");
    const bool pointerish =
        (isPointer.isValid() && isPointer.toBool()) ||
        (isTouchpad.isValid() && isTouchpad.toBool()) ||
        (supportsPointer.isValid() && supportsPointer.toBool());
    if (!pointerish) {
      // Device path may not exist — Get returns invalid.
      if (!isPointer.isValid() && !isTouchpad.isValid() && !supportsPointer.isValid()) {
        continue;
      }
      continue;
    }

    bool deviceOk = false;
    if (supportsPointer.isValid() ? supportsPointer.toBool() : true) {
      if (setKwinProperty(path, "pointerAcceleration", acceleration)) {
        deviceOk = true;
      }
      setKwinProperty(path, "pointerAccelerationProfileAdaptive", true);
      setKwinProperty(path, "pointerAccelerationProfileFlat", false);
    }
    const QVariant supportsTap = getKwinProperty(path, "supportsTapToClick");
    if ((isTouchpad.isValid() && isTouchpad.toBool()) ||
        (supportsTap.isValid() && supportsTap.toBool())) {
      if (setKwinProperty(path, "tapToClick", tapToClick)) {
        deviceOk = true;
      }
    }
    if (deviceOk) {
      ++ok;
    }
  }
  return ok;
}

int applyKeyboardRepeatViaKwin(int delayMs, int ratePerSec)
{
  int ok = 0;
  for (int i = 0; i < 64; ++i) {
    const QString path =
        QStringLiteral("/org/kde/KWin/InputDevice/event%1").arg(i);
    const QVariant isKeyboard = getKwinProperty(path, "keyboard");
    if (!isKeyboard.isValid() || !isKeyboard.toBool()) {
      continue;
    }
    bool deviceOk = false;
    if (setKwinProperty(path, "repeatDelay", uint(delayMs))) {
      deviceOk = true;
    }
    // KWin uses characters-per-second style rate on some builds; try both names.
    if (setKwinProperty(path, "repeatRate", ratePerSec) ||
        setKwinProperty(path, "keyboardRepeatRate", ratePerSec)) {
      deviceOk = true;
    }
    if (deviceOk) {
      ++ok;
    }
  }
  return ok;
}

bool tryKwinReconfigure()
{
  QDBusInterface kwin(QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
                      QStringLiteral("org.kde.KWin"), QDBusConnection::sessionBus());
  if (kwin.isValid()) {
    const QDBusMessage reply = kwin.call(QStringLiteral("reconfigure"));
    if (reply.type() != QDBusMessage::ErrorMessage) {
      return true;
    }
  }
  return false;
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
                       lower.contains(QLatin1String("htix")) ||
                       lower.startsWith(QLatin1String("elan")) ||
                       lower.contains(QLatin1String("dll"));
    const bool isMouse = lower.contains(QLatin1String("mouse")) ||
                         lower.contains(QLatin1String("trackpoint")) ||
                         lower.contains(QLatin1String("pointer"));
    if (!isPad && !isMouse) {
      QFile caps(base + QStringLiteral("/capabilities/rel"));
      if (!caps.exists()) {
        continue;
      }
      if (lower.contains(QLatin1String("keyboard")) || lower.contains(QLatin1String("button")) ||
          lower.contains(QLatin1String("lid")) || lower.contains(QLatin1String("video bus"))) {
        continue;
      }
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
    d.eventName = eventName;
    d.touchpad = isPad;
    QFile vend(base + QStringLiteral("/id/vendor"));
    QFile prod(base + QStringLiteral("/id/product"));
    if (vend.open(QIODevice::ReadOnly | QIODevice::Text)) {
      d.vendorId = hexIdToDecimal(vend.readAll());
    }
    if (prod.open(QIODevice::ReadOnly | QIODevice::Text)) {
      d.productId = hexIdToDecimal(prod.readAll());
    }
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
  s.beginGroup(QStringLiteral("Libinput"));
  s.setValue(QStringLiteral("TapToClick"), tapToClick);
  s.endGroup();
  s.sync();

  const int liveCount = applyPointerViaKwin(acceleration, tapToClick);
  tryKwinReconfigure();
  if (liveApplied) {
    *liveApplied = liveCount > 0;
  }

  QString msg = QStringLiteral("Saved to %1 (%2 device section(s)).")
                    .arg(kcminputPath())
                    .arg(written);
  if (liveCount > 0) {
    msg += QStringLiteral(" Applied live to %1 KWin input device(s).").arg(liveCount);
  } else {
    msg += QStringLiteral(" KWin InputDevice D-Bus apply failed — try again or restart "
                          "spike-session.");
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

  QProcess::execute(QStringLiteral("xset"),
                    {QStringLiteral("r"), QStringLiteral("rate"), QString::number(delayMs),
                     QString::number(ratePerSec)});

  const int liveCount = applyKeyboardRepeatViaKwin(delayMs, ratePerSec);
  tryKwinReconfigure();
  if (liveApplied) {
    *liveApplied = liveCount > 0;
  }

  QString msg = QStringLiteral("Saved keyboard repeat to %1.").arg(kcminputPath());
  if (liveCount > 0) {
    msg += QStringLiteral(" Applied live to %1 keyboard device(s).").arg(liveCount);
  } else {
    msg += QStringLiteral(" Live KWin apply unavailable — XWayland xset tried; restart "
                          "session if needed.");
  }
  return msg;
}

} // namespace spike
