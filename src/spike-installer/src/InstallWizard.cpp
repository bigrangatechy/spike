#include "InstallWizard.hpp"

#include "InstallEngine.hpp"
#include "InstallLocale.hpp"
#include "detect/Detect.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

namespace spike {

namespace {

const char *const kStepTitles[] = {
    "Welcome + language",
    "Timezone + keyboard",
    "Wi-Fi",
    "Username + password",
    "Computer name",
    "Variant",
    "Data backup (optional)",
    "Storage confirmation",
    "Installation",
    "Finish",
};

} // namespace

InstallWizard::InstallWizard(QWidget *parent)
  : QMainWindow(parent)
{
  setWindowTitle(QStringLiteral("Install Spike [pre-alpha]"));
  resize(820, 580);
  m_state.detectSummary = HardwareProbe::summary();
  m_state.variant = HardwareProbe::recommendVariant();
  m_engine = new InstallEngine(this);
  connect(m_engine, &InstallEngine::logLine, this, &InstallWizard::onInstallLog);
  connect(m_engine, &InstallEngine::finished, this, &InstallWizard::onInstallFinished);
  buildUi();
  applyPageToUi(0);
}

void InstallWizard::buildUi()
{
  auto *central = new QWidget(this);
  setCentralWidget(central);
  auto *root = new QVBoxLayout(central);

  m_stepLabel = new QLabel(central);
  m_stepLabel->setWordWrap(true);
  root->addWidget(m_stepLabel);

  m_stack = new QStackedWidget(central);
  root->addWidget(m_stack, 1);

  // 0 Welcome + language
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Welcome to Spike</h2>"), page));
    lay->addWidget(new QLabel(QStringLiteral("Let's Make Tech Repairable Again"), page));
    lay->addWidget(new QLabel(QStringLiteral("Choose your language:"), page));
    m_language = new QComboBox(page);
    for (const auto &lang : supportedLanguages()) {
      m_language->addItem(lang.first, lang.second);
    }
    connect(m_language, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &InstallWizard::onLanguageChanged);
    lay->addWidget(m_language);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 1 Timezone + keyboard
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Timezone</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Select your timezone (type to filter). Region/city from system zone database."),
        page));
    m_timezone = new QComboBox(page);
    m_timezone->setEditable(true);
    m_timezone->setInsertPolicy(QComboBox::NoInsert);
    m_timezone->addItems(availableTimeZones());
    if (auto *c = m_timezone->completer()) {
      c->setFilterMode(Qt::MatchContains);
      c->setCaseSensitivity(Qt::CaseInsensitive);
      c->setCompletionMode(QCompleter::PopupCompletion);
    }
    connect(m_timezone, &QComboBox::currentTextChanged, this, &InstallWizard::onTimezoneChanged);
    lay->addWidget(m_timezone);

    lay->addWidget(new QLabel(QStringLiteral("<h2>Keyboard layout</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Suggested from language/timezone — change if needed. Type below to test."),
        page));
    m_keyboard = new QComboBox(page);
    m_keyboard->setEditable(true);
    m_keyboard->setInsertPolicy(QComboBox::NoInsert);
    for (const auto &kb : availableKeyboardLayouts()) {
      m_keyboard->addItem(kb.first, kb.second);
    }
    if (auto *c = m_keyboard->completer()) {
      c->setFilterMode(Qt::MatchContains);
      c->setCaseSensitivity(Qt::CaseInsensitive);
      c->setCompletionMode(QCompleter::PopupCompletion);
    }
    connect(m_keyboard, QOverload<int>::of(&QComboBox::activated), this, [this](int) {
      m_keyboardTouched = true;
    });
    lay->addWidget(m_keyboard);
    m_keyboardTest = new QLineEdit(page);
    m_keyboardTest->setPlaceholderText(QStringLiteral("Type here to test your keyboard…"));
    lay->addWidget(m_keyboardTest);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // Apply language-driven defaults once combos exist
  onLanguageChanged();

  // 2 Wi-Fi
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Wi-Fi</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Connect to a network (optional). Install works offline — "
                       "Skip if you are on Ethernet or want to continue without Wi‑Fi."),
        page));
    m_wifiStatus = new QLabel(page);
    m_wifiStatus->setWordWrap(true);
    lay->addWidget(m_wifiStatus);
    m_wifiList = new QListWidget(page);
    lay->addWidget(m_wifiList, 1);
    lay->addWidget(new QLabel(QStringLiteral("Password (if required)"), page));
    m_wifiPassword = new QLineEdit(page);
    m_wifiPassword->setEchoMode(QLineEdit::Password);
    lay->addWidget(m_wifiPassword);
    auto *row = new QHBoxLayout();
    m_wifiScanBtn = new QPushButton(QStringLiteral("Scan"), page);
    m_wifiConnectBtn = new QPushButton(QStringLiteral("Connect"), page);
    m_wifiSkipBtn = new QPushButton(QStringLiteral("Skip Wi‑Fi"), page);
    row->addWidget(m_wifiScanBtn);
    row->addWidget(m_wifiConnectBtn);
    row->addWidget(m_wifiSkipBtn);
    lay->addLayout(row);
    connect(m_wifiScanBtn, &QPushButton::clicked, this, &InstallWizard::refreshWifiUi);
    connect(m_wifiConnectBtn, &QPushButton::clicked, this, &InstallWizard::connectWifi);
    connect(m_wifiSkipBtn, &QPushButton::clicked, this, [this]() {
      m_wifiSkipped = true;
      m_state.wifiSsid.clear();
      m_state.wifiConnected = QStringLiteral("skipped");
      m_wifiStatus->setText(QStringLiteral("Wi‑Fi skipped — continuing offline is fine."));
      goNext();
    });
    m_stack->addWidget(page);
  }

  // 3 User
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Your account</h2>"), page));
    lay->addWidget(new QLabel(QStringLiteral("Username"), page));
    m_username = new QLineEdit(QStringLiteral("spike"), page);
    lay->addWidget(m_username);
    lay->addWidget(new QLabel(QStringLiteral("Password"), page));
    m_password = new QLineEdit(page);
    m_password->setEchoMode(QLineEdit::Password);
    lay->addWidget(m_password);
    lay->addWidget(new QLabel(QStringLiteral("Confirm password"), page));
    m_password2 = new QLineEdit(page);
    m_password2->setEchoMode(QLineEdit::Password);
    lay->addWidget(m_password2);
    m_autoLogin = new QCheckBox(
        QStringLiteral("Log in automatically (skip the login prompt on boot)"), page);
    m_autoLogin->setChecked(false);
    lay->addWidget(m_autoLogin);
    lay->addWidget(new QLabel(
        QStringLiteral("Default is a login prompt before the desktop (same as other OSes). "
                       "You can change this later in Settings → Users."),
        page));
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 4 Hostname
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Computer name</h2>"), page));
    m_hostname = new QLineEdit(QStringLiteral("spike"), page);
    lay->addWidget(m_hostname);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 5 Variant
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Spike variant</h2>"), page));
    m_variantHint = new QLabel(page);
    m_variantHint->setWordWrap(true);
    lay->addWidget(m_variantHint);
    m_variant = new QComboBox(page);
    m_variant->addItem(QStringLiteral("Standard (recommended for this PC)"),
                       QStringLiteral("standard"));
    m_variant->addItem(QStringLiteral("Plus"), QStringLiteral("plus"));
    lay->addWidget(m_variant);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 6 Backup / restore prep
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Data backup (optional)</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Optionally copy personal files from an OS on this machine to a "
                       "USB (SpikeBackup/) before Spike erases the install disk — including "
                       "the system on that disk. Uses the same engine as Rescue My Files."),
        page));
    m_doBackup = new QCheckBox(QStringLiteral("Back up files from this computer before install"),
                               page);
    lay->addWidget(m_doBackup);
    lay->addWidget(new QLabel(QStringLiteral("System to back up:"), page));
    m_backupSystemList = new QListWidget(page);
    lay->addWidget(m_backupSystemList);
    lay->addWidget(new QLabel(QStringLiteral("Backup destination (USB / writable):"), page));
    m_backupDestList = new QListWidget(page);
    lay->addWidget(m_backupDestList);
    lay->addWidget(new QLabel(QStringLiteral("Existing SpikeBackup sessions (for reinstall restore):"),
                              page));
    m_restoreList = new QListWidget(page);
    lay->addWidget(m_restoreList, 1);
    m_restoreCheck =
        new QCheckBox(QStringLiteral("After install, restore from the selected SpikeBackup"), page);
    lay->addWidget(m_restoreCheck);
    auto *refresh = new QPushButton(QStringLiteral("Refresh USB / backups"), page);
    lay->addWidget(refresh);
    connect(refresh, &QPushButton::clicked, this, &InstallWizard::refreshBackupUi);
    m_stack->addWidget(page);
  }

  // 7 Storage
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Storage</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("<b>Warning:</b> Spike will erase the selected disk completely. "
                       "No dual-boot, no manual partitions (INSTALLER.md). "
                       "Do not select the Spike live USB."),
        page));
    m_diskList = new QListWidget(page);
    lay->addWidget(m_diskList, 1);
    m_wipeConfirm = new QCheckBox(
        QStringLiteral("I understand the selected disk will be erased"), page);
    lay->addWidget(m_wipeConfirm);
    lay->addWidget(new QLabel(QStringLiteral("Type ERASE to enable install:"), page));
    m_eraseType = new QLineEdit(page);
    m_eraseType->setPlaceholderText(QStringLiteral("ERASE"));
    lay->addWidget(m_eraseType);
    m_stack->addWidget(page);
  }

  // 8 Progress
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Installation</h2>"), page));
    m_progressBody = new QTextEdit(page);
    m_progressBody->setReadOnly(true);
    lay->addWidget(m_progressBody, 1);
    m_installBtn = new QPushButton(QStringLiteral("Install Spike now"), page);
    lay->addWidget(m_installBtn);
    connect(m_installBtn, &QPushButton::clicked, this, &InstallWizard::startInstall);
    m_stack->addWidget(page);
  }

  // 9 Finish
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Next</h2>"), page));
    m_finishBody = new QTextEdit(page);
    m_finishBody->setReadOnly(true);
    lay->addWidget(m_finishBody, 1);
    m_stack->addWidget(page);
  }

  auto *nav = new QHBoxLayout();
  m_back = new QPushButton(QStringLiteral("Back"), central);
  m_next = new QPushButton(QStringLiteral("Continue"), central);
  nav->addWidget(m_back);
  nav->addStretch(1);
  nav->addWidget(m_next);
  root->addLayout(nav);

  connect(m_back, &QPushButton::clicked, this, &InstallWizard::goBack);
  connect(m_next, &QPushButton::clicked, this, &InstallWizard::goNext);
}

