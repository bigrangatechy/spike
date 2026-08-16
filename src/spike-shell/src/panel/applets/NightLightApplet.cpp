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
#include <QProcess>
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

/** Best-effort kwinrc for Plasma-shaped installs; Spike applies gamma via wlsunset. */
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
               QStringLiteral("Mode=0"), QStringLiteral("NightTemperature=%1").arg(temperature),
               QStringLiteral("DayTemperature=6500")});

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

void reconfigureKwin()
{
  QDBusInterface kwin(QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"),
                      QStringLiteral("org.kde.KWin"), QDBusConnection::sessionBus());
  if (kwin.isValid()) {
    kwin.call(QStringLiteral("reconfigure"));
  }
}

/** KWin registers Night Light as its *own* bus name, not under org.kde.KWin. */
bool callKwinNightLight(bool enabled, int temperature)
{
  const QString service = QStringLiteral("org.kde.KWin.NightLight");
  const QString path = QStringLiteral("/org/kde/KWin/NightLight");
  const QString iface = QStringLiteral("org.kde.KWin.NightLight");

  // Probe: service must own the name (plugin loaded).
  QDBusInterface probe(service, path, iface, QDBusConnection::sessionBus());
  if (!probe.isValid()) {
    nightLightLog(QStringLiteral("KWin NightLight service absent (plugin not loaded)"));
    return false;
  }

  if (enabled) {
    QDBusMessage preview = QDBusMessage::createMethodCall(service, path, iface, QStringLiteral("preview"));
    preview << static_cast<uint>(temperature);
    const QDBusMessage reply = QDBusConnection::sessionBus().call(preview);
    if (reply.type() == QDBusMessage::ErrorMessage) {
      nightLightLog(QStringLiteral("preview(%1) error: %2").arg(temperature).arg(reply.errorMessage()));
      return false;
    }
    nightLightLog(QStringLiteral("preview(%1) ok").arg(temperature));
  } else {
    QDBusMessage stop =
        QDBusMessage::createMethodCall(service, path, iface, QStringLiteral("stopPreview"));
    const QDBusMessage reply = QDBusConnection::sessionBus().call(stop);
    if (reply.type() == QDBusMessage::ErrorMessage) {
      nightLightLog(QStringLiteral("stopPreview error: %1").arg(reply.errorMessage()));
      return false;
    }
    nightLightLog(QStringLiteral("stopPreview ok"));
  }
  return true;
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
  applyNightLight(m_enabled, m_temperature);
  refresh();
}

void NightLightApplet::stopGammaHelper()
{
  if (!m_gamma) {
    return;
  }
  m_gamma->terminate();
  if (!m_gamma->waitForFinished(1500)) {
    m_gamma->kill();
    m_gamma->waitForFinished(500);
  }
  m_gamma->deleteLater();
  m_gamma = nullptr;
}

bool NightLightApplet::startGammaHelper(int temperature)
{
  stopGammaHelper();
  const QString wlsunset = QStandardPaths::findExecutable(QStringLiteral("wlsunset"));
  const QString gammastep = QStandardPaths::findExecutable(QStringLiteral("gammastep"));

  m_gamma = new QProcess(this);
  m_gamma->setProcessChannelMode(QProcess::MergedChannels);

  if (!wlsunset.isEmpty()) {
    // Same day/night temps → constant CCT; process must stay up (wlr-gamma).
    m_gamma->start(wlsunset, {QStringLiteral("-t"), QString::number(temperature),
                              QStringLiteral("-T"), QString::number(temperature)});
    nightLightLog(QStringLiteral("start wlsunset -t/-T %1").arg(temperature));
  } else if (!gammastep.isEmpty()) {
    // Equal day/night + equator → constant temperature continuously.
    m_gamma->start(gammastep,
                   {QStringLiteral("-m"), QStringLiteral("wayland"), QStringLiteral("-l"),
                    QStringLiteral("0:0"), QStringLiteral("-t"),
                    QStringLiteral("%1:%1").arg(temperature)});
    nightLightLog(QStringLiteral("start gammastep wayland %1K").arg(temperature));
  } else {
    nightLightLog(QStringLiteral("no wlsunset/gammastep in PATH"));
    m_gamma->deleteLater();
    m_gamma = nullptr;
    return false;
  }

  if (!m_gamma->waitForStarted(2000)) {
    nightLightLog(QStringLiteral("gamma helper failed to start: %1")
                      .arg(QString::fromUtf8(m_gamma->readAll())));
    m_gamma->deleteLater();
    m_gamma = nullptr;
    return false;
  }
  return true;
}

bool NightLightApplet::tryKwinNightLight(bool enabled, int temperature)
{
  writeNightColorConfig(enabled, temperature);
  reconfigureKwin();
  return callKwinNightLight(enabled, temperature);
}

bool NightLightApplet::applyNightLight(bool enabled, int temperature)
{
  nightLightLog(QStringLiteral("apply enabled=%1 temp=%2")
                    .arg(enabled ? QStringLiteral("true") : QStringLiteral("false"))
                    .arg(temperature));

  // Prefer real gamma via wlsunset — Spike's kwin-wayland package does not ship
  // the NightLight plugin, so kwinrc/D-Bus alone never changes the screen.
  bool ok = false;
  if (enabled) {
    ok = startGammaHelper(temperature);
  } else {
    stopGammaHelper();
    ok = true;
    nightLightLog(QStringLiteral("gamma helper stopped"));
  }

  // Best-effort KWin path (correct bus name) when/if the plugin appears.
  tryKwinNightLight(enabled, temperature);

  return ok;
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
  applyNightLight(m_enabled, m_temperature);
  refresh();
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
    applyNightLight(true, kelvin);
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
