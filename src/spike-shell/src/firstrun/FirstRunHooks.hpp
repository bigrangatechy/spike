#pragma once

#include <QString>
#include <QStringList>
#include <QWidget>

namespace spike {
namespace firstrun {

/** True when /etc/spike/installed exists (installed system, not live ISO). */
bool isInstalledSystem();

/** ~/.config/spike/first_run_completed */
bool firstRunCompleted();

/** Whether the first-run wizard should show. */
bool shouldShowFirstRunWizard();

/** Write first_run_completed; best-effort remove /var/lib/spike/first-boot. */
bool markFirstRunCompleted(QString *error = nullptr);

/** /var/lib/spike/installer-notifications — drop-in notice .txt files. */
QStringList collectPostInstallNotices();

/** Placeholder: Flatpak runtime validation (not implemented yet). */
bool verifyFlatpakRuntimes(QString *msg);

/** Placeholder: security update check (not implemented yet). */
bool checkSecurityUpdates(QString *msg);

/** Placeholder: spotlight desktop tour (shows honest “coming soon”). */
void runDesktopTour(QWidget *parent);

/**
 * Open Settings → Accessibility if SpikePanel is available.
 * Returns true if Settings was invoked; false if caller should show a fallback.
 */
bool offerAccessibilityWizard(QWidget *parent);

} // namespace firstrun
} // namespace spike
