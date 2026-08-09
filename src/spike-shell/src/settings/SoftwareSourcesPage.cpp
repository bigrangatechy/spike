#include "settings/SoftwareSourcesPage.hpp"

#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString readFileTrunc(const QString &path, int maxBytes = 20000)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return {};
  }
  QByteArray data = f.read(maxBytes);
  QString text = QString::fromUtf8(data);
  if (!f.atEnd()) {
    text += QStringLiteral("\n… (truncated)\n");
  }
  return text;
}

bool startDetachedFirst(const QStringList &candidates, const QStringList &args = {})
{
  for (const QString &bin : candidates) {
    if (QProcess::startDetached(bin, args)) {
      return true;
    }
  }
  return false;
}

bool sourcesListHasComponent(const QString &comp)
{
  const QString text = readFileTrunc(QStringLiteral("/etc/apt/sources.list"), 200000);
  const QStringList lines = text.split(QLatin1Char('\n'));
  for (QString line : lines) {
    line = line.trimmed();
    if (line.startsWith(QLatin1Char('#')) || !line.startsWith(QLatin1String("deb"))) {
      continue;
    }
    if (line.contains(QLatin1Char(' ') + comp) || line.endsWith(comp)) {
      return true;
    }
  }
  return false;
}

bool addAptComponent(const QString &comp, QString *err)
{
  QProcess proc;
  proc.start(QStringLiteral("sudo"),
             {QStringLiteral("-n"), QStringLiteral("add-apt-repository"), QStringLiteral("-y"),
              comp});
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(120000)) {
    proc.kill();
    if (err) {
      *err = QStringLiteral("add-apt-repository timed out / sudo -n unavailable");
    }
    return false;
  }
  if (proc.exitCode() != 0) {
    if (err) {
      *err = QString::fromUtf8(proc.readAllStandardError()).trimmed();
      if (err->isEmpty()) {
        *err = QStringLiteral("add-apt-repository failed");
      }
    }
    return false;
  }
  return true;
}

} // namespace

QWidget *makeSoftwareSourcesPage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Software Sources</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "APT sources view. Toggle universe/multiverse when sudo -n add-apt-repository works. "
          "PPA editor and NVIDIA UX stay in external tools for now."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *universe = new QCheckBox(QStringLiteral("Enable universe"), w);
  auto *multiverse = new QCheckBox(QStringLiteral("Enable multiverse"), w);
  lay->addWidget(universe);
  lay->addWidget(multiverse);

  auto *body = new QTextEdit(w);
  body->setReadOnly(true);
  body->setObjectName(QStringLiteral("SoftwareSourcesBody"));
  lay->addWidget(body, 1);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *applyComp = new QPushButton(QStringLiteral("Apply components"), w);
  auto *props = new QPushButton(QStringLiteral("Software properties…"), w);
  auto *drivers = new QPushButton(QStringLiteral("Additional drivers…"), w);
  auto *update = new QPushButton(QStringLiteral("Check for updates…"), w);
  row->addWidget(reload);
  row->addWidget(applyComp);
  row->addWidget(props);
  row->addWidget(drivers);
  row->addWidget(update);
  row->addStretch(1);
  lay->addLayout(row);

  auto load = [body, universe, multiverse, status, statusBar]() {
    QString out;
    out += QStringLiteral("=== /etc/apt/sources.list ===\n");
    const QString mainList = readFileTrunc(QStringLiteral("/etc/apt/sources.list"));
    out += mainList.isEmpty() ? QStringLiteral("(missing or empty)\n") : mainList;
    if (!out.endsWith(QLatin1Char('\n'))) {
      out += QLatin1Char('\n');
    }
    out += QStringLiteral("\n=== /etc/apt/sources.list.d/ ===\n");
    QDir dir(QStringLiteral("/etc/apt/sources.list.d"));
    const QStringList files =
        dir.entryList(QStringList{QStringLiteral("*.list"), QStringLiteral("*.sources")},
                      QDir::Files, QDir::Name);
    if (files.isEmpty()) {
      out += QStringLiteral("(no drop-in files)\n");
    }
    for (const QString &name : files) {
      out += QStringLiteral("\n--- %1 ---\n").arg(name);
      out += readFileTrunc(dir.filePath(name));
      if (!out.endsWith(QLatin1Char('\n'))) {
        out += QLatin1Char('\n');
      }
    }
    body->setPlainText(out);
    universe->setChecked(sourcesListHasComponent(QStringLiteral("universe")));
    multiverse->setChecked(sourcesListHasComponent(QStringLiteral("multiverse")));
    status->setText(QStringLiteral("Loaded APT sources (%1 drop-ins)").arg(files.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Software Sources loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(applyComp, &QPushButton::clicked, w, [universe, multiverse, status, load]() {
    QStringList msgs;
    if (universe->isChecked() && !sourcesListHasComponent(QStringLiteral("universe"))) {
      QString err;
      if (!addAptComponent(QStringLiteral("universe"), &err)) {
        status->setText(QStringLiteral("universe failed: %1").arg(err));
        return;
      }
      msgs << QStringLiteral("universe enabled");
    }
    if (multiverse->isChecked() && !sourcesListHasComponent(QStringLiteral("multiverse"))) {
      QString err;
      if (!addAptComponent(QStringLiteral("multiverse"), &err)) {
        status->setText(QStringLiteral("multiverse failed: %1").arg(err));
        return;
      }
      msgs << QStringLiteral("multiverse enabled");
    }
    if (msgs.isEmpty()) {
      status->setText(QStringLiteral(
          "Nothing to enable (disable via Software properties — one-way enable here)."));
    } else {
      status->setText(msgs.join(QStringLiteral("; ")));
    }
    load();
  });
  QObject::connect(props, &QPushButton::clicked, w, [status]() {
    if (startDetachedFirst({QStringLiteral("software-properties-qt"),
                            QStringLiteral("software-properties-gtk"),
                            QStringLiteral("synaptic")})) {
      status->setText(QStringLiteral("Opened software properties / package manager"));
      return;
    }
    status->setText(QStringLiteral(
        "No software-properties / synaptic found. Edit /etc/apt sources as root for now."));
  });
  QObject::connect(drivers, &QPushButton::clicked, w, [status]() {
    if (startDetachedFirst({QStringLiteral("software-properties-qt"),
                            QStringLiteral("software-properties-gtk")},
                           {QStringLiteral("--open-tab"), QStringLiteral("4")})) {
      status->setText(QStringLiteral("Opened additional drivers tab (if supported)"));
      return;
    }
    if (startDetachedFirst({QStringLiteral("ubuntu-drivers")})) {
      return;
    }
    QProcess proc;
    proc.start(QStringLiteral("ubuntu-drivers"), {QStringLiteral("list")});
    if (proc.waitForFinished(8000) && proc.exitCode() == 0) {
      const QString out = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
      status->setText(out.isEmpty() ? QStringLiteral("ubuntu-drivers: no recommendations")
                                    : QStringLiteral("Drivers:\n%1").arg(out));
      return;
    }
    status->setText(QStringLiteral(
        "Additional drivers UI not installed (ubuntu-drivers / software-properties)."));
  });
  QObject::connect(update, &QPushButton::clicked, w, [status]() {
    if (startDetachedFirst({QStringLiteral("update-manager"), QStringLiteral("plasma-discover"),
                            QStringLiteral("gnome-software")})) {
      status->setText(QStringLiteral("Opened update UI"));
      return;
    }
    status->setText(QStringLiteral(
        "No update UI found. Use: sudo apt update && sudo apt upgrade (or Spike Updates page)."));
  });

  load();
  return w;
}

} // namespace spike
