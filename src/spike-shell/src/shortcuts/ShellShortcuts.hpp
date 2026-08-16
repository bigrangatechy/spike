#pragma once

#include <QObject>

#include <functional>

namespace spike {

class VolumeClient;
class BrightnessClient;
class EvdevMediaKeys;
class OsdPopup;

/**
 * Session-wide media / Fn keys: evdev KEY_* listener (volume/brightness/media) plus
 * KWin script for Meta+L / Meta+Space. Spec: HARDWARE.md, MULTIMEDIA.md,
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

  /** Register D-Bus, start evdev listener + kglobalacceld/KWin script. */
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
  EvdevMediaKeys *m_evdev = nullptr;
  OsdPopup *m_osd = nullptr;
  VoidHandler m_lock;
  VoidHandler m_launcher;
  bool m_started = false;
  bool m_scriptLoaded = false;
};

} // namespace spike
