#include "network/NmClient.hpp"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QHash>
#include <QProcess>
#include <QUuid>
#include <QVariantMap>

#include <algorithm>

namespace spike {

namespace {

constexpr const char *kNmService = "org.freedesktop.NetworkManager";
constexpr const char *kNmPath = "/org/freedesktop/NetworkManager";
constexpr const char *kNmIface = "org.freedesktop.NetworkManager";
constexpr const char *kProps = "org.freedesktop.DBus.Properties";
constexpr const char *kDeviceIface = "org.freedesktop.NetworkManager.Device";
constexpr const char *kWirelessIface = "org.freedesktop.NetworkManager.Device.Wireless";
constexpr const char *kApIface = "org.freedesktop.NetworkManager.AccessPoint";
constexpr const char *kActiveIface = "org.freedesktop.NetworkManager.Connection.Active";
constexpr const char *kSettingsPath = "/org/freedesktop/NetworkManager/Settings";
constexpr const char *kSettingsIface = "org.freedesktop.NetworkManager.Settings";
constexpr const char *kConnIface = "org.freedesktop.NetworkManager.Settings.Connection";

constexpr int kTypeEthernet = 1;
constexpr int kTypeWifi = 2;
constexpr int kStateActivated = 100;

QString deviceStateText(int state)
{
  switch (state) {
  case 10:
    return QStringLiteral("unmanaged");
  case 20:
    return QStringLiteral("unavailable");
  case 30:
    return QStringLiteral("disconnected");
  case 100:
    return QStringLiteral("connected");
  case 120:
    return QStringLiteral("failed");
  default:
    return QStringLiteral("state %1").arg(state);
  }
}

QString ssidFromVariant(const QVariant &v)
{
  if (v.canConvert<QByteArray>()) {
    return QString::fromUtf8(v.toByteArray());
  }
  if (v.userType() == qMetaTypeId<QDBusArgument>()) {
    const auto arg = qvariant_cast<QDBusArgument>(v);
    if (arg.currentType() == QDBusArgument::ArrayType) {
      QByteArray bytes;
      arg.beginArray();
      while (!arg.atEnd()) {
        uchar b = 0;
        arg >> b;
        bytes.append(char(b));
      }
      arg.endArray();
      return QString::fromUtf8(bytes);
    }
  }
  return v.toString();
}

QList<QDBusObjectPath> objectPathList(const QVariant &v)
{
  QList<QDBusObjectPath> out;
  if (v.canConvert<QList<QDBusObjectPath>>()) {
    return qvariant_cast<QList<QDBusObjectPath>>(v);
  }
  if (v.userType() == qMetaTypeId<QDBusArgument>()) {
    const auto arg = qvariant_cast<QDBusArgument>(v);
    arg.beginArray();
    while (!arg.atEnd()) {
      QDBusObjectPath p;
      arg >> p;
      out.append(p);
    }
    arg.endArray();
  }
  return out;
}

bool runNmcli(const QStringList &args, QString *error)
{
  QProcess proc;
  proc.start(QStringLiteral("nmcli"), args);
  if (!proc.waitForStarted(3000)) {
    if (error) {
      *error = QStringLiteral("nmcli not available");
    }
    return false;
  }
  if (!proc.waitForFinished(20000)) {
    proc.kill();
    if (error) {
      *error = QStringLiteral("nmcli timed out");
    }
    return false;
  }
  if (proc.exitCode() != 0) {
    if (error) {
      *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
      if (error->isEmpty()) {
        *error = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
      }
    }
    return false;
  }
  return true;
}

} // namespace

NmClient::NmClient(QObject *parent)
  : QObject(parent)
{
  qDBusRegisterMetaType<QList<QDBusObjectPath>>();
  subscribe();
}

QDBusInterface *NmClient::nm() const
{
  if (!m_nm) {
    m_nm = new QDBusInterface(QString::fromUtf8(kNmService), QString::fromUtf8(kNmPath),
                              QString::fromUtf8(kNmIface), QDBusConnection::systemBus(),
                              const_cast<NmClient *>(this));
  }
  return m_nm;
}

void NmClient::subscribe()
{
  if (m_subscribed) {
    return;
  }
  auto bus = QDBusConnection::systemBus();
  const QString svc = QString::fromUtf8(kNmService);
  const QString path = QString::fromUtf8(kNmPath);
  const QString iface = QString::fromUtf8(kNmIface);
  bus.connect(svc, path, iface, QStringLiteral("DeviceAdded"), this, SLOT(onNmEvent()));
  bus.connect(svc, path, iface, QStringLiteral("DeviceRemoved"), this, SLOT(onNmEvent()));
  bus.connect(svc, path, iface, QStringLiteral("StateChanged"), this, SLOT(onNmEvent()));
  bus.connect(svc, path, QString::fromUtf8(kProps), QStringLiteral("PropertiesChanged"), this,
              SLOT(onNmEvent()));
  m_subscribed = true;
}

void NmClient::onNmEvent()
{
  emit changed();
}

QVariant NmClient::getProp(const QString &path, const QString &iface, const QString &name) const
{
  QDBusInterface props(QString::fromUtf8(kNmService), path, QString::fromUtf8(kProps),
                       QDBusConnection::systemBus());
  QDBusReply<QDBusVariant> reply = props.call(QStringLiteral("Get"), iface, name);
  if (!reply.isValid()) {
    return {};
  }
  return reply.value().variant();
}

bool NmClient::setProp(const QString &path, const QString &iface, const QString &name,
                       const QVariant &value, QString *error) const
{
  QDBusInterface props(QString::fromUtf8(kNmService), path, QString::fromUtf8(kProps),
                       QDBusConnection::systemBus());
  QDBusMessage reply =
      props.call(QStringLiteral("Set"), iface, name, QVariant::fromValue(QDBusVariant(value)));
  if (reply.type() == QDBusMessage::ErrorMessage) {
    if (error) {
      *error = reply.errorMessage();
    }
    return false;
  }
  return true;
}

bool NmClient::isAvailable(QString *error) const
{
  if (!QDBusConnection::systemBus().isConnected()) {
    if (error) {
      *error = QStringLiteral("system bus not connected");
    }
    return false;
  }
  if (!nm()->isValid()) {
    if (error) {
      *error = nm()->lastError().message();
    }
    return false;
  }
  return true;
}

bool NmClient::networkingEnabled() const
{
  return getProp(QString::fromUtf8(kNmPath), QString::fromUtf8(kNmIface),
                 QStringLiteral("NetworkingEnabled"))
      .toBool();
}

bool NmClient::wirelessEnabled() const
{
  return getProp(QString::fromUtf8(kNmPath), QString::fromUtf8(kNmIface),
                 QStringLiteral("WirelessEnabled"))
      .toBool();
}

bool NmClient::setWirelessEnabled(bool on, QString *error)
{
  const bool ok = setProp(QString::fromUtf8(kNmPath), QString::fromUtf8(kNmIface),
                          QStringLiteral("WirelessEnabled"), on, error);
  if (ok) {
    emit changed();
  }
  return ok;
}

QVector<NmDevice> NmClient::devices(QString *error) const
{
  QVector<NmDevice> out;
  if (!isAvailable(error)) {
    return out;
  }
  QDBusReply<QList<QDBusObjectPath>> reply = nm()->call(QStringLiteral("GetDevices"));
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return out;
  }
  for (const QDBusObjectPath &op : reply.value()) {
    NmDevice d;
    d.path = op.path();
    d.type = getProp(d.path, QString::fromUtf8(kDeviceIface), QStringLiteral("DeviceType")).toInt();
    d.iface =
        getProp(d.path, QString::fromUtf8(kDeviceIface), QStringLiteral("Interface")).toString();
    d.state = getProp(d.path, QString::fromUtf8(kDeviceIface), QStringLiteral("State")).toInt();
    d.stateText = deviceStateText(d.state);

    const QDBusObjectPath active =
        qvariant_cast<QDBusObjectPath>(getProp(d.path, QString::fromUtf8(kDeviceIface),
                                               QStringLiteral("ActiveConnection")));
    if (!active.path().isEmpty() && active.path() != QLatin1String("/")) {
      d.connectionId =
          getProp(active.path(), QString::fromUtf8(kActiveIface), QStringLiteral("Id")).toString();
    }
    if (d.type == kTypeWifi && d.connectionId.isEmpty()) {
      const QDBusObjectPath ap = qvariant_cast<QDBusObjectPath>(
          getProp(d.path, QString::fromUtf8(kWirelessIface), QStringLiteral("ActiveAccessPoint")));
      if (!ap.path().isEmpty() && ap.path() != QLatin1String("/")) {
        d.connectionId =
            ssidFromVariant(getProp(ap.path(), QString::fromUtf8(kApIface), QStringLiteral("Ssid")));
      }
    }
    out.append(d);
  }
  return out;
}

