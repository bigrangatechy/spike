#include "settings/StoragePage.hpp"

#include "settings/ConfigClient.hpp"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QProcess>
#include <QPushButton>
#include <QTableWidget>
#include <QVariantMap>
#include <QVector>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

QVariant udiskProp(const QString &path, const QString &iface, const QString &name)
{
  QDBusInterface props(QStringLiteral("org.freedesktop.UDisks2"), path,
                       QStringLiteral("org.freedesktop.DBus.Properties"),
                       QDBusConnection::systemBus());
  if (!props.isValid()) {
    return {};
  }
  const QDBusMessage reply = props.call(QStringLiteral("Get"), iface, name);
  if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
    return {};
  }
  QVariant v = reply.arguments().first();
  if (v.canConvert<QDBusVariant>()) {
    v = v.value<QDBusVariant>().variant();
  }
  return v;
}

QString udiskMount(const QString &path)
{
  const QVariant mountsVar =
      udiskProp(path, QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                QStringLiteral("MountPoints"));
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

struct RemovableRow {
  QString path;
  QString label;
  QString mount;
  QString device;
};

QVector<RemovableRow> listRemovable()
{
  QVector<RemovableRow> out;
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
    if (udiskProp(op.path(), QStringLiteral("org.freedesktop.UDisks2.Block"),
                  QStringLiteral("HintSystem"))
            .toBool()) {
      continue;
    }
    if (udiskProp(op.path(), QStringLiteral("org.freedesktop.UDisks2.Block"),
                  QStringLiteral("IdUsage"))
            .toString() != QLatin1String("filesystem")) {
      continue;
    }
    RemovableRow r;
    r.path = op.path();
    r.label = udiskProp(op.path(), QStringLiteral("org.freedesktop.UDisks2.Block"),
                        QStringLiteral("IdLabel"))
                  .toString();
    r.device = QString::fromUtf8(
        udiskProp(op.path(), QStringLiteral("org.freedesktop.UDisks2.Block"),
                  QStringLiteral("Device"))
            .toByteArray());
    r.mount = udiskMount(op.path());
    if (r.label.isEmpty()) {
      r.label = r.device;
    }
    out.append(r);
  }
  return out;
}

} // namespace

