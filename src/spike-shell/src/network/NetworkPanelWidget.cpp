#include "network/NetworkPanelWidget.hpp"

#include "network/NmClient.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace spike {

NetworkPanelWidget::NetworkPanelWidget(NmClient *nm, QWidget *parent)
  : QWidget(parent)
  , m_nm(nm)
{
  setObjectName(QStringLiteral("NetworkPanelWidget"));
  auto *lay = new QVBoxLayout(this);
  lay->setContentsMargins(8, 8, 8, 8);
  lay->setSpacing(6);

  m_status = new QLabel(this);
  m_status->setWordWrap(true);
  m_status->setObjectName(QStringLiteral("NetworkStatus"));
  lay->addWidget(m_status);

  m_wifiOn = new QCheckBox(QStringLiteral("Wi‑Fi"), this);
  lay->addWidget(m_wifiOn);

  m_vpnRow = new QLabel(this);
  m_vpnRow->setWordWrap(true);
  m_vpnRow->setObjectName(QStringLiteral("NetworkVpnRow"));
  lay->addWidget(m_vpnRow);
  m_vpnDisconnect = new QPushButton(QStringLiteral("Disconnect VPN"), this);
  m_vpnDisconnect->setVisible(false);
  lay->addWidget(m_vpnDisconnect);

  m_list = new QListWidget(this);
  m_list->setMinimumHeight(160);
  lay->addWidget(m_list, 1);

  m_hint = new QLabel(this);
  m_hint->setWordWrap(true);
  lay->addWidget(m_hint);

  auto *row = new QHBoxLayout();
  auto *scan = new QPushButton(QStringLiteral("Scan"), this);
  auto *connectBtn = new QPushButton(QStringLiteral("Connect"), this);
  auto *disc = new QPushButton(QStringLiteral("Disconnect"), this);
  row->addWidget(scan);
  row->addWidget(connectBtn);
  row->addWidget(disc);
  lay->addLayout(row);

  auto *settings = new QPushButton(QStringLiteral("Network Settings"), this);
  lay->addWidget(settings);

  connect(m_wifiOn, &QCheckBox::toggled, this, &NetworkPanelWidget::onWifiToggled);
  connect(scan, &QPushButton::clicked, this, &NetworkPanelWidget::onScan);
  connect(connectBtn, &QPushButton::clicked, this, &NetworkPanelWidget::onConnectSelected);
  connect(disc, &QPushButton::clicked, this, &NetworkPanelWidget::onDisconnect);
  connect(m_list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *) {
    onConnectSelected();
  });
  connect(settings, &QPushButton::clicked, this, &NetworkPanelWidget::openFullSettings);
  connect(m_vpnDisconnect, &QPushButton::clicked, this, [this]() {
    if (!m_nm) {
      return;
    }
    QString err;
    for (const NmVpnConnection &v : m_nm->vpnConnections(&err)) {
      if (v.active) {
        m_nm->deactivateVpn(v.uuid.isEmpty() ? v.name : v.uuid, &err);
      }
    }
    refresh();
  });

  if (m_nm) {
    connect(m_nm, &NmClient::changed, this, &NetworkPanelWidget::refresh);
  }
  refresh();
}

