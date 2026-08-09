#include "panel/applets/WindowListApplet.hpp"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QHBoxLayout>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QTimer>
#include <QVariantMap>

namespace spike {

WindowListApplet::WindowListApplet(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("WindowListApplet"));
  m_lay = new QHBoxLayout(this);
  m_lay->setContentsMargins(0, 0, 0, 0);
  m_lay->setSpacing(4);

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &WindowListApplet::refresh);
  timer->start(2000);
  refresh();
}

void WindowListApplet::setEnabledVisible(bool on)
{
  m_userVisible = on;
  setVisible(on);
  if (on) {
    refresh();
  }
}

QVector<WindowListApplet::Win> WindowListApplet::listWindows() const
{
  QVector<Win> out;
  // Prefer qdbus-style KWin scripting query via plasma window management if present.
  QDBusInterface kwin(QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
                      QStringLiteral("org.kde.KWin"), QDBusConnection::sessionBus());
  if (kwin.isValid()) {
    QDBusReply<QString> active = kwin.call(QStringLiteral("activeClient"));
    Q_UNUSED(active);
  }

  // Fallback: parse `wmctrl -l` when XWayland/X11 available; else empty.
  QProcess proc;
  proc.start(QStringLiteral("wmctrl"), {QStringLiteral("-l")});
  if (proc.waitForFinished(1500) && proc.exitCode() == 0) {
    const QString outStr = QString::fromUtf8(proc.readAllStandardOutput());
    for (const QString &line : outStr.split(QLatin1Char('\n'))) {
      if (line.trimmed().isEmpty()) {
        continue;
      }
      // id desktop host title...
      const QStringList parts = line.split(QRegularExpression(QStringLiteral("\\s+")),
                                           Qt::SkipEmptyParts);
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
  }

  // Second fallback: kwin_wayland window ids via dbus org.kde.KWin /Scripting
  if (out.isEmpty()) {
    QProcess p2;
    p2.start(QStringLiteral("qdbus"),
             {QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
              QStringLiteral("org.kde.KWin.queryWindowInfo")});
    // Often unsupported — leave empty quietly.
    p2.waitForFinished(500);
  }
  return out;
}

void WindowListApplet::refresh()
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
  const auto wins = listWindows();
  int shown = 0;
  for (const Win &w : wins) {
    if (shown >= 8) {
      break;
    }
    auto *btn = new QPushButton(w.caption.left(24), this);
    btn->setFlat(true);
    btn->setFixedHeight(26);
    btn->setMaximumWidth(140);
    btn->setToolTip(w.caption);
    const QString id = w.uuid;
    connect(btn, &QPushButton::clicked, this, [id]() {
      QProcess::startDetached(QStringLiteral("wmctrl"), {QStringLiteral("-ia"), id});
    });
    m_lay->addWidget(btn);
    ++shown;
  }
}

} // namespace spike
