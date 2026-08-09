#include "SpikeBackupLayout.hpp"

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPalette>
#include <QProcess>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace {

void darkPalette(QApplication &app)
{
  QPalette pal = app.palette();
  pal.setColor(QPalette::Window, QColor(0x1a, 0x1a, 0x2e));
  pal.setColor(QPalette::WindowText, QColor(0xff, 0xff, 0xff));
  pal.setColor(QPalette::Base, QColor(0x22, 0x22, 0x36));
  pal.setColor(QPalette::Text, QColor(0xff, 0xff, 0xff));
  pal.setColor(QPalette::Button, QColor(0x22, 0x22, 0x36));
  pal.setColor(QPalette::ButtonText, QColor(0xff, 0xff, 0xff));
  pal.setColor(QPalette::Highlight, QColor(0x6d, 0x4a, 0xff));
  app.setPalette(pal);
}

QStringList volumeRoots()
{
  QStringList roots;
  for (const QString &base : {QStringLiteral("/run/media"), QStringLiteral("/media")}) {
    QDir root(base);
    if (!root.exists()) {
      continue;
    }
    for (const QString &user : root.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QDir userDir(root.filePath(user));
      for (const QString &vol : userDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        roots << userDir.filePath(vol);
      }
    }
  }
  QProcess proc;
  proc.start(QStringLiteral("findmnt"),
             {QStringLiteral("-n"), QStringLiteral("-o"), QStringLiteral("TARGET"),
              QStringLiteral("-S"), QStringLiteral("LABEL=writable")});
  if (proc.waitForFinished(5000)) {
    for (QString t : QString::fromUtf8(proc.readAllStandardOutput())
                         .split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
      t = t.trimmed();
      if (!t.isEmpty() && !roots.contains(t)) {
        roots << t;
      }
    }
  }
  return roots;
}

/** Mode A/B wizard shell (SPIKE-MIGRATION.md) over spike-rescue batch CLI. */
class MigrationWindow : public QWidget
{
  Q_OBJECT
public:
  enum Page {
    Welcome = 0,
    Scan,
    Select,
    Dest,
    Progress,
    Done,
    ImportSessions,
    ImportProgress,
    ImportDone,
  };

