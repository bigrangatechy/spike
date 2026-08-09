#include "settings/AccessibilityPage.hpp"
#include "settings/AppearanceLive.hpp"
#include "settings/BootPage.hpp"
#include "settings/ConfigClient.hpp"
#include "settings/DateTimePage.hpp"
#include "settings/KeyboardLayoutPage.hpp"
#include "settings/KeyboardPage.hpp"
#include "settings/KcmHost.hpp"
#include "settings/KernelModulesPage.hpp"
#include "settings/LanguagePage.hpp"
#include "settings/MemoryPage.hpp"
#include "settings/MousePage.hpp"
#include "settings/NotificationsPage.hpp"
#include "settings/PanelPages.hpp"
#include "settings/PowerPage.hpp"
#include "settings/SettingsWindow.hpp"
#include "settings/SoftwareSourcesPage.hpp"
#include "settings/StoragePage.hpp"
#include "settings/UpdatesPage.hpp"
#include "settings/UsersPage.hpp"
#include "settings/VpnPage.hpp"

#include "network/NetworkPanelWidget.hpp"
#include "network/NmClient.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextEdit>
#include <QSysInfo>
#include <QVBoxLayout>
#include <QWidget>
#include <unistd.h>

namespace spike {

namespace {

QString categoryHeader(const QString &id)
{
  return id;
}

QJsonObject parseModuleJson(const QString &json)
{
  const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  return doc.isObject() ? doc.object() : QJsonObject{};
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
  // DESKTOP.md Settings Panel — every category entry must appear here.
  // KCM only when the provider package does not pull plasma-desktop / plasma-workspace.
  m_pages = {
      // Personal
      {QStringLiteral("appearance"), QStringLiteral("Appearance"), QStringLiteral("PERSONAL"),
       QStringLiteral("theme panel wallpaper font icons"), false, false, {},
       QStringLiteral("Theme, panel, fonts, icons (org.spike.Config desktop)."),
       QStringLiteral("desktop")},
      {QStringLiteral("notifications"), QStringLiteral("Notifications"), QStringLiteral("PERSONAL"),
       QStringLiteral("dnd history sound"), false, false, {},
       QStringLiteral("DND / retention via privacy; daemon history later."),
       QStringLiteral("privacy")},
      {QStringLiteral("keyboard-layout"), QStringLiteral("Keyboard Layout"), QStringLiteral("PERSONAL"),
       QStringLiteral("layout switching"), false, false, {},
       QStringLiteral("Layouts and switching — Spike custom (not kcm_keyboard)."),
       {}},
      {QStringLiteral("language"), QStringLiteral("Language"), QStringLiteral("PERSONAL"),
       QStringLiteral("locale language region"), false, false, {},
       QStringLiteral("System language via localectl (not kcm_regionandlang)."),
       {}},
      // Panel / tray
      {QStringLiteral("panel"), QStringLiteral("Panel"), QStringLiteral("PANEL"),
       QStringLiteral("taskbar height position autohide"), false, false, {},
       QStringLiteral("Panel geometry and auto-hide."),
       QStringLiteral("desktop")},
      {QStringLiteral("tray-applets"), QStringLiteral("Tray Applets"), QStringLiteral("PANEL"),
       QStringLiteral("notifications night light updates window list"), false, false, {},
       QStringLiteral("Show or hide optional tray applets."),
       QStringLiteral("desktop")},
      {QStringLiteral("night-light"), QStringLiteral("Night Light"), QStringLiteral("PANEL"),
       QStringLiteral("redshift temperature night color"), false, false, {},
       QStringLiteral("Night Light enable and temperature."),
       QStringLiteral("desktop")},
      // Hardware — KCMs from standalone packages on the live ISO
      {QStringLiteral("display"), QStringLiteral("Display"), QStringLiteral("HARDWARE"),
       QStringLiteral("resolution refresh scaling brightness"), false, true,
       QStringLiteral("kcm_kscreen"), {}, {}},
      {QStringLiteral("sound"), QStringLiteral("Sound"), QStringLiteral("HARDWARE"),
       QStringLiteral("volume audio output input"), false, true, QStringLiteral("kcm_pulseaudio"),
       {}, {}},
      {QStringLiteral("power"), QStringLiteral("Power"), QStringLiteral("HARDWARE"),
       QStringLiteral("battery suspend lid"), false, false, {},
       QStringLiteral("Spike Power via org.spike.Config + logind (not powerdevil KCM)."),
       {}},
      {QStringLiteral("keyboard"), QStringLiteral("Keyboard"), QStringLiteral("HARDWARE"),
       QStringLiteral("repeat shortcuts"), false, false, {},
       QStringLiteral("Repeat rate via kcminputrc (KWin)."),
       {}},
      {QStringLiteral("mouse"), QStringLiteral("Mouse/Touchpad"), QStringLiteral("HARDWARE"),
       QStringLiteral("pointer touchpad tapping"), false, false, {},
       QStringLiteral("Pointer / touchpad via kcminputrc."),
       {}},
      {QStringLiteral("bluetooth"), QStringLiteral("Bluetooth"), QStringLiteral("HARDWARE"),
       QStringLiteral("bt devices pairing"), false, true, QStringLiteral("kcm_bluetooth"), {}, {}},
      {QStringLiteral("printer"), QStringLiteral("Printer"), QStringLiteral("HARDWARE"),
       QStringLiteral("print queue"), false, true, QStringLiteral("kcm_printer_manager"), {}, {}},
      // Network — Spike NM UI (no plasma-nm / QtWebEngine)
      {QStringLiteral("network"), QStringLiteral("Network"), QStringLiteral("NETWORK"),
       QStringLiteral("wifi ethernet hotspot"), false, false, {},
       QStringLiteral("Spike NetworkManager UI (D-Bus + nmcli). Tray applet shares this."),
       {}},
      {QStringLiteral("vpn"), QStringLiteral("VPN"), QStringLiteral("NETWORK"),
       QStringLiteral("openvpn wireguard"), false, false, {},
       QStringLiteral("OpenVPN / WireGuard via NetworkManager."),
       {}},
      // System
      {QStringLiteral("users"), QStringLiteral("Users"), QStringLiteral("SYSTEM"),
       QStringLiteral("account password"), false, false, {},
       QStringLiteral("Password + auto-login (not kcm_users)."),
       {}},
      {QStringLiteral("datetime"), QStringLiteral("Date & Time"), QStringLiteral("SYSTEM"),
       QStringLiteral("timezone ntp clock"), false, false, {},
       QStringLiteral("Timezone / NTP via timedatectl."),
       {}},
      {QStringLiteral("accessibility"), QStringLiteral("Accessibility"), QStringLiteral("SYSTEM"),
       QStringLiteral("magnifier sticky keys"), false, false, {},
       QStringLiteral("AccessX + Orca/OSK launchers (not kcm_access)."),
       QStringLiteral("accessibility")},
      {QStringLiteral("software-sources"), QStringLiteral("Software Sources"), QStringLiteral("SYSTEM"),
       QStringLiteral("repos drivers nvidia"), false, false, {},
       QStringLiteral("APT sources view + software-properties / drivers tools."),
       {}},
      {QStringLiteral("about"), QStringLiteral("About"), QStringLiteral("SYSTEM"),
       QStringLiteral("version system info guide"), false, false, {}, {}, {}},
      // Advanced
      {QStringLiteral("memory"), QStringLiteral("Memory"), QStringLiteral("ADVANCED"),
       QStringLiteral("zram swap swappiness earlyoom"), true, false, {}, {},
       QStringLiteral("memory")},
      {QStringLiteral("boot"), QStringLiteral("Boot"), QStringLiteral("ADVANCED"),
       QStringLiteral("grub timeout boot counter"), true, false, {},
       QStringLiteral("GRUB timeout, boot failure counter, splash (org.spike.Config boot)."),
       QStringLiteral("boot")},
      {QStringLiteral("kernel-modules"), QStringLiteral("Kernel Modules"), QStringLiteral("ADVANCED"),
       QStringLiteral("blacklist modules"), true, false, {},
       QStringLiteral("Module blacklist (org.spike.Config security.module_blacklist)."),
       {}},
      {QStringLiteral("updates"), QStringLiteral("Updates"), QStringLiteral("ADVANCED"),
       QStringLiteral("schedule security"), true, false, {},
       QStringLiteral("Update schedule / auto-security (org.spike.Config updates)."),
       {}},
      {QStringLiteral("storage"), QStringLiteral("Storage"), QStringLiteral("ADVANCED"),
       QStringLiteral("disk smart mount"), true, false, {},
       QStringLiteral("Disk overview from lsblk + DetectHardware."),
       {}},
      {QStringLiteral("diagnostics"), QStringLiteral("Diagnostics"), QStringLiteral("ADVANCED"),
       QStringLiteral("logs hardware report"), true, false, {},
       QStringLiteral("System info / hardware inventory via DetectHardware."),
       {}},
  };

  for (const PageDef &page : m_pages) {
    QWidget *w = nullptr;
    if (page.id == QLatin1String("about")) {
      w = makeAboutPage();
    } else if (page.id == QLatin1String("memory")) {
      w = makeMemoryPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("boot")) {
      w = makeBootPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("appearance")) {
      w = makeAppearancePage();
    } else if (page.id == QLatin1String("panel")) {
      w = makePanelGeometryPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("tray-applets")) {
      w = makeTrayAppletsPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("night-light")) {
      w = makeNightLightSettingsPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("network")) {
      w = makeNetworkPage();
    } else if (page.id == QLatin1String("vpn")) {
      auto *nm = new NmClient(this);
      w = makeVpnPage(this, nm, m_status);
      nm->setParent(w);
    } else if (page.id == QLatin1String("language")) {
      w = makeLanguagePage(this, m_status);
    } else if (page.id == QLatin1String("users")) {
      w = makeUsersPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("notifications")) {
      w = makeNotificationsPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("accessibility")) {
      w = makeAccessibilityPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("software-sources")) {
      w = makeSoftwareSourcesPage(this, m_status);
    } else if (page.id == QLatin1String("datetime")) {
      w = makeDateTimePage(this, m_status);
    } else if (page.id == QLatin1String("keyboard")) {
      w = makeKeyboardPage(this, m_status);
    } else if (page.id == QLatin1String("mouse")) {
      w = makeMousePage(this, m_status);
    } else if (page.id == QLatin1String("power")) {
      w = makePowerPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("updates")) {
      w = makeUpdatesPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("kernel-modules")) {
      w = makeKernelModulesPage(this, m_config, m_status);
    } else if (page.id == QLatin1String("storage")) {
      w = makeStoragePage(this, m_config, m_status);
    } else if (page.id == QLatin1String("keyboard-layout")) {
      w = makeKeyboardLayoutPage(this, m_status);
    } else if (page.id == QLatin1String("diagnostics")) {
      auto *diag = new QWidget(this);
      auto *lay = new QVBoxLayout(diag);
      lay->addWidget(new QLabel(QStringLiteral("<h2>Diagnostics</h2>"), diag));
      auto *info = new QLabel(
          QStringLiteral("Hardware inventory from DetectHardware (sectioned). Copy for bug reports."),
          diag);
      info->setWordWrap(true);
      lay->addWidget(info);
      auto *summary = new QLabel(diag);
      summary->setWordWrap(true);
      summary->setTextFormat(Qt::RichText);
      summary->setObjectName(QStringLiteral("DiagnosticsSummary"));
      lay->addWidget(summary);
      auto *body = new QTextEdit(diag);
      body->setObjectName(QStringLiteral("DiagnosticsBody"));
      body->setReadOnly(true);
      lay->addWidget(body, 1);
      auto *row = new QHBoxLayout();
      auto *refresh = new QPushButton(QStringLiteral("Run DetectHardware"), diag);
      auto *copy = new QPushButton(QStringLiteral("Copy report"), diag);
      row->addWidget(refresh);
      row->addWidget(copy);
      row->addStretch(1);
      lay->addLayout(row);
      auto fill = [this, body, summary]() {
        QString err;
        const QString json = m_config ? m_config->detectHardware(&err) : QString();
        if (json.isEmpty()) {
          body->setPlainText(QStringLiteral("Error: %1").arg(err));
          summary->setText(QStringLiteral("DetectHardware failed"));
          m_status->setText(QStringLiteral("DetectHardware failed"));
          return;
        }
        body->setPlainText(json);
        const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
        const QJsonObject cpu = o.value(QStringLiteral("cpu")).toObject();
        const QJsonObject ram = o.value(QStringLiteral("ram")).toObject();
        const QJsonObject gpu = o.value(QStringLiteral("gpu")).toObject();
        const QJsonObject storage = o.value(QStringLiteral("storage")).toObject();
        const QJsonObject net = o.value(QStringLiteral("network")).toObject();
        summary->setText(
            QStringLiteral(
                "<b>CPU:</b> %1 (%2 cores)<br/>"
                "<b>RAM:</b> %3 MB<br/>"
                "<b>GPU:</b> %4<br/>"
                "<b>Storage:</b> %5 — %6 GB (%7)<br/>"
                "<b>Network:</b> wifi=%8 eth=%9 bt=%10")
                .arg(cpu.value(QStringLiteral("model")).toString(
                         cpu.value(QStringLiteral("name")).toString(QStringLiteral("?"))),
                     QString::number(cpu.value(QStringLiteral("cores")).toInt(
                         cpu.value(QStringLiteral("logical_cores")).toInt())),
                     QString::number(ram.value(QStringLiteral("total_mb")).toInt(
                         ram.value(QStringLiteral("total_kb")).toInt() / 1024)),
                     gpu.value(QStringLiteral("name")).toString(
                         gpu.value(QStringLiteral("driver")).toString(QStringLiteral("?"))),
                     storage.value(QStringLiteral("device")).toString(QStringLiteral("?")),
                     QString::number(storage.value(QStringLiteral("size_gb")).toInt()),
                     storage.value(QStringLiteral("type")).toString(QStringLiteral("?")),
                     net.value(QStringLiteral("has_wifi")).toBool() ? QStringLiteral("yes")
                                                                   : QStringLiteral("no"),
                     net.value(QStringLiteral("has_ethernet")).toBool() ? QStringLiteral("yes")
                                                                       : QStringLiteral("no"),
                     net.value(QStringLiteral("has_bluetooth")).toBool() ? QStringLiteral("yes")
                                                                        : QStringLiteral("no")));
        m_status->setText(QStringLiteral("DetectHardware OK"));
      };
      connect(refresh, &QPushButton::clicked, this, fill);
      connect(copy, &QPushButton::clicked, this, [body, this]() {
        if (QClipboard *clip = QGuiApplication::clipboard()) {
          clip->setText(body->toPlainText());
          m_status->setText(QStringLiteral("Diagnostics copied to clipboard"));
        }
      });
      fill();
      w = diag;
    } else if (page.isKcm) {
      w = makeKcmPage(page);
    } else if (!page.configModule.isEmpty()) {
      w = makeModuleStatePage(page);
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
  auto *info = new QLabel(page.blurb.isEmpty()
                              ? QStringLiteral("Spike custom page — controls will talk to "
                                               "org.spike.Config over D-Bus.")
                              : page.blurb,
                          w);
  info->setWordWrap(true);
  lay->addWidget(info);
  lay->addStretch(1);
  return w;
}

QWidget *SettingsWindow::makeModuleStatePage(const PageDef &page)
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>%1</h2>").arg(page.title), w));
  if (!page.blurb.isEmpty()) {
    auto *hint = new QLabel(page.blurb, w);
    hint->setWordWrap(true);
    lay->addWidget(hint);
  }
  auto *body = new QTextEdit(w);
  body->setObjectName(QStringLiteral("ModuleState"));
  body->setReadOnly(true);
  lay->addWidget(body, 1);
  auto *refresh = new QPushButton(QStringLiteral("Load from spike-config"), w);
  lay->addWidget(refresh);
  const QString module = page.configModule;
  connect(refresh, &QPushButton::clicked, this, [this, body, module]() {
    QString err;
    const QString json = m_config ? m_config->getModuleState(module, &err) : QString();
    if (json.isEmpty()) {
      body->setPlainText(QStringLiteral("Could not reach org.spike.Config:\n%1").arg(err));
      m_status->setText(QStringLiteral("spike-config unavailable"));
    } else {
      body->setPlainText(json);
      m_status->setText(QStringLiteral("GetModuleState(%1)").arg(module));
    }
  });
  refresh->click();
  return w;
}

QWidget *SettingsWindow::makeNetworkPage()
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Network</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Wi‑Fi and Ethernet via NetworkManager (org.freedesktop.NetworkManager)."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);
  auto *nm = new NmClient(w);
  auto *panel = new NetworkPanelWidget(nm, w);
  // Settings page is already Settings — hide the nested "Network Settings" button action noise.
  connect(panel, &NetworkPanelWidget::openFullSettings, this, [this]() {
    m_status->setText(QStringLiteral("Already in Network settings"));
  });
  lay->addWidget(panel, 1);
  return w;
}

QWidget *SettingsWindow::makeAppearancePage()
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Appearance</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Accent, font size, and wallpaper apply live in Spike Shell. "
          "Panel geometry lives under Settings → Panel. Aurorae themes come later."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();

