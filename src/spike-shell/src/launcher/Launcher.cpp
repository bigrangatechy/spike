#include "launcher/Launcher.hpp"

#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QProcess>
#include <QVBoxLayout>

namespace spike {

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
  connect(m_list, &QListWidget::itemActivated, this, [this](QListWidgetItem *) {
    activateCurrent();
  });

  populateStubEntries();
}

void Launcher::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
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

  const QString cmd = item->data(Qt::UserRole).toString();
  if (!cmd.isEmpty()) {
    QProcess::startDetached(cmd, {});
  }
  hide();
}

void Launcher::populateStubEntries()
{
  // Hardcoded stubs until .desktop scanning lands.
  struct Entry {
    const char *label;
    const char *command;
  };
  const Entry entries[] = {
      {"Terminal", "xterm"},
      {"Files", "dolphin"},
      {"Firefox", "firefox"},
      {"Settings (soon)", ""},
  };

  m_list->clear();
  for (const Entry &e : entries) {
    auto *item = new QListWidgetItem(QString::fromUtf8(e.label), m_list);
    item->setData(Qt::UserRole, QString::fromUtf8(e.command));
  }
  if (m_list->count() > 0) {
    m_list->setCurrentRow(0);
  }
}

} // namespace spike
