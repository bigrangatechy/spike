#include "settings/SoftwareSourcesPage.hpp"

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

} // namespace

QWidget *makeSoftwareSourcesPage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Software Sources</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Read-only view of APT sources. Editing / adding PPAs and proprietary drivers "
          "will grow here; for now use the buttons to open system tools when installed."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *body = new QTextEdit(w);
  body->setReadOnly(true);
  body->setObjectName(QStringLiteral("SoftwareSourcesBody"));
  lay->addWidget(body, 1);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *props = new QPushButton(QStringLiteral("Software properties…"), w);
  auto *drivers = new QPushButton(QStringLiteral("Additional drivers…"), w);
  auto *update = new QPushButton(QStringLiteral("Check for updates…"), w);
  row->addWidget(reload);
  row->addWidget(props);
  row->addWidget(drivers);
  row->addWidget(update);
  row->addStretch(1);
  lay->addLayout(row);

  auto load = [body, status, statusBar]() {
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
    status->setText(QStringLiteral("Loaded APT sources (%1 drop-ins)").arg(files.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Software Sources loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
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
    // Show CLI hint in the status; listing drivers needs privileges.
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
