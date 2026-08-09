#include "panel/applets/WindowListApplet.hpp"

#include "panel/applets/TaskListAdaptor.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QTemporaryFile>
#include <QTimer>

namespace spike {

namespace {

QString kwinScriptMainPath()
{
  const QString packaged =
      QStringLiteral("/usr/share/kwin/scripts/spike-tasklist/contents/code/main.js");
  if (QFile::exists(packaged)) {
    return packaged;
  }
  return {};
}

} // namespace

WindowListApplet::WindowListApplet(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("WindowListApplet"));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_lay = new QHBoxLayout(this);
  m_lay->setContentsMargins(0, 0, 0, 0);
  m_lay->setSpacing(2);

  m_dbusReady = registerDbus();

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, [this]() {
    if (!m_scriptLoaded) {
      ensureKwinScript();
    }
    if (!m_usingKwinFeed) {
      refreshFallback();
    }
  });
  timer->start(1500);

  QTimer::singleShot(800, this, [this]() { ensureKwinScript(); });
  refreshFallback();
}

bool WindowListApplet::registerDbus()
{
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) {
    return false;
  }
  if (!bus.registerService(QStringLiteral("org.spike.Shell"))) {
    // Another object may already own the name — still try to register the path.
  }
  new TaskListAdaptor(this);
  if (!bus.registerObject(QStringLiteral("/TaskList"), this)) {
    return false;
  }
  return true;
}

void WindowListApplet::setEnabledVisible(bool on)
{
  m_userVisible = on;
  setVisible(on);
  if (on && !m_usingKwinFeed) {
    refreshFallback();
  }
}

void WindowListApplet::ensureKwinScript()
{
  if (m_scriptLoaded) {
    return;
  }
  const QString mainJs = kwinScriptMainPath();
  if (mainJs.isEmpty()) {
    return;
  }
  QDBusInterface scripting(QStringLiteral("org.kde.KWin"), QStringLiteral("/Scripting"),
                           QStringLiteral("org.kde.kwin.Scripting"),
                           QDBusConnection::sessionBus());
  if (!scripting.isValid()) {
    return;
  }
  QDBusReply<int> id =
      scripting.call(QStringLiteral("loadScript"), mainJs, QStringLiteral("spike-tasklist"));
  if (!id.isValid() || id.value() < 0) {
    return;
  }
  QDBusInterface script(QStringLiteral("org.kde.KWin"),
                        QStringLiteral("/Scripting/Script%1").arg(id.value()),
                        QStringLiteral("org.kde.kwin.Script"), QDBusConnection::sessionBus());
  script.call(QStringLiteral("run"));
  m_scriptLoaded = true;
}

void WindowListApplet::applyWindowBlob(const QString &blob)
{
  m_usingKwinFeed = true;
  QVector<Win> wins;
  for (const QString &line : blob.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
    const QStringList parts = line.split(QLatin1Char('\t'));
    if (parts.size() < 5) {
      continue;
    }
    Win w;
    w.uuid = parts.at(0);
    w.minimized = parts.at(1) == QLatin1String("1");
    w.resourceClass = parts.at(2);
    w.resourceName = parts.at(3);
    w.caption = parts.mid(4).join(QLatin1Char('\t'));
    if (w.caption.contains(QLatin1String("Spike Panel"), Qt::CaseInsensitive) ||
        w.caption.contains(QLatin1String("spike-shell"), Qt::CaseInsensitive)) {
      continue;
    }
    wins.append(w);
  }
  rebuild(wins);
}

QIcon WindowListApplet::iconFor(const Win &w) const
{
  const QStringList candidates = {
      w.resourceClass.toLower(),
      w.resourceName.toLower(),
      w.resourceClass,
      w.resourceName,
  };
  for (const QString &c : candidates) {
    if (c.isEmpty()) {
      continue;
    }
    const QIcon ic = QIcon::fromTheme(c);
    if (!ic.isNull()) {
      return ic;
    }
  }
  // Desktop-file style: firefox → firefox, org.mozilla.firefox → firefox
  QString cls = w.resourceClass;
  if (cls.contains(QLatin1Char('.'))) {
    cls = cls.section(QLatin1Char('.'), -1);
    const QIcon ic = QIcon::fromTheme(cls.toLower());
    if (!ic.isNull()) {
      return ic;
    }
  }
  return QIcon::fromTheme(QStringLiteral("application-x-executable"),
                          QIcon::fromTheme(QStringLiteral("window")));
}

