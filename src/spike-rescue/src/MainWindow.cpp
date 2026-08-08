#include "MainWindow.hpp"

#include "RescueEngine.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

namespace spike {

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  setObjectName(QStringLiteral("SpikeRescue"));
  setWindowTitle(QStringLiteral("Spike Rescue %1 [pre-alpha debug]")
                     .arg(QApplication::applicationVersion()));
  resize(780, 560);

  // Engine runs on a worker thread (must not have a QObject parent when moved).
  m_engine = new RescueEngine();
  auto *thread = new QThread(this);
  m_engine->moveToThread(thread);
  connect(thread, &QThread::finished, m_engine, &QObject::deleteLater);

  connect(m_engine, &RescueEngine::scanProgress, this,
          [this](const QString &msg, int pct) {
            if (m_scanLabel) {
              m_scanLabel->setText(msg);
            }
            if (m_scanBar) {
              m_scanBar->setValue(pct);
            }
          });
  connect(m_engine, &RescueEngine::scanFinished, this, &MainWindow::onScanFinished);
  connect(m_engine, &RescueEngine::inventoryFinished, this, &MainWindow::onInventoryFinished);
  connect(m_engine, &RescueEngine::destinationsChanged, this, &MainWindow::onDestinations);
  connect(m_engine, &RescueEngine::copyProgress, this, &MainWindow::onCopyProgress);
  connect(m_engine, &RescueEngine::copyFinished, this, &MainWindow::onCopyFinished);

  buildUi();

  connect(thread, &QThread::started, m_engine, &RescueEngine::scanSystems);
  thread->start();
}

