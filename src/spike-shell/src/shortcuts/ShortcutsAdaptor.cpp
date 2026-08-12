#include "shortcuts/ShortcutsAdaptor.hpp"

#include "shortcuts/ShellShortcuts.hpp"

namespace spike {

ShortcutsAdaptor::ShortcutsAdaptor(ShellShortcuts *parent)
  : QDBusAbstractAdaptor(parent)
  , m_shortcuts(parent)
{
  setAutoRelaySignals(false);
}

void ShortcutsAdaptor::volumeUp()
{
  if (m_shortcuts) {
    m_shortcuts->volumeUp();
  }
}

void ShortcutsAdaptor::volumeDown()
{
  if (m_shortcuts) {
    m_shortcuts->volumeDown();
  }
}

void ShortcutsAdaptor::volumeMute()
{
  if (m_shortcuts) {
    m_shortcuts->volumeMute();
  }
}

void ShortcutsAdaptor::brightnessUp()
{
  if (m_shortcuts) {
    m_shortcuts->brightnessUp();
  }
}

void ShortcutsAdaptor::brightnessDown()
{
  if (m_shortcuts) {
    m_shortcuts->brightnessDown();
  }
}

void ShortcutsAdaptor::mediaPlayPause()
{
  if (m_shortcuts) {
    m_shortcuts->mediaPlayPause();
  }
}

void ShortcutsAdaptor::mediaNext()
{
  if (m_shortcuts) {
    m_shortcuts->mediaNext();
  }
}

void ShortcutsAdaptor::mediaPrevious()
{
  if (m_shortcuts) {
    m_shortcuts->mediaPrevious();
  }
}

void ShortcutsAdaptor::lock()
{
  if (m_shortcuts) {
    m_shortcuts->lock();
  }
}

void ShortcutsAdaptor::launcher()
{
  if (m_shortcuts) {
    m_shortcuts->launcher();
  }
}

} // namespace spike