  auto *accent = new QLineEdit(w);
  accent->setPlaceholderText(QStringLiteral("#6d4aff"));
  form->addRow(QStringLiteral("Accent color"), accent);

  auto *fontSize = new QSpinBox(w);
  fontSize->setRange(8, 18);
  fontSize->setSuffix(QStringLiteral(" pt"));
  form->addRow(QStringLiteral("Font size"), fontSize);

  auto *wallpaper = new QComboBox(w);
  wallpaper->setEditable(false);
  form->addRow(QStringLiteral("Wallpaper"), wallpaper);

  lay->addLayout(form);

  auto *browseRow = new QHBoxLayout();
  auto *browse = new QPushButton(QStringLiteral("Browse image…"), w);
  browseRow->addWidget(browse);
  browseRow->addStretch(1);
  lay->addLayout(browseRow);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto fillWallpapers = [wallpaper](const QString &current) {
    wallpaper->clear();
    wallpaper->addItem(QStringLiteral("(default / solid)"), QString());
    const QStringList roots = {
        QStringLiteral("/usr/share/spike/wallpapers"),
        QStringLiteral("/usr/share/backgrounds"),
    };
    for (const QString &root : roots) {
      QDir dir(root);
      if (!dir.exists()) {
        continue;
      }
      const QStringList files =
          dir.entryList({QStringLiteral("*.png"), QStringLiteral("*.jpg"), QStringLiteral("*.jpeg"),
                         QStringLiteral("*.webp")},
                        QDir::Files, QDir::Name);
      for (const QString &f : files) {
        wallpaper->addItem(f, dir.absoluteFilePath(f));
      }
      const QStringList sub = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
      for (const QString &s : sub) {
        QDir subdir(dir.absoluteFilePath(s));
        const QStringList nested =
            subdir.entryList({QStringLiteral("*.png"), QStringLiteral("*.jpg"),
                              QStringLiteral("*.jpeg")},
                             QDir::Files, QDir::Name);
        for (const QString &f : nested) {
          wallpaper->addItem(QStringLiteral("%1/%2").arg(s, f), subdir.absoluteFilePath(f));
        }
      }
    }
    if (!current.isEmpty()) {
      int idx = wallpaper->findData(current);
      if (idx < 0) {
        wallpaper->addItem(QFileInfo(current).fileName(), current);
        idx = wallpaper->count() - 1;
      }
      wallpaper->setCurrentIndex(idx);
    }
  };

