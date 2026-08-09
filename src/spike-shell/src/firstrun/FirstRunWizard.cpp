#include "firstrun/FirstRunWizard.hpp"

#include "firstrun/FirstRunHooks.hpp"
#include "network/NmClient.hpp"

#include <QComboBox>
#include <QCompleter>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSet>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

namespace spike {

namespace {

enum FirstRunPage : int {
  PageWelcome = 0,
  PageTimezone,
  PageWifi,
  PageTour,
  PageAccessibility,
  PageImport,
  PageNotices,
  PageDone,
  PageCount
};

} // namespace

FirstRunWizard::FirstRunWizard(QWidget *parent)
    : QDialog(parent)
{
  setWindowTitle(QStringLiteral("Welcome to Spike"));
  setModal(true);
  resize(560, 480);

  m_nm = new NmClient(this);

  auto *root = new QVBoxLayout(this);
  m_stepLabel = new QLabel(this);
  root->addWidget(m_stepLabel);

  m_stack = new QStackedWidget(this);
  root->addWidget(m_stack, 1);
  buildPages();

  auto *nav = new QHBoxLayout();
  m_back = new QPushButton(QStringLiteral("Back"), this);
  m_skip = new QPushButton(QStringLiteral("Skip"), this);
  m_next = new QPushButton(QStringLiteral("Continue"), this);
  nav->addWidget(m_back);
  nav->addStretch(1);
  nav->addWidget(m_skip);
  nav->addWidget(m_next);
  root->addLayout(nav);

  connect(m_back, &QPushButton::clicked, this, &FirstRunWizard::goBack);
  connect(m_next, &QPushButton::clicked, this, &FirstRunWizard::goNext);
  connect(m_skip, &QPushButton::clicked, this, &FirstRunWizard::goNext);

  updateNav();
}

void FirstRunWizard::buildPages()
{
  // 0 Welcome
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Welcome to Spike!</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral(
            "Your computer is set up and ready to use.\n\n"
            "Here are a few things to get started:\n\n"
            "• Install apps — Click the Spike button and search, or open Discover\n"
            "• Browse the web — Firefox is ready to go\n"
            "• Change settings — Open Settings from the Spike menu\n"
            "• Need help? — Press F1 in Settings for the built-in guide"),
        page));
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 1 Timezone
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Confirm timezone</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Confirm or change your timezone (type to filter)."), page));
    m_timezone = new QComboBox(page);
    m_timezone->setEditable(true);
    m_timezone->setInsertPolicy(QComboBox::NoInsert);
    m_timezone->addItems(loadTimeZones());
    if (auto *c = m_timezone->completer()) {
      c->setFilterMode(Qt::MatchContains);
      c->setCaseSensitivity(Qt::CaseInsensitive);
    }
    const QString cur = currentTimezone();
    int idx = m_timezone->findText(cur);
    if (idx >= 0) {
      m_timezone->setCurrentIndex(idx);
    } else if (!cur.isEmpty()) {
      m_timezone->setEditText(cur);
    }
    lay->addWidget(m_timezone);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 2 Wi‑Fi
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Wi‑Fi</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Connect if you skipped Wi‑Fi during install. Skip is fine offline."),
        page));
    m_wifiStatus = new QLabel(page);
    m_wifiStatus->setWordWrap(true);
    lay->addWidget(m_wifiStatus);
    m_wifiList = new QListWidget(page);
    lay->addWidget(m_wifiList, 1);
    m_wifiPassword = new QLineEdit(page);
    m_wifiPassword->setEchoMode(QLineEdit::Password);
    m_wifiPassword->setPlaceholderText(QStringLiteral("Password (if required)"));
    lay->addWidget(m_wifiPassword);
    auto *row = new QHBoxLayout();
    auto *scan = new QPushButton(QStringLiteral("Scan"), page);
    auto *conn = new QPushButton(QStringLiteral("Connect"), page);
    row->addWidget(scan);
    row->addWidget(conn);
    lay->addLayout(row);
    connect(scan, &QPushButton::clicked, this, &FirstRunWizard::onWifiScan);
    connect(conn, &QPushButton::clicked, this, &FirstRunWizard::onWifiConnect);

    QString err;
    const QString status = m_nm->statusLabel(&err);
    if (!err.isEmpty() && !m_nm->isAvailable()) {
      m_wifiStatus->setText(QStringLiteral("NetworkManager not available — Skip to continue."));
    } else {
      m_wifiStatus->setText(QStringLiteral("Current: %1").arg(status));
    }
    m_stack->addWidget(page);
  }

  // 3 Tour (placeholder)
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Desktop tour</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("<i>Placeholder — spotlight tour drops in later via "
                       "firstrun::runDesktopTour.</i>"),
        page));
    lay->addWidget(new QLabel(
        QStringLiteral(
            "Quick tips for now:\n\n"
            "• Spike menu (bottom-left / panel) — launch apps\n"
            "• Settings — gear in the menu or tray\n"
            "• Panel tray — network, volume, battery, updates"),
        page));
    auto *tourBtn = new QPushButton(QStringLiteral("Try tour hook…"), page);
    connect(tourBtn, &QPushButton::clicked, this, [this]() { firstrun::runDesktopTour(this); });
    lay->addWidget(tourBtn);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 4 Accessibility (placeholder offer)
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Accessibility</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral(
            "Want to set up accessibility options?\n\n"
            "Full Quick Setup Wizard (ACCESSIBILITY.md) is a placeholder — "
            "Continue opens Settings → Accessibility if available."),
        page));
    auto *openBtn = new QPushButton(QStringLiteral("Open Accessibility settings"), page);
    connect(openBtn, &QPushButton::clicked, this, &FirstRunWizard::onOpenAccessibility);
    lay->addWidget(openBtn);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 5 Import files
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Import my files</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral(
            "Optional: open Move My Files to import a SpikeBackup session into this home.\n"
            "Skip if you already restored during install or have nothing to import."),
        page));
    m_importStatus = new QLabel(page);
    m_importStatus->setWordWrap(true);
    lay->addWidget(m_importStatus);
    auto *importBtn = new QPushButton(QStringLiteral("Open Move My Files"), page);
    connect(importBtn, &QPushButton::clicked, this, &FirstRunWizard::onImportFiles);
    lay->addWidget(importBtn);
    lay->addStretch(1);
    m_stack->addWidget(page);
  }

  // 6 Notices
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>Post-install notices</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("NVIDIA / HDD / updates / Flatpak checks drop into "
                       "/var/lib/spike/installer-notifications/ and the hooks below."),
        page));
    m_notices = new QTextEdit(page);
    m_notices->setReadOnly(true);
    lay->addWidget(m_notices, 1);
    m_stack->addWidget(page);
  }

  // 7 Done
  {
    auto *page = new QWidget;
    auto *lay = new QVBoxLayout(page);
    lay->addWidget(new QLabel(QStringLiteral("<h2>You're ready</h2>"), page));
    lay->addWidget(new QLabel(
        QStringLiteral("Click Get started to finish first-run setup and use your desktop."),
        page));
    lay->addStretch(1);
    m_stack->addWidget(page);
  }
}