QString MainWindow::formatBytes(qint64 n) const
{
  if (n < 1024) {
    return QStringLiteral("%1 B").arg(n);
  }
  if (n < 1024 * 1024) {
    return QStringLiteral("%1 KB").arg(n / 1024.0, 0, 'f', 1);
  }
  if (n < 1024LL * 1024 * 1024) {
    return QStringLiteral("%1 MB").arg(n / (1024.0 * 1024.0), 0, 'f', 1);
  }
  return QStringLiteral("%1 GB").arg(n / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}

void MainWindow::buildUi()
{
  auto *central = new QWidget(this);
  setCentralWidget(central);
  auto *root = new QVBoxLayout(central);
  m_stack = new QStackedWidget(central);
  root->addWidget(m_stack, 1);

  // --- Scan ---
  m_pageScan = new QWidget;
  {
    auto *lay = new QVBoxLayout(m_pageScan);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Spike Rescue</h2>"), m_pageScan));
    lay->addWidget(new QLabel(QStringLiteral("Scanning for installed systems…"), m_pageScan));
    m_scanLabel = new QLabel(QStringLiteral("Starting…"), m_pageScan);
    m_scanLabel->setWordWrap(true);
    lay->addWidget(m_scanLabel);
    m_scanBar = new QProgressBar(m_pageScan);
    m_scanBar->setRange(0, 100);
    lay->addWidget(m_scanBar);
    lay->addStretch(1);
  }
  m_stack->addWidget(m_pageScan);

  // --- Select ---
  m_pageSelect = new QWidget;
  {
    auto *lay = new QVBoxLayout(m_pageSelect);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Select a system</h2>"), m_pageSelect));
    lay->addWidget(new QLabel(QStringLiteral("Found the following systems on your computer:"),
                              m_pageSelect));
    m_systemList = new QListWidget(m_pageSelect);
    lay->addWidget(m_systemList, 1);
    m_selectStatus = new QLabel(m_pageSelect);
    m_selectStatus->setWordWrap(true);
    lay->addWidget(m_selectStatus);
    auto *row = new QHBoxLayout();
    auto *rescan = new QPushButton(QStringLiteral("Scan again"), m_pageSelect);
    auto *next = new QPushButton(QStringLiteral("Recover files"), m_pageSelect);
    row->addWidget(rescan);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(rescan, &QPushButton::clicked, this, [this]() {
      m_stack->setCurrentWidget(m_pageScan);
      QMetaObject::invokeMethod(m_engine, "scanSystems", Qt::QueuedConnection);
    });
    connect(next, &QPushButton::clicked, this, &MainWindow::goInventory);
  }
  m_stack->addWidget(m_pageSelect);

  // --- Inventory ---
  m_pageInventory = new QWidget;
  {
    auto *lay = new QVBoxLayout(m_pageInventory);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Files found</h2>"), m_pageInventory));
    m_invBody = new QTextEdit(m_pageInventory);
    m_invBody->setReadOnly(true);
    lay->addWidget(m_invBody, 1);
    auto *row = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("Back"), m_pageInventory);
    auto *next = new QPushButton(QStringLiteral("Continue to recovery"), m_pageInventory);
    row->addWidget(back);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(back, &QPushButton::clicked, this, &MainWindow::goSelect);
    connect(next, &QPushButton::clicked, this, &MainWindow::goDest);
  }
  m_stack->addWidget(m_pageInventory);

  // --- Dest ---
  m_pageDest = new QWidget;
  {
    auto *lay = new QVBoxLayout(m_pageDest);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Save recovered files</h2>"), m_pageDest));
    lay->addWidget(new QLabel(
        QStringLiteral("Choose where to save recovered files. You can use this Spike USB "
                       "(its free “writable” space) if it has enough room, or another USB."),
        m_pageDest));
    m_destList = new QListWidget(m_pageDest);
    lay->addWidget(m_destList, 1);
    m_destStatus = new QLabel(QStringLiteral("Waiting for USB drive…"), m_pageDest);
    m_destStatus->setWordWrap(true);
    lay->addWidget(m_destStatus);
    auto *row = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("Back"), m_pageDest);
    auto *refresh = new QPushButton(QStringLiteral("Refresh"), m_pageDest);
    auto *next = new QPushButton(QStringLiteral("Start recovery"), m_pageDest);
    row->addWidget(back);
    row->addWidget(refresh);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(back, &QPushButton::clicked, this, [this]() {
      if (m_destTimer) {
        m_destTimer->stop();
      }
      m_stack->setCurrentWidget(m_pageInventory);
    });
    connect(refresh, &QPushButton::clicked, this, &MainWindow::refreshDestUi);
    connect(next, &QPushButton::clicked, this, &MainWindow::goCopy);
    m_destTimer = new QTimer(this);
    m_destTimer->setInterval(2000);
    connect(m_destTimer, &QTimer::timeout, this, &MainWindow::refreshDestUi);
  }
  m_stack->addWidget(m_pageDest);

  // --- Copy ---
  m_pageCopy = new QWidget;
  {
    auto *lay = new QVBoxLayout(m_pageCopy);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Recovering files…</h2>"), m_pageCopy));
    m_copyLabel = new QLabel(QStringLiteral("Starting…"), m_pageCopy);
    m_copyLabel->setWordWrap(true);
    lay->addWidget(m_copyLabel);
    m_copyBar = new QProgressBar(m_pageCopy);
    m_copyBar->setRange(0, 100);
    lay->addWidget(m_copyBar);
    m_copyStats = new QLabel(m_pageCopy);
    m_copyStats->setWordWrap(true);
    lay->addWidget(m_copyStats);
    lay->addStretch(1);
    auto *cancel = new QPushButton(QStringLiteral("Cancel"), m_pageCopy);
    lay->addWidget(cancel, 0, Qt::AlignLeft);
    connect(cancel, &QPushButton::clicked, this, &MainWindow::cancelCopy);
  }
  m_stack->addWidget(m_pageCopy);

  // --- Done ---
  m_pageDone = new QWidget;
  {
    auto *lay = new QVBoxLayout(m_pageDone);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Recovery complete</h2>"), m_pageDone));
    m_doneBody = new QTextEdit(m_pageDone);
    m_doneBody->setReadOnly(true);
    lay->addWidget(m_doneBody, 1);
    auto *row = new QHBoxLayout();
    auto *reinstall = new QPushButton(QStringLiteral("Reinstall Spike"), m_pageDone);
    auto *exitBtn = new QPushButton(QStringLiteral("Exit to desktop"), m_pageDone);
    row->addWidget(reinstall);
    row->addStretch(1);
    row->addWidget(exitBtn);
    lay->addLayout(row);
    connect(reinstall, &QPushButton::clicked, this, &MainWindow::finishReinstall);
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::finishExit);
  }
  m_stack->addWidget(m_pageDone);

  m_stack->setCurrentWidget(m_pageScan);
}

void MainWindow::onScanFinished(bool ok)
{
  if (!ok) {
    m_selectStatus->setText(QStringLiteral("Scan failed: %1").arg(m_engine->lastError()));
  }
  goSelect();
}

