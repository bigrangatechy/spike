#include "settings/AccessibilityPage.hpp"

#include "settings/AppearanceLive.hpp"
#include "settings/ConfigClient.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

bool runOk(const QString &program, const QStringList &args)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(10000)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

QStringList buildXkbOptions(bool sticky, bool slow, bool bounce, bool mouse)
{
  QStringList opts;
  // Clear AccessX-related options first via empty then set — setxkbmap -option clears.
  if (sticky) {
    opts << QStringLiteral("stickykeys");
  }
  if (slow) {
    opts << QStringLiteral("slowkeys");
  }
  if (bounce) {
    opts << QStringLiteral("bouncekeys");
  }
  if (mouse) {
    opts << QStringLiteral("mousekeys");
  }
  return opts;
}

bool applyXkbAccess(bool sticky, bool slow, bool bounce, bool mouse, QString *error)
{
  // Reset accessx options, then apply selected.
  if (!runOk(QStringLiteral("setxkbmap"), {QStringLiteral("-option")})) {
    if (error) {
      *error = QStringLiteral("setxkbmap -option (clear) failed");
    }
    return false;
  }
  const QStringList opts = buildXkbOptions(sticky, slow, bounce, mouse);
  for (const QString &opt : opts) {
    if (!runOk(QStringLiteral("setxkbmap"), {QStringLiteral("-option"), opt})) {
      if (error) {
        *error = QStringLiteral("setxkbmap -option %1 failed").arg(opt);
      }
      return false;
    }
  }
  return true;
}

bool startDetachedFirst(const QStringList &candidates)
{
  for (const QString &bin : candidates) {
    if (QProcess::startDetached(bin, {})) {
      return true;
    }
  }
  return false;
}

bool startOnScreenKeyboard(QString *detail)
{
  const bool wayland = qEnvironmentVariableIsSet("WAYLAND_DISPLAY");
  if (wayland) {
    // maliit-keyboard expects a maliit server on Wayland.
    QProcess::startDetached(QStringLiteral("maliit-server-qt6"), {});
    QProcess::startDetached(QStringLiteral("maliit-server"), {});
    if (QProcess::startDetached(QStringLiteral("maliit-keyboard"), {})) {
      if (detail) {
        *detail = QStringLiteral("Started maliit-keyboard");
      }
      return true;
    }
  }
  if (startDetachedFirst({QStringLiteral("onboard"), QStringLiteral("maliit-keyboard"),
                          QStringLiteral("florence")})) {
    if (detail) {
      *detail = QStringLiteral("Started on-screen keyboard");
    }
    return true;
  }
  if (detail) {
    *detail = QStringLiteral(
        "No on-screen keyboard found (need maliit-keyboard or onboard on the image).");
  }
  return false;
}

} // namespace