void InstallWizard::fillBackupDestAndSessions()
{
  if (m_backupDestList) {
    m_backupDestList->clear();
    const QStringList roots = BackupScanner::volumeRoots();
    for (const QString &r : roots) {
      auto *item = new QListWidgetItem(r);
      item->setData(Qt::UserRole, r);
      m_backupDestList->addItem(item);
    }
    if (roots.isEmpty()) {
      m_backupDestList->addItem(
          QStringLiteral("(No mounted USB / writable found — plug in media and Refresh)"));
    } else {
      for (int i = 0; i < m_backupDestList->count(); ++i) {
        const QString p = m_backupDestList->item(i)->data(Qt::UserRole).toString();
        if (p.contains(QLatin1String("writable"), Qt::CaseInsensitive) ||
            p == QLatin1String("/var/log")) {
          m_backupDestList->setCurrentRow(i);
          break;
        }
      }
      if (m_backupDestList->currentRow() < 0) {
        m_backupDestList->setCurrentRow(0);
      }
    }
  }

  m_restoreList->clear();
  m_state.backupSessionsFound = BackupScanner::sessionPaths();
  const QStringList labels = BackupScanner::sessionLabels();
  for (int i = 0; i < labels.size(); ++i) {
    auto *item = new QListWidgetItem(labels.at(i));
    item->setData(Qt::UserRole, m_state.backupSessionsFound.value(i));
    m_restoreList->addItem(item);
  }
  if (labels.isEmpty()) {
    m_restoreList->addItem(QStringLiteral("(No SpikeBackup sessions found on mounted media)"));
  }
}

