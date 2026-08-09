#pragma once

#include <QDialog>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QStackedWidget;
class QTextEdit;

namespace spike {

class NmClient;

/** Post-install first-run wizard (INSTALLER.md / AGENTS.md) with placeholder hooks. */
class FirstRunWizard : public QDialog
{
  Q_OBJECT

public:
  explicit FirstRunWizard(QWidget *parent = nullptr);

private slots:
  void goBack();
  void goNext();
  void onWifiScan();
  void onWifiConnect();
  void onImportFiles();
  void onOpenAccessibility();

private:
  void buildPages();
  void applyTimezone();
  void refreshNoticesPage();
  void finishAndClose();
  void updateNav();
  static QStringList loadTimeZones();
  static QString currentTimezone();

  QStackedWidget *m_stack = nullptr;
  QLabel *m_stepLabel = nullptr;
  QPushButton *m_back = nullptr;
  QPushButton *m_next = nullptr;
  QPushButton *m_skip = nullptr;

  QComboBox *m_timezone = nullptr;
  QLabel *m_wifiStatus = nullptr;
  QListWidget *m_wifiList = nullptr;
  QLineEdit *m_wifiPassword = nullptr;
  QLabel *m_importStatus = nullptr;
  QTextEdit *m_notices = nullptr;

  NmClient *m_nm = nullptr;
};

} // namespace spike
