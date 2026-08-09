#include "panel/applets/BrightnessApplet.hpp"

#include "panel/applets/TrayHelpers.hpp"

#include <QDir>
#include <QFile>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

BrightnessApplet::BrightnessApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("BrightnessApplet"));
  setFlat(true);
  setFixedHeight(26);
  setMinimumWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));
  connect(this, &QPushButton::clicked, this, &BrightnessApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setMinimumWidth(240);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);
  lay->addWidget(new QLabel(QStringLiteral("Brightness"), m_popup));
  m_slider = new QSlider(Qt::Horizontal, m_popup);
  m_slider->setRange(1, 100);
  m_pct = new QLabel(m_popup);
  lay->addWidget(m_slider);
  lay->addWidget(m_pct);
  connect(m_slider, &QSlider::valueChanged, this, &BrightnessApplet::onSlider);

  if (!discover()) {
    hide();
    return;
  }
  refresh();
}

bool BrightnessApplet::discover()
{
  const QDir dir(QStringLiteral("/sys/class/backlight"));
  if (!dir.exists()) {
    return false;
  }
  const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &e : entries) {
    const QString base = dir.absoluteFilePath(e);
    const QString bright = base + QStringLiteral("/brightness");
    const QString maxb = base + QStringLiteral("/max_brightness");
    if (QFile::exists(bright) && QFile::exists(maxb)) {
      m_brightnessPath = bright;
      m_maxPath = maxb;
      m_hasBacklight = true;
      return true;
    }
  }
  return false;
}

int BrightnessApplet::readMax() const
{
  QFile f(m_maxPath);
  if (!f.open(QIODevice::ReadOnly)) {
    return 100;
  }
  return QString::fromUtf8(f.readAll()).trimmed().toInt();
}

int BrightnessApplet::readBrightness() const
{
  QFile f(m_brightnessPath);
  if (!f.open(QIODevice::ReadOnly)) {
    return 0;
  }
  return QString::fromUtf8(f.readAll()).trimmed().toInt();
}

bool BrightnessApplet::writeBrightness(int value)
{
  QFile f(m_brightnessPath);
  if (!f.open(QIODevice::WriteOnly)) {
    return false;
  }
  f.write(QByteArray::number(value));
  return true;
}

void BrightnessApplet::refresh()
{
  if (!m_hasBacklight) {
    hide();
    return;
  }
  show();
  const int maxv = qMax(1, readMax());
  const int cur = readBrightness();
  const int pct = qBound(1, qRound(100.0 * cur / maxv), 100);
  if (m_slider && !m_slider->isSliderDown()) {
    m_slider->blockSignals(true);
    m_slider->setValue(pct);
    m_slider->blockSignals(false);
  }
  if (m_pct) {
    m_pct->setText(QStringLiteral("%1%").arg(pct));
  }
  tray::setTrayIcon(this, QStringLiteral("display-brightness"), QStringLiteral("%1%").arg(pct));
  setToolTip(QStringLiteral("Brightness %1%").arg(pct));
}

void BrightnessApplet::onSlider(int value)
{
  const int maxv = qMax(1, readMax());
  const int raw = qBound(1, qRound(maxv * (value / 100.0)), maxv);
  writeBrightness(raw);
  if (m_pct) {
    m_pct->setText(QStringLiteral("%1%").arg(value));
  }
  tray::setTrayIcon(this, QStringLiteral("display-brightness"), QStringLiteral("%1%").arg(value));
}

void BrightnessApplet::togglePopup()
{
  if (!m_hasBacklight || !m_popup) {
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