QWidget *makeAccessibilityPage(QWidget *parent, ConfigClient *config, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>Accessibility</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Keyboard AccessX options via setxkbmap (not kcm_access). Magnifier / screen reader "
          "launch external tools when installed. High contrast applies Spike Shell chrome live."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *form = new QFormLayout();
  auto *sticky = new QCheckBox(QStringLiteral("Sticky keys"), w);
  auto *slow = new QCheckBox(QStringLiteral("Slow keys"), w);
  auto *bounce = new QCheckBox(QStringLiteral("Bounce keys"), w);
  auto *mouse = new QCheckBox(QStringLiteral("Mouse keys (keypad moves pointer)"), w);
  auto *visualBell = new QCheckBox(QStringLiteral("Visual bell (prefer flash over beep)"), w);
  auto *highContrast = new QCheckBox(QStringLiteral("High contrast (Spike Shell chrome)"), w);
  form->addRow(QString(), sticky);
  form->addRow(QString(), slow);
  form->addRow(QString(), bounce);
  form->addRow(QString(), mouse);
  form->addRow(QString(), visualBell);
  form->addRow(QString(), highContrast);
  lay->addLayout(form);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *apply = new QPushButton(QStringLiteral("Apply"), w);
  auto *orca = new QPushButton(QStringLiteral("Screen reader (Orca)…"), w);
  auto *osk = new QPushButton(QStringLiteral("On-screen keyboard…"), w);
  auto *magnifier = new QPushButton(QStringLiteral("Open Magnifier…"), w);
  row->addWidget(reload);
  row->addWidget(apply);
  row->addWidget(orca);
  row->addWidget(osk);
  row->addWidget(magnifier);
  row->addStretch(1);
  lay->addLayout(row);
  lay->addStretch(1);

  auto load = [config, sticky, slow, bounce, mouse, visualBell, highContrast, status, statusBar]() {
    if (!config) {
      status->setText(QStringLiteral("spike-config unavailable — using defaults"));
      return;
    }
    QString err;
    const QString json = config->getModuleState(QStringLiteral("accessibility"), &err);
    if (json.isEmpty()) {
      // Module may be missing on older state.json — prefs still editable.
      status->setText(QStringLiteral("No accessibility module yet (%1) — Apply will create keys.")
                          .arg(err));
      return;
    }
    const QJsonObject o = QJsonDocument::fromJson(json.toUtf8()).object();
    sticky->setChecked(o.value(QStringLiteral("sticky_keys")).toBool(false));
    slow->setChecked(o.value(QStringLiteral("slow_keys")).toBool(false));
    bounce->setChecked(o.value(QStringLiteral("bounce_keys")).toBool(false));
    mouse->setChecked(o.value(QStringLiteral("mouse_keys")).toBool(false));
    visualBell->setChecked(o.value(QStringLiteral("visual_bell")).toBool(false));
    highContrast->setChecked(o.value(QStringLiteral("high_contrast")).toBool(false));
    status->setText(QStringLiteral("Loaded accessibility prefs"));
    if (statusBar) {
      statusBar->setText(QStringLiteral("Accessibility loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  QObject::connect(
      apply, &QPushButton::clicked, w,
      [config, sticky, slow, bounce, mouse, visualBell, highContrast, status, statusBar, load]() {
        QString xerr;
        const bool xok = applyXkbAccess(sticky->isChecked(), slow->isChecked(), bounce->isChecked(),
                                        mouse->isChecked(), &xerr);
        if (config) {
          QString err;
          auto set = [&](const QString &key, const QVariant &v) -> bool {
            // Ensure module exists: first GetState won't create it — SetSetting needs module.
            if (!config->setSetting(QStringLiteral("accessibility"), key, v, &err)) {
              status->setText(QStringLiteral("SetSetting(%1) failed: %2").arg(key, err));
              return false;
            }
            return true;
          };
          // If module missing, SetSetting fails with unknown module — seed via privacy-like
          // workaround: try once; show error.
          if (!set(QStringLiteral("sticky_keys"), sticky->isChecked()) ||
              !set(QStringLiteral("slow_keys"), slow->isChecked()) ||
              !set(QStringLiteral("bounce_keys"), bounce->isChecked()) ||
              !set(QStringLiteral("mouse_keys"), mouse->isChecked()) ||
              !set(QStringLiteral("visual_bell"), visualBell->isChecked()) ||
              !set(QStringLiteral("high_contrast"), highContrast->isChecked())) {
            // Still report xkb result.
            if (xok) {
              status->setText(status->text() +
                              QStringLiteral(" (session xkb applied; persist failed)"));
            }
            return;
          }
        }
        if (visualBell->isChecked()) {
          // Best-effort X11 visual bell; Wayland may ignore.
          runOk(QStringLiteral("xset"), {QStringLiteral("b"), QStringLiteral("off")});
        }
        applyShellChromeLive(qApp, QStringLiteral("#6d4aff"), qApp->font().pointSize(),
                             highContrast->isChecked());
        status->setText(xok ? QStringLiteral(
                                  "Saved + setxkbmap AccessX applied. High contrast "
                                  "applied to Spike Shell chrome (other apps later).")
                            : QStringLiteral("Saved prefs, but setxkbmap failed: %1").arg(xerr));
        if (statusBar) {
          statusBar->setText(QStringLiteral("Accessibility applied"));
        }
        load();
      });
  QObject::connect(orca, &QPushButton::clicked, w, [status]() {
    if (startDetachedFirst({QStringLiteral("orca")})) {
      status->setText(QStringLiteral("Started Orca"));
      return;
    }
    status->setText(QStringLiteral("Orca not installed (apt install orca)."));
  });
  QObject::connect(osk, &QPushButton::clicked, w, [status]() {
    QString detail;
    if (startOnScreenKeyboard(&detail)) {
      status->setText(detail);
      return;
    }
    status->setText(detail);
  });
  QObject::connect(magnifier, &QPushButton::clicked, w, [status]() {
    if (startDetachedFirst({QStringLiteral("kmag"), QStringLiteral("gnome-magnifier"),
                            QStringLiteral("magnus")})) {
      status->setText(QStringLiteral("Started magnifier"));
      return;
    }
    status->setText(QStringLiteral("No magnifier found (try: apt install kmag)."));
  });

  load();
  return w;
}

} // namespace spike
