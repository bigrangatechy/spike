#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Storage — read-only disk overview from DetectHardware. */
QWidget *makeStoragePage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
