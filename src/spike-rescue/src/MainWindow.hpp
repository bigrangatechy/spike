#pragma once

#include "RescueTypes.hpp"

#include <QMainWindow>

class QLabel;
class QListWidget;
class QProgressBar;
class QPushButton;
class QStackedWidget;
class QTextEdit;
class QTimer;

namespace spike {

class RescueEngine;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  void onScanFinished(bool ok);
  void onInventoryFinished(bool ok);
  void onDestinations(const QVector<DestVolume> &vols);
  void onCopyProgress(const QString &file, qint64 done, qint64 total, qint64 doneBytes,
                      qint64 totalBytes);
  void onCopyFinished(bool ok);

  void goSelect();
  void goInventory();
  void goDest();
  void goCopy();
  void cancelCopy();
  void finishExit();
  void finishReinstall();
  void refreshDestUi();

private:
  void buildUi();
  QString formatBytes(qint64 n) const;

  RescueEngine *m_engine = nullptr;
  QStackedWidget *m_stack = nullptr;

  QWidget *m_pageScan = nullptr;
  QLabel *m_scanLabel = nullptr;
  QProgressBar *m_scanBar = nullptr;

  QWidget *m_pageSelect = nullptr;
  QListWidget *m_systemList = nullptr;
  QLabel *m_selectStatus = nullptr;

  QWidget *m_pageInventory = nullptr;
  QTextEdit *m_invBody = nullptr;

  QWidget *m_pageDest = nullptr;
  QListWidget *m_destList = nullptr;
  QLabel *m_destStatus = nullptr;
  QTimer *m_destTimer = nullptr;

  QWidget *m_pageCopy = nullptr;
  QLabel *m_copyLabel = nullptr;
  QProgressBar *m_copyBar = nullptr;
  QLabel *m_copyStats = nullptr;

  QWidget *m_pageDone = nullptr;
  QTextEdit *m_doneBody = nullptr;

  int m_selectedSystem = -1;
  qint64 m_neededBytes = 0;
};

} // namespace spike