QStringList FirstRunWizard::loadTimeZones()
{
  QSet<QString> zones;
  zones.insert(QStringLiteral("UTC"));
  QFile f(QStringLiteral("/usr/share/zoneinfo/zone.tab"));
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&f);
    while (!in.atEnd()) {
      const QString line = in.readLine().trimmed();
      if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
        continue;
      }
      const QStringList parts = line.split(QLatin1Char('\t'));
      if (parts.size() >= 3) {
        const QString tz = parts.at(2).trimmed();
        if (!tz.isEmpty() && tz.contains(QLatin1Char('/'))) {
          zones.insert(tz);
        }
      }
    }
  }
  QStringList out = zones.values();
  out.sort(Qt::CaseInsensitive);
  out.removeAll(QStringLiteral("UTC"));
  out.prepend(QStringLiteral("UTC"));
  return out;
}

QString FirstRunWizard::currentTimezone()
{
  QFile f(QStringLiteral("/etc/timezone"));
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return QString::fromUtf8(f.readAll()).trimmed();
  }
  QProcess p;
  p.start(QStringLiteral("timedatectl"), {QStringLiteral("show"), QStringLiteral("-p"),
                                          QStringLiteral("Timezone"), QStringLiteral("--value")});
  if (p.waitForFinished(3000) && p.exitCode() == 0) {
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
  }
  return QStringLiteral("UTC");
}

void FirstRunWizard::applyTimezone()
{
  const QString tz = m_timezone->currentText().trimmed();
  if (tz.isEmpty()) {
    return;
  }
  QProcess p;
  p.start(QStringLiteral("timedatectl"), {QStringLiteral("set-timezone"), tz});
  if (!p.waitForFinished(8000) || p.exitCode() != 0) {
    // Best-effort without polkit — installer already set TZ; user can fix in Settings.
    QMessageBox::information(
        this, QStringLiteral("Timezone"),
        QStringLiteral("Could not apply timezone now (may need admin rights).\n"
                       "You can change it later in Settings → Date & Time.\n\n"
                       "Selected: %1")
            .arg(tz));
  }
}

