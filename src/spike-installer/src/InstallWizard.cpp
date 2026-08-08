#include "InstallWizard.hpp"

#include "detect/Detect.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
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
        QStringLiteral("Full NetworkManager UI lands next. For now you can continue offline "
                       "or connect Wi-Fi from the shell panel, then continue."),
        page));
    m_wifiStatus = new QLabel(page);
    m_wifiStatus->setWordWrap(true);
    lay->addWidget(m_wifiStatus);
    lay->addStretch(1);
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
        QStringLiteral("Spike will erase the selected disk. No dual-boot, no manual partitions "
                       "(INSTALLER.md). This build does not wipe yet — confirmation only."),
        page));
    m_diskList = new QListWidget(page);
    lay->addWidget(m_diskList, 1);
    m_wipeConfirm = new QCheckBox(
        QStringLiteral("I understand the selected disk will be erased when install is implemented"),
        page);
    lay->addWidget(m_wipeConfirm);
    m_stack->addWidget(page);
  }

  // 8 Progress (stub)
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Installation</h2>"), page));
    m_progressBody = new QTextEdit(page);
    m_progressBody->setReadOnly(true);
    lay->addWidget(m_progressBody, 1);
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

void InstallWizard::applyPageToUi(int index)
{
  m_stepLabel->setText(QStringLiteral("<b>Step %1 of 10</b> — %2")
                           .arg(index + 1)
                           .arg(QString::fromUtf8(kStepTitles[index])));
  m_back->setEnabled(index > 0);
  m_stack->setCurrentIndex(index);

  if (index == 2) {
    m_wifiStatus->setText(
        QStringLiteral("Detect summary (for developers):\n%1").arg(m_state.detectSummary));
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
    m_progressBody->setPlainText(
        QStringLiteral("Pre-alpha: the disk wipe / squashfs copy engine is not implemented yet.\n"
                       "Nothing will be erased by this build.\n\n"
                       "Collected install plan:\n\n%1")
            .arg(stateDump()));
    m_next->setText(QStringLiteral("Continue"));
  }
  if (index == 9) {
    m_finishBody->setPlainText(
        QStringLiteral("Installer UI skeleton is working.\n\n"
                       "Next engineering steps: real Wi-Fi page, Step 7 backup via spike-common, "
                       "partition + unpack, bootloader, then Layer 4 restore into the new home.\n\n"
                       "Plan snapshot:\n\n%1")
            .arg(stateDump()));
    m_next->setText(QStringLiteral("Close"));
  } else if (index < 8) {
    m_next->setText(QStringLiteral("Continue"));
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
  }
  return true;
}

QString InstallWizard::stateDump() const
{
  QStringList lines;
  lines << QStringLiteral("language=%1").arg(m_state.language);
  lines << QStringLiteral("timezone=%1").arg(m_state.timezone);
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

void InstallWizard::onFinishClose()
{
  close();
}

} // namespace spike
