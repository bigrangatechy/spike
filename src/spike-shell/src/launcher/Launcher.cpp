#include "launcher/Launcher.hpp"

#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProcess>
#include <QPushButton>
#include <QSet>
#include <QStandardPaths>
#include <QVBoxLayout>

#include <algorithm>

namespace spike {

namespace {

struct AppEntry {
  QString name;
  QString exec;
  QString category;
  QString icon;
  bool terminal = false;
};

QString cleanExec(QString exec)
{
  static const char *const codes[] = {"%f", "%F", "%u", "%U", "%d", "%D", "%n", "%N",
                                      "%i", "%c", "%k", "%v", "%m"};
  for (const char *code : codes) {
    exec.replace(QString::fromUtf8(code), QString());
  }
  return exec.simplified();
}

QString categoryFor(const QString &categories)
{
  const QString c = categories.toLower();
  if (c.contains(QLatin1String("x-spike-tools"))) {
    return QStringLiteral("Spike Tools");
  }
  if (c.contains(QLatin1String("network")) || c.contains(QLatin1String("webbrowser")) ||
      c.contains(QLatin1String("email")) || c.contains(QLatin1String("chat"))) {
    return QStringLiteral("Internet");
  }
  if (c.contains(QLatin1String("office")) || c.contains(QLatin1String("wordprocessor")) ||
      c.contains(QLatin1String("spreadsheet")) || c.contains(QLatin1String("viewer"))) {
    return QStringLiteral("Office");
  }
  if (c.contains(QLatin1String("audio")) || c.contains(QLatin1String("video")) ||
      c.contains(QLatin1String("player")) || c.contains(QLatin1String("tv"))) {
    return QStringLiteral("Media");
  }
  if (c.contains(QLatin1String("graphics")) || c.contains(QLatin1String("photography"))) {
    return QStringLiteral("Graphics");
  }
  if (c.contains(QLatin1String("game"))) {
    return QStringLiteral("Games");
  }
  if (c.contains(QLatin1String("development")) || c.contains(QLatin1String("ide"))) {
    return QStringLiteral("Development");
  }
  if (c.contains(QLatin1String("system")) || c.contains(QLatin1String("settings")) ||
      c.contains(QLatin1String("terminalemulator")) || c.contains(QLatin1String("filesystem"))) {
    return QStringLiteral("System");
  }
  return QStringLiteral("Accessories");
}

QString terminalWrapper()
{
  if (!QStandardPaths::findExecutable(QStringLiteral("konsole")).isEmpty()) {
    return QStringLiteral("konsole");
  }
  if (!QStandardPaths::findExecutable(QStringLiteral("foot")).isEmpty()) {
    return QStringLiteral("foot");
  }
  return {};
}

QString wrapForTerminal(const QString &exec)
{
  const QString term = terminalWrapper();
  if (term.isEmpty()) {
    return exec;
  }
  const QString quoted = QLatin1Char('\'') + QString(exec).replace(QLatin1Char('\''), QStringLiteral("'\\''")) +
                         QLatin1Char('\'');
  if (term == QLatin1String("konsole")) {
    return QStringLiteral("konsole -e sh -c %1").arg(quoted);
  }
  return QStringLiteral("foot sh -c %1").arg(quoted);
}

QList<AppEntry> scanDesktopApps()
{
  QList<AppEntry> apps;
  QSet<QString> seenNames;

  QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
  if (!dirs.contains(QStringLiteral("/usr/share/applications"))) {
    dirs.prepend(QStringLiteral("/usr/share/applications"));
  }

  for (const QString &dirPath : dirs) {
    QDir dir(dirPath);
    if (!dir.exists()) {
      continue;
    }
    const QFileInfoList files =
        dir.entryInfoList({QStringLiteral("*.desktop")}, QDir::Files, QDir::Name);
    for (const QFileInfo &fi : files) {
      QFile file(fi.absoluteFilePath());
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        continue;
      }

      QString name;
      QString exec;
      QString categories;
      QString icon;
      bool noDisplay = false;
      bool hidden = false;
      bool terminal = false;
      bool inDesktopEntry = false;

      while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (line.startsWith(QLatin1Char('['))) {
          inDesktopEntry = (line == QLatin1String("[Desktop Entry]"));
          continue;
        }
        if (!inDesktopEntry || line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
          continue;
        }
        const int eq = line.indexOf(QLatin1Char('='));
        if (eq <= 0) {
          continue;
        }
        const QString key = line.left(eq);
        const QString value = line.mid(eq + 1);
        if (key == QLatin1String("Name") && name.isEmpty()) {
          name = value;
        } else if (key == QLatin1String("Exec") && exec.isEmpty()) {
          exec = cleanExec(value);
        } else if (key == QLatin1String("Icon") && icon.isEmpty()) {
          icon = value;
        } else if (key == QLatin1String("Categories") && categories.isEmpty()) {
          categories = value;
        } else if (key == QLatin1String("NoDisplay") && value == QLatin1String("true")) {
          noDisplay = true;
        } else if (key == QLatin1String("Hidden") && value == QLatin1String("true")) {
          hidden = true;
        } else if (key == QLatin1String("Terminal") && value == QLatin1String("true")) {
          terminal = true;
        } else if (key == QLatin1String("Type") && value != QLatin1String("Application")) {
          noDisplay = true;
        }
      }

      if (noDisplay || hidden || name.isEmpty() || exec.isEmpty()) {
        continue;
      }
      if (seenNames.contains(name)) {
        continue;
      }
      seenNames.insert(name);
      apps.push_back({name, exec, categoryFor(categories), icon, terminal});
    }
  }

