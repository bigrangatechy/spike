#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class QDBusInterface;

namespace spike {

struct NmAccessPoint {
  QString path;
  QString ssid;
  int strength = 0; // 0–100
  bool secured = false;
  bool active = false;
};

struct NmVpnConnection {
  QString name;
  QString uuid;
  QString type; // vpn, wireguard, …
  bool active = false;
};

struct NmDevice {
  QString path;
  int type = 0; // 1 ethernet, 2 wifi, …
  QString iface;
  int state = 0;
  QString stateText;
  QString connectionId; // active connection id / SSID when known
};

/** Thin system-bus client for org.freedesktop.NetworkManager (DESKTOP.md). */
class NmClient : public QObject
{
  Q_OBJECT

public:
  explicit NmClient(QObject *parent = nullptr);

  bool isAvailable(QString *error = nullptr) const;
  bool networkingEnabled() const;
  bool wirelessEnabled() const;
  bool setWirelessEnabled(bool on, QString *error = nullptr);

  QVector<NmDevice> devices(QString *error = nullptr) const;
  QString wifiDevicePath(QString *error = nullptr) const;
  QVector<NmAccessPoint> accessPoints(const QString &wifiDevicePath,
                                      QString *error = nullptr) const;
  bool requestScan(const QString &wifiDevicePath, QString *error = nullptr);
  bool disconnectActive(QString *error = nullptr);
  bool connectToSsid(const QString &ssid, const QString &password, bool secured,
                     QString *error = nullptr);

  QVector<NmVpnConnection> vpnConnections(QString *error = nullptr) const;
  bool activateVpn(const QString &nameOrUuid, QString *error = nullptr);
  bool deactivateVpn(const QString &nameOrUuid, QString *error = nullptr);

  /** Short status for the tray: e.g. "Wi‑Fi · Home", "Ethernet", "Offline". */
  QString statusLabel(QString *error = nullptr) const;
  /** Icon hint: wifi / ethernet / offline / disabled */
  QString statusKind(QString *error = nullptr) const;
  int wifiStrength(QString *error = nullptr) const;

signals:
  void changed();

private slots:
  void onNmEvent();

private:
  QDBusInterface *nm() const;
  QVariant getProp(const QString &path, const QString &iface, const QString &name) const;
  bool setProp(const QString &path, const QString &iface, const QString &name,
               const QVariant &value, QString *error) const;
  void subscribe();

  mutable QDBusInterface *m_nm = nullptr;
  bool m_subscribed = false;
};

} // namespace spike
