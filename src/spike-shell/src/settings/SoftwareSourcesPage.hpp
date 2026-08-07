#pragma once

class QLabel;
class QWidget;

namespace spike {

/** Settings → Software Sources — APT sources list + external tools. */
QWidget *makeSoftwareSourcesPage(QWidget *parent, QLabel *statusBar);

} // namespace spike
