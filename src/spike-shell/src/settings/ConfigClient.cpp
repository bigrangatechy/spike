#include "settings/ConfigClient.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusVariant>

namespace spike {

namespace {
constexpr const char *kService = "org.spike.Config";
constexpr const char *kPath = "/org/spike/Config";
constexpr const char *kIface = "org.spike.Config";
} // namespace

ConfigClient::ConfigClient(QObject *parent)
  : QObject(parent)
{
}

QDBusInterface *ConfigClient::iface() const
{
  if (!m_iface) {
    m_iface = new QDBusInterface(QString::fromUtf8(kService), QString::fromUtf8(kPath),
                                 QString::fromUtf8(kIface), QDBusConnection::systemBus(),
                                 const_cast<ConfigClient *>(this));
  }
  return m_iface;
}

bool ConfigClient::isAvailable() const
{
  // Trigger activation: a method call will start spike-config via D-Bus activation.
  return QDBusConnection::systemBus().isConnected();
}

QString ConfigClient::getState(QString *error)
{
  QDBusReply<QString> reply = iface()->call(QStringLiteral("GetState"));
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return {};
  }
  return reply.value();
}

QString ConfigClient::getModuleState(const QString &module, QString *error)
{
  QDBusReply<QString> reply = iface()->call(QStringLiteral("GetModuleState"), module);
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return {};
  }
  return reply.value();
}

bool ConfigClient::setSetting(const QString &module, const QString &key, const QVariant &value,
                              QString *error)
{
  QDBusReply<bool> reply =
      iface()->call(QStringLiteral("SetSetting"), module, key, QVariant::fromValue(QDBusVariant(value)));
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return false;
  }
  return reply.value();
}

bool ConfigClient::generateAll(QString *error)
{
  QDBusReply<bool> reply = iface()->call(QStringLiteral("GenerateAll"));
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return false;
  }
  return reply.value();
}

bool ConfigClient::generateModule(const QString &module, QString *error)
{
  QDBusReply<bool> reply = iface()->call(QStringLiteral("GenerateModule"), module);
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return false;
  }
  return reply.value();
}

QString ConfigClient::detectHardware(QString *error)
{
  QDBusReply<QString> reply = iface()->call(QStringLiteral("DetectHardware"));
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return {};
  }
  return reply.value();
}

QString ConfigClient::getChangelog(int count, QString *error)
{
  QDBusReply<QString> reply = iface()->call(QStringLiteral("GetChangelog"), count);
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return {};
  }
  return reply.value();
}

bool ConfigClient::validateAll(QString *error)
{
  QDBusReply<bool> reply = iface()->call(QStringLiteral("ValidateAll"));
  if (!reply.isValid()) {
    if (error) {
      *error = reply.error().message();
    }
    return false;
  }
  return reply.value();
}

} // namespace spike
