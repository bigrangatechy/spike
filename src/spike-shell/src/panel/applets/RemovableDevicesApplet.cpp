#include "panel/applets/RemovableDevicesApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

RemovableDevicesApplet::RemovableDevicesApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("RemovableDevicesApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &RemovableDevicesApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(280);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Removable devices"), m_popup));
  m_list = new QListWidget(m_popup);
  lay->addWidget(m_list, 1);
  auto *eject = new QPushButton(QStringLiteral("Unmount / Eject"), m_popup);
  lay->addWidget(eject);
  connect(eject, &QPushButton::clicked, this, &RemovableDevicesApplet::onEject);

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &RemovableDevicesApplet::refresh);
  timer->start(5000);
  refresh();
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
    QDBusInterface props(QStringLiteral("org.freedesktop.UDisks2"), op.path(),
                         QStringLiteral("org.freedesktop.DBus.Properties"),
                         QDBusConnection::systemBus());
    if (!props.isValid()) {
      continue;
    }
    const QVariant hint =
        props.call(QStringLiteral("Get"), QStringLiteral("org.freedesktop.UDisks2.Block"),
                   QStringLiteral("HintSystem"))
            .arguments()
            .value(0);
    // HintSystem is nested variant
    bool system = false;
    if (hint.canConvert<QDBusVariant>()) {
      system = hint.value<QDBusVariant>().variant().toBool();
    }
    if (system) {
      continue;
    }
    const QVariant fs =
        props.call(QStringLiteral("Get"), QStringLiteral("org.freedesktop.UDisks2.Block"),
                   QStringLiteral("IdUsage"))
            .arguments()
            .value(0);
    QString usage;
    if (fs.canConvert<QDBusVariant>()) {
      usage = fs.value<QDBusVariant>().variant().toString();
    }
    if (usage != QLatin1String("filesystem")) {
      continue;
    }
    QDBusInterface fsIface(QStringLiteral("org.freedesktop.UDisks2"), op.path(),
                           QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                           QDBusConnection::systemBus());
    if (!fsIface.isValid()) {
      continue;
    }
    const QVariant mountsVar = fsIface.property("MountPoints");
    // MountPoints is aay
    QString mount;
    const QVariantList points = mountsVar.toList();
    // On some systems it's QDBusArgument
    Q_UNUSED(points);
    const QByteArray raw = mountsVar.toByteArray();
    if (!raw.isEmpty()) {
      mount = QString::fromUtf8(raw);
    }
    // Prefer MountPoints via Properties Get
    QDBusMessage msg = props.call(QStringLiteral("Get"),
                                  QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                                  QStringLiteral("MountPoints"));
    if (msg.type() == QDBusMessage::ReplyMessage && !msg.arguments().isEmpty()) {
      const QVariant v = msg.arguments().first();
      QVariant inner = v;
      if (v.canConvert<QDBusVariant>()) {
        inner = v.value<QDBusVariant>().variant();
      }
      const QDBusArgument arg = inner.value<QDBusArgument>();
      if (arg.currentType() == QDBusArgument::ArrayType) {
        arg.beginArray();
        while (!arg.atEnd()) {
          QByteArray mp;
          arg >> mp;
          if (!mp.isEmpty()) {
            mount = QString::fromUtf8(mp);
            break;
          }
        }
        arg.endArray();
      }
    }
    if (mount.isEmpty()) {
      continue; // only show mounted removables for MVP
    }
    Vol vol;
    vol.path = op.path();
    vol.mount = mount;
    const QVariant labelVar =
        props.call(QStringLiteral("Get"), QStringLiteral("org.freedesktop.UDisks2.Block"),
                   QStringLiteral("IdLabel"))
            .arguments()
            .value(0);
    if (labelVar.canConvert<QDBusVariant>()) {
      vol.label = labelVar.value<QDBusVariant>().variant().toString();
    }
    if (vol.label.isEmpty()) {
      vol.label = QFileInfo(mount).fileName();
    }
    out.append(vol);
  }
  return out;
}

void RemovableDevicesApplet::refresh()
{
  const auto vols = listVolumes();
  if (vols.isEmpty()) {
    hide();
    return;
  }
  show();
  tray::setTrayIcon(this, QStringLiteral("drive-removable-media"),
                    QStringLiteral("USB%1").arg(vols.size()));
  setToolTip(QStringLiteral("%1 removable volume(s)").arg(vols.size()));
  if (m_list) {
    m_list->clear();
    for (const Vol &v : vols) {
      auto *item = new QListWidgetItem(QStringLiteral("%1\n%2").arg(v.label, v.mount), m_list);
      item->setData(Qt::UserRole, v.path);
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

void RemovableDevicesApplet::onEject()
{
  if (!m_list || !m_list->currentItem()) {
    return;
  }
  const QString path = m_list->currentItem()->data(Qt::UserRole).toString();
  QDBusInterface fs(QStringLiteral("org.freedesktop.UDisks2"), path,
                    QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                    QDBusConnection::systemBus());
  fs.call(QStringLiteral("Unmount"), QVariantMap());
  refresh();
}

} // namespace spike
