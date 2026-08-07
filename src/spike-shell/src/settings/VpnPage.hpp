#pragma once

class QLabel;
class QWidget;

namespace spike {

class NmClient;

/** Settings → VPN — NetworkManager OpenVPN / WireGuard connections. */
QWidget *makeVpnPage(QWidget *parent, NmClient *nm, QLabel *statusBar);

} // namespace spike
