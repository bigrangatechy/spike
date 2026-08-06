#pragma once

#include <QWidget>

namespace spike {

class ConfigClient;
class Launcher;
class SettingsWindow;

class Panel : public QWidget
{
  Q_OBJECT

public:
  explicit Panel(QWidget *parent = nullptr);

public slots:
  void openSettings(const QString &pageId = QString());

private slots:
  void toggleLauncher();
  void onLogout();
  void onReboot();
  void onShutdown();

private:
  Launcher *m_launcher = nullptr;
  ConfigClient *m_config = nullptr;
  SettingsWindow *m_settings = nullptr;
};

} // namespace spike
