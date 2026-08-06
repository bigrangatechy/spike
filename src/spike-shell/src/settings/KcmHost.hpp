#pragma once

#include <QString>
#include <QWidget>

class KCModule;
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace spike {

/**
 * Hosts a KDE System Settings module (KCM) inside Spike Settings.
 * Spec: DESKTOP.md — hybrid custom pages + KCM loader.
 */
class KcmHost : public QWidget
{
  Q_OBJECT

public:
  explicit KcmHost(QWidget *parent = nullptr);
  ~KcmHost() override;

  /** Load plugin id (e.g. "kcm_users") from plasma/kcms/systemsettings*. */
  bool loadPlugin(const QString &pluginId);

  QString pluginId() const { return m_pluginId; }
  bool isLoaded() const { return m_module != nullptr; }

public slots:
  void apply();
  void reset();
  void defaults();

private:
  void clearModule();
  void updateButtons();

  QString m_pluginId;
  KCModule *m_module = nullptr;
  QVBoxLayout *m_layout = nullptr;
  QWidget *m_moduleContainer = nullptr;
  QLabel *m_error = nullptr;
  QPushButton *m_apply = nullptr;
  QPushButton *m_reset = nullptr;
  QPushButton *m_defaults = nullptr;
};

} // namespace spike
