#pragma once

#include <QString>
#include <QVector>

namespace spike {

struct PointerDevice {
  QString name;
  int vendorId = 0;  // decimal, as Plasma kcminputrc uses
  int productId = 0;
  bool touchpad = false;
};

/** Discover pointer / touchpad devices from sysfs (for kcminputrc Libinput sections). */
QVector<PointerDevice> listPointerDevices();

/**
 * Write mouse/touchpad prefs the way KWin expects:
 *   [Mouse] PointerAcceleration=…
 *   [Libinput][<vendor>][<product>] TapToClick=… / PointerAcceleration=…
 * Then ask KWin to reconfigure (best-effort live apply).
 * Returns a short human status string.
 */
QString applyPointerSettings(double acceleration /* -1..1 */, bool tapToClick, bool *liveApplied);

/** Write [Keyboard] RepeatDelay / RepeatRate and ask KWin to reconfigure. */
QString applyKeyboardRepeat(int delayMs, int ratePerSec, bool *liveApplied);

} // namespace spike
