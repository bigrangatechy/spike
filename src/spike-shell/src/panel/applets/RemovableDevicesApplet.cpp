#include "panel/applets/RemovableDevicesApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QProcess>
#include <QPushButton>
#include <QSet>
#include <QTimer>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QVariant dbusProp(const QString &path, const QString &iface, const QString &name)
{
  QDBusInterface props(QStringLiteral("org.freedesktop.UDisks2"), path,
                       QStringLiteral("org.freedesktop.DBus.Properties"),
                       QDBusConnection::systemBus());
  if (!props.isValid()) {
    return {};
  }
  const QDBusMessage reply =
      props.call(QStringLiteral("Get"), iface, name);
  if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
    return {};
  }
  QVariant v = reply.arguments().first();
  if (v.canConvert<QDBusVariant>()) {
    v = v.value<QDBusVariant>().variant();
  }
  return v;
}

QString firstMountPoint(const QString &path)
{
  const QVariant mountsVar =
      dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
               QStringLiteral("MountPoints"));
  if (!mountsVar.isValid()) {
    return {};
  }
  const QDBusArgument arg = mountsVar.value<QDBusArgument>();
  if (arg.currentType() == QDBusArgument::ArrayType) {
    arg.beginArray();
    while (!arg.atEnd()) {
      QByteArray mp;
      arg >> mp;
      if (!mp.isEmpty()) {
        arg.endArray();
        return QString::fromUtf8(mp);
      }
    }
    arg.endArray();
  }
  return {};
}

void notifyUsb(const QString &summary, const QString &body)
{
  QDBusInterface note(QStringLiteral("org.freedesktop.Notifications"),
                      QStringLiteral("/org/freedesktop/Notifications"),
                      QStringLiteral("org.freedesktop.Notifications"),
                      QDBusConnection::sessionBus());
  if (!note.isValid()) {
    return;
  }
  note.call(QStringLiteral("Notify"), QStringLiteral("Spike"), uint(0),
            QStringLiteral("drive-removable-media"), summary, body, QStringList(),
            QVariantMap(), 8000);
}

} // namespace

RemovableDevicesApplet::RemovableDevicesApplet(QWidget *parent)
  : QPushButton(parent)
{
  qDBusRegisterMetaType<QList<QDBusObjectPath>>();
  setObjectName(QStringLiteral("RemovableDevicesApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &RemovableDevicesApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(320);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Disks & USB"), m_popup));
  m_list = new QListWidget(m_popup);
  lay->addWidget(m_list, 1);
  auto *row = new QHBoxLayout();
  auto *openBtn = new QPushButton(QStringLiteral("Open"), m_popup);
  auto *mountBtn = new QPushButton(QStringLiteral("Mount"), m_popup);
  auto *eject = new QPushButton(QStringLiteral("Eject"), m_popup);
  row->addWidget(openBtn);
  row->addWidget(mountBtn);
  row->addWidget(eject);
  lay->addLayout(row);
  connect(openBtn, &QPushButton::clicked, this, &RemovableDevicesApplet::onOpen);
  connect(mountBtn, &QPushButton::clicked, this, &RemovableDevicesApplet::onMount);
  connect(eject, &QPushButton::clicked, this, &RemovableDevicesApplet::onEject);
  connect(m_list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *) { onOpen(); });

  // Hotplug: signatures must match ObjectManager or the connect is a no-op.
  QDBusConnection::systemBus().connect(
      QStringLiteral("org.freedesktop.UDisks2"), QStringLiteral("/org/freedesktop/UDisks2"),
      QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesAdded"),
      this, SLOT(onInterfacesAdded(QDBusObjectPath,QVariantMap)));
  QDBusConnection::systemBus().connect(
      QStringLiteral("org.freedesktop.UDisks2"), QStringLiteral("/org/freedesktop/UDisks2"),
      QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesRemoved"),
      this, SLOT(onInterfacesRemoved(QDBusObjectPath,QStringList)));

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &RemovableDevicesApplet::refresh);
  timer->start(4000);
  // Delay first scan so NotificationDaemon can register.
  QTimer::singleShot(1500, this, &RemovableDevicesApplet::refresh);
}

void RemovableDevicesApplet::onInterfacesAdded(const QDBusObjectPath &, const QVariantMap &)
{
  onUdisksChanged();
}

