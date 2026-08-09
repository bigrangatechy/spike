#include "panel/applets/TaskListAdaptor.hpp"

#include "panel/applets/WindowListApplet.hpp"

namespace spike {

TaskListAdaptor::TaskListAdaptor(WindowListApplet *parent)
  : QDBusAbstractAdaptor(parent)
  , m_applet(parent)
{
}

void TaskListAdaptor::setWindows(const QString &blob)
{
  if (m_applet) {
    m_applet->applyWindowBlob(blob);
  }
}

} // namespace spike
