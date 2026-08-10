#include "desktop/DesktopIcons.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QIcon>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QToolButton>
#include <QUrl>

namespace spike {

namespace {

QString desktopDir()
{
  QString d = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
  if (d.isEmpty()) {
    d = QDir::homePath() + QStringLiteral("/Desktop");
  }
  return d;
}

DesktopEntry parseDesktopFile(const QString &path)
{
  DesktopEntry e;
  e.path = path;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return e;
  }
  bool inEntry = false;
  while (!f.atEnd()) {
    QString line = QString::fromUtf8(f.readLine()).trimmed();
    if (line.startsWith(QLatin1Char('['))) {
      inEntry = (line == QLatin1String("[Desktop Entry]"));
      continue;
    }
    if (!inEntry || line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
      continue;
    }
    const int eq = line.indexOf(QLatin1Char('='));
    if (eq < 0) {
      continue;
    }
    const QString key = line.left(eq);
    const QString val = line.mid(eq + 1);
    if (key == QLatin1String("Name") && e.name.isEmpty()) {
      e.name = val;
    } else if (key == QLatin1String("Icon") && e.icon.isEmpty()) {
      e.icon = val;
    } else if (key == QLatin1String("Exec") && e.exec.isEmpty()) {
      e.exec = val;
    } else if (key == QLatin1String("Type") && val != QLatin1String("Application") &&
               val != QLatin1String("Link")) {
      // skip non-apps later if needed
    }
  }
  if (e.name.isEmpty()) {
    e.name = QFileInfo(path).completeBaseName();
  }
  return e;
}

QString stripExecFieldCodes(QString exec)
{
  // Remove %u %f %c etc. desktop Exec field codes.
  static const QRegularExpression re(QStringLiteral("%[a-zA-Z]"));
  return exec.replace(re, QString()).simplified();
}

} // namespace

DesktopIcons::DesktopIcons(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("SpikeDesktopIcons"));
  setAttribute(Qt::WA_TranslucentBackground);
  setAutoFillBackground(false);
  m_grid = new QGridLayout(this);
  m_grid->setContentsMargins(24, 24, 24, 48);
  m_grid->setHorizontalSpacing(16);
  m_grid->setVerticalSpacing(20);
  m_grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  reload();
}

bool DesktopIcons::ensureSeeded()
{
  const QString dir = desktopDir();
  QDir().mkpath(dir);
  const QString live = QStringLiteral("/usr/share/spike/live");
  struct Seed {
    const char *src;
    const char *dest;
  };
  const Seed seeds[] = {
      {"spike-installer.desktop", "Install Spike.desktop"},
      {"spike-rescue.desktop", "Rescue My Files.desktop"},
      {"spike-migration.desktop", "Move My Files.desktop"},
  };
  bool any = false;
  for (const Seed &s : seeds) {
    const QString src = live + QLatin1Char('/') + QLatin1String(s.src);
    const QString dest = dir + QLatin1Char('/') + QLatin1String(s.dest);
    if (!QFile::exists(src) || QFile::exists(dest)) {
      continue;
    }
    if (QFile::copy(src, dest)) {
      QFile::setPermissions(dest, QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                      QFileDevice::ExeOwner | QFileDevice::ReadGroup |
                                      QFileDevice::ExeGroup | QFileDevice::ReadOther |
                                      QFileDevice::ExeOther);
      any = true;
    }
  }
  // Always-useful tools (live + installed): copy logs to a spare USB.
  const QString logsSrc =
      QStringLiteral("/usr/share/spike/desktop/spike-save-logs.desktop");
  const QString logsDest = dir + QStringLiteral("/Copy Spike Logs to USB.desktop");
  if (QFile::exists(logsSrc) && !QFile::exists(logsDest)) {
    if (QFile::copy(logsSrc, logsDest)) {
      QFile::setPermissions(logsDest, QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                          QFileDevice::ExeOwner | QFileDevice::ReadGroup |
                                          QFileDevice::ExeGroup | QFileDevice::ReadOther |
                                          QFileDevice::ExeOther);
      any = true;
    }
  }
  return any;
}

QVector<DesktopEntry> DesktopIcons::scanDesktop()
{
  ensureSeeded();
  QVector<DesktopEntry> out;
  QDir dir(desktopDir());
  if (!dir.exists()) {
    return out;
  }
  const QStringList files =
      dir.entryList({QStringLiteral("*.desktop")}, QDir::Files, QDir::Name);
  for (const QString &f : files) {
    DesktopEntry e = parseDesktopFile(dir.absoluteFilePath(f));
    if (e.exec.isEmpty()) {
      continue;
    }
    out.append(e);
  }
  return out;
}

void DesktopIcons::launch(const DesktopEntry &entry)
{
  QString cmd = stripExecFieldCodes(entry.exec);
  if (cmd.isEmpty()) {
    return;
  }
  if (QProcess::startDetached(QStringLiteral("gio"),
                              {QStringLiteral("launch"), entry.path})) {
    return;
  }
  const QStringList parts = QProcess::splitCommand(cmd);
  if (parts.isEmpty()) {
    return;
  }
  QProcess::startDetached(parts.first(), parts.mid(1));
}

void DesktopIcons::reload()
{
  while (QLayoutItem *it = m_grid->takeAt(0)) {
    if (it->widget()) {
      it->widget()->deleteLater();
    }
    delete it;
  }

  const QVector<DesktopEntry> entries = scanDesktop();
  constexpr int cols = 1; // vertical column down the left (classic desktop)
  int row = 0;
  int col = 0;
  for (const DesktopEntry &e : entries) {
    auto *btn = new QToolButton(this);
    btn->setObjectName(QStringLiteral("DesktopIconButton"));
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(48, 48));
    btn->setFixedWidth(96);
    btn->setAutoRaise(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setText(e.name);
    btn->setToolTip(e.name);
    QIcon icon = QIcon::fromTheme(e.icon);
    if (icon.isNull() && QFile::exists(e.icon)) {
      icon = QIcon(e.icon);
    }
    if (icon.isNull()) {
      icon = QIcon::fromTheme(QStringLiteral("application-x-executable"));
    }
    btn->setIcon(icon);
    btn->setStyleSheet(
        QStringLiteral("QToolButton { color: white; background: transparent; border: none; "
                       "padding: 4px; }"
                       "QToolButton:hover { background: rgba(255,255,255,40); border-radius: 8px; }"
                       "QToolButton:pressed { background: rgba(255,255,255,70); }"));
    const DesktopEntry copy = e;
    connect(btn, &QToolButton::clicked, this, [this, copy]() { launch(copy); });
    m_grid->addWidget(btn, row, col, Qt::AlignTop);
    ++row;
    if (row >= 8) {
      row = 0;
      ++col;
    }
    Q_UNUSED(cols);
  }
  m_grid->setRowStretch(m_grid->rowCount(), 1);
  m_grid->setColumnStretch(m_grid->columnCount(), 1);
}

} // namespace spike