void FirstRunWizard::refreshNoticesPage()
{
  QStringList lines;
  const QStringList notices = firstrun::collectPostInstallNotices();
  if (notices.isEmpty()) {
    lines << QStringLiteral("No special notices in /var/lib/spike/installer-notifications/.");
  } else {
    lines << QStringLiteral("Installer notices:");
    for (const QString &n : notices) {
      lines << QStringLiteral("———");
      lines << n;
    }
  }

  QString flatpakMsg;
  firstrun::verifyFlatpakRuntimes(&flatpakMsg);
  lines << QStringLiteral("\n") << flatpakMsg;

  QString updatesMsg;
  firstrun::checkSecurityUpdates(&updatesMsg);
  lines << updatesMsg;

  m_notices->setPlainText(lines.join(QLatin1Char('\n')));
}

void FirstRunWizard::updateNav()
{
  const int i = m_stack->currentIndex();
  m_stepLabel->setText(QStringLiteral("<b>First run — step %1 of %2</b>")
                           .arg(i + 1)
                           .arg(PageCount));
  m_back->setEnabled(i > 0);
  const bool skippable = (i == PageWifi || i == PageTour || i == PageAccessibility ||
                          i == PageImport);
  m_skip->setVisible(skippable);
  if (i == PageDone) {
    m_next->setText(QStringLiteral("Get started"));
  } else {
    m_next->setText(QStringLiteral("Continue"));
  }
  if (i == PageNotices) {
    refreshNoticesPage();
  }
}

void FirstRunWizard::goBack()
{
  const int i = m_stack->currentIndex();
  if (i > 0) {
    m_stack->setCurrentIndex(i - 1);
    updateNav();
  }
}

void FirstRunWizard::goNext()
{
  const int i = m_stack->currentIndex();
  if (i == PageTimezone) {
    applyTimezone();
  }
  if (i >= PageDone) {
    finishAndClose();
    return;
  }
  m_stack->setCurrentIndex(i + 1);
  updateNav();
}

void FirstRunWizard::onWifiScan()
{
  QString err;
  if (!m_nm->isAvailable(&err)) {
    m_wifiStatus->setText(QStringLiteral("NetworkManager unavailable: %1").arg(err));
    return;
  }
  const QString wifi = m_nm->wifiDevicePath(&err);
  if (wifi.isEmpty()) {
    m_wifiStatus->setText(QStringLiteral("No Wi‑Fi device: %1").arg(err));
    return;
  }
  m_nm->requestScan(wifi, &err);
  m_wifiList->clear();
  const auto aps = m_nm->accessPoints(wifi, &err);
  for (const auto &ap : aps) {
    auto *item = new QListWidgetItem(
        QStringLiteral("%1  (%2%)%3")
            .arg(ap.ssid)
            .arg(ap.strength)
            .arg(ap.secured ? QStringLiteral(" · secured") : QString()));
    item->setData(Qt::UserRole, ap.ssid);
    item->setData(Qt::UserRole + 1, ap.secured);
    m_wifiList->addItem(item);
  }
  m_wifiStatus->setText(QStringLiteral("Found %1 networks.").arg(aps.size()));
}

void FirstRunWizard::onWifiConnect()
{
  if (!m_wifiList->currentItem()) {
    QMessageBox::information(this, QStringLiteral("Wi‑Fi"),
                             QStringLiteral("Select a network first."));
    return;
  }
  const QString ssid = m_wifiList->currentItem()->data(Qt::UserRole).toString();
  const bool secured = m_wifiList->currentItem()->data(Qt::UserRole + 1).toBool();
  QString err;
  if (!m_nm->connectToSsid(ssid, m_wifiPassword->text(), secured, &err)) {
    m_wifiStatus->setText(QStringLiteral("Connect failed: %1").arg(err));
    return;
  }
  m_wifiStatus->setText(QStringLiteral("Connected to %1.").arg(ssid));
}

void FirstRunWizard::onImportFiles()
{
  if (QProcess::startDetached(QStringLiteral("spike-migration"), {})) {
    m_importStatus->setText(QStringLiteral("Opened Move My Files — finish there, then Continue."));
  } else {
    m_importStatus->setText(
        QStringLiteral("spike-migration not found on PATH — install spike-migration or Skip."));
  }
}

void FirstRunWizard::onOpenAccessibility()
{
  if (firstrun::offerAccessibilityWizard(this)) {
    return;
  }
  QMessageBox::information(
      this, QStringLiteral("Accessibility"),
      QStringLiteral("Could not open Settings yet.\n"
                     "Use Spike menu → Settings → Accessibility after Get started.\n\n"
                     "Full Quick Setup Wizard is a placeholder "
                     "(firstrun::offerAccessibilityWizard)."));
}

void FirstRunWizard::finishAndClose()
{
  QString err;
  if (!firstrun::markFirstRunCompleted(&err)) {
    QMessageBox::warning(this, QStringLiteral("First run"),
                         QStringLiteral("Could not save completion flag:\n%1\n"
                                        "Wizard may show again next login.")
                             .arg(err));
  }
  accept();
}

} // namespace spike
