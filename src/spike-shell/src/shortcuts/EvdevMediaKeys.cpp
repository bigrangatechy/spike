#include "shortcuts/EvdevMediaKeys.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QSocketNotifier>

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include <cstring>

namespace spike {

namespace {

bool hasKey(int fd, int key)
{
  // KEY_MAX is large; use enough longs for the bitmask.
  static constexpr size_t kBytes = (KEY_MAX / 8) + 1;
  unsigned char mask[kBytes];
  std::memset(mask, 0, sizeof(mask));
  if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(mask)), mask) < 0) {
    return false;
  }
  return mask[key / 8] & (1u << (key % 8));
}

bool looksLikeKeyboard(int fd)
{
  // Prefer devices that can emit volume or brightness — covers dedicated media
  // keyboards and Fn-row laptops without grabbing every HID gadget.
  return hasKey(fd, KEY_VOLUMEUP) || hasKey(fd, KEY_VOLUMEDOWN) || hasKey(fd, KEY_MUTE) ||
         hasKey(fd, KEY_BRIGHTNESSUP) || hasKey(fd, KEY_BRIGHTNESSDOWN) ||
         hasKey(fd, KEY_PLAYPAUSE) || hasKey(fd, KEY_NEXTSONG) || hasKey(fd, KEY_PREVIOUSSONG);
}

} // namespace

EvdevMediaKeys::EvdevMediaKeys(QObject *parent)
  : QObject(parent)
{
}

EvdevMediaKeys::~EvdevMediaKeys()
{
  closeAll();
}

void EvdevMediaKeys::closeAll()
{
  for (Device &d : m_devices) {
    delete d.notifier;
    d.notifier = nullptr;
    if (d.fd >= 0) {
      ::close(d.fd);
      d.fd = -1;
    }
  }
  m_devices.clear();
}

bool EvdevMediaKeys::openDevice(const QString &path)
{
  const int fd = ::open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);
  if (fd < 0) {
    return false;
  }
  if (!looksLikeKeyboard(fd)) {
    ::close(fd);
    return false;
  }
  Device d;
  d.fd = fd;
  d.notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
  QObject::connect(d.notifier, &QSocketNotifier::activated, this, [this, fd]() { onReadable(fd); });
  m_devices.append(d);
  return true;
}

void EvdevMediaKeys::start()
{
  closeAll();
  const QDir dir(QStringLiteral("/dev/input"));
  if (!dir.exists()) {
    return;
  }
  QStringList paths;
  for (const QString &sub : {QStringLiteral("by-path"), QStringLiteral("by-id")}) {
    QDir s(dir.filePath(sub));
    if (!s.exists()) {
      continue;
    }
    for (const QString &e : s.entryList(QDir::Files)) {
      paths << s.absoluteFilePath(e);
    }
  }
  if (paths.isEmpty()) {
    for (const QString &e : dir.entryList({QStringLiteral("event*")}, QDir::System | QDir::Files)) {
      paths << dir.absoluteFilePath(e);
    }
  }
  QSet<QString> seen;
  for (const QString &p : paths) {
    const QString canon = QFileInfo(p).canonicalFilePath();
    if (canon.isEmpty() || seen.contains(canon)) {
      continue;
    }
    if (openDevice(canon)) {
      seen.insert(canon);
    }
  }
}

void EvdevMediaKeys::onReadable(int fd)
{
  input_event ev{};
  while (true) {
    const ssize_t n = ::read(fd, &ev, sizeof(ev));
    if (n < 0) {
      break;
    }
    if (static_cast<size_t>(n) < sizeof(ev)) {
      break;
    }
    if (ev.type != EV_KEY || ev.value == 0) { // ignore key release; allow press + repeat
      continue;
    }
    switch (ev.code) {
    case KEY_VOLUMEUP:
      emit volumeUp();
      break;
    case KEY_VOLUMEDOWN:
      emit volumeDown();
      break;
    case KEY_MUTE:
      emit volumeMute();
      break;
    case KEY_BRIGHTNESSUP:
      emit brightnessUp();
      break;
    case KEY_BRIGHTNESSDOWN:
      emit brightnessDown();
      break;
    case KEY_PLAYPAUSE:
    case KEY_PLAY:
    case KEY_PAUSE:
    case KEY_MEDIA:
      emit mediaPlayPause();
      break;
    case KEY_NEXTSONG:
      emit mediaNext();
      break;
    case KEY_PREVIOUSSONG:
      emit mediaPrevious();
      break;
    default:
      break;
    }
  }
}

} // namespace spike
