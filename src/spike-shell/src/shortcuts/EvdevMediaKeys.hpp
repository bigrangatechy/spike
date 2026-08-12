#pragma once

#include <QObject>
#include <QVector>

class QSocketNotifier;

namespace spike {

/**
 * Listen for Fn / media keycodes on /dev/input (EV_KEY) without grabbing.
 * Reliable on Spike's KWin-only session where kglobalaccel/KWin registerShortcut
 * often never binds XF86 volume/brightness keys.
 */
class EvdevMediaKeys : public QObject
{
  Q_OBJECT

public:
  explicit EvdevMediaKeys(QObject *parent = nullptr);
  ~EvdevMediaKeys() override;

  /** Open capable keyboards; safe to call again (re-scan). */
  void start();

signals:
  void volumeUp();
  void volumeDown();
  void volumeMute();
  void brightnessUp();
  void brightnessDown();
  void mediaPlayPause();
  void mediaNext();
  void mediaPrevious();

private:
  struct Device {
    int fd = -1;
    QSocketNotifier *notifier = nullptr;
  };

  void closeAll();
  void onReadable(int fd);
  bool openDevice(const QString &path);

  QVector<Device> m_devices;
};

} // namespace spike
