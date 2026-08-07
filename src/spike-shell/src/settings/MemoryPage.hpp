#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Memory — ZRAM / swappiness / earlyoom via org.spike.Config. */
QWidget *makeMemoryPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
