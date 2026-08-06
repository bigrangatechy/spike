#include "settings/KcmHost.hpp"
#include "settings/SettingsWindow.hpp"

#include "settings/ConfigClient.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString categoryHeader(const QString &id)
{
  return id;
}

} // namespace

SettingsWindow::SettingsWindow(ConfigClient *config, QWidget *parent)
  : QMainWindow(parent)
  , m_config(config)
{
  setObjectName(QStringLiteral("SpikeSettings"));
  setWindowTitle(QStringLiteral("Settings"));
  resize(900, 600);

  auto *central = new QWidget(this);
  setCentralWidget(central);
  auto *root = new QVBoxLayout(central);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(8);

  auto *top = new QHBoxLayout();
  m_search = new QLineEdit(this);
  m_search->setPlaceholderText(QStringLiteral("Search settings…"));
  top->addWidget(m_search, 1);

  auto *help = new QPushButton(QStringLiteral("?"), this);
  help->setFixedWidth(32);
  help->setToolTip(QStringLiteral("Help (user guide — coming soon)"));
  top->addWidget(help);
  root->addLayout(top);

  auto *split = new QSplitter(Qt::Horizontal, this);

  auto *left = new QWidget(split);
  auto *leftLay = new QVBoxLayout(left);
  leftLay->setContentsMargins(0, 0, 0, 0);
  m_nav = new QListWidget(left);
  m_nav->setObjectName(QStringLiteral("SettingsNav"));
  leftLay->addWidget(m_nav, 1);
  m_showAdvanced = new QCheckBox(QStringLiteral("Show Advanced"), left);
  leftLay->addWidget(m_showAdvanced);
  split->addWidget(left);

  auto *right = new QWidget(split);
  auto *rightLay = new QVBoxLayout(right);
  rightLay->setContentsMargins(0, 0, 0, 0);
  m_stack = new QStackedWidget(right);
  rightLay->addWidget(m_stack, 1);
  m_status = new QLabel(right);
  m_status->setWordWrap(true);
  rightLay->addWidget(m_status);
  split->addWidget(right);
  split->setStretchFactor(1, 1);
  split->setSizes({220, 680});
  root->addWidget(split, 1);

  buildPages();
  rebuildNav();

  connect(m_nav, &QListWidget::currentRowChanged, this, &SettingsWindow::onNavChanged);
  connect(m_search, &QLineEdit::textChanged, this, &SettingsWindow::onSearchChanged);
  connect(m_showAdvanced, &QCheckBox::toggled, this, &SettingsWindow::onAdvancedToggled);
  connect(help, &QPushButton::clicked, this, &SettingsWindow::onHelpClicked);

  if (m_nav->count() > 0) {
    m_nav->setCurrentRow(0);
  }
}

