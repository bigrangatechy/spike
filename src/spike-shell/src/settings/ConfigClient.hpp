#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

class QDBusInterface;

namespace spike {

/** Thin Qt client for org.spike.Config (system bus). */
class ConfigClient : public QObject
{
  Q_OBJECT

public:
  explicit ConfigClient(QObject *parent = nullptr);

  bool isAvailable() const;

  QString getState(QString *error = nullptr);
  QString getModuleState(const QString &module, QString *error = nullptr);
  bool setSetting(const QString &module, const QString &key, const QVariant &value,
                  QString *error = nullptr);
  bool generateAll(QString *error = nullptr);
  bool generateModule(const QString &module, QString *error = nullptr);
  QString detectHardware(QString *error = nullptr);
  QString getChangelog(int count, QString *error = nullptr);
  bool validateAll(QString *error = nullptr);

signals:
  void stateChanged(const QString &module, const QString &key, const QVariant &oldValue,
                    const QVariant &newValue);
  void configRegenerated(const QString &module, const QStringList &files);

private:
  QDBusInterface *iface() const;
  mutable QDBusInterface *m_iface = nullptr;
};

} // namespace spike