QString NmClient::wifiDevicePath(QString *error) const
{
  for (const NmDevice &d : devices(error)) {
    if (d.type == kTypeWifi) {
      return d.path;
    }
  }
  if (error && error->isEmpty()) {
    *error = QStringLiteral("no Wi‑Fi device");
  }
  return {};
}

QVector<NmAccessPoint> NmClient::accessPoints(const QString &wifiDevicePath,
                                              QString *error) const
{
  QVector<NmAccessPoint> out;
  if (wifiDevicePath.isEmpty()) {
    if (error) {
      *error = QStringLiteral("no Wi‑Fi device path");
    }
    return out;
  }
  const QVariant raw =
      getProp(wifiDevicePath, QString::fromUtf8(kWirelessIface), QStringLiteral("AccessPoints"));
  const QList<QDBusObjectPath> paths = objectPathList(raw);
  const QDBusObjectPath activeAp = qvariant_cast<QDBusObjectPath>(
      getProp(wifiDevicePath, QString::fromUtf8(kWirelessIface), QStringLiteral("ActiveAccessPoint")));

  QHash<QString, int> bestIndex;
  for (const QDBusObjectPath &op : paths) {
    NmAccessPoint ap;
    ap.path = op.path();
    ap.ssid = ssidFromVariant(getProp(ap.path, QString::fromUtf8(kApIface), QStringLiteral("Ssid")));
    if (ap.ssid.isEmpty()) {
      continue;
    }
    ap.strength =
        int(getProp(ap.path, QString::fromUtf8(kApIface), QStringLiteral("Strength")).toUInt());
    const uint flags =
        getProp(ap.path, QString::fromUtf8(kApIface), QStringLiteral("Flags")).toUInt();
    const uint wpa =
        getProp(ap.path, QString::fromUtf8(kApIface), QStringLiteral("WpaFlags")).toUInt();
    const uint rsn =
        getProp(ap.path, QString::fromUtf8(kApIface), QStringLiteral("RsnFlags")).toUInt();
    ap.secured = (flags & 0x1u) != 0 || wpa != 0 || rsn != 0;
    ap.active = (ap.path == activeAp.path());

    if (bestIndex.contains(ap.ssid)) {
      const int i = bestIndex.value(ap.ssid);
      if (ap.strength > out[i].strength || ap.active) {
        out[i] = ap;
      }
    } else {
      bestIndex.insert(ap.ssid, out.size());
      out.append(ap);
    }
  }

  std::sort(out.begin(), out.end(), [](const NmAccessPoint &a, const NmAccessPoint &b) {
    if (a.active != b.active) {
      return a.active;
    }
    return a.strength > b.strength;
  });
  return out;
}