void SettingsWindow::buildPages()
{
  m_pages = {
      // Personal
      {QStringLiteral("appearance"), QStringLiteral("Appearance"), QStringLiteral("PERSONAL"),
       QStringLiteral("theme panel wallpaper font icons"), false, false, {}},
      {QStringLiteral("notifications"), QStringLiteral("Notifications"), QStringLiteral("PERSONAL"),
       QStringLiteral("dnd history sound"), false, false, {}},
      {QStringLiteral("keyboard-layout"), QStringLiteral("Keyboard Layout"), QStringLiteral("PERSONAL"),
       QStringLiteral("layout switching"), false, false, {}},
      {QStringLiteral("language"), QStringLiteral("Language"), QStringLiteral("PERSONAL"),
       QStringLiteral("locale language region"), false, true, QStringLiteral("kcm_regionandlang")},
      // Hardware
      {QStringLiteral("display"), QStringLiteral("Display"), QStringLiteral("HARDWARE"),
       QStringLiteral("resolution refresh scaling brightness"), false, true,
       QStringLiteral("kcm_kscreen")},
      {QStringLiteral("sound"), QStringLiteral("Sound"), QStringLiteral("HARDWARE"),
       QStringLiteral("volume audio output input"), false, true, QStringLiteral("kcm_pulseaudio")},
      {QStringLiteral("power"), QStringLiteral("Power"), QStringLiteral("HARDWARE"),
       QStringLiteral("battery suspend lid"), false, true, QStringLiteral("kcm_powerdevilprofilesconfig")},
      {QStringLiteral("keyboard"), QStringLiteral("Keyboard"), QStringLiteral("HARDWARE"),
       QStringLiteral("repeat shortcuts"), false, true, QStringLiteral("kcm_keyboard")},
      {QStringLiteral("mouse"), QStringLiteral("Mouse/Touchpad"), QStringLiteral("HARDWARE"),
       QStringLiteral("pointer touchpad tapping"), false, true, QStringLiteral("kcm_touchpad")},
      {QStringLiteral("bluetooth"), QStringLiteral("Bluetooth"), QStringLiteral("HARDWARE"),
       QStringLiteral("bt devices pairing"), false, true, QStringLiteral("kcm_bluetooth")},
      {QStringLiteral("printer"), QStringLiteral("Printer"), QStringLiteral("HARDWARE"),
       QStringLiteral("print queue"), false, true, QStringLiteral("kcm_printer_manager")},
      // Network
      {QStringLiteral("network"), QStringLiteral("Network"), QStringLiteral("NETWORK"),
       QStringLiteral("wifi ethernet hotspot"), false, true, QStringLiteral("kcm_networkmanagement")},
      {QStringLiteral("vpn"), QStringLiteral("VPN"), QStringLiteral("NETWORK"),
       QStringLiteral("openvpn wireguard"), false, true, QStringLiteral("kcm_networkmanagement")},
      // System
      {QStringLiteral("users"), QStringLiteral("Users"), QStringLiteral("SYSTEM"),
       QStringLiteral("account password"), false, true, QStringLiteral("kcm_users")},
      {QStringLiteral("datetime"), QStringLiteral("Date & Time"), QStringLiteral("SYSTEM"),
       QStringLiteral("timezone ntp clock"), false, true, QStringLiteral("kcm_clock")},
      {QStringLiteral("accessibility"), QStringLiteral("Accessibility"), QStringLiteral("SYSTEM"),
       QStringLiteral("magnifier sticky keys"), false, true, QStringLiteral("kcm_access")},
      {QStringLiteral("software-sources"), QStringLiteral("Software Sources"), QStringLiteral("SYSTEM"),
       QStringLiteral("repos drivers nvidia"), false, false, {}},
      {QStringLiteral("about"), QStringLiteral("About"), QStringLiteral("SYSTEM"),
       QStringLiteral("version system info guide"), false, false, {}},
      // Advanced
      {QStringLiteral("memory"), QStringLiteral("Memory"), QStringLiteral("ADVANCED"),
       QStringLiteral("zram swap swappiness earlyoom"), true, false, {}},
      {QStringLiteral("boot"), QStringLiteral("Boot"), QStringLiteral("ADVANCED"),
       QStringLiteral("grub timeout boot counter"), true, false, {}},
      {QStringLiteral("kernel-modules"), QStringLiteral("Kernel Modules"), QStringLiteral("ADVANCED"),
       QStringLiteral("blacklist modules"), true, false, {}},
      {QStringLiteral("updates"), QStringLiteral("Updates"), QStringLiteral("ADVANCED"),
       QStringLiteral("schedule security"), true, false, {}},
      {QStringLiteral("storage"), QStringLiteral("Storage"), QStringLiteral("ADVANCED"),
       QStringLiteral("disk smart mount"), true, false, {}},
      {QStringLiteral("diagnostics"), QStringLiteral("Diagnostics"), QStringLiteral("ADVANCED"),
       QStringLiteral("logs hardware report"), true, false, {}},
  };

  for (const PageDef &page : m_pages) {
    QWidget *w = nullptr;
    if (page.id == QLatin1String("about")) {
      w = makeAboutPage();
    } else if (page.id == QLatin1String("memory")) {
      w = makeMemoryPage();
    } else if (page.isKcm) {
      w = makeKcmPage(page);
    } else {
      w = makePlaceholder(page);
    }
    w->setProperty("pageId", page.id);
    m_stack->addWidget(w);
  }
}

