#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Updates — schedule / auto-security prefs via org.spike.Config. */
QWidget *makeUpdatesPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