  std::sort(apps.begin(), apps.end(), [](const AppEntry &a, const AppEntry &b) {
    if (a.category != b.category) {
      return a.category.toLower() < b.category.toLower();
    }
    return a.name.toLower() < b.name.toLower();
  });
  return apps;
}

} // namespace

Launcher::Launcher(QWidget *parent)
  : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
{
  setObjectName(QStringLiteral("SpikeLauncher"));
  setFixedSize(520, 480);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(8);

  m_search = new QLineEdit(this);
  m_search->setObjectName(QStringLiteral("LauncherSearch"));
  m_search->setPlaceholderText(QStringLiteral("Search applications…"));
  root->addWidget(m_search);

  auto *body = new QHBoxLayout();
  body->setSpacing(8);

  auto *leftCol = new QVBoxLayout();
  leftCol->addWidget(new QLabel(QStringLiteral("Favorites"), this));
  m_favorites = new QListWidget(this);
  m_favorites->setObjectName(QStringLiteral("LauncherFavorites"));
  m_favorites->setFixedWidth(160);
  leftCol->addWidget(m_favorites, 1);
  body->addLayout(leftCol);

  auto *rightCol = new QVBoxLayout();
  rightCol->addWidget(new QLabel(QStringLiteral("All Applications"), this));
  m_apps = new QListWidget(this);
  m_apps->setObjectName(QStringLiteral("LauncherList"));
  rightCol->addWidget(m_apps, 1);
  body->addLayout(rightCol, 1);
  root->addLayout(body, 1);

  auto *footer = new QHBoxLayout();
  auto *settingsBtn = new QPushButton(QStringLiteral("Settings"), this);
  auto *logoutBtn = new QPushButton(QStringLiteral("Log out"), this);
  auto *rebootBtn = new QPushButton(QStringLiteral("Restart"), this);
  auto *powerBtn = new QPushButton(QStringLiteral("Shut down"), this);
  footer->addWidget(settingsBtn);
  footer->addStretch(1);
  footer->addWidget(logoutBtn);
  footer->addWidget(rebootBtn);
  footer->addWidget(powerBtn);
  root->addLayout(footer);

  connect(m_search, &QLineEdit::textChanged, this, &Launcher::filterChanged);
  connect(m_search, &QLineEdit::returnPressed, this, &Launcher::activateAppList);
  connect(m_apps, &QListWidget::itemClicked, this, [this](QListWidgetItem *) { activateAppList(); });
  connect(m_apps, &QListWidget::itemActivated, this, [this](QListWidgetItem *) { activateAppList(); });
  connect(m_favorites, &QListWidget::itemClicked, this, [this](QListWidgetItem *) { activateFavorite(); });
  connect(m_favorites, &QListWidget::itemActivated, this,
          [this](QListWidgetItem *) { activateFavorite(); });

  connect(settingsBtn, &QPushButton::clicked, this, [this]() {
    hide();
    emit openSettingsRequested();
  });
  connect(logoutBtn, &QPushButton::clicked, this, [this]() {
    hide();
    emit logoutRequested();
  });
  connect(rebootBtn, &QPushButton::clicked, this, [this]() {
    hide();
    emit rebootRequested();
  });
  connect(powerBtn, &QPushButton::clicked, this, [this]() {
    hide();
    emit shutdownRequested();
  });

  populateFavorites();
  populateApps();
}

void Launcher::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  populateFavorites();
  populateApps();
  m_search->clear();
  filterChanged(QString());
  m_search->setFocus(Qt::OtherFocusReason);
}

void Launcher::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Escape) {
    hide();
    event->accept();
    return;
  }
  QWidget::keyPressEvent(event);
}

