#pragma once

class QLabel;
class QWidget;

namespace spike {

/** Settings → Mouse/Touchpad — pointer speed / tap via kcminputrc. */
QWidget *makeMousePage(QWidget *parent, QLabel *statusBar);

} // namespace spike
