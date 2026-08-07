#include "settings/LanguagePage.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString runCapture(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(20000)) {
    proc.kill();
    return {};
  }
  return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

bool runOk(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(20000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool localectlSet(const QStringList &args)
{
  if (runOk(QStringLiteral("localectl"), args)) {
    return true;
  }
  QStringList sudo = {QStringLiteral("-n"), QStringLiteral("localectl")};
  sudo.append(args);
  return runOk(QStringLiteral("sudo"), sudo);
}

QString localectlProp(const QString &key)
{
  const QString out = runCapture(QStringLiteral("localectl"),
                                 {QStringLiteral("show"), QStringLiteral("-p"), key,
                                  QStringLiteral("--value")});
  return out;
}

} // namespace

QWidget *makeLanguagePage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Language</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "System LANG via systemd-localed (localectl). Some apps need a new session to pick up "
          "the change. Not kcm_regionandlang."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *current = new QLabel(w);
  lay->addWidget(current);

  auto *form = new QFormLayout();
  auto *locale = new QComboBox(w);
  locale->setEditable(false);
  locale->setMaxVisibleItems(24);
  form->addRow(QStringLiteral("Locale (LANG)"), locale);
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

  auto load = [locale, current, status, statusBar]() {
    if (locale->count() == 0) {
      const QString listOut =
          runCapture(QStringLiteral("localectl"), {QStringLiteral("list-locales")});
      QStringList locales = listOut.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
      if (locales.isEmpty()) {
        locales = {QStringLiteral("C.UTF-8"), QStringLiteral("en_US.UTF-8"),
                   QStringLiteral("en_GB.UTF-8"), QStringLiteral("en_AU.UTF-8")};
      }
      locale->addItems(locales);
    }
    const QString lang = localectlProp(QStringLiteral("Locale"));
    // localectl show -p Locale may return LANG=en_US.UTF-8 or a multi-line blob.
    QString cur = lang;
    for (const QString &line : lang.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
      if (line.startsWith(QLatin1String("LANG="))) {
        cur = line.mid(5);
        break;
      }
    }
    if (cur.isEmpty()) {
      cur = qEnvironmentVariable("LANG");
    }
    current->setText(QStringLiteral("<b>Current LANG:</b> %1")
                         .arg(cur.isEmpty() ? QStringLiteral("(unknown)") : cur));
    const int idx = locale->findText(cur);
    if (idx >= 0) {
      locale->setCurrentIndex(idx);
    } else if (!cur.isEmpty()) {
      locale->insertItem(0, cur);
      locale->setCurrentIndex(0);
    }
    status->setText(QStringLiteral("Loaded from localectl"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Language loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w, [locale, status, statusBar, load]() {
    const QString lang = locale->currentText().trimmed();
    if (lang.isEmpty()) {
      status->setText(QStringLiteral("No locale selected"));
      return;
    }
    if (!localectlSet({QStringLiteral("set-locale"), QStringLiteral("LANG=%1").arg(lang)})) {
      status->setText(
          QStringLiteral("Failed to set locale (need polkit or sudo -n localectl)."));
      return;
    }
    status->setText(QStringLiteral("LANG set — restart session for all apps."));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Language applied"));
    }
    load();
  });

  load();
  return w;
}

} // namespace spike
