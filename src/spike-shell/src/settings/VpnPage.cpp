#include "settings/VpnPage.hpp"

#include "network/NmClient.hpp"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace spike {

namespace {

bool runOk(const QString &program, const QStringList &args, QString *errOut = nullptr)
{
  QProcess proc;
  proc.start(program, args);
  if (!proc.waitForStarted(3000) || !proc.waitForFinished(30000)) {
    proc.kill();
    if (errOut) {
      *errOut = QStringLiteral("timed out");
    }
    return false;
  }
  if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
    if (errOut) {
      *errOut = QString::fromUtf8(proc.readAllStandardError()).trimmed();
      if (errOut->isEmpty()) {
        *errOut = QStringLiteral("exit %1").arg(proc.exitCode());
      }
    }
    return false;
  }
  return true;
}

} // namespace

QWidget *makeVpnPage(QWidget *parent, NmClient *nm, QLabel *statusBar)
{
  auto *w = new QWidget(parent);
  auto *lay = new QVBoxLayout(w);
  lay->addWidget(new QLabel(QStringLiteral("<h2>VPN</h2>"), w));
  auto *hint = new QLabel(
      QStringLiteral(
          "Connect / disconnect NetworkManager VPN and WireGuard profiles. "
          "Import a WireGuard .conf or OpenVPN .ovpn, or open the connection editor."),
      w);
  hint->setWordWrap(true);
  lay->addWidget(hint);

  auto *list = new QListWidget(w);
  lay->addWidget(list, 1);

  auto *status = new QLabel(w);
  status->setWordWrap(true);
  lay->addWidget(status);

  auto *row = new QHBoxLayout();
  auto *reload = new QPushButton(QStringLiteral("Reload"), w);
  auto *connectBtn = new QPushButton(QStringLiteral("Connect"), w);
  auto *disconnectBtn = new QPushButton(QStringLiteral("Disconnect"), w);
  auto *importBtn = new QPushButton(QStringLiteral("Import…"), w);
  auto *editor = new QPushButton(QStringLiteral("Connection editor…"), w);
  row->addWidget(reload);
  row->addWidget(connectBtn);
  row->addWidget(disconnectBtn);
  row->addWidget(importBtn);
  row->addWidget(editor);
  row->addStretch(1);
  lay->addLayout(row);

  auto load = [nm, list, status, statusBar]() {
    list->clear();
    if (!nm) {
      status->setText(QStringLiteral("NetworkManager client unavailable"));
      return;
    }
    QString err;
    const QVector<NmVpnConnection> vpns = nm->vpnConnections(&err);
    if (!err.isEmpty() && vpns.isEmpty()) {
      status->setText(QStringLiteral("Failed to list VPNs: %1").arg(err));
      return;
    }
    for (const NmVpnConnection &c : vpns) {
      auto *item = new QListWidgetItem(
          QStringLiteral("%1%2  (%3)")
              .arg(c.active ? QStringLiteral("● ") : QStringLiteral("○ "), c.name, c.type));
      item->setData(Qt::UserRole, c.name);
      item->setData(Qt::UserRole + 1, c.uuid);
      item->setData(Qt::UserRole + 2, c.active);
      list->addItem(item);
    }
    status->setText(vpns.isEmpty()
                        ? QStringLiteral("No VPN / WireGuard yet — use Import… or Connection editor.")
                        : QStringLiteral("%1 VPN profile(s)").arg(vpns.size()));
    if (statusBar) {
      statusBar->setText(QStringLiteral("VPN loaded"));
    }
  };

  QObject::connect(reload, &QPushButton::clicked, w, load);
  if (nm) {
    QObject::connect(nm, &NmClient::changed, w, load);
  }
  QObject::connect(connectBtn, &QPushButton::clicked, w, [nm, list, status, statusBar, load]() {
    if (!nm || !list->currentItem()) {
      status->setText(QStringLiteral("Select a VPN profile first"));
      return;
    }
    const QString name = list->currentItem()->data(Qt::UserRole).toString();
    QString err;
    if (!nm->activateVpn(name, &err)) {
      status->setText(QStringLiteral("Connect failed: %1").arg(err));
      return;
    }
    status->setText(QStringLiteral("Connected: %1").arg(name));
    if (statusBar) {
      statusBar->setText(QStringLiteral("VPN connected"));
    }
    load();
  });
  QObject::connect(disconnectBtn, &QPushButton::clicked, w, [nm, list, status, statusBar, load]() {
    if (!nm || !list->currentItem()) {
      status->setText(QStringLiteral("Select a VPN profile first"));
      return;
    }
    const QString name = list->currentItem()->data(Qt::UserRole).toString();
    QString err;
    if (!nm->deactivateVpn(name, &err)) {
      status->setText(QStringLiteral("Disconnect failed: %1").arg(err));
      return;
    }
    status->setText(QStringLiteral("Disconnected: %1").arg(name));
    if (statusBar) {
      statusBar->setText(QStringLiteral("VPN disconnected"));
    }
    load();
  });
  QObject::connect(importBtn, &QPushButton::clicked, w, [status, load]() {
    const QString path = QFileDialog::getOpenFileName(
        nullptr, QStringLiteral("Import VPN / WireGuard"), QString(),
        QStringLiteral("VPN configs (*.conf *.ovpn);;All (*)"));
    if (path.isEmpty()) {
      return;
    }
    QString type = QStringLiteral("wireguard");
    if (path.endsWith(QLatin1String(".ovpn"), Qt::CaseInsensitive)) {
      type = QStringLiteral("openvpn");
    }
    QString err;
    if (!runOk(QStringLiteral("nmcli"),
               {QStringLiteral("connection"), QStringLiteral("import"), QStringLiteral("type"), type,
                QStringLiteral("file"), path},
               &err)) {
      status->setText(QStringLiteral("Import failed (%1): %2 — try Connection editor…")
                          .arg(type, err));
      return;
    }
    status->setText(QStringLiteral("Imported %1").arg(path));
    load();
  });
  QObject::connect(editor, &QPushButton::clicked, w, [status]() {
    if (QProcess::startDetached(QStringLiteral("nm-connection-editor"), {})) {
      return;
    }
    status->setText(QStringLiteral("nm-connection-editor not found — use Import… or nmcli."));
  });

  load();
  return w;
}

} // namespace spike
