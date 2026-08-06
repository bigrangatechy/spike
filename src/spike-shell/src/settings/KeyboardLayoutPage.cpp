#include "settings/KeyboardLayoutPage.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>
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
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(15000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

QString spikeLayoutConfig()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/spike-keyboard-layout.conf");
}

QStringList commonLayouts()
{
  return {
      QStringLiteral("us"),   QStringLiteral("gb"),   QStringLiteral("de"),
      QStringLiteral("fr"),   QStringLiteral("es"),   QStringLiteral("it"),
      QStringLiteral("pt"),   QStringLiteral("br"),   QStringLiteral("ru"),
      QStringLiteral("pl"),   QStringLiteral("nl"),   QStringLiteral("se"),
      QStringLiteral("no"),   QStringLiteral("dk"),   QStringLiteral("fi"),
      QStringLiteral("au"),   QStringLiteral("ca"),   QStringLiteral("jp"),
      QStringLiteral("kr"),   QStringLiteral("cn"),
  };
}

} // namespace

QWidget *makeKeyboardLayoutPage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Keyboard Layout</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Primary XKB layout for this session. System default can also be set "
                     "with localectl when permitted."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *layoutBox = new QComboBox(w);
  layoutBox->setEditable(true);
  form->addRow(QStringLiteral("Layout"), layoutBox);
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

  auto load = [layoutBox, status, statusBar]() {
    if (layoutBox->count() == 0) {
      QStringList layouts = commonLayouts();
      const QString listed =
          runCapture(QStringLiteral("localectl"), {QStringLiteral("list-x11-keymap-layouts")});
      if (!listed.isEmpty()) {
        const QStringList fromSystem = listed.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        // Keep list usable on Tier-1: prefer common, then a capped system list.
        for (const QString &l : fromSystem) {
          if (!layouts.contains(l) && layouts.size() < 80) {
            layouts.append(l);
          }
        }
      }
      layoutBox->addItems(layouts);
    }

    QSettings s(spikeLayoutConfig(), QSettings::IniFormat);
    QString cur = s.value(QStringLiteral("layout")).toString();
    if (cur.isEmpty()) {
      cur = runCapture(QStringLiteral("localectl"),
                       {QStringLiteral("status")});
      // Parse "X11 Layout: us"
      for (const QString &line : cur.split(QLatin1Char('\n'))) {
        if (line.contains(QLatin1String("X11 Layout"), Qt::CaseInsensitive) ||
            line.contains(QLatin1String("VC Keymap"), Qt::CaseInsensitive)) {
          const QStringList parts = line.split(QLatin1Char(':'));
          if (parts.size() >= 2) {
            cur = parts.last().trimmed().split(QLatin1Char(',')).first().trimmed();
            break;
          }
        }
      }
    }
    if (cur.isEmpty()) {
      cur = QStringLiteral("us");
    }
    int idx = layoutBox->findText(cur);
    if (idx < 0) {
      layoutBox->insertItem(0, cur);
      idx = 0;
    }
    layoutBox->setCurrentIndex(idx);
    status->setText(QStringLiteral("Current layout preference: %1").arg(cur));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Keyboard layout loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w, [layoutBox, status, statusBar]() {
    const QString layout = layoutBox->currentText().trimmed();
    if (layout.isEmpty()) {
      status->setText(QStringLiteral("Layout required"));
      return;
    }
    QSettings s(spikeLayoutConfig(), QSettings::IniFormat);
    s.setValue(QStringLiteral("layout"), layout);
    s.sync();

    // Session apply (X11 / XWayland). Wayland-native clients follow compositor keymap.
    runOk(QStringLiteral("setxkbmap"), {QStringLiteral("-layout"), layout});

    // Persist system default when allowed (live polkit / sudo -n).
    if (!runOk(QStringLiteral("localectl"),
               {QStringLiteral("set-x11-keymap"), layout})) {
      runOk(QStringLiteral("sudo"),
            {QStringLiteral("-n"), QStringLiteral("localectl"), QStringLiteral("set-x11-keymap"),
             layout});
    }

    // KWin / Plasma-compatible layout list for next session.
    QSettings kx(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
                     QStringLiteral("/kxkbrc"),
                 QSettings::IniFormat);
    kx.beginGroup(QStringLiteral("Layout"));
    kx.setValue(QStringLiteral("LayoutList"), layout);
    kx.setValue(QStringLiteral("Use"), true);
    kx.endGroup();
    kx.sync();

    status->setText(QStringLiteral("Layout %1 applied for this session").arg(layout));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Keyboard layout applied"));
    }
  });

  load();
  return w;
}

} // namespace spike
