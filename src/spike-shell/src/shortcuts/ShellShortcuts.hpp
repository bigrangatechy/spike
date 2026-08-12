#pragma once

#include <QObject>

#include <functional>

namespace spike {

class VolumeClient;
class BrightnessClient;

/**
 * Session-wide media / Fn keys: KWin script registers shortcuts (via
 * kglobalacceld) and calls this D-Bus object. Spec: HARDWARE.md, MULTIMEDIA.md,
 * POWER-MANAGEMENT.md, SECURITY.md (Super+L).
 */
class ShellShortcuts : public QObject
{
  Q_OBJECT

public:
  using VoidHandler = std::function<void()>;

  explicit ShellShortcuts(QObject *parent = nullptr);

  void setLockHandler(VoidHandler handler);
  void setLauncherHandler(VoidHandler handler);

  /** Register D-Bus, start kglobalacceld, load KWin spike-shortcuts script. */
  void start();

public slots:
  void volumeUp();
  void volumeDown();
  void volumeMute();
  void brightnessUp();
  void brightnessDown();
  void mediaPlayPause();
  void mediaNext();
  void mediaPrevious();
  void lock();
  void launcher();

private:
  void ensureKwinScript();
  void notifyOsd(const QString &summary, const QString &body);

  VolumeClient *m_volume = nullptr;
  BrightnessClient *m_brightness = nullptr;
  VoidHandler m_lock;
  VoidHandler m_launcher;
  bool m_started = false;
  bool m_scriptLoaded = false;
};

} // namespace spike
