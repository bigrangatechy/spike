# Spike Dev Guide — Spike Rescue Internals

## Purpose

Implementation map for **Spike Rescue** (`spike-rescue`) — live-ISO GUI for disaster recover **and** restore. Product UX: `DISASTER-RECOVERY.md` Layer 3 and `SPIKE-RECOVERY-TOOL-GENERAL.md`.

Keep this page updated when mount policy, SpikeBackup layout, or helper commands change.

## Status

| Item | State |
| :-: | :-: |
| Recover (RO scan → USB SpikeBackup) | ✅ |
| Restore (SpikeBackup → `/home/…`) | ✅ MVP |
| Shared layout helper | ✅ `src/spike-common/` |
| Installer Step 7 / Layer 4 | 🔲 Consumes same layout later |
| spike-migration | 🔲 Same layout later |

Current package version: see `src/spike-rescue/CMakeLists.txt` / `STATE.md`.

## Process model

```
spike-rescue (Qt6, user "spike")
        │
        ├── RescueEngine on QThread
        │         │
        │         └── sudo -n /usr/lib/spike/spike-rescue-mount …
        │                   (NOPASSWD via /etc/sudoers.d/spike-rescue)
        │
        └── MainWindow (stacked wizard)
                  ├── Mode: Rescue | Restore
                  ├── Recover: scan → select OS → inventory → dest → copy
                  └── Restore: scan backups → pick session → pick home → copy
```

## Privileged helper

File: `src/spike-rescue/data/spike-rescue-mount` → `/usr/lib/spike/spike-rescue-mount`.

| Command | Use |
| :-: | :-: |
| `prepare` | Ensure `/run/spike-rescue` |
| `mount` / `umount` | RO sources under `/run/spike-rescue/src-*` |
| `mount-rw` | **Only** `LABEL=writable` under `/run/spike-rescue/dest-*` |
| `list-dirs` / `find-files` / `sha256` | Inventory when user cannot read 700/600 homes |
| `copy-file` / `mkdir-dest` | Privileged copy / dest tree |

**Source allow:** `/run/spike-rescue/*`, `*/SpikeBackup/*`, common media mounts.  
**Dest allow:** `/run/spike-rescue/dest-*`, `/run/media/*`, `/media/*`, `/home/*`, and `/var/log` only when `LABEL=writable` (legacy).  
**Never:** write into `/run/spike-rescue/src-*`.

`findmnt -T` fails on paths that do not exist yet — `allow_dest_path` walks up to an existing ancestor before checking the writable label.

## Writable destination (recover)

`RescueEngine::ensureLiveUsbWritableDest()`:

1. Prefer RW mount of `LABEL=writable` at `/run/spike-rescue/dest-writable`.
2. Reject casper’s `/var/log` bind (that is `install-logs-*/log/`, not partition root).
3. Fall back to other whole-partition mounts (`/media/writable`, findmnt targets that look like partition roots).

Resulting tree on the stick:

```
SpikeBackup/<utc-stamp>/<os-label>/home/<user>/…/file
SpikeBackup/<utc-stamp>/<os-label>/REPORT.txt
```

## Restore mode

1. `scanBackups()` → `spike-common::discoverAllBackupSessions` (volume roots + legacy `install-logs-*/log/SpikeBackup`).
2. User picks a session; `refreshRestoreTargets()` lists writable `/home/*`.
3. `startRestore()` → `buildRestoreMappings` → same SHA256 `copyOneFile` path as recover.
4. Writes `SpikeRestore-REPORT.txt` in the target home.

Mapping: `home/<user>/Documents/…` or `Users/<user>/…` → `$TARGET/Documents/…`.

## Shared code

```
src/spike-common/SpikeBackupLayout.{hpp,cpp}
```

Linked into the rescue binary; header also installed to `/usr/include/spike/` for future installer/migration packages.

## Packaging / desktop

- `Categories=X-Spike-Tools;System;Utility;`
- Live Desktop: hook copies `/usr/share/spike/live/spike-rescue.desktop` → `Rescue My Files.desktop`
- Build: `./scripts/package-spike-rescue.sh` (see `05-building-components.md`)

## Debug / smoke

- Window title includes version + `[pre-alpha debug]`.
- Recover always writes `REPORT.txt` beside the backup.
- Capture journal shows `spike-rescue-mount` sudo lines (`mkdir-dest`, `copy-file`, …).
- Collect: mount `writable` or use `scripts/spike-collect-usb-logs.sh`.

## Related

- Spec: `DISASTER-RECOVERY.md`, `SPIKE-RECOVERY-TOOL-GENERAL.md`
- Layout also: `SPIKE-MIGRATION.md`, `INSTALLER.md` Step 7 / Layer 4
- Repo map: `02-repo-structure.md`