bool NmClient::requestScan(const QString &wifiDevicePath, QString *error)
{
  if (wifiDevicePath.isEmpty()) {
    if (error) {
      *error = QStringLiteral("no Wi‑Fi device");
    }
    return false;
  }
  QDBusInterface wifi(QString::fromUtf8(kNmService), wifiDevicePath,
                      QString::fromUtf8(kWirelessIface), QDBusConnection::systemBus());
  QVariantMap options;
  QDBusMessage reply = wifi.call(QStringLiteral("RequestScan"), QVariant::fromValue(options));
  if (reply.type() == QDBusMessage::ErrorMessage) {
    // Fall back to nmcli (still NetworkManager).
    if (!runNmcli({QStringLiteral("device"), QStringLiteral("wifi"), QStringLiteral("rescan")},
                  error)) {
      if (error && error->isEmpty()) {
        *error = reply.errorMessage();
      }
      return false;
    }
  }
  emit changed();
  return true;
}

bool NmClient::disconnectActive(QString *error)
{
  if (!isAvailable(error)) {
    return false;
  }
  const QDBusObjectPath active = qvariant_cast<QDBusObjectPath>(
      getProp(QString::fromUtf8(kNmPath), QString::fromUtf8(kNmIface),
              QStringLiteral("PrimaryConnection")));
  if (!active.path().isEmpty() && active.path() != QLatin1String("/")) {
    QDBusReply<void> reply =
        nm()->call(QStringLiteral("DeactivateConnection"), QVariant::fromValue(active));
    if (reply.isValid()) {
      emit changed();
      return true;
    }
  }
  // nmcli fallback
  if (runNmcli({QStringLiteral("networking"), QStringLiteral("connectivity")}, nullptr)) {
    // Prefer disconnecting wifi device.
    QString wifiErr;
    const QString wifi = wifiDevicePath(&wifiErr);
    if (!wifi.isEmpty()) {
      const QString iface =
          getProp(wifi, QString::fromUtf8(kDeviceIface), QStringLiteral("Interface")).toString();
      if (!iface.isEmpty() &&
          runNmcli({QStringLiteral("device"), QStringLiteral("disconnect"), iface}, error)) {
        emit changed();
        return true;
      }
    }
  }
  if (error && error->isEmpty()) {
    *error = QStringLiteral("nothing to disconnect");
  }
  return false;
}

