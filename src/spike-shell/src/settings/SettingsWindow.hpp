#pragma once

#include <QMainWindow>
#include <QVector>

class QCheckBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QStackedWidget;
class QTextEdit;

namespace spike {

class ConfigClient;

/** Spike Settings — hybrid custom pages + KDE KCM hosts (DESKTOP.md). */
class SettingsWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit SettingsWindow(ConfigClient *config, QWidget *parent = nullptr);

  void openPage(const QString &pageId);

private slots:
  void onNavChanged();
  void onSearchChanged(const QString &text);
  void onAdvancedToggled(bool on);
  void onHelpClicked();
  void refreshCurrentPage();

private:
  struct PageDef {
    QString id;
    QString title;
    QString category;
    QString keywords;
    bool advanced = false;
    bool isKcm = false;
    QString kcmPlugin; // plugin id for KcmHost when isKcm
  };

  void buildPages();
  void rebuildNav();
  QWidget *makePlaceholder(const PageDef &page);
  QWidget *makeKcmPage(const PageDef &page);
  QWidget *makeAboutPage();
  QWidget *makeMemoryPage();
  int pageIndexForId(const QString &id) const;

  ConfigClient *m_config = nullptr;
  QLineEdit *m_search = nullptr;
  QListWidget *m_nav = nullptr;
  QStackedWidget *m_stack = nullptr;
  QCheckBox *m_showAdvanced = nullptr;
  QLabel *m_status = nullptr;
  QVector<PageDef> m_pages;
};

} // namespace spike
