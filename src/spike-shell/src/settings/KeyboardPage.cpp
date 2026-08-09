#include "settings/KeyboardPage.hpp"

#include "settings/InputConfig.hpp"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
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

QWidget *makeKeyboardPage(QWidget *parent, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Keyboard</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral("Key repeat delay and rate. Saved for KWin and applied live via "
                     "KWin InputDevice when available (also xset for XWayland)."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *delay = new QSpinBox(w);
  delay->setRange(100, 2000);
  delay->setSingleStep(50);
  delay->setSuffix(QStringLiteral(" ms"));
  form->addRow(QStringLiteral("Repeat delay"), delay);

  auto *rate = new QSpinBox(w);
  rate->setRange(1, 100);
  rate->setSuffix(QStringLiteral(" /s"));
  form->addRow(QStringLiteral("Repeat rate"), rate);
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

  auto load = [delay, rate, status, statusBar]() {
    QSettings s(kcminputPath(), QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Keyboard"));
    delay->setValue(s.value(QStringLiteral("RepeatDelay"), 600).toInt());
    rate->setValue(s.value(QStringLiteral("RepeatRate"), 25).toInt());
    s.endGroup();
    status->setText(QStringLiteral("Loaded from %1").arg(kcminputPath()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Keyboard settings loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(apply, &QPushButton::clicked, w, [delay, rate, status, statusBar]() {
    bool live = false;
    const QString msg = applyKeyboardRepeat(delay->value(), rate->value(), &live);
    status->setText(msg);
    if (statusBar) {
      statusBar->setText(live ? QStringLiteral("Keyboard: saved + KWin reconfigure sent")
                              : QStringLiteral("Keyboard: saved (restart session if unchanged)"));
    }
  });

  load();
  return w;
}

} // namespace spike
