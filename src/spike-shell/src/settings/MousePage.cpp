#include "settings/MousePage.hpp"

#include "settings/InputConfig.hpp"

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

namespace spike {

namespace {

QString kcminputPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/kcminputrc");
}

} // namespace

QWidget *makeMousePage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Mouse / Touchpad</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Pointer speed and tap-to-click. Settings are written for KWin "
                     "(device-specific Libinput sections). Live apply is best-effort — "
                     "if feel does not change, restart spike-session."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  const QVector<PointerDevice> devices = listPointerDevices();
  bool hasPad = false;
  for (const PointerDevice &d : devices) {
    if (d.touchpad) {
      hasPad = true;
      break;
    }
  }
  // Laptop from smoke had HTIX5288 without "touchpad" in the name — treat any
  // discovered pointer as eligible for tap when only one device exists.
  if (!hasPad && !devices.isEmpty()) {
    hasPad = true;
  }

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

  tap->setEnabled(hasPad);
  if (!hasPad) {
    tap->setToolTip(QStringLiteral("No pointer/touchpad detected under /sys/class/input"));
  }

  auto *devs = new QLabel(w);
  devs->setWordWrap(true);
  if (devices.isEmpty()) {
    devs->setText(QStringLiteral("No pointer devices discovered yet."));
  } else {
    QStringList lines;
    for (const PointerDevice &d : devices) {
      lines << QStringLiteral("• %1 (vid=%2 pid=%3%4)")
                   .arg(d.name)
                   .arg(d.vendorId)
                   .arg(d.productId)
                   .arg(d.touchpad ? QStringLiteral(", touchpad") : QString());
    }
    devs->setText(lines.join(QLatin1Char('\n')));
  }
  lay->addWidget(devs);

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
    accelVal->setText(QString::number(accel->value() / 100.0, 'f', 2));
  };
  QObject::connect(accel, &QSlider::valueChanged, w, syncAccelLabel);

  auto load = [accel, tap, status, statusBar, syncAccelLabel]() {
    QSettings s(kcminputPath(), QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Mouse"));
    const double a = s.value(QStringLiteral("PointerAcceleration"), 0.0).toDouble();
    accel->setValue(qRound(a * 100.0));
    s.endGroup();
    s.beginGroup(QStringLiteral("Libinput"));
    tap->setChecked(s.value(QStringLiteral("TapToClick"), true).toBool());
    s.endGroup();
    syncAccelLabel();
    status->setText(QStringLiteral("Loaded from %1").arg(kcminputPath()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Mouse settings loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w, [accel, tap, status, statusBar]() {
    bool live = false;
    const QString msg =
        applyPointerSettings(accel->value() / 100.0, tap->isChecked(), &live);
    status->setText(msg);
    if (statusBar) {
      statusBar->setText(live ? QStringLiteral("Mouse: saved + KWin reconfigure sent")
                              : QStringLiteral("Mouse: saved (restart session to feel change)"));
    }
  });

  load();
  return w;
}

} // namespace spike
