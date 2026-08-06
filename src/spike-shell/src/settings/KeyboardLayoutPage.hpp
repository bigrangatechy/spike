#pragma once

class QLabel;
class QWidget;

namespace spike {

/** Settings → Keyboard Layout — XKB / localectl (DESKTOP.md). */
QWidget *makeKeyboardLayoutPage(QWidget *parent, QLabel *statusBar);

} // namespace spike
