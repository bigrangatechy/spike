#include "panel/applets/BluetoothApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QCheckBox>
#include <QLabel>
#include <QListWidget>
#include <QProcess>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

BluetoothApplet::BluetoothApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("BluetoothApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &BluetoothApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(280);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Bluetooth"), m_popup));
  m_power = new QCheckBox(QStringLiteral("Bluetooth powered"), m_popup);
  lay->addWidget(m_power);
  m_list = new QListWidget(m_popup);
  lay->addWidget(m_list, 1);
  auto *connectBtn = new QPushButton(QStringLiteral("Connect / Disconnect"), m_popup);
  auto *settings = new QPushButton(QStringLiteral("Bluetooth Settings"), m_popup);
  lay->addWidget(connectBtn);
  lay->addWidget(settings);
  connect(m_power, &QCheckBox::toggled, this, &BluetoothApplet::onPower);
  connect(connectBtn, &QPushButton::clicked, this, &BluetoothApplet::onDeviceActivated);
  connect(settings, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("bluetooth"));
  });

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &BluetoothApplet::refresh);
  timer->start(4000);
  refresh();
}

QString BluetoothApplet::adapterPath() const
{
  QDBusInterface hci(QStringLiteral("org.bluez"), QStringLiteral("/org/bluez/hci0"),
                     QStringLiteral("org.bluez.Adapter1"), QDBusConnection::systemBus());
  if (hci.isValid() && !hci.lastError().isValid()) {
    const QVariant powered = hci.property("Powered");
    if (powered.isValid()) {
      return QStringLiteral("/org/bluez/hci0");
    }
  }
  return {};
}

void BluetoothApplet::refresh()
{
  const QString path = adapterPath();
  m_hasAdapter = !path.isEmpty();
  if (!m_hasAdapter) {
    hide();
    return;
  }
  show();
  QDBusInterface adapter(QStringLiteral("org.bluez"), path, QStringLiteral("org.bluez.Adapter1"),
                         QDBusConnection::systemBus());
  const bool powered = adapter.property("Powered").toBool();
  if (m_power) {
    m_power->blockSignals(true);
    m_power->setChecked(powered);
    m_power->blockSignals(false);
  }
  tray::setTrayIcon(this,
                    powered ? QStringLiteral("bluetooth-active") : QStringLiteral("bluetooth-disabled"),
                    powered ? QStringLiteral("BT") : QStringLiteral("bt"));
  setToolTip(powered ? QStringLiteral("Bluetooth on") : QStringLiteral("Bluetooth off"));

  if (m_list) {
    m_list->clear();
    QProcess proc;
    proc.start(QStringLiteral("bluetoothctl"), {QStringLiteral("devices")});
    if (proc.waitForFinished(3000)) {
      const QString out = QString::fromUtf8(proc.readAllStandardOutput());
      for (const QString &line : out.split(QLatin1Char('\n'))) {
        // Device AA:BB:.. Name
        if (!line.startsWith(QLatin1String("Device "))) {
          continue;
        }
        const QString rest = line.mid(7).trimmed();
        const int sp = rest.indexOf(QLatin1Char(' '));
        if (sp < 0) {
          continue;
        }
        const QString mac = rest.left(sp);
        const QString name = rest.mid(sp + 1);
        auto *item = new QListWidgetItem(name, m_list);
        item->setData(Qt::UserRole, mac);
      }
    }
    if (m_list->count() == 0) {
      m_list->addItem(QStringLiteral("No paired devices"));
    }
  }
}

void BluetoothApplet::onPower(bool on)
{
  const QString path = adapterPath();
  if (path.isEmpty()) {
    return;
  }
  QDBusInterface adapter(QStringLiteral("org.bluez"), path, QStringLiteral("org.bluez.Adapter1"),
                         QDBusConnection::systemBus());
  adapter.setProperty("Powered", on);
  refresh();
}

void BluetoothApplet::onDeviceActivated()
{
  if (!m_list || !m_list->currentItem()) {
    return;
  }
  const QString mac = m_list->currentItem()->data(Qt::UserRole).toString();
  if (mac.isEmpty()) {
    return;
  }
  QProcess::startDetached(QStringLiteral("bluetoothctl"),
                          {QStringLiteral("connect"), mac});
  QTimer::singleShot(1500, this, &BluetoothApplet::refresh);
}

void BluetoothApplet::togglePopup()
{
  if (!m_hasAdapter || !m_popup) {
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

} // namespace spike