void InstallWizard::refreshBackupUi()
{
  fillBackupDestAndSessions();

  if (!m_backupSystemList) {
    return;
  }

  if (m_listSystemsProc) {
    m_listSystemsProc->disconnect();
    m_listSystemsProc->kill();
    m_listSystemsProc->deleteLater();
    m_listSystemsProc = nullptr;
  }

  m_backupSystemList->clear();
  m_backupSystemList->addItem(QStringLiteral("(Scanning disks for systems — UI stays responsive…)"));
  m_listSystemsBusy = true;

  m_listSystemsProc = new QProcess(this);
  m_listSystemsProc->setProcessChannelMode(QProcess::SeparateChannels);
  connect(m_listSystemsProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          &InstallWizard::onListSystemsFinished);
  m_listSystemsProc->start(QStringLiteral("spike-rescue"), {QStringLiteral("--list-systems")});
  if (!m_listSystemsProc->waitForStarted(5000)) {
    m_listSystemsBusy = false;
    m_backupSystemList->clear();
    m_backupSystemList->addItem(
        QStringLiteral("(spike-rescue failed to start — is it installed?)"));
    m_listSystemsProc->deleteLater();
    m_listSystemsProc = nullptr;
    return;
  }
  // Cap scan so a stuck mount cannot block Next forever.
  QTimer::singleShot(90000, this, [this]() {
    if (!m_listSystemsProc || !m_listSystemsBusy) {
      return;
    }
    m_listSystemsProc->kill();
  });
}

