# Spike Recovery And Migration Tools — Overview

## Purpose

This document is the short map of Spike’s **GUI data tools** for non-technical users. It answers one question: which tool do I open?

Full specifications live elsewhere. This file does not replace them.

```
├── Disaster / broken system → Spike Rescue (spike-rescue)
├── Switching TO Spike (planned move) → Spike Migration (spike-migration)
├── Installer USB backup / reinstall restore → Installer (INSTALLER.md)
└── Manual tips, apps, accounts → MIGRATION-GUIDE.md
```

## The Two Desktop Tools

| | **Spike Rescue** | **Spike Migration** |
| :-: | :-: | :-: |
| Binary | `spike-rescue` | `spike-migration` |
| Desktop name | **Rescue My Files** | **Move My Files** |
| Intent | Disaster: system won’t boot or is dying | Planned switch from Windows / macOS / Linux |
| Where it runs | Live ISO only | Live ISO (pre-install) **and** installed Spike (post-install) |
| Typical dest | External USB `SpikeBackup/` | Pre: USB `SpikeBackup/`; Post: your new home folder |
| Spec | `DISASTER-RECOVERY.md` (Layer 3) | `SPIKE-MIGRATION.md` |

Both tools:

```
├── Are 100% GUI (no terminal required)
├── Mount source disks read-only
├── Copy known personal folders only (Documents, Pictures, …)
├── Verify copies with SHA256
├── Use the same SpikeBackup/ layout so installer restore can find the data
└── Do NOT migrate system settings, app configs, or saved passwords (v1)
```

## When To Use Which

### Use Spike Rescue when

```
├── Spike (or another OS) will not boot normally
├── An update broke the desktop and recovery mode is too technical
├── The disk is failing and you need files off it NOW
└── You already have Spike installed and need emergency copy-out
```

Flow: boot Spike USB → **Rescue My Files** → copy to USB → (optional) reinstall with restore.

### Use Spike Migration when

```
├── You are switching TO Spike on purpose
├── Windows / macOS / another Linux still boots (or the disk is healthy)
├── You want a guided copy before the installer erases the drive
└── OR Spike is already installed and you still need to pull files in
```

Flow (pre-install): boot Spike USB → **Move My Files** → copy to USB → **Install Spike**.

Flow (post-install): open **Move My Files** → import from USB `SpikeBackup/` **or** from an old partition still on disk.

### Use the Installer (not these tools) when

```
├── Step 7 optional backup during a normal install (same SpikeBackup/ idea)
├── “Fresh install and restore my data” after Spike was already on the disk
└── Full product flow: INSTALLER.md + DISASTER-RECOVERY.md Layer 4
```

Migration owns the **guided wizard**. Installer restore owns the **reinstall path**. They share the `SpikeBackup/` folder format on purpose.

### Use the Migration Guide (manual) when

```
├── Browser bookmarks / password export
├── Email client profiles
├── Finding Linux equivalents of Windows/macOS apps
└── First-week expectations after switching
```

See `MIGRATION-GUIDE.md`. Drag-and-drop in Files remains a valid fallback if the user prefers it.

## Shared Rules (v1)

What is copied:

```
├── Documents, Pictures, Videos/Movies, Music, Downloads, Desktop
├── Shallow home-level office docs (PDF, ODF, common Office formats)
└── Per-user homes under /home or /Users
```

What is never copied by these tools (v1):

```
├── System settings (theme, network, keyboard)
├── Application configs (.config/, .local/)
├── Browser history, bookmarks, saved passwords
├── Installed applications
└── Files outside standard home locations
```

Rationale matches the recovery promise in `DISASTER-RECOVERY.md` and `PHILOSOPHY.md`: personal files first; restoring broken system configs risks bringing the breakage back.

## Live Desktop Layout

On the Spike live ISO desktop, tools appear alongside Install Spike:

```
├── Install Spike
├── Rescue My Files   → spike-rescue (recover + restore; Spike Tools)
└── Move My Files     → spike-migration (pre-install mode; when shipped)
```

Launcher category **Spike Tools** (`X-Spike-Tools`) groups Rescue and future Spike tooling.

On an installed Spike system:

```
├── Move My Files → spike-migration (post-install mode)
└── Rescue My Files is not required (use live USB if the install is broken)
```

## Implementation Status

```
├── spike-common: shared SpikeBackup layout/find/map — src/spike-common/
├── spike-rescue: recover + restore MVP + **batch CLI 0.0.12** (quick --list-systems) — src/spike-rescue/
├── spike-installer: wizard **0.0.10** (async Step 7 scan; locale/TZ/keyboard; RESTORE_STATUS; blacklist) — src/spike-installer/
├── spike-migration: wizard shell **0.0.3** — src/spike-migration/, SPIKE-MIGRATION.md
└── Desktop category: Spike Tools (X-Spike-Tools) for Rescue / Installer / Move My Files
```

## Related Documents

```
├── SPIKE-MIGRATION.md         → Full spike-migration product/tech spec
├── DISASTER-RECOVERY.md       → 4-layer recovery; Spike Rescue detail
├── INSTALLER.md               → Step 7 backup; reinstall with restore
├── MIGRATION-GUIDE.md         → User-facing switch guide (manual paths)
├── PHILOSOPHY.md              → Recovery / personal-files promise
└── ROADMAP.md                 → When migration ships relative to rescue
```

🐕 BigRangaTech