  explicit MigrationWindow(QWidget *parent = nullptr)
    : QWidget(parent)
  {
    setWindowTitle(QStringLiteral("Spike Migration"));
    resize(740, 540);
    auto *root = new QVBoxLayout(this);
    root->addWidget(new QLabel(QStringLiteral("<h2>Spike Migration</h2>"), this));

    m_stack = new QStackedWidget(this);
    root->addWidget(m_stack, 1);

    m_stack->addWidget(buildWelcome());
    m_stack->addWidget(buildScan());
    m_stack->addWidget(buildSelect());
    m_stack->addWidget(buildDest());
    m_stack->addWidget(buildProgress());
    m_stack->addWidget(buildDone());
    m_stack->addWidget(buildImportSessions());
    m_stack->addWidget(buildImportProgress());
    m_stack->addWidget(buildImportDone());
  }

private:
  QWidget *buildWelcome()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(
        QStringLiteral(
            "Moving to Spike?\n\n"
            "This tool copies your personal files (documents, photos, videos, music) "
            "to a USB drive before you install Spike.\n\n"
            "Your old system disk is never changed.\n\n"
            "Already installed Spike? Import from SpikeBackup into this home.\n\n"
            "Size preview and conflict rename come later — copies use Spike Rescue."),
        page));
    auto *before = new QPushButton(QStringLiteral("Before install — back up old OS to USB"), page);
    auto *into = new QPushButton(QStringLiteral("Into this Spike — import SpikeBackup"), page);
    auto *install = new QPushButton(QStringLiteral("Open Install Spike…"), page);
    lay->addWidget(before);
    lay->addWidget(into);
    lay->addWidget(install);
    lay->addStretch(1);
    connect(before, &QPushButton::clicked, this, [this]() {
      m_modeA = true;
      go(Scan);
      startListSystems();
    });
    connect(into, &QPushButton::clicked, this, [this]() {
      m_modeA = false;
      refreshSessions();
      go(ImportSessions);
    });
    connect(install, &QPushButton::clicked, this, []() {
      QProcess::startDetached(QStringLiteral("spike-installer"), {});
    });
    return page;
  }

  QWidget *buildScan()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Scan for systems</h3>"), page));
    m_scanLog = new QTextEdit(page);
    m_scanLog->setReadOnly(true);
    lay->addWidget(m_scanLog, 1);
    auto *row = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("Back"), page);
    auto *again = new QPushButton(QStringLiteral("Scan again"), page);
    auto *next = new QPushButton(QStringLiteral("Continue"), page);
    m_scanNext = next;
    next->setEnabled(false);
    row->addWidget(back);
    row->addWidget(again);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(back, &QPushButton::clicked, this, [this]() { go(Welcome); });
    connect(again, &QPushButton::clicked, this, &MigrationWindow::startListSystems);
    connect(next, &QPushButton::clicked, this, [this]() {
      copySystemsToSelect();
      go(Select);
    });
    return page;
  }

  QWidget *buildSelect()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Select the system you are moving from</h3>"), page));
    m_selectList = new QListWidget(page);
    lay->addWidget(m_selectList, 1);
    auto *row = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("Back"), page);
    auto *next = new QPushButton(QStringLiteral("Continue"), page);
    row->addWidget(back);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(back, &QPushButton::clicked, this, [this]() { go(Scan); });
    connect(next, &QPushButton::clicked, this, [this]() {
      if (!m_selectList->currentItem() ||
          !m_selectList->currentItem()->data(Qt::UserRole).isValid()) {
        QMessageBox::warning(this, QStringLiteral("Select"),
                             QStringLiteral("Select a system."));
        return;
      }
      m_systemIndex = m_selectList->currentItem()->data(Qt::UserRole).toInt();
      refreshDests();
      go(Dest);
    });
    return page;
  }

  QWidget *buildDest()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Choose backup destination</h3>"), page));
    lay->addWidget(new QLabel(QStringLiteral("USB or Spike live writable (SpikeBackup/)."), page));
    m_destList = new QListWidget(page);
    lay->addWidget(m_destList, 1);
    auto *row = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("Back"), page);
    auto *refresh = new QPushButton(QStringLiteral("Refresh mounts"), page);
    auto *next = new QPushButton(QStringLiteral("Start backup"), page);
    row->addWidget(back);
    row->addWidget(refresh);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(back, &QPushButton::clicked, this, [this]() { go(Select); });
    connect(refresh, &QPushButton::clicked, this, &MigrationWindow::refreshDests);
    connect(next, &QPushButton::clicked, this, [this]() {
      if (!m_destList->currentItem() ||
          m_destList->currentItem()->data(Qt::UserRole).toString().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Destination"),
                             QStringLiteral("Select a USB / writable mount."));
        return;
      }
      m_destMount = m_destList->currentItem()->data(Qt::UserRole).toString();
      go(Progress);
      startRecover();
    });
    return page;
  }

  QWidget *buildProgress()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Copying files…</h3>"), page));
    m_progressLog = new QTextEdit(page);
    m_progressLog->setReadOnly(true);
    lay->addWidget(m_progressLog, 1);
    m_progressHint = new QLabel(QStringLiteral("Running spike-rescue --batch-recover …"), page);
    lay->addWidget(m_progressHint);
    return page;
  }

  QWidget *buildDone()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Backup complete</h3>"), page));
    m_doneLabel = new QLabel(page);
    m_doneLabel->setWordWrap(true);
    lay->addWidget(m_doneLabel);
    auto *row = new QHBoxLayout();
    auto *home = new QPushButton(QStringLiteral("Back to start"), page);
    auto *install = new QPushButton(QStringLiteral("Open Install Spike…"), page);
    row->addWidget(home);
    row->addStretch(1);
    row->addWidget(install);
    lay->addLayout(row);
    lay->addStretch(1);
    connect(home, &QPushButton::clicked, this, [this]() { go(Welcome); });
    connect(install, &QPushButton::clicked, this, []() {
      QProcess::startDetached(QStringLiteral("spike-installer"), {});
    });
    return page;
  }

  QWidget *buildImportSessions()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Choose a SpikeBackup session</h3>"), page));
    m_sessionList = new QListWidget(page);
    lay->addWidget(m_sessionList, 1);
    auto *row = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("Back"), page);
    auto *refresh = new QPushButton(QStringLiteral("Refresh"), page);
    auto *next = new QPushButton(QStringLiteral("Restore into $HOME"), page);
    row->addWidget(back);
    row->addWidget(refresh);
    row->addStretch(1);
    row->addWidget(next);
    lay->addLayout(row);
    connect(back, &QPushButton::clicked, this, [this]() { go(Welcome); });
    connect(refresh, &QPushButton::clicked, this, &MigrationWindow::refreshSessions);
    connect(next, &QPushButton::clicked, this, [this]() {
      if (!m_sessionList->currentItem() ||
          m_sessionList->currentItem()->data(Qt::UserRole).toString().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Restore"),
                             QStringLiteral("Select a SpikeBackup session."));
        return;
      }
      m_sessionPath = m_sessionList->currentItem()->data(Qt::UserRole).toString();
      go(ImportProgress);
      startRestore();
    });
    return page;
  }

  QWidget *buildImportProgress()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Restoring…</h3>"), page));
    m_importLog = new QTextEdit(page);
    m_importLog->setReadOnly(true);
    lay->addWidget(m_importLog, 1);
    return page;
  }

  QWidget *buildImportDone()
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h3>Import complete</h3>"), page));
    m_importDoneLabel = new QLabel(page);
    m_importDoneLabel->setWordWrap(true);
    lay->addWidget(m_importDoneLabel);
    auto *home = new QPushButton(QStringLiteral("Back to start"), page);
    lay->addWidget(home);
    lay->addStretch(1);
    connect(home, &QPushButton::clicked, this, [this]() { go(Welcome); });
    return page;
  }

  void go(Page p) { m_stack->setCurrentIndex(static_cast<int>(p)); }

  void killProc()
  {
    if (m_proc) {
      m_proc->disconnect();
      m_proc->kill();
      m_proc->deleteLater();
      m_proc = nullptr;
    }
  }

  void startListSystems()
  {
    killProc();
    m_scanLog->clear();
    m_scanNext->setEnabled(false);
    m_scannedSystems.clear();
    m_scanLog->append(QStringLiteral("Running spike-rescue --list-systems …"));
    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead, this, [this]() {
      m_scanLog->append(QString::fromUtf8(m_proc->readAll()).trimmed());
    });
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int code, QProcess::ExitStatus) {
              const QString text = m_scanLog->toPlainText();
              m_scannedSystems.clear();
              for (const QString &line : text.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
                if (line.startsWith(QLatin1String("systems=")) || line.startsWith(QLatin1Char('[')) ||
                    line.startsWith(QLatin1String("ERROR")) ||
                    line.startsWith(QLatin1String("WARN"))) {
                  continue;
                }
                const QStringList p = line.split(QLatin1Char('\t'));
                if (p.size() < 3) {
                  continue;
                }
                bool ok = false;
                const int idx = p.at(0).toInt(&ok);
                if (!ok) {
                  continue;
                }
                m_scannedSystems.append(
                    qMakePair(idx, QStringLiteral("[%1] %2 — %3 (%4)")
                                       .arg(p.at(0), p.value(2), p.value(1), p.value(3))));
              }
              if (m_scannedSystems.isEmpty()) {
                m_scanLog->append(QStringLiteral("No systems found (exit %1).").arg(code));
              }
              m_scanNext->setEnabled(!m_scannedSystems.isEmpty());
              m_proc->deleteLater();
              m_proc = nullptr;
            });
    m_proc->start(QStringLiteral("spike-rescue"), {QStringLiteral("--list-systems")});
  }

  void copySystemsToSelect()
  {
    m_selectList->clear();
    for (const auto &pair : m_scannedSystems) {
      auto *item = new QListWidgetItem(pair.second);
      item->setData(Qt::UserRole, pair.first);
      m_selectList->addItem(item);
    }
    if (m_selectList->count() > 0) {
      m_selectList->setCurrentRow(0);
    }
  }

  void refreshDests()
  {
    m_destList->clear();
    for (const QString &r : volumeRoots()) {
      auto *item = new QListWidgetItem(r);
      item->setData(Qt::UserRole, r);
      m_destList->addItem(item);
    }
    if (m_destList->count() == 0) {
      m_destList->addItem(QStringLiteral("(No USB / writable mounts — plug in media)"));
    } else {
      for (int i = 0; i < m_destList->count(); ++i) {
        if (m_destList->item(i)->text().contains(QLatin1String("writable"), Qt::CaseInsensitive)) {
          m_destList->setCurrentRow(i);
          return;
        }
      }
      m_destList->setCurrentRow(0);
    }
  }

  void refreshSessions()
  {
    m_sessionList->clear();
    for (const spike::BackupSession &s : spike::discoverAllBackupSessions(volumeRoots())) {
      auto *item = new QListWidgetItem(
          QStringLiteral("%1 / %2 — %3 files").arg(s.stamp, s.osLabel).arg(s.fileCount));
      item->setData(Qt::UserRole, s.sessionPath);
      m_sessionList->addItem(item);
    }
    if (m_sessionList->count() == 0) {
      m_sessionList->addItem(QStringLiteral("(No SpikeBackup sessions found)"));
    } else {
      m_sessionList->setCurrentRow(0);
    }
  }

  void startRecover()
  {
    killProc();
    m_progressLog->clear();
    m_progressHint->setText(QStringLiteral("Running…"));
    m_progressLog->append(QStringLiteral("spike-rescue --batch-recover --dest %1 --system %2")
                              .arg(m_destMount)
                              .arg(m_systemIndex));
    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead, this, [this]() {
      m_progressLog->append(QString::fromUtf8(m_proc->readAll()).trimmed());
    });
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int code, QProcess::ExitStatus) {
              QString session;
              for (const QString &line :
                   m_progressLog->toPlainText().split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
                if (line.startsWith(QLatin1String("SESSION_PATH="))) {
                  session = line.mid(QStringLiteral("SESSION_PATH=").size()).trimmed();
                }
              }
              if (code == 0) {
                m_doneLabel->setText(
                    session.isEmpty()
                        ? QStringLiteral("Backup finished.")
                        : QStringLiteral("Backup finished.\n\n%1\n\nYou can Open Install Spike next.")
                              .arg(session));
              } else {
                m_doneLabel->setText(
                    QStringLiteral("Backup failed (exit %1). See log on previous step.")
                        .arg(code));
              }
              m_proc->deleteLater();
              m_proc = nullptr;
              go(Done);
            });
    m_proc->start(QStringLiteral("spike-rescue"),
                  {QStringLiteral("--batch-recover"), QStringLiteral("--dest"), m_destMount,
                   QStringLiteral("--system"), QString::number(m_systemIndex)});
  }

  void startRestore()
  {
    killProc();
    m_importLog->clear();
    const QString home = QDir::homePath();
    m_importLog->append(QStringLiteral("spike-rescue --batch-restore --session %1 --home %2")
                            .arg(m_sessionPath, home));
    m_proc = new QProcess(this);
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_proc, &QProcess::readyRead, this, [this]() {
      m_importLog->append(QString::fromUtf8(m_proc->readAll()).trimmed());
    });
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this, home](int code, QProcess::ExitStatus) {
              m_importDoneLabel->setText(
                  code == 0 ? QStringLiteral("Restore finished into %1.").arg(home)
                            : QStringLiteral("Restore failed (exit %1).").arg(code));
              m_proc->deleteLater();
              m_proc = nullptr;
              go(ImportDone);
            });
    m_proc->start(QStringLiteral("spike-rescue"),
                  {QStringLiteral("--batch-restore"), QStringLiteral("--session"), m_sessionPath,
                   QStringLiteral("--home"), home});
  }

  QStackedWidget *m_stack = nullptr;
  QTextEdit *m_scanLog = nullptr;
  QPushButton *m_scanNext = nullptr;
  QListWidget *m_selectList = nullptr;
  QListWidget *m_destList = nullptr;
  QTextEdit *m_progressLog = nullptr;
  QLabel *m_progressHint = nullptr;
  QLabel *m_doneLabel = nullptr;
  QListWidget *m_sessionList = nullptr;
  QTextEdit *m_importLog = nullptr;
  QLabel *m_importDoneLabel = nullptr;

  QProcess *m_proc = nullptr;
  bool m_modeA = true;
  int m_systemIndex = 0;
  QString m_destMount;
  QString m_sessionPath;
  QList<QPair<int, QString>> m_scannedSystems;
};

} // namespace

#include "main.moc"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-migration"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.3"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));
  darkPalette(app);

  MigrationWindow win;
  win.show();
  return app.exec();
}