void InstallWizard::onListSystemsFinished(int /*exitCode*/, QProcess::ExitStatus /*status*/)
{
  m_listSystemsBusy = false;
  QProcess *proc = m_listSystemsProc;
  m_listSystemsProc = nullptr;
  if (!proc || !m_backupSystemList) {
    if (proc) {
      proc->deleteLater();
    }
    return;
  }

  const QString out = QString::fromUtf8(proc->readAllStandardOutput());
  const QString err = QString::fromUtf8(proc->readAllStandardError()).trimmed();
  proc->deleteLater();

  m_backupSystemList->clear();
  for (const QString &line : out.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    if (line.startsWith(QLatin1String("systems=")) || line.startsWith(QLatin1Char('[')) ||
        line.startsWith(QLatin1String("ERROR")) || line.startsWith(QLatin1String("WARN"))) {
      continue;
    }
    const QStringList p = line.split(QLatin1Char('\t'));
    if (p.size() < 3) {
      continue;
    }
    auto *item = new QListWidgetItem(
        QStringLiteral("[%1] %2 — %3 (%4)")
            .arg(p.at(0), p.value(2), p.value(1), p.value(3)));
    item->setData(Qt::UserRole, p.value(1)); // partition path
    m_backupSystemList->addItem(item);
  }
  if (m_backupSystemList->count() == 0) {
    QString msg = QStringLiteral("(No systems found — check disks or uncheck backup)");
    if (!err.isEmpty()) {
      // Last non-empty stderr line is usually the scan summary.
      const QStringList elines = err.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
      msg += QStringLiteral("\n") + elines.last();
    }
    m_backupSystemList->addItem(msg);
  } else {
    m_backupSystemList->setCurrentRow(0);
  }
}

QString InstallWizard::runNmcli(const QStringList &args, int timeoutMs)
{
  QProcess proc;
  proc.start(QStringLiteral("nmcli"), args);
  if (!proc.waitForStarted(3000)) {
    return QStringLiteral("__ERR__nmcli not available");
  }
  if (!proc.waitForFinished(timeoutMs)) {
    proc.kill();
    return QStringLiteral("__ERR__nmcli timed out");
  }
  const QString out = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
  const QString err = QString::fromUtf8(proc.readAllStandardError()).trimmed();
  if (proc.exitCode() != 0) {
    return QStringLiteral("__ERR__%1").arg(err.isEmpty() ? out : err);
  }
  return out;
}

void InstallWizard::refreshWifiUi()
{
  if (!m_wifiList || !m_wifiStatus) {
    return;
  }
  m_wifiList->clear();

  // Prefer Ethernet: no need to force Wi‑Fi.
  const QString eth = runNmcli({QStringLiteral("-t"), QStringLiteral("-f"),
                                QStringLiteral("DEVICE,TYPE,STATE"), QStringLiteral("device")},
                               8000);
  if (!eth.startsWith(QLatin1String("__ERR__"))) {
    for (const QString &line : eth.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
      const QStringList p = line.split(QLatin1Char(':'));
      if (p.size() >= 3 && p.at(1) == QLatin1String("ethernet") &&
          p.at(2).contains(QLatin1String("connected"))) {
        m_wifiStatus->setText(
            QStringLiteral("Ethernet connected (%1). Wi‑Fi optional — Skip or Continue.")
                .arg(p.at(0)));
        m_state.wifiConnected = QStringLiteral("ethernet:%1").arg(p.at(0));
        break;
      }
    }
  }

  const QString radio = runNmcli({QStringLiteral("radio"), QStringLiteral("wifi")}, 5000);
  if (radio.startsWith(QLatin1String("__ERR__"))) {
    m_wifiStatus->setText(
        QStringLiteral("NetworkManager/nmcli unavailable (%1). Skip and continue offline.")
            .arg(radio.mid(7)));
    return;
  }
  if (radio.contains(QLatin1String("disabled"), Qt::CaseInsensitive)) {
    runNmcli({QStringLiteral("radio"), QStringLiteral("wifi"), QStringLiteral("on")}, 5000);
  }

  runNmcli({QStringLiteral("device"), QStringLiteral("wifi"), QStringLiteral("rescan")}, 15000);
  const QString list = runNmcli({QStringLiteral("-t"), QStringLiteral("-f"),
                                 QStringLiteral("SSID,SIGNAL,SECURITY,ACTIVE"),
                                 QStringLiteral("device"), QStringLiteral("wifi"),
                                 QStringLiteral("list")},
                                15000);
  if (list.startsWith(QLatin1String("__ERR__"))) {
    m_wifiStatus->setText(
        QStringLiteral("Wi‑Fi scan failed (%1). Skip to continue offline.").arg(list.mid(7)));
    return;
  }

  int added = 0;
  for (const QString &line : list.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    const QStringList p = line.split(QLatin1Char(':'));
    if (p.isEmpty()) {
      continue;
    }
    const QString ssid = p.value(0).trimmed();
    if (ssid.isEmpty() || ssid == QLatin1String("--")) {
      continue;
    }
    const QString signal = p.value(1);
    const QString security = p.value(2);
    const bool active = p.value(3).startsWith(QLatin1Char('y'), Qt::CaseInsensitive);
    auto *item = new QListWidgetItem(
        QStringLiteral("%1  (%2%)  %3%4")
            .arg(ssid, signal, security.isEmpty() ? QStringLiteral("open") : security,
                 active ? QStringLiteral("  · connected") : QString()));
    item->setData(Qt::UserRole, ssid);
    item->setData(Qt::UserRole + 1, !security.isEmpty() && security != QLatin1String("--"));
    m_wifiList->addItem(item);
    ++added;
    if (active) {
      m_wifiList->setCurrentItem(item);
      m_state.wifiSsid = ssid;
      m_state.wifiConnected = QStringLiteral("connected:%1").arg(ssid);
    }
  }
  if (added == 0) {
    m_wifiStatus->setText(
        QStringLiteral("No networks found. Move closer to the AP, Scan again, or Skip."));
  } else if (m_state.wifiConnected.startsWith(QLatin1String("ethernet:"))) {
    // keep ethernet status
  } else if (m_state.wifiConnected.startsWith(QLatin1String("connected:"))) {
    m_wifiStatus->setText(QStringLiteral("Connected to %1.").arg(m_state.wifiSsid));
  } else {
    m_wifiStatus->setText(
        QStringLiteral("Select a network, enter the password if needed, then Connect "
                       "— or Skip Wi‑Fi."));
  }
}

