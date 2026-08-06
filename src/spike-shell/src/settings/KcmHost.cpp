#include "settings/KcmHost.hpp"

#include <KCModule>
#include <KCModuleLoader>
#include <KPluginMetaData>

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace spike {

namespace {

KPluginMetaData findKcmMetaData(const QString &pluginId)
{
  // Prefer QWidgets systemsettings plugins, then the general pool (QML KCMs wrap OK).
  const QStringList dirs = {
      QStringLiteral("plasma/kcms/systemsettings_qwidgets"),
      QStringLiteral("plasma/kcms/systemsettings"),
      QStringLiteral("plasma/kcms/systemsettings_sidebar"),
      QStringLiteral("kcms"),
  };
  for (const QString &dir : dirs) {
    const KPluginMetaData md = KPluginMetaData::findPluginById(dir, pluginId);
    if (md.isValid()) {
      return md;
    }
  }
  return {};
}

} // namespace

KcmHost::KcmHost(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("SpikeKcmHost"));
  m_layout = new QVBoxLayout(this);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(8);

  m_error = new QLabel(this);
  m_error->setWordWrap(true);
  m_error->setObjectName(QStringLiteral("KcmHostError"));
  m_error->hide();
  m_layout->addWidget(m_error);

  auto *scroll = new QScrollArea(this);
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  m_moduleContainer = new QWidget(scroll);
  m_moduleContainer->setLayout(new QVBoxLayout());
  m_moduleContainer->layout()->setContentsMargins(0, 0, 0, 0);
  scroll->setWidget(m_moduleContainer);
  m_layout->addWidget(scroll, 1);

  auto *footer = new QHBoxLayout();
  footer->addStretch(1);
  m_defaults = new QPushButton(QStringLiteral("Defaults"), this);
  m_reset = new QPushButton(QStringLiteral("Reset"), this);
  m_apply = new QPushButton(QStringLiteral("Apply"), this);
  m_apply->setDefault(true);
  footer->addWidget(m_defaults);
  footer->addWidget(m_reset);
  footer->addWidget(m_apply);
  m_layout->addLayout(footer);

  connect(m_apply, &QPushButton::clicked, this, &KcmHost::apply);
  connect(m_reset, &QPushButton::clicked, this, &KcmHost::reset);
  connect(m_defaults, &QPushButton::clicked, this, &KcmHost::defaults);

  updateButtons();
}

KcmHost::~KcmHost()
{
  clearModule();
}

void KcmHost::clearModule()
{
  if (!m_module) {
    return;
  }
  if (QWidget *w = m_module->widget()) {
    w->setParent(nullptr);
  }
  delete m_module;
  m_module = nullptr;
  // Clear leftover widgets in container.
  if (auto *lay = qobject_cast<QVBoxLayout *>(m_moduleContainer->layout())) {
    while (QLayoutItem *item = lay->takeAt(0)) {
      if (QWidget *w = item->widget()) {
        w->deleteLater();
      }
      delete item;
    }
  }
}

bool KcmHost::loadPlugin(const QString &pluginId)
{
  clearModule();
  m_pluginId = pluginId;
  m_error->hide();

  const KPluginMetaData md = findKcmMetaData(pluginId);
  if (!md.isValid()) {
    m_error->setText(
        QStringLiteral(
            "KDE module <b>%1</b> is not installed yet.\n"
            "Spike will host it here once the package is on the image "
            "(add packages as we flesh out each page).")
            .arg(pluginId));
    m_error->setTextFormat(Qt::RichText);
    m_error->show();
    updateButtons();
    return false;
  }

  m_module = KCModuleLoader::loadModule(md, m_moduleContainer);
  if (!m_module) {
    m_error->setText(QStringLiteral("Failed to load KCM plugin %1.").arg(pluginId));
    m_error->show();
    updateButtons();
    return false;
  }

  QWidget *w = m_module->widget();
  if (!w) {
    m_error->setText(QStringLiteral("KCM %1 loaded but has no widget.").arg(pluginId));
    m_error->show();
    clearModule();
    updateButtons();
    return false;
  }

  auto *lay = qobject_cast<QVBoxLayout *>(m_moduleContainer->layout());
  lay->addWidget(w);
  m_module->load();

  connect(m_module, &KCModule::needsSaveChanged, this, [this]() { updateButtons(); });
  updateButtons();
  return true;
}

void KcmHost::apply()
{
  if (!m_module) {
    return;
  }
  m_module->save();
  updateButtons();
}

void KcmHost::reset()
{
  if (!m_module) {
    return;
  }
  m_module->load();
  updateButtons();
}

void KcmHost::defaults()
{
  if (!m_module) {
    return;
  }
  m_module->defaults();
  updateButtons();
}

void KcmHost::updateButtons()
{
  const bool ok = m_module != nullptr;
  const bool dirty = ok && m_module->needsSave();
  m_apply->setEnabled(ok && dirty);
  m_reset->setEnabled(ok && dirty);
  m_defaults->setEnabled(ok);
}

} // namespace spike
