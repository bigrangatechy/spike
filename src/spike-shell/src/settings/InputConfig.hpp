#pragma once

#include <QString>
#include <QVector>

namespace spike {

struct PointerDevice {
  QString name;
  QString eventName; // e.g. "event5" — matches KWin InputDevice path
  int vendorId = 0;  // decimal, as Plasma kcminputrc uses
  int productId = 0;
  bool touchpad = false;
};

/** Discover pointer / touchpad devices from sysfs (for kcminputrc Libinput sections). */
QVector<PointerDevice> listPointerDevices();

/**
 * Persist mouse/touchpad prefs for KWin (kcminputrc) and apply live via
 * org.kde.KWin.InputDevice D-Bus properties (Wayland).
 */
QString applyPointerSettings(double acceleration /* -1..1 */, bool tapToClick, bool *liveApplied);

/**
 * Persist keyboard repeat and apply live via KWin InputDevice keyboardRepeat*
 * properties when available; also xset for XWayland.
 */
QString applyKeyboardRepeat(int delayMs, int ratePerSec, bool *liveApplied);

} // namespace spike