void InstallWizard::connectWifi()
{
  if (!m_wifiList || !m_wifiList->currentItem()) {
    QMessageBox::information(this, QStringLiteral("Wi‑Fi"),
                             QStringLiteral("Select a network from the list (or Skip)."));
    return;
  }
  const QString ssid = m_wifiList->currentItem()->data(Qt::UserRole).toString();
  const bool secured = m_wifiList->currentItem()->data(Qt::UserRole + 1).toBool();
  const QString password = m_wifiPassword ? m_wifiPassword->text() : QString();
  if (secured && password.isEmpty()) {
    QMessageBox::warning(this, QStringLiteral("Wi‑Fi"),
                         QStringLiteral("This network needs a password."));
    return;
  }
  m_wifiStatus->setText(QStringLiteral("Connecting to %1…").arg(ssid));
  QStringList args = {QStringLiteral("device"), QStringLiteral("wifi"),
                      QStringLiteral("connect"), ssid};
  if (!password.isEmpty()) {
    args << QStringLiteral("password") << password;
  }
  const QString result = runNmcli(args, 45000);
  if (result.startsWith(QLatin1String("__ERR__"))) {
    m_wifiStatus->setText(QStringLiteral("Connect failed: %1").arg(result.mid(7)));
    QMessageBox::warning(this, QStringLiteral("Wi‑Fi"),
                         QStringLiteral("Could not connect:\n%1").arg(result.mid(7)));
    return;
  }
  m_wifiSkipped = false;
  m_state.wifiSsid = ssid;
  m_state.wifiConnected = QStringLiteral("connected:%1").arg(ssid);
  m_wifiStatus->setText(QStringLiteral("Connected to %1.").arg(ssid));
  // Prefer a system-wide connection so install helper can copy credentials.
  runNmcli({QStringLiteral("connection"), QStringLiteral("modify"), ssid,
            QStringLiteral("connection.permissions"), QString()},
           8000);
  runNmcli({QStringLiteral("connection"), QStringLiteral("modify"), ssid,
            QStringLiteral("connection.autoconnect"), QStringLiteral("yes")},
           8000);
  refreshWifiUi();
}