void RemovableDevicesApplet::onInterfacesRemoved(const QDBusObjectPath &, const QStringList &)
{
  onUdisksChanged();
}

void RemovableDevicesApplet::onUdisksChanged()
{
  QTimer::singleShot(500, this, [this]() {
    for (const Vol &v : listVolumes()) {
      if (v.mount.isEmpty()) {
        maybeAutomount(v.path);
      }
    }
    refresh();
  });
  QTimer::singleShot(2000, this, [this]() {
    for (const Vol &v : listVolumes()) {
      if (v.mount.isEmpty()) {
        maybeAutomount(v.path);
      }
    }
    refresh();
  });
}

QVector<RemovableDevicesApplet::Vol> RemovableDevicesApplet::listVolumes() const
{
  QVector<Vol> out;
  QDBusInterface mgr(QStringLiteral("org.freedesktop.UDisks2"),
                     QStringLiteral("/org/freedesktop/UDisks2/Manager"),
                     QStringLiteral("org.freedesktop.UDisks2.Manager"),
                     QDBusConnection::systemBus());
  if (!mgr.isValid()) {
    return out;
  }
  QDBusReply<QList<QDBusObjectPath>> block =
      mgr.call(QStringLiteral("GetBlockDevices"), QVariantMap());
  if (!block.isValid()) {
    return out;
  }
  for (const QDBusObjectPath &op : block.value()) {
    const QString path = op.path();
    const bool system =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("HintSystem"))
            .toBool();
    if (system) {
      continue;
    }
    const QString usage =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("IdUsage"))
            .toString();
    if (usage != QLatin1String("filesystem")) {
      continue;
    }
    // Prefer USB / HintAuto / removable drive; still allow other non-system volumes.
    const QString bus =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("IdBus"))
            .toString();
    const bool hintAuto =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("HintAuto"))
            .toBool();
    Q_UNUSED(bus);
    Q_UNUSED(hintAuto);

    Vol vol;
    vol.path = path;
    vol.mount = firstMountPoint(path);
    vol.label =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("IdLabel"))
            .toString();
    if (vol.label.isEmpty()) {
      const QByteArray device =
          dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                   QStringLiteral("Device"))
              .toByteArray();
      vol.label = device.isEmpty() ? QFileInfo(path).fileName()
                                   : QString::fromUtf8(device).trimmed();
    }
    vol.size =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("Size"))
            .toULongLong();
    out.append(vol);
  }
  return out;
}

bool RemovableDevicesApplet::mountVolume(const QString &path, QString *error)
{
  QDBusInterface fs(QStringLiteral("org.freedesktop.UDisks2"), path,
                    QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                    QDBusConnection::systemBus());
  if (!fs.isValid()) {
    if (error) {
      *error = QStringLiteral("Filesystem interface missing");
    }
    return false;
  }
  const QDBusMessage reply = fs.call(QStringLiteral("Mount"), QVariantMap());
  if (reply.type() == QDBusMessage::ErrorMessage) {
    if (error) {
      *error = reply.errorMessage();
    }
    return false;
  }
  return true;
}

void RemovableDevicesApplet::maybeAutomount(const QString &path)
{
  const bool system =
      dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
               QStringLiteral("HintSystem"))
          .toBool();
  if (system) {
    return;
  }
  const QString usage =
      dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
               QStringLiteral("IdUsage"))
          .toString();
  if (usage != QLatin1String("filesystem")) {
    return;
  }
  if (!firstMountPoint(path).isEmpty()) {
    return;
  }
  QString err;
  if (mountVolume(path, &err)) {
    const QString label =
        dbusProp(path, QStringLiteral("org.freedesktop.UDisks2.Block"),
                 QStringLiteral("IdLabel"))
            .toString();
    const QString mount = firstMountPoint(path);
    notifyUsb(QStringLiteral("USB drive detected"),
              QStringLiteral("%1 mounted at %2")
                  .arg(label.isEmpty() ? QStringLiteral("Removable disk") : label,
                       mount.isEmpty() ? QStringLiteral("(mounting…)") : mount));
  }
}

