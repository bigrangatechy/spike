#include "InstallWizard.hpp"

#include "InstallEngine.hpp"
#include "detect/Detect.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace spike {

namespace {

const char *const kStepTitles[] = {
    "Welcome + language",
    "Timezone",
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
    m_language->addItem(QStringLiteral("English (United States)"), QStringLiteral("en_US"));
    m_language->addItem(QStringLiteral("English (United Kingdom)"), QStringLiteral("en_GB"));
    m_language->addItem(QStringLiteral("Français"), QStringLiteral("fr_FR"));
    m_language->addItem(QStringLiteral("Deutsch"), QStringLiteral("de_DE"));
    m_language->addItem(QStringLiteral("Español"), QStringLiteral("es_ES"));
    lay->addWidget(m_language);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 1 Timezone
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Timezone</h2>"), page));
    m_timezone = new QComboBox(page);
    m_timezone->addItems({QStringLiteral("UTC"), QStringLiteral("Australia/Sydney"),
                          QStringLiteral("America/New_York"), QStringLiteral("Europe/London"),
                          QStringLiteral("Europe/Berlin"), QStringLiteral("Asia/Tokyo")});
    lay->addWidget(m_timezone);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

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
        QStringLiteral("Step 7 will copy personal files to SpikeBackup/ on a USB before wipe. "
                       "Backup engine is stubbed in this pre-alpha build — you can still mark "
                       "the choice and pick a restore source for after install."),
        page));
    m_doBackup = new QCheckBox(QStringLiteral("Back up files from this computer before install"),
                               page);
    lay->addWidget(m_doBackup);
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

void InstallWizard::refreshBackupUi()
{
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
    m_finishBody->setPlainText(
        m_installOk
            ? QStringLiteral("Spike was installed on %1.\n\n"
                             "Remove the live USB, then reboot into the installed system.\n\n"
                             "Log: /var/log/spike/install.log (on the live session).\n\n"
                             "Plan:\n%2")
                  .arg(m_state.targetDisk, stateDump())
            : QStringLiteral("Install did not complete successfully.\n\n"
                             "Check the Installation log and /var/log/spike/install.log.\n"
                             "You can go Back and retry, or reboot to the live desktop.\n\n"
                             "Plan:\n%1")
                  .arg(stateDump()));
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
    m_state.timezone = m_timezone->currentText();
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
  if (index == 6 && m_restoreCheck->isChecked()) {
    if (!m_restoreList->currentItem() ||
        m_restoreList->currentItem()->data(Qt::UserRole).toString().isEmpty()) {
      QMessageBox::warning(this, QStringLiteral("Restore"),
                           QStringLiteral("Select a SpikeBackup session, or uncheck restore."));
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
  lines << QStringLiteral("wifi=%1").arg(m_state.wifiConnected.isEmpty()
                                             ? (m_state.wifiSsid.isEmpty()
                                                    ? QStringLiteral("none")
                                                    : m_state.wifiSsid)
                                             : m_state.wifiConnected);
  lines << QStringLiteral("username=%1").arg(m_state.username);
  lines << QStringLiteral("hostname=%1").arg(m_state.hostname);
  lines << QStringLiteral("variant=%1").arg(m_state.variant);
  lines << QStringLiteral("doBackup=%1").arg(m_state.doBackup ? QStringLiteral("yes")
                                                              : QStringLiteral("no"));
  lines << QStringLiteral("restoreAfterInstall=%1")
               .arg(m_state.restoreAfterInstall ? QStringLiteral("yes") : QStringLiteral("no"));
  lines << QStringLiteral("restoreSession=%1").arg(m_state.restoreSessionPath);
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

} // namespace spike
