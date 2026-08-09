#include "lock/SpikeLockScreen.hpp"

#include "lock/PamAuth.hpp"

#include <LayerShellQt/Window>

#include <QDateTime>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>

namespace spike {

SpikeLockScreen &SpikeLockScreen::instance()
{
  static SpikeLockScreen s;
  return s;
}

SpikeLockScreen::SpikeLockScreen(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("SpikeLockScreen"));
  setWindowTitle(QStringLiteral("Spike Lock"));
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_ShowWithoutActivating, false);
  setStyleSheet(QStringLiteral(
      "#SpikeLockScreen { background-color: #1a1a2e; color: #ffffff; }"
      "QLabel { color: #ffffff; }"
      "QLineEdit { background: #222236; color: #ffffff; border: 1px solid #444466; "
      "padding: 8px; border-radius: 4px; min-width: 240px; }"
      "QPushButton { background: #2a2a40; color: #ffffff; padding: 8px 16px; "
      "border-radius: 4px; }"
      "QPushButton:hover { background: #3a3a55; }"));

  auto *root = new QVBoxLayout(this);
  root->setAlignment(Qt::AlignCenter);
  root->addStretch(1);

  m_clock = new QLabel(this);
  m_clock->setAlignment(Qt::AlignCenter);
  QFont clockFont = m_clock->font();
  clockFont.setPointSize(48);
  clockFont.setBold(true);
  m_clock->setFont(clockFont);
  root->addWidget(m_clock);

  m_date = new QLabel(this);
  m_date->setAlignment(Qt::AlignCenter);
  QFont dateFont = m_date->font();
  dateFont.setPointSize(14);
  m_date->setFont(dateFont);
  root->addWidget(m_date);

  root->addSpacing(24);

  auto *brand = new QLabel(QStringLiteral("Spike"), this);
  brand->setAlignment(Qt::AlignCenter);
  root->addWidget(brand);

  root->addSpacing(16);

  m_password = new QLineEdit(this);
  m_password->setEchoMode(QLineEdit::Password);
  m_password->setPlaceholderText(QStringLiteral("Password"));
  m_password->setAlignment(Qt::AlignCenter);
  root->addWidget(m_password, 0, Qt::AlignHCenter);
  connect(m_password, &QLineEdit::returnPressed, this, &SpikeLockScreen::tryUnlock);

  m_unlock = new QPushButton(QStringLiteral("Unlock"), this);
  root->addWidget(m_unlock, 0, Qt::AlignHCenter);
  connect(m_unlock, &QPushButton::clicked, this, &SpikeLockScreen::tryUnlock);

  m_status = new QLabel(this);
  m_status->setAlignment(Qt::AlignCenter);
  root->addWidget(m_status);

  root->addSpacing(32);

  auto *powerRow = new QHBoxLayout();
  powerRow->setAlignment(Qt::AlignCenter);
  auto *reboot = new QPushButton(QStringLiteral("Restart"), this);
  auto *poweroff = new QPushButton(QStringLiteral("Shut Down"), this);
  powerRow->addWidget(reboot);
  powerRow->addWidget(poweroff);
  root->addLayout(powerRow);
  connect(reboot, &QPushButton::clicked, this, [this]() {
    runPower(QStringLiteral("Reboot"), QStringLiteral("reboot"));
  });
  connect(poweroff, &QPushButton::clicked, this, [this]() {
    runPower(QStringLiteral("PowerOff"), QStringLiteral("poweroff"));
  });

  root->addStretch(1);

  m_clockTimer = new QTimer(this);
  connect(m_clockTimer, &QTimer::timeout, this, &SpikeLockScreen::updateClock);
  m_clockTimer->setInterval(1000);
}

void SpikeLockScreen::applyLayerShell()
{
  winId();
  QWindow *win = windowHandle();
  if (!win) {
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
      setGeometry(screen->geometry());
    }
    return;
  }
  LayerShellQt::Window *layer = LayerShellQt::Window::get(win);
  if (!layer) {
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
      setGeometry(screen->geometry());
    }
    return;
  }
  using LS = LayerShellQt::Window;
  layer->setScope(QStringLiteral("spike-lock"));
  layer->setLayer(LS::LayerOverlay);
  LS::Anchors anchors = LS::Anchors(LS::AnchorTop) | LS::AnchorBottom | LS::AnchorLeft |
                        LS::AnchorRight;
  layer->setAnchors(anchors);
  layer->setExclusiveZone(-1);
  layer->setKeyboardInteractivity(LS::KeyboardInteractivityExclusive);
  if (QScreen *screen = QGuiApplication::primaryScreen()) {
    layer->setScreen(screen);
    layer->setDesiredSize(screen->geometry().size());
  }
}

void SpikeLockScreen::lock()
{
  if (m_locked) {
    raise();
    activateWindow();
    if (m_password) {
      m_password->setFocus(Qt::OtherFocusReason);
    }
    return;
  }
  m_locked = true;
  if (m_password) {
    m_password->clear();
  }
  if (m_status) {
    m_status->clear();
  }
  updateClock();
  applyLayerShell();
  show();
  raise();
  activateWindow();
  m_clockTimer->start();
  if (m_password) {
    m_password->setFocus(Qt::OtherFocusReason);
  }
}

void SpikeLockScreen::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  applyLayerShell();
}

void SpikeLockScreen::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Escape) {
    // Keep locked; clear password field only.
    if (m_password) {
      m_password->clear();
    }
    return;
  }
  QWidget::keyPressEvent(event);
}

void SpikeLockScreen::updateClock()
{
  const QDateTime now = QDateTime::currentDateTime();
  if (m_clock) {
    m_clock->setText(now.toString(QStringLiteral("HH:mm")));
  }
  if (m_date) {
    m_date->setText(now.toString(QStringLiteral("dddd, d MMMM yyyy")));
  }
}

void SpikeLockScreen::tryUnlock()
{
  if (!m_password) {
    return;
  }
  const QString pw = m_password->text();
  QString err;
  if (!pamAuthenticateUser(pw, &err)) {
    if (m_status) {
      m_status->setText(err.isEmpty() ? QStringLiteral("Incorrect password") : err);
    }
    m_password->clear();
    m_password->setFocus(Qt::OtherFocusReason);
    return;
  }
  m_locked = false;
  m_clockTimer->stop();
  m_password->clear();
  if (m_status) {
    m_status->clear();
  }
  hide();
  emit unlocked();
}

void SpikeLockScreen::runPower(const QString &logindMethod, const QString &systemctlVerb)
{
  if (QProcess::startDetached(QStringLiteral("sudo"),
                              {QStringLiteral("-n"), QStringLiteral("systemctl"), systemctlVerb})) {
    return;
  }
  QProcess::startDetached(
      QStringLiteral("busctl"),
      {QStringLiteral("call"), QStringLiteral("org.freedesktop.login1"),
       QStringLiteral("/org/freedesktop/login1"), QStringLiteral("org.freedesktop.login1.Manager"),
       logindMethod, QStringLiteral("b"), QStringLiteral("false")});
}

} // namespace spike
