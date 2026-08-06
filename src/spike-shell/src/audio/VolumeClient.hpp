#pragma once

#include <QObject>

namespace spike {

/** Thin PulseAudio / PipeWire-Pulse helper via pactl (DESKTOP.md Volume applet). */
class VolumeClient : public QObject
{
  Q_OBJECT

public:
  explicit VolumeClient(QObject *parent = nullptr);

  int volumePercent() const { return m_volume; }
  bool muted() const { return m_muted; }
  bool available() const { return m_available; }

  void refresh();
  void setVolumePercent(int percent);
  void setMuted(bool muted);
  void toggleMute();
  void adjustBy(int delta);

signals:
  void changed();

private:
  int m_volume = 0;
  bool m_muted = false;
  bool m_available = false;
};

} // namespace spike