bool NmClient::connectToSsid(const QString &ssid, const QString &password, bool secured,
                             QString *error)
{
  if (ssid.isEmpty()) {
    if (error) {
      *error = QStringLiteral("empty SSID");
    }
    return false;
  }
  if (!isAvailable(error)) {
    return false;
  }

  // Try activating a saved connection first (D-Bus ListConnections + ActivateConnection).
  QDBusInterface settings(QString::fromUtf8(kNmService), QString::fromUtf8(kSettingsPath),
                          QString::fromUtf8(kSettingsIface), QDBusConnection::systemBus());
  QDBusReply<QList<QDBusObjectPath>> listReply = settings.call(QStringLiteral("ListConnections"));
  const QString wifiPath = wifiDevicePath(error);
  if (listReply.isValid() && !wifiPath.isEmpty()) {
    for (const QDBusObjectPath &cpath : listReply.value()) {
      QDBusInterface conn(QString::fromUtf8(kNmService), cpath.path(),
                          QString::fromUtf8(kConnIface), QDBusConnection::systemBus());
      QDBusReply<QVariant> gs = conn.call(QStringLiteral("GetSettings"));
      Q_UNUSED(gs);
      // Id is not a property; use nmcli for connect reliability on live.
      break;
    }
  }

  // Primary path for live ISO: nmcli (NetworkManager CLI) — avoids fragile a{sa{sv}}
  // marshaling; still the same daemon DESKTOP.md specifies.
  QStringList args{QStringLiteral("-w"), QStringLiteral("20"), QStringLiteral("device"),
                   QStringLiteral("wifi"), QStringLiteral("connect"), ssid};
  if (secured) {
    if (password.isEmpty()) {
      // Open saved connection without password, or fail with clear message.
      args = {QStringLiteral("-w"), QStringLiteral("20"), QStringLiteral("connection"),
              QStringLiteral("up"), QStringLiteral("id"), ssid};
      if (runNmcli(args, error)) {
        emit changed();
        return true;
      }
      if (error) {
        *error = QStringLiteral("Password required for “%1”").arg(ssid);
      }
      return false;
    }
    args << QStringLiteral("password") << password;
  }
  if (!runNmcli(args, error)) {
    return false;
  }
  emit changed();
  return true;
}

