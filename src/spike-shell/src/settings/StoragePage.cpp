#include "settings/StoragePage.hpp"

#include "settings/ConfigClient.hpp"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QTableWidget>
#include <QVector>
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

} // namespace

QWidget *makeStoragePage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Storage</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Read-only disk overview (lsblk + DetectHardware primary disk). "
          "No wipe or partition actions — use Install Spike for disk changes. "
          "SMART health lands later."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *primary = new QLabel(w);
  primary->setWordWrap(true);
  primary->setObjectName(QStringLiteral("StoragePrimary"));
  lay->addWidget(primary);

  auto *table = new QTableWidget(w);
  table->setColumnCount(5);
  table->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Size"),
                                    QStringLiteral("Type"), QStringLiteral("Tran"),
                                    QStringLiteral("Mount")});
  table->horizontalHeader()->setStretchLastSection(true);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  lay->addWidget(table, 1);

  auto *note = new QLabel(
      QStringLiteral("SMART health: not queried yet (inventory only)."), w);
  note->setWordWrap(true);
  lay->addWidget(note);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Refresh"), w);
  row->addWidget(reload);
  row->addStretch(1);
  lay->addLayout(row);

  auto load = [config, table, primary, status, statusBar]() {
    table->setRowCount(0);
    primary->setText(QStringLiteral("Primary disk (DetectHardware): …"));

    if (config) {
      QString err;
      const QString json = config->detectHardware(&err);
      if (!json.isEmpty()) {
        const QJsonObject root = QJsonDocument::fromJson(json.toUtf8()).object();
        const QJsonObject st = root.value(QStringLiteral("storage")).toObject();
        if (!st.isEmpty()) {
          primary->setText(
              QStringLiteral("Primary disk (DetectHardware): <b>%1</b> — %2 GB (%3)")
                  .arg(st.value(QStringLiteral("device")).toString(QStringLiteral("?")),
                       QString::number(st.value(QStringLiteral("size_gb")).toInt()),
                       st.value(QStringLiteral("type")).toString(QStringLiteral("unknown"))));
        } else {
          primary->setText(QStringLiteral("Primary disk: (not in DetectHardware output)"));
        }
      } else {
        primary->setText(QStringLiteral("DetectHardware unavailable: %1").arg(err));
      }
    }

    const QString out = runCapture(
        QStringLiteral("lsblk"),
        {QStringLiteral("-J"), QStringLiteral("-o"),
         QStringLiteral("NAME,SIZE,TYPE,TRAN,MOUNTPOINT,MODEL")});
    if (out.isEmpty()) {
      status->setText(QStringLiteral("lsblk failed"));
      return;
    }
    const QJsonObject root = QJsonDocument::fromJson(out.toUtf8()).object();
    const QJsonArray block = root.value(QStringLiteral("blockdevices")).toArray();

    QVector<QJsonObject> rows;
    QVector<QJsonArray> stack;
    stack.append(block);
    while (!stack.isEmpty()) {
      const QJsonArray arr = stack.takeLast();
      for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        rows.append(o);
        const QJsonArray kids = o.value(QStringLiteral("children")).toArray();
        if (!kids.isEmpty()) {
          stack.append(kids);
        }
      }
    }

    table->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
      const QJsonObject o = rows.at(i);
      table->setItem(i, 0, new QTableWidgetItem(o.value(QStringLiteral("name")).toString()));
      table->setItem(i, 1, new QTableWidgetItem(o.value(QStringLiteral("size")).toString()));
      table->setItem(i, 2, new QTableWidgetItem(o.value(QStringLiteral("type")).toString()));
      table->setItem(i, 3, new QTableWidgetItem(o.value(QStringLiteral("tran")).toString()));
      const QString mount = o.value(QStringLiteral("mountpoint")).toString();
      const QString model = o.value(QStringLiteral("model")).toString();
      table->setItem(i, 4,
                     new QTableWidgetItem(mount.isEmpty() ? model : QStringLiteral("%1 %2")
                                                                       .arg(mount, model)
                                                                       .trimmed()));
    }
    status->setText(QStringLiteral("%1 block device row(s)").arg(rows.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Storage loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  load();
  return w;
}

} // namespace spike
