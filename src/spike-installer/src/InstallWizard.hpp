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

class InstallWizard : public QMainWindow
{
  Q_OBJECT

public:
  explicit InstallWizard(QWidget *parent = nullptr);

private slots:
  void goNext();
  void goBack();
  void refreshBackupUi();
  void onFinishClose();

private:
  void buildUi();
  void syncStateFromPage(int index);
  void applyPageToUi(int index);
  bool validateCurrent();
  QString stateDump() const;

  InstallState m_state;
  QStackedWidget *m_stack = nullptr;
  QLabel *m_stepLabel = nullptr;
  QPushButton *m_back = nullptr;
  QPushButton *m_next = nullptr;

  // Step widgets / inputs
  QComboBox *m_language = nullptr;
  QComboBox *m_timezone = nullptr;
  QLabel *m_wifiStatus = nullptr;
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
  QTextEdit *m_progressBody = nullptr;
  QTextEdit *m_finishBody = nullptr;
};

} // namespace spike
