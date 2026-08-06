#include "panel/applets/VolumeApplet.hpp"

#include "audio/VolumeClient.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>

namespace spike {

VolumeApplet::VolumeApplet(QWidget *parent)
  : QPushButton(parent)
{
  setObjectName(QStringLiteral("VolumeApplet"));
  setFlat(true);
  setFixedHeight(26);
  setFixedWidth(34);
  setCursor(Qt::PointingHandCursor);
  setIconSize(QSize(20, 20));

  m_client = new VolumeClient(this);
  connect(m_client, &VolumeClient::changed, this, &VolumeApplet::refresh);
  connect(this, &QPushButton::clicked, this, &VolumeApplet::togglePopup);

  m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
  m_popup->setObjectName(QStringLiteral("VolumePopup"));
  m_popup->setMinimumWidth(240);
  auto *lay = new QVBoxLayout(m_popup);
  lay->setContentsMargins(12, 10, 12, 10);

  auto *title = new QLabel(QStringLiteral("Volume"), m_popup);
  lay->addWidget(title);

  auto *row = new QHBoxLayout();
  m_slider = new QSlider(Qt::Horizontal, m_popup);
  m_slider->setRange(0, 100);
  m_pctLabel = new QLabel(m_popup);
  m_pctLabel->setMinimumWidth(40);
  row->addWidget(m_slider, 1);
  row->addWidget(m_pctLabel);
  lay->addLayout(row);

  auto *mute = new QPushButton(QStringLiteral("Mute"), m_popup);
  lay->addWidget(mute);
  auto *soundSettings = new QPushButton(QStringLiteral("Sound Settings"), m_popup);
  lay->addWidget(soundSettings);

  connect(m_slider, &QSlider::sliderReleased, this, [this]() {
    if (m_client) {
      m_client->setVolumePercent(m_slider->value());
    }
  });
  connect(mute, &QPushButton::clicked, this, [this]() {
    if (m_client) {
      m_client->toggleMute();
    }
  });
  connect(soundSettings, &QPushButton::clicked, this, &VolumeApplet::openSoundSettings);

  auto *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, m_client, &VolumeClient::refresh);
  timer->start(2000);

  refresh();
}

void VolumeApplet::wheelEvent(QWheelEvent *event)
{
  if (!m_client) {
    return;
  }
  const int delta = event->angleDelta().y() > 0 ? 5 : -5;
  m_client->adjustBy(delta);
  event->accept();
}

void VolumeApplet::updateIcon()
{
  QString name = QStringLiteral("audio-volume-muted");
  if (m_client && !m_client->muted()) {
    const int v = m_client->volumePercent();
    if (v >= 70) {
      name = QStringLiteral("audio-volume-high");
    } else if (v >= 30) {
      name = QStringLiteral("audio-volume-medium");
    } else if (v > 0) {
      name = QStringLiteral("audio-volume-low");
    } else {
      name = QStringLiteral("audio-volume-muted");
    }
  }
  const QIcon icon = QIcon::fromTheme(name);
  if (!icon.isNull()) {
    setIcon(icon);
    setText(QString());
  } else {
    setIcon(QIcon());
    setText(m_client && m_client->muted() ? QStringLiteral("Mute")
                                          : QStringLiteral("%1%").arg(m_client ? m_client->volumePercent() : 0));
  }
}

void VolumeApplet::refresh()
{
  if (!m_client || !m_client->available()) {
    setToolTip(QStringLiteral("Volume (PipeWire/Pulse unavailable)"));
    setIcon(QIcon::fromTheme(QStringLiteral("audio-volume-muted")));
    if (icon().isNull()) {
      setText(QStringLiteral("Vol"));
    }
    return;
  }
  setToolTip(m_client->muted()
                 ? QStringLiteral("Muted")
                 : QStringLiteral("Volume %1%").arg(m_client->volumePercent()));
  if (m_slider && !m_slider->isSliderDown()) {
    m_slider->setValue(qMin(100, m_client->volumePercent()));
  }
  if (m_pctLabel) {
    m_pctLabel->setText(m_client->muted() ? QStringLiteral("Mute")
                                          : QStringLiteral("%1%").arg(m_client->volumePercent()));
  }
  updateIcon();
}

void VolumeApplet::placePopup()
{
  if (!m_popup) {
    return;
  }
  m_popup->adjustSize();
  const QPoint global = mapToGlobal(QPoint(width() - m_popup->width(), 0));
  int x = global.x();
  int y = global.y() - m_popup->height() - 4;
  if (QScreen *screen = QApplication::screenAt(mapToGlobal(rect().center()))) {
    const QRect geo = screen->availableGeometry();
    x = qBound(geo.left(), x, geo.right() - m_popup->width());
    if (y < geo.top()) {
      y = mapToGlobal(QPoint(0, height())).y() + 4;
    }
  }
  m_popup->move(x, y);
}

void VolumeApplet::togglePopup()
{
  if (!m_popup) {
    return;
  }
  if (m_popup->isVisible()) {
    m_popup->hide();
    return;
  }
  if (m_client) {
    m_client->refresh();
  }
  placePopup();
  m_popup->show();
  m_popup->raise();
  m_popup->activateWindow();
}

void VolumeApplet::openSoundSettings()
{
  if (m_popup) {
    m_popup->hide();
  }
  QWidget *w = parentWidget();
  while (w) {
    if (w->objectName() == QLatin1String("SpikePanel")) {
      QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection,
                                Q_ARG(QString, QStringLiteral("sound")));
      break;
    }
    w = w->parentWidget();
  }
}

} // namespace spike
