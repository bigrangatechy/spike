#pragma once

#include <QJsonObject>
#include <QWidget>

class QTimer;
class QVariant;

namespace spike {

class ConfigClient;
class Launcher;
class SettingsWindow;
class NotificationDaemon;
class NotificationsApplet;
class NightLightApplet;
class UpdateNotifierApplet;
class WindowListApplet;

class Panel : public QWidget
{
  Q_OBJECT

public:
  explicit Panel(QWidget *parent = nullptr);

  /** Apply layer-shell anchors / size from current panel settings. */
  void applyLayerShell();
  int panelHeight() const { return m_panelHeight; }
  bool panelOnTop() const { return m_panelOnTop; }

public slots:
  void openSettings(const QString &pageId = QString());
  void reloadDesktopSettings();

private slots:
  void toggleLauncher();
  void onLogout();
  void onReboot();
  void onShutdown();
  void onConfigStateChanged(const QString &module, const QString &key, const QVariant &oldValue,
                            const QVariant &newValue);
  void onAutoHideTick();

private:
  void applyPanelChrome();
  bool cursorNearPanelEdge() const;
  void applyTrayVisibility(const QJsonObject &desktop);

  Launcher *m_launcher = nullptr;
  ConfigClient *m_config = nullptr;
  SettingsWindow *m_settings = nullptr;
  QTimer *m_autoHideTimer = nullptr;
  NotificationDaemon *m_notify = nullptr;

  NotificationsApplet *m_notifications = nullptr;
  NightLightApplet *m_nightLight = nullptr;
  UpdateNotifierApplet *m_updates = nullptr;
  WindowListApplet *m_windowList = nullptr;

  int m_panelHeight = 32;
  bool m_panelOnTop = false;
  bool m_autoHide = false;
  bool m_panelRevealed = true;
};

} // namespace spike
