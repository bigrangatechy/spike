#pragma once

#include <QWidget>

class QCheckBox;
class QLabel;
class QListWidget;
class QPushButton;

namespace spike {

class NmClient;

/** Shared NetworkManager UI for Settings page and tray popup. */
class NetworkPanelWidget : public QWidget
{
  Q_OBJECT

public:
  explicit NetworkPanelWidget(NmClient *nm, QWidget *parent = nullptr);

public slots:
  void refresh();

signals:
  void openFullSettings();

private slots:
  void onWifiToggled(bool on);
  void onScan();
  void onDisconnect();
  void onConnectSelected();
  void onItemActivated();

private:
  NmClient *m_nm = nullptr;
  QLabel *m_status = nullptr;
  QCheckBox *m_wifiOn = nullptr;
  QLabel *m_vpnRow = nullptr;
  QPushButton *m_vpnDisconnect = nullptr;
  QListWidget *m_list = nullptr;
  QLabel *m_hint = nullptr;
  bool m_blockWifiToggle = false;
};

} // namespace spike
