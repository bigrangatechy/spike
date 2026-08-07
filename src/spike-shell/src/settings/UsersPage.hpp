#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Users — thin account / password / auto-login (Plasma-free). */
QWidget *makeUsersPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
