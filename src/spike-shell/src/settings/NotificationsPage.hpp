#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Notifications — privacy retention + DND prefs (daemon later). */
QWidget *makeNotificationsPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
