#include "launcher/Launcher.hpp"

#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QProcess>
#include <QSet>
#include <QStandardPaths>
#include <QVBoxLayout>

#include <algorithm>

namespace spike {

namespace {

struct AppEntry {
  QString name;
  QString exec;
};

QString cleanExec(QString exec)
{
  // Strip freedesktop field codes and keep a runnable command line.
  static const char *const codes[] = {"%f", "%F", "%u", "%U", "%d", "%D", "%n", "%N",
                                      "%i", "%c", "%k", "%v", "%m"};
  for (const char *code : codes) {
    exec.replace(QString::fromUtf8(code), QString());
  }
  return exec.simplified();
}

QList<AppEntry> scanDesktopApps()
{
  QList<AppEntry> apps;
  QSet<QString> seenNames;

  QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
  // Ensure system path is present even if XDG vars are odd on live.
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
      bool noDisplay = false;
      bool hidden = false;
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
        } else if (key == QLatin1String("NoDisplay") && value == QLatin1String("true")) {
          noDisplay = true;
        } else if (key == QLatin1String("Hidden") && value == QLatin1String("true")) {
          hidden = true;
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
      apps.push_back({name, exec});
    }
  }

  std::sort(apps.begin(), apps.end(),
            [](const AppEntry &a, const AppEntry &b) { return a.name.toLower() < b.name.toLower(); });
  return apps;
}

void addFallback(QListWidget *list, const QString &label, const QString &command)
{
  // Only advertise fallbacks that exist on PATH.
  if (QStandardPaths::findExecutable(command.split(QLatin1Char(' ')).constFirst()).isEmpty()) {
    return;
  }
  auto *item = new QListWidgetItem(label, list);
  item->setData(Qt::UserRole, command);
}

} // namespace

Launcher::Launcher(QWidget *parent)
  : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
{
  setObjectName(QStringLiteral("SpikeLauncher"));
  setFixedSize(320, 420);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(8);

  m_search = new QLineEdit(this);
  m_search->setObjectName(QStringLiteral("LauncherSearch"));
  m_search->setPlaceholderText(QStringLiteral("Search applications…"));
  layout->addWidget(m_search);

  m_list = new QListWidget(this);
  m_list->setObjectName(QStringLiteral("LauncherList"));
  layout->addWidget(m_list, 1);

  connect(m_search, &QLineEdit::textChanged, this, &Launcher::filterChanged);
  connect(m_search, &QLineEdit::returnPressed, this, &Launcher::activateCurrent);
  // Single click should launch (itemActivated is usually double-click / Enter).
  connect(m_list, &QListWidget::itemClicked, this, [this](QListWidgetItem *) {
    activateCurrent();
  });
  connect(m_list, &QListWidget::itemActivated, this, [this](QListWidgetItem *) {
    activateCurrent();
  });

  populateEntries();
}

void Launcher::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  populateEntries();
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
  for (int i = 0; i < m_list->count(); ++i) {
    QListWidgetItem *item = m_list->item(i);
    const bool match = needle.isEmpty() || item->text().toLower().contains(needle);
    item->setHidden(!match);
  }
}

void Launcher::activateCurrent()
{
  QListWidgetItem *item = m_list->currentItem();
  if (!item) {
    for (int i = 0; i < m_list->count(); ++i) {
      if (!m_list->item(i)->isHidden()) {
        item = m_list->item(i);
        break;
      }
    }
  }
  if (!item) {
    return;
  }

  const QString cmd = item->data(Qt::UserRole).toString().trimmed();
  if (!cmd.isEmpty()) {
    // desktop Exec lines may include args; run via sh -c for simplicity.
    QProcess::startDetached(QStringLiteral("/bin/sh"), {QStringLiteral("-c"), cmd});
  }
  hide();
}

void Launcher::populateEntries()
{
  m_list->clear();

  const QList<AppEntry> apps = scanDesktopApps();
  for (const AppEntry &app : apps) {
    auto *item = new QListWidgetItem(app.name, m_list);
    item->setData(Qt::UserRole, app.exec);
  }

  if (m_list->count() == 0) {
    addFallback(m_list, QStringLiteral("Terminal"), QStringLiteral("foot"));
    addFallback(m_list, QStringLiteral("Terminal (xterm)"), QStringLiteral("xterm"));
  }

  if (m_list->count() > 0) {
    m_list->setCurrentRow(0);
  }
}

} // namespace spike
