#pragma once

#include "InstallState.hpp"

#include <QMainWindow>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QCheckBox;
class QStackedWidget;
class QTextEdit;
class QPushButton;

namespace spike {

class InstallEngine;

class InstallWizard : public QMainWindow
{
  Q_OBJECT

public:
  explicit InstallWizard(QWidget *parent = nullptr);

private slots:
  void goNext();
  void goBack();
  void refreshBackupUi();
  void refreshWifiUi();
  void connectWifi();
  void onFinishClose();
  void startInstall();
  void onInstallLog(const QString &line);
  void onInstallFinished(bool ok, const QString &message);

private:
  void buildUi();
  void syncStateFromPage(int index);
  void applyPageToUi(int index);
  bool validateCurrent();
  QString stateDump() const;
  static QString runNmcli(const QStringList &args, int timeoutMs = 20000);

  InstallState m_state;
  InstallEngine *m_engine = nullptr;
  bool m_installOk = false;
  bool m_installStarted = false;
  bool m_wifiSkipped = false;

  QStackedWidget *m_stack = nullptr;
  QLabel *m_stepLabel = nullptr;
  QPushButton *m_back = nullptr;
  QPushButton *m_next = nullptr;
  QPushButton *m_installBtn = nullptr;

  QComboBox *m_language = nullptr;
  QComboBox *m_timezone = nullptr;
  QLabel *m_wifiStatus = nullptr;
  QListWidget *m_wifiList = nullptr;
  QLineEdit *m_wifiPassword = nullptr;
  QPushButton *m_wifiScanBtn = nullptr;
  QPushButton *m_wifiConnectBtn = nullptr;
  QPushButton *m_wifiSkipBtn = nullptr;
  QLineEdit *m_username = nullptr;
  QLineEdit *m_password = nullptr;
  QLineEdit *m_password2 = nullptr;
  QLineEdit *m_hostname = nullptr;
  QComboBox *m_variant = nullptr;
  QLabel *m_variantHint = nullptr;
  QCheckBox *m_doBackup = nullptr;
  QListWidget *m_backupDestList = nullptr;
  QCheckBox *m_restoreCheck = nullptr;
  QListWidget *m_restoreList = nullptr;
  QListWidget *m_diskList = nullptr;
  QCheckBox *m_wipeConfirm = nullptr;
  QLineEdit *m_eraseType = nullptr;
  QTextEdit *m_progressBody = nullptr;
  QTextEdit *m_finishBody = nullptr;
};

} // namespace spike