QWidget *SettingsWindow::makeKcmPage(const PageDef &page)
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->setContentsMargins(0, 0, 0, 0);
  auto *title = new QLabel(QStringLiteral("<h2>%1</h2>").arg(page.title), w);
  lay->addWidget(title);
  auto *host = new KcmHost(w);
  host->setObjectName(QStringLiteral("KcmHost"));
  host->setProperty("kcmPlugin", page.kcmPlugin);
  // Lazy-load on first show via refreshCurrentPage.
  lay->addWidget(host, 1);
  return w;
}

QWidget *SettingsWindow::makePlaceholder(const PageDef &page)
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  auto *title = new QLabel(QStringLiteral("<h2>%1</h2>").arg(page.title), w);
  lay->addWidget(title);
  auto *info = new QLabel(
      QStringLiteral("Spike custom page — controls will talk to org.spike.Config over D-Bus.\n"
                     "Placeholder for this milestone."),
      w);
  info->setWordWrap(true);
  lay->addWidget(info);
  lay->addStretch(1);
  return w;
}

QWidget *SettingsWindow::makeAboutPage()
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>About</h2>"), w));
  auto *body = new QTextEdit(w);
  body->setObjectName(QStringLiteral("AboutState"));
  body->setReadOnly(true);
  lay->addWidget(body, 1);
  auto *refresh = new QPushButton(QStringLiteral("Refresh from spike-config"), w);
  lay->addWidget(refresh);
  connect(refresh, &QPushButton::clicked, this, [this, body]() {
    QString err;
    const QString json = m_config ? m_config->getState(&err) : QString();
    if (json.isEmpty()) {
      body->setPlainText(QStringLiteral("Could not reach org.spike.Config:\n%1\n\n"
                                        "Is spike-config installed with D-Bus activation?")
                             .arg(err));
      m_status->setText(QStringLiteral("spike-config unavailable"));
    } else {
      body->setPlainText(json);
      m_status->setText(QStringLiteral("Loaded state from org.spike.Config"));
    }
  });
  // Initial load
  refresh->click();
  return w;
}

QWidget *SettingsWindow::makeMemoryPage()
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Memory</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Advanced · ZRAM / swap / swappiness (via spike-config)."), w);
  hint->setWordWrap(true);
  lay->addWidget(hint);
  auto *body = new QTextEdit(w);
  body->setObjectName(QStringLiteral("MemoryState"));
  body->setReadOnly(true);
  lay->addWidget(body, 1);
  auto *refresh = new QPushButton(QStringLiteral("Load memory module state"), w);
  lay->addWidget(refresh);
  connect(refresh, &QPushButton::clicked, this, [this, body]() {
    QString err;
    const QString json =
        m_config ? m_config->getModuleState(QStringLiteral("memory"), &err) : QString();
    if (json.isEmpty()) {
      body->setPlainText(QStringLiteral("Error: %1").arg(err));
    } else {
      body->setPlainText(json);
      m_status->setText(QStringLiteral("Settings applied path: GetModuleState(memory)"));
    }
  });
  refresh->click();
  return w;
}

