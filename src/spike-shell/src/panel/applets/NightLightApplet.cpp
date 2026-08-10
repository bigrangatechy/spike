#include "panel/applets/NightLightApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMetaObject>
#include <QPushButton>
#include <QRegularExpression>
#include <QSlider>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

QString kwinrcPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
         QStringLiteral("/kwinrc");
}

void nightLightLog(const QString &line)
{
  // Prefer /var/log/spike (adm-writable on installed); fall back to ~/.local/state.
  QString path = QStringLiteral("/var/log/spike/night-light.log");
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    const QString homeLog =
        QStandardPaths::writableLocation(QStandardPaths::GenericStateLocation) +
        QStringLiteral("/spike");
    QDir().mkpath(homeLog);
    path = homeLog + QStringLiteral("/night-light.log");
    f.setFileName(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
      return;
    }
  }
  QTextStream ts(&f);
  ts << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << ' ' << line << '\n';
}

/** Write/merge NightColor keys; ensure nightlight plugin is on (Mode=0 Constant). */
void writeNightColorConfig(bool enabled, int temperature)
{
  const QString path = kwinrcPath();
  QString text;
  {
    QFile in(path);
    if (in.open(QIODevice::ReadOnly | QIODevice::Text)) {
      text = QString::fromUtf8(in.readAll());
    }
  }

  auto upsertGroup = [](QString *doc, const QString &group, const QStringList &lines) {
    const QRegularExpression re(
        QStringLiteral(R"((?ms)^\[%1\][^\n]*\n(?:(?!^\[).*\n)*)").arg(QRegularExpression::escape(group)));
    QString block = QStringLiteral("[%1]\n").arg(group);
    for (const QString &l : lines) {
      block += l + QLatin1Char('\n');
    }
    if (re.match(*doc).hasMatch()) {
      *doc = doc->replace(re, block);
    } else {
      if (!doc->isEmpty() && !doc->endsWith(QLatin1Char('\n'))) {
        *doc += QLatin1Char('\n');
      }
      *doc += block;
    }
  };

  upsertGroup(&text, QStringLiteral("NightColor"),
              {QStringLiteral("Active=%1").arg(enabled ? QStringLiteral("true")
                                                       : QStringLiteral("false")),
               // KWin stores NightLightMode enum as int: Constant=0, DarkLight=1
               QStringLiteral("Mode=0"), QStringLiteral("NightTemperature=%1").arg(temperature),
               QStringLiteral("DayTemperature=6500")});

  // Ensure plugin flag without wiping other [Plugins] keys.
  if (text.contains(QLatin1String("[Plugins]"))) {
    if (text.contains(QLatin1String("nightlightEnabled="))) {
      text.replace(QRegularExpression(QStringLiteral(R"(nightlightEnabled=\S*)")),
                   QStringLiteral("nightlightEnabled=true"));
    } else {
      text.replace(QStringLiteral("[Plugins]"),
                   QStringLiteral("[Plugins]\nnightlightEnabled=true"));
    }
  } else {
    if (!text.isEmpty() && !text.endsWith(QLatin1Char('\n'))) {
      text += QLatin1Char('\n');
    }
    text += QStringLiteral("[Plugins]\nnightlightEnabled=true\n");
  }

  QFile out(path);
  if (out.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    QTextStream ts(&out);
    ts << text;
  }
}

bool reconfigureKwin()
{
  QDBusInterface kwin(QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
                      QStringLiteral("org.kde.KWin"), QDBusConnection::sessionBus());
  if (!kwin.isValid()) {
    return false;
  }
  kwin.call(QStringLiteral("reconfigure"));
  return true;
}

void callPreview(bool enabled, int temperature)
{
  if (enabled) {
    QDBusMessage preview = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"), QStringLiteral("/org/kde/KWin/NightLight"),
        QStringLiteral("org.kde.KWin.NightLight"), QStringLiteral("preview"));
    preview << static_cast<uint>(temperature);
    const QDBusMessage reply = QDBusConnection::sessionBus().call(preview);
    if (reply.type() == QDBusMessage::ErrorMessage) {
      nightLightLog(QStringLiteral("preview(%1) error: %2")
                        .arg(temperature)
                        .arg(reply.errorMessage()));
    } else {
      nightLightLog(QStringLiteral("preview(%1) ok").arg(temperature));
    }
  } else {
    QDBusMessage stop = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"), QStringLiteral("/org/kde/KWin/NightLight"),
        QStringLiteral("org.kde.KWin.NightLight"), QStringLiteral("stopPreview"));
    const QDBusMessage reply = QDBusConnection::sessionBus().call(stop);
    if (reply.type() == QDBusMessage::ErrorMessage) {
      nightLightLog(QStringLiteral("stopPreview error: %1").arg(reply.errorMessage()));
    } else {
      nightLightLog(QStringLiteral("stopPreview ok"));
    }
  }
}

} // namespace