  auto load = [this, accent, fontSize, wallpaper, status, fillWallpapers]() {
    QString err;
    const QString json =
        m_config ? m_config->getModuleState(QStringLiteral("desktop"), &err) : QString();
    if (json.isEmpty()) {
      status->setText(QStringLiteral("spike-config unavailable: %1").arg(err));
      fillWallpapers({});
      return;
    }
    const QJsonObject o = parseModuleJson(json);
    accent->setText(o.value(QStringLiteral("accent_color")).toString(QStringLiteral("#6d4aff")));
    fontSize->setValue(o.value(QStringLiteral("font_size_pt")).toInt(10));
    fillWallpapers(o.value(QStringLiteral("wallpaper")).toString());
    status->setText(QStringLiteral("Loaded desktop module from spike-config"));
  };

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  connect(reload, &QPushButton::clicked, this, load);
  connect(browse, &QPushButton::clicked, this, [wallpaper, status]() {
    const QString path = QFileDialog::getOpenFileName(
        wallpaper, QStringLiteral("Choose wallpaper"), QStringLiteral("/usr/share"),
        QStringLiteral("Images (*.png *.jpg *.jpeg *.webp)"));
    if (path.isEmpty()) {
      return;
    }
    wallpaper->addItem(QFileInfo(path).fileName(), path);
    wallpaper->setCurrentIndex(wallpaper->count() - 1);
    status->setText(QStringLiteral("Selected %1 — Apply to set").arg(path));
  });
  connect(apply, &QPushButton::clicked, this, [this, accent, fontSize, wallpaper, status]() {
    if (!m_config) {
      return;
    }
    QString err;
    auto set = [&](const QString &key, const QVariant &v) -> bool {
      if (!m_config->setSetting(QStringLiteral("desktop"), key, v, &err)) {
        status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
        return false;
      }
      return true;
    };
    const QString wall = wallpaper->currentData().toString();
    if (!set(QStringLiteral("accent_color"), accent->text().trimmed()) ||
        !set(QStringLiteral("font_size_pt"), fontSize->value()) ||
        !set(QStringLiteral("wallpaper"), wall)) {
      return;
    }

    applyShellChromeLive(qApp, accent->text().trimmed(), fontSize->value(), false);
    QString wallDetail;
    const bool wallOk = applyWallpaperLive(wall, &wallDetail);
    QString msg = QStringLiteral("Saved + live: accent, font");
    if (wallOk) {
      msg += QStringLiteral("; wallpaper OK");
    } else {
      msg += QStringLiteral("; wallpaper: %1").arg(wallDetail);
    }
    status->setText(msg);
    m_status->setText(QStringLiteral("appearance applied live"));
  });

