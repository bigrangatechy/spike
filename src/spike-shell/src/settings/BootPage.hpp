#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Boot — GRUB / boot-count via org.spike.Config. */
QWidget *makeBootPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