QVector<NmVpnConnection> NmClient::vpnConnections(QString *error) const
{
  QVector<NmVpnConnection> out;
  QProcess proc;
  proc.start(QStringLiteral("nmcli"),
             {QStringLiteral("-t"), QStringLiteral("-f"),
              QStringLiteral("NAME,UUID,TYPE,DEVICE"), QStringLiteral("connection"),
              QStringLiteral("show")});
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    if (error) {
      *error = QStringLiteral("nmcli connection show failed");
    }
    return out;
  }
  if (proc.exitCode() != 0) {
    if (error) {
      *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
    }
    return out;
  }
  const QString text = QString::fromUtf8(proc.readAllStandardOutput());
  for (const QString &line : text.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    const QStringList parts = line.split(QLatin1Char(':'));
    if (parts.size() < 3) {
      continue;
    }
    const QString type = parts.at(2);
    if (type != QLatin1String("vpn") && type != QLatin1String("wireguard") &&
        !type.startsWith(QLatin1String("vpn-"))) {
      continue;
    }
    NmVpnConnection c;
    c.name = parts.at(0);
    c.uuid = parts.size() > 1 ? parts.at(1) : QString();
    c.type = type;
    c.active = parts.size() > 3 && !parts.at(3).isEmpty() && parts.at(3) != QLatin1String("--");
    out.append(c);
  }
  return out;
}

bool NmClient::activateVpn(const QString &nameOrUuid, QString *error)
{
  if (nameOrUuid.isEmpty()) {
    if (error) {
      *error = QStringLiteral("empty VPN name");
    }
    return false;
  }
  if (!runNmcli({QStringLiteral("-w"), QStringLiteral("30"), QStringLiteral("connection"),
                 QStringLiteral("up"), QStringLiteral("id"), nameOrUuid},
                error)) {
    // Retry by UUID if name failed.
    if (!runNmcli({QStringLiteral("-w"), QStringLiteral("30"), QStringLiteral("connection"),
                   QStringLiteral("up"), QStringLiteral("uuid"), nameOrUuid},
                  error)) {
      return false;
    }
  }
  emit changed();
  return true;
}

bool NmClient::deactivateVpn(const QString &nameOrUuid, QString *error)
{
  if (nameOrUuid.isEmpty()) {
    if (error) {
      *error = QStringLiteral("empty VPN name");
    }
    return false;
  }
  if (!runNmcli({QStringLiteral("connection"), QStringLiteral("down"), QStringLiteral("id"),
                 nameOrUuid},
                error)) {
    if (!runNmcli({QStringLiteral("connection"), QStringLiteral("down"), QStringLiteral("uuid"),
                   nameOrUuid},
                  error)) {
      return false;
    }
  }
  emit changed();
  return true;
}

QString NmClient::statusLabel(QString *error) const
{
  if (!isAvailable(error)) {
    return QStringLiteral("No NM");
  }
  if (!networkingEnabled()) {
    return QStringLiteral("Offline");
  }
  for (const NmDevice &d : devices()) {
    if (d.state == kStateActivated) {
      if (d.type == kTypeEthernet) {
        return d.connectionId.isEmpty() ? QStringLiteral("Ethernet")
                                        : QStringLiteral("Eth · %1").arg(d.connectionId);
      }
      if (d.type == kTypeWifi) {
        return d.connectionId.isEmpty() ? QStringLiteral("Wi‑Fi")
                                        : d.connectionId;
      }
      if (!d.connectionId.isEmpty()) {
        return d.connectionId;
      }
    }
  }
  if (!wirelessEnabled()) {
    return QStringLiteral("Wi‑Fi off");
  }
  return QStringLiteral("Offline");
}

QString NmClient::statusKind(QString *error) const
{
  if (!isAvailable(error) || !networkingEnabled()) {
    return QStringLiteral("offline");
  }
  for (const NmDevice &d : devices()) {
    if (d.state != kStateActivated) {
      continue;
    }
    if (d.type == kTypeEthernet) {
      return QStringLiteral("ethernet");
    }
    if (d.type == kTypeWifi) {
      return QStringLiteral("wifi");
    }
  }
  if (!wirelessEnabled()) {
    return QStringLiteral("disabled");
  }
  return QStringLiteral("offline");
}

int NmClient::wifiStrength(QString *error) const
{
  const QString wifi = wifiDevicePath(error);
  if (wifi.isEmpty()) {
    return -1;
  }
  const QDBusObjectPath ap = qvariant_cast<QDBusObjectPath>(
      getProp(wifi, QString::fromUtf8(kWirelessIface), QStringLiteral("ActiveAccessPoint")));
  if (ap.path().isEmpty() || ap.path() == QLatin1String("/")) {
    return -1;
  }
  return int(getProp(ap.path(), QString::fromUtf8(kApIface), QStringLiteral("Strength")).toUInt());
}

} // namespace spike