  load();
  return w;
}

QWidget *SettingsWindow::makeAboutPage()
{
  auto *w = new QWidget(this);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>About Spike</h2>"), w));

  auto *card = new QLabel(w);
  card->setWordWrap(true);
  card->setTextFormat(Qt::RichText);
  card->setObjectName(QStringLiteral("AboutCard"));
  lay->addWidget(card);

  auto *guide = new QPushButton(QStringLiteral("User Guide (coming soon)"), w);
  guide->setEnabled(true);
  lay->addWidget(guide);

  auto *rawBox = new QGroupBox(QStringLiteral("Raw config state (debug)"), w);
  rawBox->setCheckable(true);
  rawBox->setChecked(false);
  auto *rawLay = new QVBoxLayout(rawBox);
  auto *body = new QTextEdit(rawBox);
  body->setObjectName(QStringLiteral("AboutState"));
  body->setReadOnly(true);
  body->setVisible(false);
  rawLay->addWidget(body);
  lay->addWidget(rawBox, 1);

  auto *refresh = new QPushButton(QStringLiteral("Refresh"), w);
  lay->addWidget(refresh);

  auto fill = [this, card, body]() {
    char host[256] = {};
    gethostname(host, sizeof(host) - 1);
    QString variant = QStringLiteral("?");
    QString err;
    const QString stateJson = m_config ? m_config->getState(&err) : QString();
    if (!stateJson.isEmpty()) {
      body->setPlainText(stateJson);
      const QJsonObject root = QJsonDocument::fromJson(stateJson.toUtf8()).object();
      variant = root.value(QStringLiteral("variant")).toString(QStringLiteral("standard"));
    } else {
      body->setPlainText(QStringLiteral("Could not reach org.spike.Config:\n%1").arg(err));
    }

    QString cpuLine = QStringLiteral("?");
    QString ramLine = QStringLiteral("?");
    if (m_config) {
      QString herr;
      const QString hw = m_config->detectHardware(&herr);
      if (!hw.isEmpty()) {
        const QJsonObject o = QJsonDocument::fromJson(hw.toUtf8()).object();
        const QJsonObject cpu = o.value(QStringLiteral("cpu")).toObject();
        const QJsonObject ram = o.value(QStringLiteral("ram")).toObject();
        cpuLine = cpu.value(QStringLiteral("model")).toString(
            cpu.value(QStringLiteral("name")).toString(QStringLiteral("?")));
        const int mb = ram.value(QStringLiteral("total_mb")).toInt(
            ram.value(QStringLiteral("total_kb")).toInt() / 1024);
        ramLine = QStringLiteral("%1 MB").arg(mb);
      }
    }

    card->setText(
        QStringLiteral(
            "<b>Spike Linux</b> (pre-alpha)<br/>"
            "Shell: <b>%1</b><br/>"
            "Host: <b>%2</b><br/>"
            "Kernel: %3<br/>"
            "Variant: %4<br/>"
            "CPU: %5<br/>"
            "RAM: %6<br/>"
            "<i>Installer engines paused — Settings polish in progress.</i>")
            .arg(QApplication::applicationVersion(), QString::fromUtf8(host),
                 QSysInfo::kernelVersion(), variant, cpuLine, ramLine));
    m_status->setText(QStringLiteral("About refreshed"));
  };

  connect(refresh, &QPushButton::clicked, this, fill);
  connect(rawBox, &QGroupBox::toggled, body, &QWidget::setVisible);
  connect(guide, &QPushButton::clicked, this, [this]() {
    m_status->setText(QStringLiteral("User Guide not shipped yet"));
    QMessageBox::information(this, QStringLiteral("User Guide"),
                             QStringLiteral("The Spike User Guide will appear here when written. "
                                            "See docs/USER-GUIDE.md in the source tree."));
  });
  fill();
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
    QString iconName;
    if (p.id == QLatin1String("appearance")) {
      iconName = QStringLiteral("preferences-desktop-theme");
    } else if (p.id == QLatin1String("notifications")) {
      iconName = QStringLiteral("preferences-desktop-notification");
    } else if (p.id == QLatin1String("keyboard-layout") || p.id == QLatin1String("keyboard")) {
      iconName = QStringLiteral("input-keyboard");
    } else if (p.id == QLatin1String("language")) {
      iconName = QStringLiteral("preferences-desktop-locale");
    } else if (p.id == QLatin1String("display")) {
      iconName = QStringLiteral("preferences-desktop-display");
    } else if (p.id == QLatin1String("sound")) {
      iconName = QStringLiteral("audio-volume-high");
    } else if (p.id == QLatin1String("power")) {
      iconName = QStringLiteral("preferences-system-power-management");
    } else if (p.id == QLatin1String("mouse")) {
      iconName = QStringLiteral("input-mouse");
    } else if (p.id == QLatin1String("bluetooth")) {
      iconName = QStringLiteral("preferences-system-bluetooth");
    } else if (p.id == QLatin1String("printer")) {
      iconName = QStringLiteral("printer");
    } else if (p.id == QLatin1String("network") || p.id == QLatin1String("vpn")) {
      iconName = QStringLiteral("network-wired");
    } else if (p.id == QLatin1String("users")) {
      iconName = QStringLiteral("system-users");
    } else if (p.id == QLatin1String("datetime")) {
      iconName = QStringLiteral("preferences-system-time");
    } else if (p.id == QLatin1String("accessibility")) {
      iconName = QStringLiteral("preferences-desktop-accessibility");
    } else if (p.id == QLatin1String("about")) {
      iconName = QStringLiteral("help-about");
    } else {
      iconName = QStringLiteral("preferences-system");
    }
    const QIcon ic = QIcon::fromTheme(iconName);
    if (!ic.isNull()) {
      item->setIcon(ic);
    }
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
