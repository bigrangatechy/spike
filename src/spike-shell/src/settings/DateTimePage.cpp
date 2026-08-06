#include "settings/DateTimePage.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

bool runOk(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool timedatectlSet(const QStringList &args)
{
  QStringList full = args;
  if (runOk(QStringLiteral("timedatectl"), full)) {
    return true;
  }
  QStringList sudo = {QStringLiteral("-n"), QStringLiteral("timedatectl")};
  sudo.append(args);
  return runOk(QStringLiteral("sudo"), sudo);
}

QString timedatectlProp(const QString &key)
{
  const QString out = runCapture(QStringLiteral("timedatectl"), {QStringLiteral("show"),
                                                                QStringLiteral("-p"), key,
                                                                QStringLiteral("--value")});
  return out;
}

} // namespace

QWidget *makeDateTimePage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Date &amp; Time</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Timezone and NTP via systemd-timedated (timedatectl). No plasma-desktop."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *nowLabel = new QLabel(w);
  nowLabel->setObjectName(QStringLiteral("DateTimeNow"));
  lay->addWidget(nowLabel);

  auto *form = new QFormLayout();
  auto *tz = new QComboBox(w);
  tz->setEditable(false);
  tz->setMaxVisibleItems(20);
  form->addRow(QStringLiteral("Timezone"), tz);

  auto *ntp = new QCheckBox(QStringLiteral("Set time automatically (NTP)"), w);
  form->addRow(QString(), ntp);
  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  auto refreshClock = [nowLabel]() {
    const QString local = runCapture(QStringLiteral("date"),
                                     {QStringLiteral("+%Y-%m-%d %H:%M:%S %Z")});
    nowLabel->setText(QStringLiteral("<b>Local time:</b> %1")
                          .arg(local.isEmpty() ? QStringLiteral("(unavailable)") : local));
  };

  auto load = [tz, ntp, status, statusBar, refreshClock]() {
    refreshClock();
    if (tz->count() == 0) {
      const QString zones =
          runCapture(QStringLiteral("timedatectl"), {QStringLiteral("list-timezones")});
      const QStringList list = zones.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
      tz->addItems(list.isEmpty()
                       ? QStringList{QStringLiteral("UTC"), QStringLiteral("America/New_York"),
                                     QStringLiteral("Europe/London"), QStringLiteral("Australia/Sydney")}
                       : list);
    }
    const QString cur = timedatectlProp(QStringLiteral("Timezone"));
    const int idx = tz->findText(cur);
    if (idx >= 0) {
      tz->setCurrentIndex(idx);
    } else if (!cur.isEmpty()) {
      tz->insertItem(0, cur);
      tz->setCurrentIndex(0);
    }
    const QString ntpVal = timedatectlProp(QStringLiteral("NTP"));
    ntp->setChecked(ntpVal.compare(QStringLiteral("yes"), Qt::CaseInsensitive) == 0 ||
                    ntpVal == QLatin1String("1"));
    status->setText(QStringLiteral("Loaded from timedatectl"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Date & Time loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w, [tz, ntp, status, statusBar, load]() {
    const QString zone = tz->currentText().trimmed();
    if (!zone.isEmpty() && !timedatectlSet({QStringLiteral("set-timezone"), zone})) {
      status->setText(QStringLiteral("Failed to set timezone (need polkit or sudo -n timedatectl)."));
      return;
    }
    if (!timedatectlSet({QStringLiteral("set-ntp"), ntp->isChecked() ? QStringLiteral("true")
                                                                     : QStringLiteral("false")})) {
      status->setText(QStringLiteral("Failed to set NTP."));
      return;
    }
    status->setText(QStringLiteral("Timezone / NTP applied"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Date & Time applied"));
    }
    load();
  });

  auto *timer = new QTimer(w);
  QObject::connect(timer, &QTimer::timeout, w, refreshClock);
  timer->start(1000);

  load();
  return w;
}

} // namespace spike