void InstallWizard::applyPageToUi(int index)
{
  m_stepLabel->setText(QStringLiteral("<b>Step %1 of 10</b> — %2")
                           .arg(index + 1)
                           .arg(QString::fromUtf8(kStepTitles[index])));
  m_back->setEnabled(index > 0);
  m_stack->setCurrentIndex(index);

  if (index == 2) {
    m_wifiSkipped = false;
    refreshWifiUi();
  }
  if (index == 5) {
    m_variantHint->setText(
        QStringLiteral("Recommended for this machine: <b>%1</b>\n\n%2")
            .arg(m_state.variant.toUpper(), m_state.detectSummary));
    const int idx = m_variant->findData(m_state.variant);
    if (idx >= 0) {
      m_variant->setCurrentIndex(idx);
    }
  }
  if (index == 6) {
    refreshBackupUi();
  }
  if (index == 7) {
    m_diskList->clear();
    for (const QString &d : HardwareProbe::blockDevicesHuman()) {
      m_diskList->addItem(d);
    }
    if (m_diskList->count() > 0) {
      m_diskList->setCurrentRow(0);
    }
  }
  if (index == 8) {
    m_installStarted = false;
    m_installOk = false;
    m_progressBody->setPlainText(
        QStringLiteral("Ready to install.\n\n"
                       "This will partition, format, copy the live system, configure your "
                       "account, and install the bootloader on:\n  %1\n\n"
                       "Plan:\n%2\n\n"
                       "Click “Install Spike now”. Do not power off.")
            .arg(m_state.targetDisk, stateDump()));
    m_installBtn->setEnabled(true);
    m_installBtn->setVisible(true);
    m_next->setEnabled(false);
    m_next->setText(QStringLiteral("Continue"));
    m_back->setEnabled(true);
  }
  if (index == 9) {
    const QString backupLine =
        QStringLiteral("Backup: %1\n").arg(m_state.backupStatus.isEmpty()
                                               ? QStringLiteral("n/a")
                                               : m_state.backupStatus);
    const QString restoreLine =
        QStringLiteral("Restore: %1\n").arg(m_state.restoreStatus.isEmpty()
                                                ? QStringLiteral("n/a")
                                                : m_state.restoreStatus);
    m_finishBody->setPlainText(
        m_installOk
            ? QStringLiteral("Spike was installed on %1.\n\n"
                             "%2%3\n"
                             "Remove the live USB, then reboot into the installed system.\n\n"
                             "Log: /var/log/spike/install.log (on the live session).\n\n"
                             "Plan:\n%4")
                  .arg(m_state.targetDisk, backupLine, restoreLine, stateDump())
            : QStringLiteral("Install did not complete successfully.\n\n"
                             "%1%2\n"
                             "Check the Installation log and /var/log/spike/install.log.\n"
                             "You can go Back and retry, or reboot to the live desktop.\n\n"
                             "Plan:\n%3")
                  .arg(backupLine, restoreLine, stateDump()));
    m_next->setText(QStringLiteral("Close"));
    m_next->setEnabled(true);
  } else if (index < 8) {
    m_next->setText(QStringLiteral("Continue"));
    m_next->setEnabled(true);
  }
}

void InstallWizard::syncStateFromPage(int index)
{
  switch (index) {
  case 0:
    m_state.language = m_language->currentData().toString();
    break;
  case 1:
    m_state.timezone = m_timezone->currentText().trimmed();
    if (m_keyboard->currentData().isValid() && !m_keyboard->currentData().toString().isEmpty()) {
      m_state.keyboard = m_keyboard->currentData().toString();
    } else {
      // Editable combo may leave data empty — match by text or take typed id
      const QString typed = m_keyboard->currentText().trimmed();
      int idx = m_keyboard->findData(typed);
      if (idx < 0) {
        idx = m_keyboard->findText(typed);
      }
      if (idx >= 0) {
        m_state.keyboard = m_keyboard->itemData(idx).toString();
      } else if (!typed.isEmpty() && !typed.contains(QLatin1Char(' '))) {
        m_state.keyboard = typed;
      }
    }
    break;
  case 2:
    if (!m_wifiSkipped && m_wifiList && m_wifiList->currentItem()) {
      const QString ssid = m_wifiList->currentItem()->data(Qt::UserRole).toString();
      if (!ssid.isEmpty()) {
        m_state.wifiSsid = ssid;
      }
    }
    break;
  case 3:
    m_state.username = m_username->text().trimmed();
    m_state.password = m_password->text();
    m_state.autoLogin = m_autoLogin && m_autoLogin->isChecked();
    break;
  case 4:
    m_state.hostname = m_hostname->text().trimmed();
    break;
  case 5:
    m_state.variant = m_variant->currentData().toString();
    break;
  case 6:
    m_state.doBackup = m_doBackup->isChecked();
    m_state.restoreAfterInstall = m_restoreCheck->isChecked();
    if (m_backupSystemList && m_backupSystemList->currentItem()) {
      m_state.backupSystemPartition =
          m_backupSystemList->currentItem()->data(Qt::UserRole).toString();
    }
    if (m_backupDestList && m_backupDestList->currentItem()) {
      m_state.backupDestMount =
          m_backupDestList->currentItem()->data(Qt::UserRole).toString();
    }
    if (m_restoreList->currentItem()) {
      m_state.restoreSessionPath =
          m_restoreList->currentItem()->data(Qt::UserRole).toString();
    }
    break;
  case 7:
    if (m_diskList->currentItem()) {
      m_state.targetDisk = m_diskList->currentItem()->text().section(QLatin1Char(' '), 0, 0);
      if (!m_state.targetDisk.startsWith(QLatin1Char('/'))) {
        m_state.targetDisk = QStringLiteral("/dev/") + m_state.targetDisk;
      }
    }
    break;
  default:
    break;
  }
}

