#include "panel/Panel.hpp"

#include "launcher/Launcher.hpp"
#include "panel/applets/ClockApplet.hpp"
#include "panel/applets/SessionMenuApplet.hpp"
#include "settings/ConfigClient.hpp"
#include "settings/SettingsWindow.hpp"

#include <QHBoxLayout>
#include <QProcess>
#include <QPushButton>

namespace spike {

namespace {

bool runWait(const QString &program, const QStringList &args, int timeoutMs = 8000)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000)) {
    return false;
  }
  if (!proc.waitForFinished(timeoutMs)) {
    proc.kill();
    return false;
  }
  return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

void runPowerAction(const QString &logindMethod, const QString &systemctlVerb)
{
  if (runWait(QStringLiteral("sudo"),
              {QStringLiteral("-n"), QStringLiteral("systemctl"), systemctlVerb})) {
    return;
  }
  if (runWait(QStringLiteral("busctl"),
              {QStringLiteral("call"), QStringLiteral("org.freedesktop.login1"),
               QStringLiteral("/org/freedesktop/login1"),
               QStringLiteral("org.freedesktop.login1.Manager"), logindMethod, QStringLiteral("b"),
               QStringLiteral("false")})) {
    return;
  }
  QProcess::startDetached(QStringLiteral("systemctl"), {systemctlVerb});
}

} // namespace

Panel::Panel(QWidget *parent)
  : QWidget(parent)
{
  setObjectName(QStringLiteral("SpikePanel"));
  setFixedHeight(32);
  setWindowTitle(QStringLiteral("Spike Panel"));
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

  m_config = new ConfigClient(this);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 2, 8, 2);
  layout->setSpacing(8);

  auto *spikeBtn = new QPushButton(QStringLiteral("Spike"), this);
  spikeBtn->setObjectName(QStringLiteral("SpikeButton"));
  spikeBtn->setFixedHeight(26);
  spikeBtn->setToolTip(QStringLiteral("Open start menu"));
  connect(spikeBtn, &QPushButton::clicked, this, &Panel::toggleLauncher);
  layout->addWidget(spikeBtn);

  layout->addStretch(1);

  auto *clock = new ClockApplet(this);
  layout->addWidget(clock);

  auto *session = new SessionMenuApplet(this);
  layout->addWidget(session);

  m_launcher = new Launcher(this);
  m_launcher->hide();
  connect(m_launcher, &Launcher::openSettingsRequested, this, [this]() { openSettings(); });
  connect(m_launcher, &Launcher::logoutRequested, this, &Panel::onLogout);
  connect(m_launcher, &Launcher::rebootRequested, this, &Panel::onReboot);
  connect(m_launcher, &Launcher::shutdownRequested, this, &Panel::onShutdown);
}

void Panel::toggleLauncher()
{
  if (!m_launcher) {
    return;
  }
  if (m_launcher->isVisible()) {
    m_launcher->hide();
    return;
  }

  const QPoint global = mapToGlobal(QPoint(8, 0));
  m_launcher->move(global.x(), global.y() - m_launcher->height() - 4);
  m_launcher->show();
  m_launcher->raise();
  m_launcher->activateWindow();
}

void Panel::openSettings(const QString &pageId)
{
  if (!m_settings) {
    m_settings = new SettingsWindow(m_config, nullptr);
  }
  if (!pageId.isEmpty()) {
    m_settings->openPage(pageId);
  }
  m_settings->show();
  m_settings->raise();
  m_settings->activateWindow();
}

void Panel::onLogout()
{
  const QString sessionId = qEnvironmentVariable("XDG_SESSION_ID");
  if (!sessionId.isEmpty()) {
    QProcess::startDetached(QStringLiteral("loginctl"),
                            {QStringLiteral("terminate-session"), sessionId});
  } else {
    QProcess::startDetached(QStringLiteral("loginctl"),
                            {QStringLiteral("terminate-user"), qEnvironmentVariable("USER")});
  }
}

void Panel::onReboot()
{
  runPowerAction(QStringLiteral("Reboot"), QStringLiteral("reboot"));
}

void Panel::onShutdown()
{
  runPowerAction(QStringLiteral("PowerOff"), QStringLiteral("poweroff"));
}

} // namespace spike
