#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Kernel Modules — module blacklist via security.module_blacklist. */
QWidget *makeKernelModulesPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
