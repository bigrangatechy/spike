#include "settings/KernelModulesPage.hpp"

#include "settings/ConfigClient.hpp"

#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

bool validModuleName(const QString &name)
{
  static const QRegularExpression re(QStringLiteral("^[A-Za-z0-9_\\-]+$"));
  return re.match(name).hasMatch();
}

} // namespace

QWidget *makeKernelModulesPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Kernel Modules</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Blacklist modules via org.spike.Config security.module_blacklist → "
          "/etc/modprobe.d/spike-blacklist.conf. Takes effect on next module load / reboot."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *list = new QListWidget(w);
  lay->addWidget(list, 1);

  auto *addRow = new QHBoxLayout();
  auto *modEdit = new QLineEdit(w);
  modEdit->setPlaceholderText(QStringLiteral("module_name"));
  auto *addBtn = new QPushButton(QStringLiteral("Add"), w);
  addRow->addWidget(modEdit, 1);
  addRow->addWidget(addBtn);
  lay->addLayout(addRow);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *remove = new QPushButton(QStringLiteral("Remove selected"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  row->addWidget(reload);
  row->addWidget(remove);
  row->addWidget(apply);
  row->addStretch(1);
  lay->addLayout(row);

  auto load = [config, list, status, statusBar]() {
    list->clear();
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("security"), &err);
    if (json.isEmpty()) {
      status->setText(QStringLiteral("Load failed: %1").arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    const QJsonArray arr = o.value(QStringLiteral("module_blacklist")).toArray();
    for (const QJsonValue &v : arr) {
      const QString name = v.toString().trimmed();
      if (!name.isEmpty()) {
        list->addItem(name);
      }
    }
    status->setText(arr.isEmpty() ? QStringLiteral("No modules blacklisted")
                                  : QStringLiteral("%1 blacklisted").arg(arr.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Kernel modules loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(addBtn, &QPushButton::clicked, w, [modEdit, list, status]() {
    const QString name = modEdit->text().trimmed();
    if (!validModuleName(name)) {
      status->setText(QStringLiteral("Invalid module name (letters, digits, _, - only)"));
      return;
    }
    for (int i = 0; i < list->count(); ++i) {
      if (list->item(i)->text() == name) {
        status->setText(QStringLiteral("Already listed"));
        return;
      }
    }
    list->addItem(name);
    modEdit->clear();
    status->setText(QStringLiteral("Added %1 (Apply to write)").arg(name));
  });
  QObject::connect(remove, &QPushButton::clicked, w, [list, status]() {
    qDeleteAll(list->selectedItems());
    status->setText(QStringLiteral("Removed selection (Apply to write)"));
  });
  QObject::connect(apply, &QPushButton::clicked, w, [config, list, status, statusBar]() {
    if (!config) {
      return;
    }
    QJsonArray arr;
    for (int i = 0; i < list->count(); ++i) {
      arr.append(list->item(i)->text());
    }
    const QString payload =
        QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
    QString err;
    if (!config->setSetting(QStringLiteral("security"), QStringLiteral("module_blacklist"),
                            payload, &err)) {
      status->setText(QStringLiteral("SetSetting failed: %1").arg(err));
      return;
    }
    status->setText(
        QStringLiteral("Wrote blacklist (%1) → spike-blacklist.conf regenerated.").arg(arr.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Kernel modules applied"));
    }
  });

  load();
  return w;
}

} // namespace spike
