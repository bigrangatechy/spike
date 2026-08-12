#pragma once

#include "InstallState.hpp"

#include <QString>
#include <QStringList>

namespace spike {

/** Lightweight probes for wizard display (full detect lands later). */
class HardwareProbe
{
public:
  static QString summary();
  static QStringList blockDevicesHuman();
  static QString recommendVariant(); // standard | plus
};

/** SpikeBackup discovery for Step 7 / Layer 4 UI (uses spike-common). */
class BackupScanner
{
public:
  /**
   * Mount LABEL=writable at /run/spike-rescue/dest-writable (partition root).
   * Never returns casper’s /var/log bind — that nests SpikeBackup under install-logs.
   */
  static QString ensureLiveUsbWritableRoot();
  static QStringList volumeRoots();
  static QStringList sessionLabels(); // human lines for list widgets
  static QStringList sessionPaths();  // parallel absolute session paths
};

} // namespace spike