bool InstallWizard::validateCurrent()
{
  const int index = m_stack->currentIndex();
  if (index == 1) {
    const QString tz = m_timezone->currentText().trimmed();
    if (tz.isEmpty()) {
      QMessageBox::warning(this, QStringLiteral("Timezone"),
                           QStringLiteral("Select a timezone."));
      return false;
    }
    if (m_timezone->findText(tz) < 0) {
      QMessageBox::warning(
          this, QStringLiteral("Timezone"),
          QStringLiteral("Unknown timezone “%1”. Pick one from the list (type to filter).")
              .arg(tz));
      return false;
    }
  }
  if (index == 3) {
    if (m_username->text().trimmed().isEmpty()) {
      QMessageBox::warning(this, QStringLiteral("Account"),
                           QStringLiteral("Enter a username."));
      return false;
    }
    if (m_password->text().isEmpty() || m_password->text() != m_password2->text()) {
      QMessageBox::warning(this, QStringLiteral("Account"),
                           QStringLiteral("Passwords must match and not be empty."));
      return false;
    }
  }
  if (index == 4 && m_hostname->text().trimmed().isEmpty()) {
    QMessageBox::warning(this, QStringLiteral("Hostname"),
                         QStringLiteral("Enter a computer name."));
    return false;
  }
  if (index == 6 && m_doBackup && m_doBackup->isChecked()) {
    if (m_listSystemsBusy) {
      QMessageBox::information(this, QStringLiteral("Backup"),
                               QStringLiteral("Still scanning for systems — wait a moment, or "
                                              "uncheck backup to continue."));
      return false;
    }
    if (!m_backupSystemList || !m_backupSystemList->currentItem() ||
        m_backupSystemList->currentItem()->data(Qt::UserRole).toString().isEmpty()) {
      QMessageBox::warning(this, QStringLiteral("Backup"),
                           QStringLiteral("Select a system to back up (or uncheck backup)."));
      return false;
    }
    if (!m_backupDestList || !m_backupDestList->currentItem() ||
        m_backupDestList->currentItem()->data(Qt::UserRole).toString().isEmpty()) {
      QMessageBox::warning(this, QStringLiteral("Backup"),
                           QStringLiteral("Select a backup destination USB (or uncheck backup)."));
      return false;
    }
  }
  if (index == 6 && m_restoreCheck->isChecked()) {
    const bool hasSession = m_restoreList->currentItem() &&
                            !m_restoreList->currentItem()->data(Qt::UserRole).toString().isEmpty();
    const bool willCreateSession = m_doBackup && m_doBackup->isChecked();
    if (!hasSession && !willCreateSession) {
      QMessageBox::warning(this, QStringLiteral("Restore"),
                           QStringLiteral("Select a SpikeBackup session, enable backup "
                                          "(to create one), or uncheck restore."));
      return false;
    }
  }
  if (index == 7) {
    if (!m_diskList->currentItem()) {
      QMessageBox::warning(this, QStringLiteral("Storage"),
                           QStringLiteral("Select a disk."));
      return false;
    }
    if (!m_wipeConfirm->isChecked()) {
      QMessageBox::warning(this, QStringLiteral("Storage"),
                           QStringLiteral("Confirm you understand the disk will be erased."));
      return false;
    }
    if (m_eraseType->text().trimmed().compare(QStringLiteral("ERASE"), Qt::CaseInsensitive) != 0) {
      QMessageBox::warning(this, QStringLiteral("Storage"),
                           QStringLiteral("Type ERASE (any case) to continue."));
      return false;
    }
  }
  if (index == 8) {
    if (!m_installOk) {
      QMessageBox::information(this, QStringLiteral("Installation"),
                               QStringLiteral("Run “Install Spike now” and wait for it to finish."));
      return false;
    }
  }
  return true;
}

QString InstallWizard::stateDump() const
{
  QStringList lines;
  lines << QStringLiteral("language=%1").arg(m_state.language);
  lines << QStringLiteral("timezone=%1").arg(m_state.timezone);
  lines << QStringLiteral("keyboard=%1").arg(m_state.keyboard);
  lines << QStringLiteral("wifi=%1").arg(m_state.wifiConnected.isEmpty()
                                             ? (m_state.wifiSsid.isEmpty()
                                                    ? QStringLiteral("none")
                                                    : m_state.wifiSsid)
                                             : m_state.wifiConnected);
  lines << QStringLiteral("username=%1").arg(m_state.username);
  lines << QStringLiteral("hostname=%1").arg(m_state.hostname);
  lines << QStringLiteral("variant=%1").arg(m_state.variant);
  lines << QStringLiteral("autoLogin=%1").arg(m_state.autoLogin ? QStringLiteral("yes")
                                                                  : QStringLiteral("no"));
  lines << QStringLiteral("doBackup=%1").arg(m_state.doBackup ? QStringLiteral("yes")
                                                              : QStringLiteral("no"));
  lines << QStringLiteral("backupDest=%1").arg(m_state.backupDestMount);
  lines << QStringLiteral("backupSystem=%1").arg(m_state.backupSystemPartition);
  lines << QStringLiteral("backupStatus=%1").arg(m_state.backupStatus);
  lines << QStringLiteral("restoreAfterInstall=%1")
               .arg(m_state.restoreAfterInstall ? QStringLiteral("yes") : QStringLiteral("no"));
  lines << QStringLiteral("restoreSession=%1").arg(m_state.restoreSessionPath);
  lines << QStringLiteral("restoreStatus=%1").arg(m_state.restoreStatus);
  lines << QStringLiteral("targetDisk=%1").arg(m_state.targetDisk);
  lines << QStringLiteral("passwordSet=%1").arg(m_state.password.isEmpty() ? QStringLiteral("no")
                                                                            : QStringLiteral("yes"));
  return lines.join(QLatin1Char('\n'));
}

