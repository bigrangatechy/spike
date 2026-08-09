#include "firstrun/FirstRunHooks.hpp"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QWidget>

namespace spike {
namespace firstrun {
namespace {

QString firstRunCompletedPath()
{
  const QString cfg = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  return cfg + QStringLiteral("/spike/first_run_completed");
}

} // namespace

bool isInstalledSystem()
{
  return QFile::exists(QStringLiteral("/etc/spike/installed"));
}

bool firstRunCompleted()
{
  return QFile::exists(firstRunCompletedPath());
}

bool shouldShowFirstRunWizard()
{
  if (!isInstalledSystem()) {
    return false;
  }
  if (firstRunCompleted()) {
    return false;
  }
  // Prefer /var/lib/spike/first-boot when present; still show if missing so upgrades
  // onto an installed system without the marker get one onboarding pass.
  return true;
}

bool markFirstRunCompleted(QString *error)
{
  const QString path = firstRunCompletedPath();
  QDir().mkpath(QFileInfo(path).absolutePath());
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    if (error) {
      *error = QStringLiteral("Could not write %1").arg(path);
    }
    return false;
  }
  f.write("1\n");
  f.close();

  const QString marker = QStringLiteral("/var/lib/spike/first-boot");
  if (QFile::exists(marker)) {
    QFile::remove(marker); // best-effort (may fail without write perms)
  }
  return true;
}

QStringList collectPostInstallNotices()
{
  QStringList out;
  const QString dirPath = QStringLiteral("/var/lib/spike/installer-notifications");
  QDir dir(dirPath);
  if (!dir.exists()) {
    return out;
  }
  const QStringList files =
      dir.entryList({QStringLiteral("*.txt")}, QDir::Files, QDir::Name);
  for (const QString &name : files) {
    QFile f(dir.filePath(name));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
      continue;
    }
    const QString body = QString::fromUtf8(f.readAll()).trimmed();
    if (body.isEmpty()) {
      out << QStringLiteral("%1 (empty)").arg(name);
    } else {
      out << QStringLiteral("%1\n%2").arg(name, body);
    }
  }
  return out;
}

bool verifyFlatpakRuntimes(QString *msg)
{
  if (msg) {
    *msg = QStringLiteral("Flatpak runtime check — placeholder (not implemented yet).");
  }
  return false;
}

bool checkSecurityUpdates(QString *msg)
{
  if (msg) {
    *msg = QStringLiteral("Security update check — placeholder (not implemented yet).");
  }
  return false;
}

void runDesktopTour(QWidget *parent)
{
  QMessageBox::information(
      parent, QStringLiteral("Desktop tour"),
      QStringLiteral("Desktop tour — coming soon.\n\n"
                     "This is a placeholder hook (firstrun::runDesktopTour). "
                     "A spotlight tour can drop in here later."));
}

bool offerAccessibilityWizard(QWidget *parent)
{
  for (QWidget *w : QApplication::allWidgets()) {
    if (w && w->objectName() == QLatin1String("SpikePanel")) {
      QMetaObject::invokeMethod(w, "openSettings", Qt::QueuedConnection,
                                Q_ARG(QString, QStringLiteral("accessibility")));
      return true;
    }
  }
  Q_UNUSED(parent);
  return false;
}

} // namespace firstrun
} // namespace spike