QWidget *makeStoragePage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Storage & devices</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Internal disks (read-only overview) and removable USB volumes. "
          "Mount/eject uses UDisks2. Wipe/partition only via Install Spike."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *primary = new QLabel(w);
  primary->setWordWrap(true);
  lay->addWidget(primary);

  auto *table = new QTableWidget(w);
  table->setColumnCount(5);
  table->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Size"),
                                    QStringLiteral("Type"), QStringLiteral("Tran"),
                                    QStringLiteral("Mount")});
  table->horizontalHeader()->setStretchLastSection(true);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  lay->addWidget(new QLabel(QStringLiteral("<b>All block devices</b>"), w));
  lay->addWidget(table, 1);

  lay->addWidget(new QLabel(QStringLiteral("<b>Removable / USB</b>"), w));
  auto *usbList = new QListWidget(w);
  usbList->setMinimumHeight(100);
  lay->addWidget(usbList);
  auto *usbRow = new QHBoxLayout();
  auto *usbMount = new QPushButton(QStringLiteral("Mount"), w);
  auto *usbEject = new QPushButton(QStringLiteral("Eject"), w);
  auto *usbOpen = new QPushButton(QStringLiteral("Open"), w);
  usbRow->addWidget(usbMount);
  usbRow->addWidget(usbEject);
  usbRow->addWidget(usbOpen);
  usbRow->addStretch(1);
  lay->addLayout(usbRow);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Refresh"), w);
  row->addWidget(reload);
  row->addStretch(1);
  lay->addLayout(row);

  auto loadUsb = [usbList, status]() {
    usbList->clear();
    const auto vols = listRemovable();
    if (vols.isEmpty()) {
      usbList->addItem(QStringLiteral("(No removable volumes — plug in a USB stick)"));
      return;
    }
    for (const RemovableRow &r : vols) {
      auto *item = new QListWidgetItem(
          QStringLiteral("%1 (%2)\n%3")
              .arg(r.label, r.device,
                   r.mount.isEmpty() ? QStringLiteral("(not mounted)") : r.mount));
      item->setData(Qt::UserRole, r.path);
      item->setData(Qt::UserRole + 1, r.mount);
      usbList->addItem(item);
    }
    status->setText(QStringLiteral("%1 removable volume(s)").arg(vols.size()));
  };

  auto load = [config, table, primary, status, statusBar, loadUsb]() {
    table->setRowCount(0);
    primary->setText(QStringLiteral("Primary disk (DetectHardware): …"));

    if (config) {
      QString err;
      const QString json = config->detectHardware(&err);
      if (!json.isEmpty()) {
        const QJsonObject root = QJsonDocument::fromJson(json.toUtf8()).object();
        const QJsonObject st = root.value(QStringLiteral("storage")).toObject();
        if (!st.isEmpty()) {
          primary->setText(
              QStringLiteral("Primary disk (DetectHardware): <b>%1</b> — %2 GB (%3)")
                  .arg(st.value(QStringLiteral("device")).toString(QStringLiteral("?")),
                       QString::number(st.value(QStringLiteral("size_gb")).toInt()),
                       st.value(QStringLiteral("type")).toString(QStringLiteral("unknown"))));
        } else {
          primary->setText(QStringLiteral("Primary disk: (not in DetectHardware output)"));
        }
      } else {
        primary->setText(QStringLiteral("DetectHardware unavailable: %1").arg(err));
      }
    }

    const QString out = runCapture(
        QStringLiteral("lsblk"),
        {QStringLiteral("-J"), QStringLiteral("-o"),
         QStringLiteral("NAME,SIZE,TYPE,TRAN,MOUNTPOINT,MODEL")});
    if (out.isEmpty()) {
      status->setText(QStringLiteral("lsblk failed"));
      loadUsb();
      return;
    }
    const QJsonObject root = QJsonDocument::fromJson(out.toUtf8()).object();
    const QJsonArray block = root.value(QStringLiteral("blockdevices")).toArray();

    QVector<QJsonObject> rows;
    QVector<QJsonArray> stack;
    stack.append(block);
    while (!stack.isEmpty()) {
      const QJsonArray arr = stack.takeLast();
      for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        rows.append(o);
        const QJsonArray kids = o.value(QStringLiteral("children")).toArray();
        if (!kids.isEmpty()) {
          stack.append(kids);
        }
      }
    }

    table->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
      const QJsonObject o = rows.at(i);
      table->setItem(i, 0, new QTableWidgetItem(o.value(QStringLiteral("name")).toString()));
      table->setItem(i, 1, new QTableWidgetItem(o.value(QStringLiteral("size")).toString()));
      table->setItem(i, 2, new QTableWidgetItem(o.value(QStringLiteral("type")).toString()));
      table->setItem(i, 3, new QTableWidgetItem(o.value(QStringLiteral("tran")).toString()));
      const QString mount = o.value(QStringLiteral("mountpoint")).toString();
      const QString model = o.value(QStringLiteral("model")).toString();
      table->setItem(i, 4,
                     new QTableWidgetItem(mount.isEmpty() ? model : QStringLiteral("%1 %2")
                                                                       .arg(mount, model)
                                                                       .trimmed()));
    }
    status->setText(QStringLiteral("%1 block device row(s)").arg(rows.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Storage loaded"));
    }
    loadUsb();
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(usbMount, &QPushButton::clicked, w, [usbList, status, loadUsb]() {
    if (!usbList->currentItem()) {
      return;
    }
    const QString path = usbList->currentItem()->data(Qt::UserRole).toString();
    if (path.isEmpty()) {
      return;
    }
    QDBusInterface fs(QStringLiteral("org.freedesktop.UDisks2"), path,
                      QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                      QDBusConnection::systemBus());
    const QDBusMessage reply = fs.call(QStringLiteral("Mount"), QVariantMap());
    if (reply.type() == QDBusMessage::ErrorMessage) {
      status->setText(QStringLiteral("Mount failed: %1").arg(reply.errorMessage()));
    }
    loadUsb();
  });
  QObject::connect(usbEject, &QPushButton::clicked, w, [usbList, status, loadUsb]() {
    if (!usbList->currentItem()) {
      return;
    }
    const QString path = usbList->currentItem()->data(Qt::UserRole).toString();
    if (path.isEmpty()) {
      return;
    }
    QDBusInterface fs(QStringLiteral("org.freedesktop.UDisks2"), path,
                      QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                      QDBusConnection::systemBus());
    const QDBusMessage reply = fs.call(QStringLiteral("Unmount"), QVariantMap());
    if (reply.type() == QDBusMessage::ErrorMessage) {
      status->setText(QStringLiteral("Eject failed: %1").arg(reply.errorMessage()));
    } else {
      status->setText(QStringLiteral("Ejected — safe to unplug."));
    }
    loadUsb();
  });
  QObject::connect(usbOpen, &QPushButton::clicked, w, [usbList, status]() {
    if (!usbList->currentItem()) {
      return;
    }
    QString mount = usbList->currentItem()->data(Qt::UserRole + 1).toString();
    const QString path = usbList->currentItem()->data(Qt::UserRole).toString();
    if (mount.isEmpty() && !path.isEmpty()) {
      QDBusInterface fs(QStringLiteral("org.freedesktop.UDisks2"), path,
                        QStringLiteral("org.freedesktop.UDisks2.Filesystem"),
                        QDBusConnection::systemBus());
      fs.call(QStringLiteral("Mount"), QVariantMap());
      mount = udiskMount(path);
    }
    if (mount.isEmpty()) {
      status->setText(QStringLiteral("Nothing to open (not mounted)."));
      return;
    }
    if (!QProcess::startDetached(QStringLiteral("xdg-open"), {mount})) {
      QProcess::startDetached(QStringLiteral("dolphin"), {mount});
    }
  });

  load();
  return w;
}

} // namespace spike