void RemovableDevicesApplet::refresh()
{
  const auto vols = listVolumes();
  QSet<QString> now;
  for (const Vol &v : vols) {
    now.insert(v.path);
    if (!m_knownPaths.contains(v.path)) {
      if (m_seenInitialScan) {
        if (!v.mount.isEmpty()) {
          notifyUsb(QStringLiteral("USB drive detected"),
                    QStringLiteral("%1 — %2").arg(v.label, v.mount));
        } else {
          maybeAutomount(v.path);
        }
      } else if (v.mount.isEmpty()) {
        maybeAutomount(v.path);
      }
    }
  }
  // Detect removals after initial scan.
  if (m_seenInitialScan) {
    for (const QString &prev : m_knownPaths) {
      if (!now.contains(prev)) {
        notifyUsb(QStringLiteral("USB drive removed"),
                  QStringLiteral("Removable storage is no longer available."));
        break;
      }
    }
  }
  m_knownPaths = now;
  m_seenInitialScan = true;

  if (vols.isEmpty()) {
    hide();
    if (m_list) {
      m_list->clear();
    }
    return;
  }
  show();
  tray::setTrayIcon(this, QStringLiteral("drive-removable-media"),
                    QStringLiteral("USB%1").arg(vols.size()));
  setToolTip(QStringLiteral("%1 removable volume(s)").arg(vols.size()));
  if (m_list) {
    m_list->clear();
    for (const Vol &v : vols) {
      const QString size = v.size > 0
                               ? QStringLiteral("%1 GB").arg(v.size / (1024.0 * 1024.0 * 1024.0), 0,
                                                            'f', 1)
                               : QString();
      const QString line =
          v.mount.isEmpty()
              ? QStringLiteral("%1 %2\n(not mounted)").arg(v.label, size).trimmed()
              : QStringLiteral("%1 %2\n%3").arg(v.label, size, v.mount).trimmed();
      auto *item = new QListWidgetItem(line, m_list);
      item->setData(Qt::UserRole, v.path);
      item->setData(Qt::UserRole + 1, v.mount);
    }
  }
}

void RemovableDevicesApplet::togglePopup()
{
  if (!m_popup || isHidden()) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  refresh();
  tray::placePopupAbove(this, m_popup);
  m_popup->show();
}

void RemovableDevicesApplet::onOpen()
{
  if (!m_list || !m_list->currentItem()) {
    return;
  }
  QString mount = m_list->currentItem()->data(Qt::UserRole + 1).toString();
  if (mount.isEmpty()) {
    onMount();
    mount = m_list->currentItem() ? m_list->currentItem()->data(Qt::UserRole + 1).toString()
                                  : QString();
    // Re-read after mount
    const QString path = m_list->currentItem()->data(Qt::UserRole).toString();
    mount = firstMountPoint(path);
  }
  if (mount.isEmpty()) {
    return;
  }
  if (!QProcess::startDetached(QStringLiteral("xdg-open"), {mount})) {
    QProcess::startDetached(QStringLiteral("dolphin"), {mount});
  }
  if (m_popup) {
    m_popup->hide();
  }
}

void RemovableDevicesApplet::onMount()
{
  if (!m_list || !m_list->currentItem()) {
    return;
  }
  const QString path = m_list->currentItem()->data(Qt::UserRole).toString();
  QString err;
  if (!mountVolume(path, &err)) {
    notifyUsb(QStringLiteral("Could not mount USB"), err);
  }
  refresh();
}

void RemovableDevicesApplet::onEject()
{
  if (!m_list || !m_list->currentItem()) {
    return;
  }
  const QString path = m_list->currentItem()->data(Qt::UserRole).toString();
  QDBusInterface fs(QStringLiteral("org.freedesktop.UDisks2"), path,
                    QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                    QDBusConnection::systemBus());
  const QDBusMessage reply = fs.call(QStringLiteral("Unmount"), QVariantMap());
  if (reply.type() != QDBusMessage::ErrorMessage) {
    notifyUsb(QStringLiteral("USB drive can be safely unplugged"),
              QStringLiteral("Volume ejected."));
  }
  // Try power-off the drive if possible
  QDBusInterface block(QStringLiteral("org.freedesktop.UDisks2"), path,
                       QStringLiteral("org.freedesktop.DBus.Properties"),
                       QDBusConnection::systemBus());
  Q_UNUSED(block);
  refresh();
}

} // namespace spike