void MainWindow::goSelect()
{
  m_systemList->clear();
  const auto systems = m_engine->systems();
  for (int i = 0; i < systems.size(); ++i) {
    const DetectedSystem &s = systems.at(i);
    const QString users = s.users.join(QStringLiteral(", "));
    auto *item = new QListWidgetItem(
        QStringLiteral("%1 on %2 (%3)\n  Users: %4 — %5 files (%6)")
            .arg(s.osLabel, s.partition.path, s.partition.fstype, users)
            .arg(s.fileCount)
            .arg(formatBytes(s.byteTotal)));
    item->setData(Qt::UserRole, i);
    m_systemList->addItem(item);
  }
  if (systems.isEmpty()) {
    const QString detail = m_engine->lastScanSummary();
    m_selectStatus->setText(
        QStringLiteral("No recoverable systems found.\n%1")
            .arg(detail.isEmpty()
                     ? QStringLiteral("Check the disk is connected and try Scan again.")
                     : detail));
  } else {
    m_selectStatus->setText(
        QStringLiteral("Select a system to recover files from.\n\n[debug] %1")
            .arg(m_engine->lastScanSummary()));
    m_systemList->setCurrentRow(0);
  }
  m_stack->setCurrentWidget(m_pageSelect);
}

void MainWindow::goInventory()
{
  if (!m_systemList->currentItem()) {
    m_selectStatus->setText(QStringLiteral("Select a system first."));
    return;
  }
  m_selectedSystem = m_systemList->currentItem()->data(Qt::UserRole).toInt();
  m_invBody->setPlainText(QStringLiteral("Scanning files…"));
  m_stack->setCurrentWidget(m_pageInventory);
  QMetaObject::invokeMethod(m_engine, "inventorySystem", Qt::QueuedConnection,
                            Q_ARG(int, m_selectedSystem));
}

void MainWindow::onInventoryFinished(bool ok)
{
  if (!ok) {
    m_invBody->setPlainText(QStringLiteral("Inventory failed: %1").arg(m_engine->lastError()));
    return;
  }
  const Inventory inv = m_engine->inventory();
  m_neededBytes = inv.byteTotal;
  QString text;
  const auto systems = m_engine->systems();
  if (m_selectedSystem >= 0 && m_selectedSystem < systems.size()) {
    const DetectedSystem &s = systems.at(m_selectedSystem);
    text += QStringLiteral("Files found on %1 (%2):\n").arg(s.osLabel, s.partition.path);
    text += QStringLiteral("[debug] fstype=%1 mount=%2 homeMount=%3 users=%4\n\n")
                .arg(s.partition.fstype, s.mountPoint, s.homeMountPoint,
                     s.users.join(QLatin1Char(',')));
  }
  for (const CategorySummary &c : inv.categories) {
    text += QStringLiteral("  %1 — %2 files, %3\n")
                .arg(c.name)
                .arg(c.files)
                .arg(formatBytes(c.bytes));
  }
  text += QStringLiteral("\nTotal: %1 files (%2)\n").arg(inv.fileCount).arg(formatBytes(inv.byteTotal));
  if (!inv.unreadable.isEmpty()) {
    text += QStringLiteral("\n⚠️ %1 paths could not be read during scan.\n").arg(inv.unreadable.size());
  }
  if (!inv.files.isEmpty()) {
    text += QStringLiteral("\n[debug] sample source paths:\n");
    for (const QString &p : inv.files.mid(0, 8)) {
      text += QStringLiteral("  %1\n").arg(p);
    }
  }
  text += QStringLiteral("\n[debug] scan: %1\n").arg(m_engine->lastScanSummary());
  m_invBody->setPlainText(text);
}

void MainWindow::goDest()
{
  m_destTimer->start();
  refreshDestUi();
  m_stack->setCurrentWidget(m_pageDest);
}

void MainWindow::refreshDestUi()
{
  QMetaObject::invokeMethod(m_engine, "refreshDestinations", Qt::QueuedConnection);
}

void MainWindow::onDestinations(const QVector<DestVolume> &vols)
{
  const QString prev =
      m_destList->currentItem() ? m_destList->currentItem()->data(Qt::UserRole).toString()
                                : QString();
  m_destList->clear();
  for (const DestVolume &d : vols) {
    const bool enough = d.freeBytes >= m_neededBytes;
    auto *item = new QListWidgetItem(
        QStringLiteral("%1\n  path: %2\n  device: %3\n  Free: %4 %5")
            .arg(d.label, d.path, d.device, formatBytes(d.freeBytes),
                 enough ? QStringLiteral("✓ enough space") : QStringLiteral("✗ need more space")));
    item->setData(Qt::UserRole, d.path);
    item->setData(Qt::UserRole + 1, d.freeBytes);
    item->setFlags(enough ? (Qt::ItemIsEnabled | Qt::ItemIsSelectable) : Qt::NoItemFlags);
    m_destList->addItem(item);
    if (d.path == prev) {
      m_destList->setCurrentItem(item);
    }
  }
  if (vols.isEmpty()) {
    m_destStatus->setText(
        QStringLiteral("No writable destination found. Insert another USB, or free space on "
                       "this Spike USB’s writable partition, then Refresh."));
  } else {
    m_destStatus->setText(
        QStringLiteral("Files will be saved under SpikeBackup/ on the selected drive.\n"
                       "Need about %1 free.")
            .arg(formatBytes(m_neededBytes)));
    if (!m_destList->currentItem()) {
      for (int i = 0; i < m_destList->count(); ++i) {
        if (m_destList->item(i)->flags() & Qt::ItemIsSelectable) {
          m_destList->setCurrentRow(i);
          break;
        }
      }
    }
  }
}