void SettingsWindow::rebuildNav()
{
  const QString needle = m_search ? m_search->text().trimmed().toLower() : QString();
  const bool showAdv = m_showAdvanced && m_showAdvanced->isChecked();
  const QString currentId =
      m_nav && m_nav->currentItem() ? m_nav->currentItem()->data(Qt::UserRole).toString() : QString();

  m_nav->clear();
  QString lastCat;
  int selectRow = -1;

  for (int i = 0; i < m_pages.size(); ++i) {
    const PageDef &p = m_pages.at(i);
    if (p.advanced && !showAdv) {
      continue;
    }
    const QString hay = (p.title + QLatin1Char(' ') + p.keywords + QLatin1Char(' ') + p.category)
                            .toLower();
    if (!needle.isEmpty() && !hay.contains(needle)) {
      continue;
    }
    if (p.category != lastCat) {
      auto *hdr = new QListWidgetItem(categoryHeader(p.category));
      hdr->setFlags(Qt::NoItemFlags);
      QFont f = hdr->font();
      f.setBold(true);
      hdr->setFont(f);
      m_nav->addItem(hdr);
      lastCat = p.category;
    }
    auto *item = new QListWidgetItem(p.title);
    item->setData(Qt::UserRole, p.id);
    item->setData(Qt::UserRole + 1, i);
    m_nav->addItem(item);
    if (p.id == currentId || (selectRow < 0 && needle.isEmpty() && !p.advanced)) {
      selectRow = m_nav->count() - 1;
    }
  }
  if (selectRow >= 0) {
    m_nav->setCurrentRow(selectRow);
  }
}

void SettingsWindow::openPage(const QString &pageId)
{
  if (pageId.startsWith(QLatin1String("advanced:")) || pageId == QLatin1String("memory") ||
      pageId == QLatin1String("boot") || pageId == QLatin1String("diagnostics") ||
      pageId == QLatin1String("storage") || pageId == QLatin1String("updates") ||
      pageId == QLatin1String("kernel-modules")) {
    m_showAdvanced->setChecked(true);
  }
  for (int i = 0; i < m_nav->count(); ++i) {
    QListWidgetItem *item = m_nav->item(i);
    if (item->data(Qt::UserRole).toString() == pageId) {
      m_nav->setCurrentRow(i);
      return;
    }
  }
}

void SettingsWindow::onNavChanged()
{
  QListWidgetItem *item = m_nav->currentItem();
  if (!item || item->data(Qt::UserRole).toString().isEmpty()) {
    return;
  }
  const int pageIndex = item->data(Qt::UserRole + 1).toInt();
  if (pageIndex >= 0 && pageIndex < m_stack->count()) {
    m_stack->setCurrentIndex(pageIndex);
    refreshCurrentPage();
  }
}

void SettingsWindow::onSearchChanged(const QString &)
{
  rebuildNav();
}

void SettingsWindow::onAdvancedToggled(bool)
{
  rebuildNav();
}

void SettingsWindow::onHelpClicked()
{
  QMessageBox::information(this, QStringLiteral("Help"),
                           QStringLiteral("Context-aware user guide reader will open here "
                                          "(DESKTOP.md). Offline guide not shipped yet."));
}

void SettingsWindow::refreshCurrentPage()
{
  QWidget *page = m_stack->currentWidget();
  if (!page) {
    return;
  }
  auto *host = page->findChild<KcmHost *>(QStringLiteral("KcmHost"));
  if (!host) {
    return;
  }
  const QString plugin = host->property("kcmPlugin").toString();
  if (plugin.isEmpty()) {
    return;
  }
  if (host->isLoaded() && host->pluginId() == plugin) {
    return;
  }
  if (host->loadPlugin(plugin)) {
    m_status->setText(QStringLiteral("Loaded %1 in-window").arg(plugin));
  } else {
    m_status->setText(QStringLiteral("KCM %1 not available yet").arg(plugin));
  }
}

int SettingsWindow::pageIndexForId(const QString &id) const
{
  for (int i = 0; i < m_pages.size(); ++i) {
    if (m_pages.at(i).id == id) {
      return i;
    }
  }
  return -1;
}

} // namespace spike