void NetworkPanelWidget::refresh()
{
  if (!m_nm) {
    m_status->setText(QStringLiteral("NetworkManager client missing"));
    return;
  }
  QString err;
  if (!m_nm->isAvailable(&err)) {
    m_status->setText(QStringLiteral("NetworkManager unavailable:\n%1").arg(err));
    m_list->clear();
    return;
  }

  m_status->setText(m_nm->statusLabel());

  {
    QString verr;
    const auto vpns = m_nm->vpnConnections(&verr);
    QStringList activeNames;
    for (const NmVpnConnection &v : vpns) {
      if (v.active) {
        activeNames << (v.name.isEmpty() ? v.uuid : v.name);
      }
    }
    if (activeNames.isEmpty()) {
      m_vpnRow->setText(QStringLiteral("VPN: none active"));
      m_vpnDisconnect->setVisible(false);
    } else {
      m_vpnRow->setText(QStringLiteral("VPN: %1").arg(activeNames.join(QStringLiteral(", "))));
      m_vpnDisconnect->setVisible(true);
    }
  }

  m_blockWifiToggle = true;
  m_wifiOn->setChecked(m_nm->wirelessEnabled());
  m_wifiOn->setEnabled(true);
  m_blockWifiToggle = false;

  m_list->clear();
  const QString wifi = m_nm->wifiDevicePath(&err);
  if (wifi.isEmpty()) {
    // Still show ethernet devices.
    for (const NmDevice &d : m_nm->devices()) {
      if (d.type == 1) {
        auto *item = new QListWidgetItem(
            QStringLiteral("Ethernet · %1 (%2)").arg(d.iface, d.stateText), m_list);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
      }
    }
    m_hint->setText(QStringLiteral("No Wi‑Fi adapter detected."));
    return;
  }

  if (!m_nm->wirelessEnabled()) {
    m_hint->setText(QStringLiteral("Wi‑Fi is off."));
    return;
  }

  const auto aps = m_nm->accessPoints(wifi, &err);
  for (const NmAccessPoint &ap : aps) {
    QString label = ap.ssid;
    if (ap.active) {
      label.prepend(QStringLiteral("● "));
    }
    label += QStringLiteral("  %1%").arg(ap.strength);
    if (ap.secured) {
      label += QStringLiteral("  🔒");
    }
    auto *item = new QListWidgetItem(label, m_list);
    item->setData(Qt::UserRole, ap.ssid);
    item->setData(Qt::UserRole + 1, ap.secured);
    item->setData(Qt::UserRole + 2, ap.active);
  }
  m_hint->setText(aps.isEmpty() ? QStringLiteral("No networks found — try Scan.")
                                : QStringLiteral("Double-click or Connect to join a network."));
}

void NetworkPanelWidget::onWifiToggled(bool on)
{
  if (m_blockWifiToggle || !m_nm) {
    return;
  }
  QString err;
  if (!m_nm->setWirelessEnabled(on, &err)) {
    QMessageBox::warning(this, QStringLiteral("Wi‑Fi"), err);
    m_blockWifiToggle = true;
    m_wifiOn->setChecked(m_nm->wirelessEnabled());
    m_blockWifiToggle = false;
  }
  refresh();
}

void NetworkPanelWidget::onScan()
{
  if (!m_nm) {
    return;
  }
  QString err;
  const QString wifi = m_nm->wifiDevicePath(&err);
  if (!m_nm->requestScan(wifi, &err)) {
    m_hint->setText(QStringLiteral("Scan failed: %1").arg(err));
  } else {
    m_hint->setText(QStringLiteral("Scanning…"));
  }
  // Results often arrive a moment later via NM signals.
  refresh();
}

void NetworkPanelWidget::onDisconnect()
{
  if (!m_nm) {
    return;
  }
  QString err;
  if (!m_nm->disconnectActive(&err)) {
    m_hint->setText(err);
  }
  refresh();
}

void NetworkPanelWidget::onConnectSelected()
{
  if (!m_nm || !m_list->currentItem()) {
    return;
  }
  const QString ssid = m_list->currentItem()->data(Qt::UserRole).toString();
  if (ssid.isEmpty()) {
    return;
  }
  const bool secured = m_list->currentItem()->data(Qt::UserRole + 1).toBool();
  const bool active = m_list->currentItem()->data(Qt::UserRole + 2).toBool();
  if (active) {
    m_hint->setText(QStringLiteral("Already connected to %1").arg(ssid));
    return;
  }

  QString password;
  if (secured) {
    bool ok = false;
    password = QInputDialog::getText(this, QStringLiteral("Wi‑Fi password"),
                                     QStringLiteral("Password for “%1”:").arg(ssid),
                                     QLineEdit::Password, {}, &ok);
    if (!ok) {
      return;
    }
  }

  QString err;
  m_hint->setText(QStringLiteral("Connecting to %1…").arg(ssid));
  if (!m_nm->connectToSsid(ssid, password, secured, &err)) {
    QMessageBox::warning(this, QStringLiteral("Connect"), err);
    m_hint->setText(err);
  } else {
    m_hint->setText(QStringLiteral("Connected to %1").arg(ssid));
  }
  refresh();
}

void NetworkPanelWidget::onItemActivated()
{
  onConnectSelected();
}

} // namespace spike
