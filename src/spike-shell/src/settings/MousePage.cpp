#include "settings/MousePage.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>

#include <QDir>
#include <QFile>

namespace spike {

namespace {

QString kcminputPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/kcminputrc");
}

bool touchpadLikelyPresent()
{
  const QDir dir(QStringLiteral("/sys/class/input"));
  const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &e : entries) {
    QFile name(dir.filePath(e + QStringLiteral("/device/name")));
    if (!name.open(QIODevice::ReadOnly | QIODevice::Text)) {
      continue;
    }
    const QString n = QString::fromUtf8(name.readAll()).toLower();
    if (n.contains(QLatin1String("touchpad")) || n.contains(QLatin1String("synaptics")) ||
        n.contains(QLatin1String("trackpad")) || n.contains(QLatin1String("clickpad"))) {
      return true;
    }
  }
  return false;
}

} // namespace

QWidget *makeMousePage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Mouse / Touchpad</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Pointer acceleration and touchpad tap-to-click "
                     "(~/.config/kcminputrc for KWin / libinput)."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *accel = new QSlider(Qt::Horizontal, w);
  accel->setRange(-100, 100);
  accel->setTickPosition(QSlider::TicksBelow);
  accel->setTickInterval(25);
  auto *accelVal = new QLabel(w);
  auto *accelRow = new QHBoxLayout();
  accelRow->addWidget(accel, 1);
  accelRow->addWidget(accelVal);
  form->addRow(QStringLiteral("Pointer speed"), accelRow);

  auto *tap = new QCheckBox(QStringLiteral("Tap to click (touchpad)"), w);
  form->addRow(QString(), tap);
  lay->addLayout(form);

  const bool hasPad = touchpadLikelyPresent();
  tap->setEnabled(hasPad);
  if (!hasPad) {
    tap->setToolTip(QStringLiteral("No touchpad detected"));
  }

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

  auto syncAccelLabel = [accel, accelVal]() {
    const double v = accel->value() / 100.0;
    accelVal->setText(QString::number(v, 'f', 2));
  };
  QObject::connect(accel, &QSlider::valueChanged, w, syncAccelLabel);

  auto load = [accel, tap, status, statusBar, syncAccelLabel]() {
    QSettings s(kcminputPath(), QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Mouse"));
    const double a = s.value(QStringLiteral("PointerAcceleration"), 0.0).toDouble();
    accel->setValue(qRound(a * 100.0));
    s.endGroup();
    s.beginGroup(QStringLiteral("Libinput"));
    // Flat key used when device-specific sections are unknown at apply time.
    tap->setChecked(s.value(QStringLiteral("TapToClick"), true).toBool());
    s.endGroup();
    syncAccelLabel();
    status->setText(QStringLiteral("Loaded %1").arg(kcminputPath()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Mouse settings loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w, [accel, tap, status, statusBar]() {
    QSettings s(kcminputPath(), QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Mouse"));
    s.setValue(QStringLiteral("PointerAcceleration"), accel->value() / 100.0);
    s.endGroup();
    s.beginGroup(QStringLiteral("Libinput"));
    s.setValue(QStringLiteral("TapToClick"), tap->isChecked());
    s.endGroup();
    s.sync();
    status->setText(QStringLiteral("Saved. Pointer settings apply for new KWin sessions."));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Mouse settings saved"));
    }
  });

  load();
  return w;
}

} // namespace spike