NightLightApplet::NightLightApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("NightLightApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &NightLightApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(260);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Night Light"), m_popup));
  m_toggle = new QCheckBox(QStringLiteral("Enable Night Light"), m_popup);
  lay->addWidget(m_toggle);
  m_temp = new QSlider(Qt::Horizontal, m_popup);
  m_temp->setRange(3000, 6500);
  m_temp->setValue(m_temperature);
  m_tempLabel = new QLabel(m_popup);
  lay->addWidget(m_temp);
  lay->addWidget(m_tempLabel);
  auto *settings = new QPushButton(QStringLiteral("Night Light Settings"), m_popup);
  lay->addWidget(settings);
  connect(m_toggle, &QCheckBox::toggled, this, &NightLightApplet::onToggle);
  connect(m_temp, &QSlider::valueChanged, this, &NightLightApplet::onTemp);
  connect(settings, &QPushButton::clicked, this, [this]() {
    m_popup->hide();
    tray::openPanelSettings(this, QStringLiteral("night-light"));
  });
  refresh();
}

void NightLightApplet::setEnabledVisible(bool on)
{
  m_userVisible = on;
  setVisible(on);
}

void NightLightApplet::applyFromConfig(bool enabled, int temperature)
{
  m_enabled = enabled;
  m_temperature = qBound(3000, temperature, 6500);
  tryKwinSet(m_enabled, m_temperature);
  refresh();
}

bool NightLightApplet::tryKwinSet(bool enabled, int temperature)
{
  // KWin 6 NightLight D-Bus is mostly read-only; enablement lives in kwinrc
  // [NightColor] with Mode=Constant (0). preview() applies temperature immediately.
  nightLightLog(QStringLiteral("apply enabled=%1 temp=%2 kwinrc=%3")
                    .arg(enabled ? QStringLiteral("true") : QStringLiteral("false"))
                    .arg(temperature)
                    .arg(kwinrcPath()));
  writeNightColorConfig(enabled, temperature);
  const bool reconfigured = reconfigureKwin();
  nightLightLog(QStringLiteral("reconfigure=%1").arg(reconfigured ? QStringLiteral("ok")
                                                                  : QStringLiteral("fail")));
  callPreview(enabled, temperature);
  // Plugin may need a beat after reconfigure before preview sticks.
  QTimer::singleShot(250, qApp, [enabled, temperature]() { callPreview(enabled, temperature); });
  QTimer::singleShot(750, qApp, [enabled, temperature]() {
    reconfigureKwin();
    callPreview(enabled, temperature);
  });

  QDBusInterface night(QStringLiteral("org.kde.KWin"), QStringLiteral("/org/kde/KWin/NightLight"),
                       QStringLiteral("org.kde.KWin.NightLight"), QDBusConnection::sessionBus());
  const bool valid = night.isValid();
  nightLightLog(QStringLiteral("NightLight iface valid=%1")
                    .arg(valid ? QStringLiteral("true") : QStringLiteral("false")));
  return valid;
}

void NightLightApplet::refresh()
{
  if (!m_userVisible) {
    hide();
    return;
  }
  show();
  if (m_toggle) {
    m_toggle->blockSignals(true);
    m_toggle->setChecked(m_enabled);
    m_toggle->blockSignals(false);
  }
  if (m_temp) {
    m_temp->blockSignals(true);
    m_temp->setValue(m_temperature);
    m_temp->blockSignals(false);
  }
  if (m_tempLabel) {
    m_tempLabel->setText(QStringLiteral("%1 K").arg(m_temperature));
  }
  tray::setTrayIcon(this,
                    m_enabled ? QStringLiteral("redshift-status-on")
                              : QStringLiteral("redshift-status-off"),
                    m_enabled ? QStringLiteral("NL") : QStringLiteral("nl"));
  setToolTip(m_enabled ? QStringLiteral("Night Light on (%1 K)").arg(m_temperature)
                       : QStringLiteral("Night Light off"));
}

void NightLightApplet::onToggle(bool on)
{
  m_enabled = on;
  tryKwinSet(m_enabled, m_temperature);
  refresh();
  // Persist via Panel → spike-config so Settings stays in sync.
  QWidget *w = this;
  while (w) {
    if (w->objectName() == QLatin1String("SpikePanel")) {
      QMetaObject::invokeMethod(w, "persistNightLight", Qt::QueuedConnection, Q_ARG(bool, on),
                                Q_ARG(int, m_temperature));
      break;
    }
    w = w->parentWidget();
  }
}

void NightLightApplet::onTemp(int kelvin)
{
  m_temperature = kelvin;
  if (m_tempLabel) {
    m_tempLabel->setText(QStringLiteral("%1 K").arg(kelvin));
  }
  if (m_enabled) {
    tryKwinSet(true, kelvin);
    QWidget *w = this;
    while (w) {
      if (w->objectName() == QLatin1String("SpikePanel")) {
        QMetaObject::invokeMethod(w, "persistNightLight", Qt::QueuedConnection, Q_ARG(bool, true),
                                  Q_ARG(int, kelvin));
        break;
      }
      w = w->parentWidget();
    }
  }
}

void NightLightApplet::togglePopup()
{
  if (!m_popup) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  refresh();
  tray::placePopupAbove(this, m_popup);
  m_popup->show();
}

} // namespace spike
