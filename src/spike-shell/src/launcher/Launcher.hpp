#pragma once

#include <QWidget>

class QKeyEvent;
class QLineEdit;
class QListWidget;
class QShowEvent;

namespace spike {

/** Kickoff-style application menu (DESKTOP.md). */
class Launcher : public QWidget
{
  Q_OBJECT

public:
  explicit Launcher(QWidget *parent = nullptr);

signals:
  void openSettingsRequested();
  void logoutRequested();
  void shutdownRequested();
  void rebootRequested();

protected:
  void showEvent(QShowEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void filterChanged(const QString &text);
  void activateAppList();
  void activateFavorite();

private:
  void populateApps();
  void populateFavorites();
  void launchCommand(const QString &cmd);

  QLineEdit *m_search = nullptr;
  QListWidget *m_favorites = nullptr;
  QListWidget *m_apps = nullptr;
};

} // namespace spike