void Launcher::filterChanged(const QString &text)
{
  const QString needle = text.trimmed().toLower();
  for (int i = 0; i < m_apps->count(); ++i) {
    QListWidgetItem *item = m_apps->item(i);
    if (item->flags() == Qt::NoItemFlags) {
      // Category headers: hide if no following visible apps — keep simple: always show.
      item->setHidden(false);
      continue;
    }
    const bool match = needle.isEmpty() || item->text().toLower().contains(needle);
    item->setHidden(!match);
  }
}

void Launcher::activateAppList()
{
  QListWidgetItem *item = m_apps->currentItem();
  if (!item) {
    for (int i = 0; i < m_apps->count(); ++i) {
      if (!m_apps->item(i)->isHidden() && m_apps->item(i)->flags() != Qt::NoItemFlags) {
        item = m_apps->item(i);
        break;
      }
    }
  }
  if (!item || item->flags() == Qt::NoItemFlags) {
    return;
  }
  launchCommand(item->data(Qt::UserRole).toString());
}

void Launcher::activateFavorite()
{
  QListWidgetItem *item = m_favorites->currentItem();
  if (!item) {
    return;
  }
  const QString role = item->data(Qt::UserRole).toString();
  if (role == QLatin1String("__settings__")) {
    hide();
    emit openSettingsRequested();
    return;
  }
  launchCommand(role);
}

void Launcher::launchCommand(const QString &cmd)
{
  if (cmd.trimmed().isEmpty()) {
    return;
  }
  QProcess::startDetached(QStringLiteral("/bin/sh"), {QStringLiteral("-c"), cmd});
  hide();
}

void Launcher::populateFavorites()
{
  m_favorites->clear();

  struct Fav {
    QString label;
    QString command;
  };
  // Spec install defaults; only show entries that exist on PATH.
  const Fav defaults[] = {
      {QStringLiteral("Dolphin"), QStringLiteral("dolphin")},
      {QStringLiteral("Kate"), QStringLiteral("kate")},
      {QStringLiteral("Discover"), QStringLiteral("plasma-discover")},
      {QStringLiteral("Konsole"), QStringLiteral("konsole")},
      {QStringLiteral("Settings"), QStringLiteral("__settings__")},
  };
  for (const Fav &fav : defaults) {
    if (fav.command != QLatin1String("__settings__") &&
        QStandardPaths::findExecutable(fav.command).isEmpty()) {
      continue;
    }
    auto *item = new QListWidgetItem(fav.label, m_favorites);
    item->setData(Qt::UserRole, fav.command);
    QString iconName = fav.command;
    if (fav.command == QLatin1String("__settings__")) {
      iconName = QStringLiteral("preferences-system");
    } else if (fav.command == QLatin1String("dolphin")) {
      iconName = QStringLiteral("system-file-manager");
    } else if (fav.command == QLatin1String("kate")) {
      iconName = QStringLiteral("accessories-text-editor");
    } else if (fav.command == QLatin1String("plasma-discover")) {
      iconName = QStringLiteral("plasmadiscover");
    } else if (fav.command == QLatin1String("konsole")) {
      iconName = QStringLiteral("utilities-terminal");
    }
    const QIcon ic = QIcon::fromTheme(iconName);
    if (!ic.isNull()) {
      item->setIcon(ic);
    }
  }
}

void Launcher::populateApps()
{
  m_apps->clear();
  const QList<AppEntry> apps = scanDesktopApps();
  QString lastCat;
  for (const AppEntry &app : apps) {
    if (app.category != lastCat) {
      auto *hdr = new QListWidgetItem(app.category);
      hdr->setFlags(Qt::NoItemFlags);
      QFont f = hdr->font();
      f.setBold(true);
      hdr->setFont(f);
      m_apps->addItem(hdr);
      lastCat = app.category;
    }
    auto *item = new QListWidgetItem(app.name, m_apps);
    item->setData(Qt::UserRole, app.terminal ? wrapForTerminal(app.exec) : app.exec);
    if (!app.icon.isEmpty()) {
      QIcon ic;
      if (app.icon.startsWith(QLatin1Char('/'))) {
        ic = QIcon(app.icon);
      } else {
        ic = QIcon::fromTheme(app.icon);
      }
      if (!ic.isNull()) {
        item->setIcon(ic);
      }
    }
  }

  if (m_apps->count() == 0) {
    auto *item = new QListWidgetItem(QStringLiteral("Konsole"), m_apps);
    item->setData(Qt::UserRole, QStringLiteral("konsole"));
    item->setIcon(QIcon::fromTheme(QStringLiteral("utilities-terminal")));
  }
}

} // namespace spike
