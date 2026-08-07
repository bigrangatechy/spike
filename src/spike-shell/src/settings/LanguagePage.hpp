#pragma once

class QLabel;
class QWidget;

namespace spike {

/** Settings → Language — localectl (Plasma-free). */
QWidget *makeLanguagePage(QWidget *parent, QLabel *statusBar);

} // namespace spike
