#pragma once

#include <QDBusAbstractAdaptor>
#include <QString>

namespace spike {

class WindowListApplet;

/** D-Bus sink for the spike-tasklist KWin script (org.spike.Shell.TaskList). */
class TaskListAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.spike.Shell.TaskList")

public:
  explicit TaskListAdaptor(WindowListApplet *parent);

public slots:
  Q_NOREPLY void setWindows(const QString &blob);

private:
  WindowListApplet *m_applet = nullptr;
};

} // namespace spike
