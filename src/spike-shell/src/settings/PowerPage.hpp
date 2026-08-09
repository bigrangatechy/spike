#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Power — spike-config + logind (not powerdevil KCM). */
QWidget *makePowerPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
