#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;
class NightLightApplet;

QWidget *makePanelGeometryPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);
QWidget *makeTrayAppletsPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);
QWidget *makeNightLightSettingsPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