void MainWindow::goCopy()
{
  if (!m_destList->currentItem()) {
    m_destStatus->setText(QStringLiteral("Select a destination USB with enough free space."));
    return;
  }
  const QString dest = m_destList->currentItem()->data(Qt::UserRole).toString();
  const qint64 free = m_destList->currentItem()->data(Qt::UserRole + 1).toLongLong();
  if (free < m_neededBytes) {
    m_destStatus->setText(QStringLiteral("Not enough free space on that drive."));
    return;
  }
  m_destTimer->stop();
  m_copyBar->setValue(0);
  m_copyLabel->setText(QStringLiteral("Starting…"));
  m_stack->setCurrentWidget(m_pageCopy);
  QMetaObject::invokeMethod(m_engine, "startCopy", Qt::QueuedConnection, Q_ARG(int, m_selectedSystem),
                            Q_ARG(QString, dest));
}

void MainWindow::onCopyProgress(const QString &file, qint64 done, qint64 total, qint64 doneBytes,
                                qint64 totalBytes)
{
  m_copyLabel->setText(QStringLiteral("Currently copying: %1").arg(file));
  const int pct = total > 0 ? int((100 * done) / total) : 0;
  m_copyBar->setValue(pct);
  m_copyStats->setText(QStringLiteral("Files: %1 / %2\nData: %3 / %4")
                           .arg(done)
                           .arg(total)
                           .arg(formatBytes(doneBytes), formatBytes(totalBytes)));
}

void MainWindow::cancelCopy()
{
  const auto r = QMessageBox::question(
      this, QStringLiteral("Stop recovery?"),
      QStringLiteral("Stop recovery? Files already copied will remain on the USB drive."));
  if (r == QMessageBox::Yes) {
    QMetaObject::invokeMethod(m_engine, "requestCancel", Qt::QueuedConnection);
  }
}

void MainWindow::onCopyFinished(bool ok)
{
  const CopyResult r = m_engine->lastCopy();
  QString text;
  if (r.cancelled) {
    text += QStringLiteral("Recovery cancelled (partial copy kept).\n\n");
  } else if (!ok) {
    text += QStringLiteral("Recovery failed: %1\n\n").arg(m_engine->lastError());
  } else {
    text += QStringLiteral("Recovery complete!\n\n");
  }
  text += QStringLiteral("Files recovered: %1\n").arg(r.copied);
  text += QStringLiteral("Could not read (source): %1\n").arg(r.failedRead);
  text += QStringLiteral("Could not write (destination): %1\n").arg(r.failedWrite);
  text += QStringLiteral("Verification failed: %1\n").arg(r.failedVerify);
  text += QStringLiteral("Data recovered: %1\n").arg(formatBytes(r.bytesCopied));
  text += QStringLiteral("\nSaved under:\n%1\n").arg(r.destRoot);
  text += QStringLiteral("\nREPORT.txt is written next to the backup (bring that USB for debug).\n");
  if (!r.failureDetails.isEmpty()) {
    text += QStringLiteral("\nFailure details:\n");
    for (const QString &p : r.failureDetails.mid(0, 25)) {
      text += QStringLiteral("  • %1\n").arg(p);
    }
    if (r.failureDetails.size() > 25) {
      text += QStringLiteral("  … and %1 more\n").arg(r.failureDetails.size() - 25);
    }
  }
  text += QStringLiteral("\n======== DEBUG LOG ========\n");
  const QStringList dbg = r.debugLog.isEmpty() ? m_engine->debugLog() : r.debugLog;
  for (const QString &line : dbg) {
    text += line + QLatin1Char('\n');
  }
  m_doneBody->setPlainText(text);
  m_stack->setCurrentWidget(m_pageDone);
}

void MainWindow::finishExit()
{
  QMetaObject::invokeMethod(m_engine, "cleanupMounts", Qt::BlockingQueuedConnection);
  close();
}

void MainWindow::finishReinstall()
{
  if (QProcess::startDetached(QStringLiteral("spike-installer"), {}) ||
      QProcess::startDetached(QStringLiteral("install-spike"), {})) {
    return;
  }
  QMessageBox::information(
      this, QStringLiteral("Installer"),
      QStringLiteral("Installer is not on this image yet. Exit to the desktop and use "
                     "Install Spike when it ships."));
}

} // namespace spike
