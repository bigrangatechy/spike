#pragma once

class QLabel;
class QWidget;

namespace spike {

/** Settings → Keyboard (hardware) — repeat delay/rate via kcminputrc. */
QWidget *makeKeyboardPage(QWidget *parent, QLabel *statusBar);

} // namespace spike
