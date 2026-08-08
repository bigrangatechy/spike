# Spike Installer

Custom Qt Widgets installer per `docs/INSTALLER.md`. **Not Calamares.**

## Status (0.0.1)

| Area | State |
| --- | --- |
| 10-step wizard UI | ✅ Collects answers (language → finish) |
| Hardware / disk listing | ✅ Lightweight probes |
| SpikeBackup session scan | ✅ via `src/spike-common/` (Layer 4 choice) |
| Step 7 backup copy | 🔲 Stub (checkbox only) |
| Partition / wipe / squashfs copy | 🔲 **Not enabled** — will not erase disks |
| Bootloader / first boot | 🔲 |
| Layer 4 restore into new home | 🔲 Uses selected session path when engine lands |

Alpha gate remains: live → **real** install → reboot to installed desktop.

## Build

```
./scripts/package-spike-installer.sh
# → build/packages/spike-installer_0.0.1-1_amd64.deb
```

ISO: `build-iso.sh` packages + stages this deb; hook installs Desktop **Install Spike**.

## Layout

```
src/spike-installer/
├── src/
│   ├── main.cpp
│   ├── InstallWizard.*     → 10-step stacked wizard
│   ├── InstallState.hpp    → collected answers
│   └── detect/             → HardwareProbe + BackupScanner
├── data/*.desktop          → X-Spike-Tools
└── CMakeLists.txt
```

Shared SpikeBackup helpers: `../spike-common/`.

## Related

- Spec: `docs/INSTALLER.md`
- Internals: `docs/dev-guide/07-installer-internals.md`
- Recovery map: `docs/SPIKE-RECOVERY-TOOL-GENERAL.md`