void InstallWizard::goBack()
{
  const int index = m_stack->currentIndex();
  if (index <= 0) {
    return;
  }
  if (index == 8 && m_engine && m_engine->isRunning()) {
    return;
  }
  applyPageToUi(index - 1);
}

void InstallWizard::goNext()
{
  const int index = m_stack->currentIndex();
  if (index >= 9) {
    onFinishClose();
    return;
  }
  if (!validateCurrent()) {
    return;
  }
  syncStateFromPage(index);
  applyPageToUi(index + 1);
}

void InstallWizard::startInstall()
{
  if (m_engine && m_engine->isRunning()) {
    return;
  }
  syncStateFromPage(7);
  if (m_state.targetDisk.isEmpty()) {
    QMessageBox::warning(this, QStringLiteral("Install"),
                         QStringLiteral("No target disk selected."));
    return;
  }
  const auto reply = QMessageBox::question(
      this, QStringLiteral("Erase and install?"),
      QStringLiteral("This will PERMANENTLY ERASE %1 and install Spike.\n\nContinue?")
          .arg(m_state.targetDisk),
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (reply != QMessageBox::Yes) {
    return;
  }
  m_installStarted = true;
  m_installOk = false;
  m_installBtn->setEnabled(false);
  m_back->setEnabled(false);
  m_next->setEnabled(false);
  m_progressBody->append(QStringLiteral("\n--- install started ---\n"));
  m_engine->start(m_state);
}

void InstallWizard::onInstallLog(const QString &line)
{
  m_progressBody->append(line);
}

void InstallWizard::onInstallFinished(bool ok, const QString &message)
{
  m_installOk = ok;
  if (m_engine) {
    const InstallState st = m_engine->lastState();
    m_state.backupStatus = st.backupStatus;
    m_state.restoreStatus = st.restoreStatus;
    m_state.restoreSessionPath = st.restoreSessionPath;
  }
  m_progressBody->append(QStringLiteral("\n%1\n").arg(message));
  m_installBtn->setEnabled(!ok);
  m_back->setEnabled(true);
  m_next->setEnabled(ok);
  if (ok) {
    m_next->setText(QStringLiteral("Finish"));
  }
}

void InstallWizard::onFinishClose()
{
  close();
}

void InstallWizard::selectTimezone(const QString &tz)
{
  if (!m_timezone || tz.isEmpty()) {
    return;
  }
  int idx = m_timezone->findText(tz);
  if (idx < 0) {
    // Prefer exact zone if present under another casing
    for (int i = 0; i < m_timezone->count(); ++i) {
      if (m_timezone->itemText(i).compare(tz, Qt::CaseInsensitive) == 0) {
        idx = i;
        break;
      }
    }
  }
  if (idx >= 0) {
    m_timezone->setCurrentIndex(idx);
  } else {
    m_timezone->setEditText(tz);
  }
}

void InstallWizard::selectKeyboard(const QString &layoutId)
{
  if (!m_keyboard || layoutId.isEmpty()) {
    return;
  }
  int idx = m_keyboard->findData(layoutId);
  if (idx >= 0) {
    m_keyboard->setCurrentIndex(idx);
    return;
  }
  // Rare layout id not in list — show raw id
  m_keyboard->setEditText(layoutId);
}

void InstallWizard::onLanguageChanged()
{
  if (!m_language) {
    return;
  }
  const QString lang = m_language->currentData().toString();
  m_state.language = lang;

  // Suggest timezone + keyboard from language (keyboard only if user hasn't overridden)
  if (m_timezone) {
    selectTimezone(suggestTimezoneForLanguage(lang));
  }
  if (!m_keyboardTouched) {
    selectKeyboard(suggestKeyboardForLanguage(lang));
  }
}

void InstallWizard::onTimezoneChanged(const QString &tz)
{
  if (m_keyboardTouched || tz.trimmed().isEmpty()) {
    return;
  }
  const QString kb = suggestKeyboardForTimezone(tz.trimmed());
  if (!kb.isEmpty()) {
    selectKeyboard(kb);
  }
}

} // namespace spike
