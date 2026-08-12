#pragma once

#include <QDBusAbstractAdaptor>

namespace spike {

class ShellShortcuts;

/** D-Bus sink for the spike-shortcuts KWin script (org.spike.Shell.Shortcuts). */
class ShortcutsAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.spike.Shell.Shortcuts")

public:
  explicit ShortcutsAdaptor(ShellShortcuts *parent);

public Q_SLOTS:
  Q_NOREPLY void volumeUp();
  Q_NOREPLY void volumeDown();
  Q_NOREPLY void volumeMute();
  Q_NOREPLY void brightnessUp();
  Q_NOREPLY void brightnessDown();
  Q_NOREPLY void mediaPlayPause();
  Q_NOREPLY void mediaNext();
  Q_NOREPLY void mediaPrevious();
  Q_NOREPLY void lock();
  Q_NOREPLY void launcher();

private:
  ShellShortcuts *m_shortcuts = nullptr;
};

} // namespace spike