void WindowListApplet::rebuild(const QVector<Win> &wins)
{
  if (!m_userVisible) {
    hide();
    return;
  }
  show();
  while (QLayoutItem *it = m_lay->takeAt(0)) {
    if (it->widget()) {
      it->widget()->deleteLater();
    }
    delete it;
  }
  int shown = 0;
  for (const Win &w : wins) {
    if (shown >= 12) {
      break;
    }
    auto *btn = new QPushButton(this);
    btn->setFlat(true);
    btn->setFixedSize(28, 28);
    btn->setIconSize(QSize(22, 22));
    btn->setIcon(iconFor(w));
    QString tip = w.caption.isEmpty() ? w.resourceClass : w.caption;
    if (w.minimized) {
      tip += QStringLiteral(" (minimized)");
    }
    btn->setToolTip(tip);
    btn->setCheckable(true);
    btn->setChecked(!w.minimized);
    const QString id = w.uuid;
    connect(btn, &QPushButton::clicked, this, [this, id]() { activateWindow(id); });
    m_lay->addWidget(btn);
    ++shown;
  }
}

void WindowListApplet::activateWindow(const QString &uuid)
{
  if (uuid.isEmpty()) {
    return;
  }
  // Prefer one-shot KWin script (Wayland-safe).
  QTemporaryFile tmp(QDir::temp().filePath(QStringLiteral("spike-activate-XXXXXX.js")));
  tmp.setAutoRemove(true);
  if (tmp.open()) {
    QString esc = uuid;
    esc.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    esc.replace(QLatin1Char('"'), QStringLiteral("\\\""));
    const QByteArray js =
        QStringLiteral(
            "workspace.windowList().forEach((w) => {\n"
            "  if (w.internalId.toString() === \"%1\") {\n"
            "    if (w.minimized) w.minimized = false;\n"
            "    workspace.activeWindow = w;\n"
            "  }\n"
            "});\n")
            .arg(esc)
            .toUtf8();
    tmp.write(js);
    tmp.flush();
    QDBusInterface scripting(QStringLiteral("org.kde.KWin"), QStringLiteral("/Scripting"),
                             QStringLiteral("org.kde.kwin.Scripting"),
                             QDBusConnection::sessionBus());
    QDBusReply<int> id = scripting.call(QStringLiteral("loadScript"), tmp.fileName(),
                                        QStringLiteral("spike-activate"));
    if (id.isValid() && id.value() >= 0) {
      QDBusInterface script(QStringLiteral("org.kde.KWin"),
                            QStringLiteral("/Scripting/Script%1").arg(id.value()),
                            QStringLiteral("org.kde.kwin.Script"),
                            QDBusConnection::sessionBus());
      script.call(QStringLiteral("run"));
      script.call(QStringLiteral("stop"));
      return;
    }
  }
  // X11 fallback
  QProcess::startDetached(QStringLiteral("wmctrl"), {QStringLiteral("-ia"), uuid});
}

QVector<WindowListApplet::Win> WindowListApplet::listWindowsWmctrl() const
{
  QVector<Win> out;
  QProcess proc;
  proc.start(QStringLiteral("wmctrl"), {QStringLiteral("-l")});
  if (!proc.waitForFinished(1500) || proc.exitCode() != 0) {
    return out;
  }
  const QString outStr = QString::fromUtf8(proc.readAllStandardOutput());
  for (const QString &line : outStr.split(QLatin1Char('\n'))) {
    if (line.trimmed().isEmpty()) {
      continue;
    }
    const QStringList parts =
        line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    if (parts.size() < 4) {
      continue;
    }
    Win w;
    w.uuid = parts.at(0);
    w.caption = parts.mid(3).join(QLatin1Char(' '));
    if (w.caption.contains(QLatin1String("Spike Panel"), Qt::CaseInsensitive) ||
        w.caption.contains(QLatin1String("spike-shell"), Qt::CaseInsensitive)) {
      continue;
    }
    out.append(w);
  }
  return out;
}

void WindowListApplet::refreshFallback()
{
  if (m_usingKwinFeed) {
    return;
  }
  rebuild(listWindowsWmctrl());
}

} // namespace spike
