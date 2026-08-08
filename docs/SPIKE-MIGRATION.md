# Spike Migration Tool

## Purpose

This document specifies **spike-migration** — the GUI tool that helps users move personal files when switching **to** Spike Linux from Windows, macOS, or another Linux.

Status: **specification only** (not implemented). Implementation follows after Spike Rescue is stable on the live ISO. Overview of how this tool relates to Rescue and the installer: `SPIKE-RECOVERY-TOOL-GENERAL.md`.

User-facing preparation, app equivalents, and manual tips remain in `MIGRATION-GUIDE.md`. This file is the product/tech contract for the wizard.

## Design Philosophy

Spike was designed for people leaving Windows 10 (or an old Mac) who do not want a terminal. Migration should feel like getting a familiar laptop back after repairs — not like learning a new operating system.

```
├── Guided wizard, plain language, progress always visible
├── Personal files first (documents, photos, media)
├── Same SpikeBackup/ layout as Spike Rescue and installer restore
├── Source disks always mounted read-only
└── Never require the user to type mount commands
```

### Migration vs Rescue vs Installer

| | **spike-migration** | **spike-rescue** | **Installer** |
| :-: | :-: | :-: | :-: |
| Intent | Planned switch TO Spike | Disaster recovery | Install / reinstall Spike |
| Desktop name | **Move My Files** | **Rescue My Files** | **Install Spike** |
| Runs | Live (pre) + installed (post) | Live only | Live |
| Spec | This document | `DISASTER-RECOVERY.md` Layer 3 | `INSTALLER.md` |

Migration owns the **guided copy/import wizard**. Installer Step 7 backup and Layer 4 “reinstall with restore” remain installer flows; they consume the same `SpikeBackup/` format.

## Product Modes

spike-migration is **one application** with **two entry points**.

### Mode A — Pre-install (live ISO)

Use when the user still has Windows / macOS / Linux on disk and is about to install Spike (which will erase the target drive).

```
1. Boot Spike live USB
2. Launch Move My Files
3. Detect old operating system(s)
4. Inventory personal files
5. Copy to external USB under SpikeBackup/
6. Hand off: Install Spike (or exit to desktop)
```

### Mode B — Post-install (installed Spike)

Use when Spike is already installed and files still need to come in.

```
1. Launch Move My Files from the installed desktop
2. Choose source:
   ├── USB drive containing SpikeBackup/
   └── OR old OS partition still present on another disk / leftover volume
3. Inventory / preview
4. Copy into the current user’s home (Documents, Pictures, …)
5. Report success / conflicts / unreadable files
```

Post-install does **not** wipe disks. Pre-install never writes to the old OS disk (read-only only).

## What It Does

```
├── Scans block devices for recognizable operating systems
├── Mounts source partitions read-only
├── Inventories known personal folders per user
├── Copies files with SHA256 verification
├── Pre-install: writes SpikeBackup/ on a destination USB
├── Post-install: writes into ~/Documents, ~/Pictures, …
└── Shows a clear summary (copied / skipped / failed)
```

## What It Does NOT Do (v1)

```
├── Dual-boot or preserve the old OS
├── Unlock BitLocker / FileVault (prompt user to decrypt first)
├── Import browser bookmarks, passwords, or profiles
├── Copy .config/, .local/, or other application data
├── Install Windows/macOS application equivalents
├── Guess arbitrary paths outside known home folders
└── Require terminal usage
```

Browser and account steps stay in `MIGRATION-GUIDE.md` (manual export/import).

## Prerequisites

### Pre-install

```
├── Spike live USB
├── Second USB (or enough free space on a non-target drive) for SpikeBackup/
├── Old OS disk readable (not encrypted, or already unlocked)
└── Enough free space on the destination for the selected files
```

### Post-install

```
├── Working Spike installation
├── Either:
│   ├── USB with SpikeBackup/ from a prior Move/Rescue/Installer backup
│   └── Readable old OS partition (second disk, external drive, leftover volume)
└── Enough free space in the user’s home
```

## Pre-install Flow (Detailed)

### Step 1: Launch

```
├── Live desktop icon: Move My Files
├── Also reachable from a launcher entry (same binary)
└── Window title: Spike Migration
```

### Step 2: Welcome

```
┌──────────────────────────────────────────────────┐
│  Spike Migration                                  │
│                                                  │
│  Moving to Spike?                                 │
│                                                  │
│  This tool copies your personal files (documents, │
│  photos, videos, music) to a USB drive before     │
│  you install Spike.                               │
│                                                  │
│  Your old system disk is never changed.            │
│                                                  │
│  Already installed Spike? Use this tool from the   │
│  installed desktop to import files later.          │
│                                                  │
│  [Cancel]  [Continue]                             │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Step 3: Scan

Same detection rules as Spike Rescue:

```
├── Enumerate block devices (lsblk); skip live medium
├── Activate LVM volumes if needed (vgchange -ay)
├── For each partition (ext4, btrfs, ntfs, hfsplus, …):
│   ├── Read-only mount (constrained helper — see Privileges)
│   ├── Detect OS:
│   │   ├── Linux / Spike: /etc/os-release
│   │   ├── Windows: /Windows/System32
│   │   ├── macOS: /System/Library/CoreServices
│   │   └── Generic: /home/ or /Users/
│   ├── btrfs: try subvol=root / @ / @root; mount home / @home if needed
│   └── Unmount after probe (remount for inventory/copy)
└── Present list to user
```

```
┌──────────────────────────────────────────────────┐
│  Spike Migration                                  │
│                                                  │
│  Found these systems:                             │
│                                                  │
│  ● Windows 10 on /dev/sda3 (ntfs)                 │
│    User: John — ~12.4 GB personal files           │
│                                                  │
│  ○ Kubuntu on /dev/sdb2 (ext4)                    │
│    User: john — ~3.1 GB personal files            │
│                                                  │
│  Select the system you are moving from.           │
│                                                  │
│  [Scan again]  [Back]  [Continue]                 │
│                                                  │
└──────────────────────────────────────────────────┘
```

If nothing is found: show a scan summary (partitions considered, mount failures) and link language: decrypt BitLocker/FileVault first, or use Rescue if the disk is failing.

### Step 4: Inventory preview

```
├── Categories: Documents, Pictures, Videos (Movies on macOS),
│   Music, Downloads, Desktop
├── Shallow home-level *.pdf / Office / ODF files
├── Per-user selection: include/exclude users (default: all)
└── Optional: uncheck whole categories (default: all on)
```

### Step 5: Destination USB

```
├── Wait for a removable volume with enough free space
├── Refuse to use the Spike live medium as destination
├── Create / update SpikeBackup/ on the chosen volume
└── Layout MUST match Spike Rescue / installer restore
```

Canonical layout (shared with Spike Rescue and installer restore):

```
SpikeBackup/
└── <utc-stamp>/                 # e.g. 20260808-113057
    └── <os-label>/              # Linux, Windows, macOS, …
        ├── home/<user>/Documents/
        ├── home/<user>/Pictures/
        ├── home/<user>/Videos/
        ├── home/<user>/Music/
        ├── home/<user>/Downloads/
        ├── home/<user>/Desktop/
        ├── Users/<user>/…        # Windows / macOS sources
        └── REPORT.txt
```

Restore maps `home/<user>/Documents/…` (or `Users/…`) into the target `~/Documents/…`.
Legacy paths under `install-logs-*/log/SpikeBackup/` are still discovered for older sticks.

Multi-user backups nest under `home/<username>/` (or `Users/<username>/`) — **must match**
the shared helper in `src/spike-common/SpikeBackupLayout.*`.


### Step 6: Copy with verification

```
├── SHA256 verify each file (same process as DISASTER-RECOVERY.md)
├── Progress: current file, counts, bytes, ETA
├── Cancel: keep already-copied files; show partial report
└── Unreadable / verify-fail: log and continue
```

### Step 7: Complete (pre-install)

```
┌──────────────────────────────────────────────────┐
│  Spike Migration                                  │
│                                                  │
│  Files ready for Spike                            │
│                                                  │
│  Copied: 4,210 files (12.1 GB)                    │
│  Could not read: 2                                │
│  Saved to: SpikeBackup/ on your USB drive         │
│                                                  │
│  Next: install Spike. During or after install you │
│  can restore these files automatically.            │
│                                                  │
│  [Install Spike]  [Exit to desktop]               │
│                                                  │
└──────────────────────────────────────────────────┘
```

**Install Spike** launches the installer (or focuses its desktop icon). The backup USB stays connected for Step 7 / restore.

## Post-install Flow (Detailed)

### Entry

```
├── Applications / Settings shortcut: Move My Files
├── First-boot welcome may offer “Import my files” → same binary
└── Detect mode: if not running from live ISO → post-install
```

### Source choice

```
┌──────────────────────────────────────────────────┐
│  Spike Migration                                  │
│                                                  │
│  How do you want to bring files into Spike?       │
│                                                  │
│  ● From a USB backup (SpikeBackup folder)         │
│  ○ From another disk / old system still present   │
│                                                  │
│  [Cancel]  [Continue]                             │
│                                                  │
└──────────────────────────────────────────────────┘
```

**From USB:** locate `SpikeBackup/` on connected removable media; preview; copy into `$HOME`.

**From old disk:** run the same RO scan as pre-install; select system/user; copy into `$HOME` (no intermediate USB required if source and destination fit).

### Conflict policy (post-install into home)

When a destination file already exists:

```
├── Default: rename incoming as name (from migration).ext
│   Example: report.docx → report (from migration).docx
├── Alternative (advanced toggle): skip existing
└── Never overwrite without an explicit “Replace existing files”
    confirmation (off by default)
```

Directories are merged. Ownership of new files: current user.

### Complete (post-install)

```
├── Summary of copied / renamed / skipped / failed
├── Open Files at ~/Documents (optional button)
└── Reminder: bookmarks/passwords are manual — MIGRATION-GUIDE tip link
```

## File Locations By Source OS

**Linux / Spike:**

```
├── /home/[user]/Documents|Pictures|Videos|Music|Downloads|Desktop
└── Shallow office docs at /home/[user]/
```

**Windows:**

```
├── /Users/[user]/Documents|Pictures|Videos|Music|Downloads|Desktop
└── (OneDrive-redirected folders: follow the real path if resolvable;
    otherwise copy the visible Documents/Pictures trees)
```

**macOS:**

```
├── /Users/[user]/Documents|Pictures|Movies|Music|Downloads|Desktop
└── Map Movies → Videos in SpikeBackup/ and ~/Videos
```

Excluded (all OSes), same as Rescue:

```
├── AppData, Library Application Support (except as above)
├── Hidden dotfiles / .config / .local
├── Browser profiles, email databases
└── System directories (Windows, Program Files, /usr, …)
```

## Privileges And Packaging

### Mount helper

Reuse the constrained helper pattern from spike-rescue 0.0.2+:

```
├── /usr/lib/spike/spike-rescue-mount (or shared spike-mount-ro)
├── /etc/sudoers.d/ entry for live/installed user
├── Only prepare | mount (must include ro) | umount
├── Mountpoints only under /run/spike-rescue/ or /run/spike-migration/
└── Devices only under /dev/
```

Prefer **one shared library/engine** for scan, inventory, SHA256 copy, and mounts so Rescue and Migration do not diverge.

### Proposed packaging (future)

```
├── Source: src/spike-migration/ (Qt6 Widgets, same stack as rescue)
├── Package: spike-migration_*.deb via scripts/package-spike-migration.sh
├── Live: desktop Move My Files + hook install like spike-rescue
├── Installed: /usr/bin/spike-migration + applications menu entry
└── Depends: same as rescue (Qt6, util-linux, sudo, mount; ntfs-3g, lvm2)
```

Binary may be a thin mode switch over a shared `libspike-recover` (name TBD) used by both apps.

## Encryption And Unreadable Disks

```
├── crypto_LUKS / BitLocker / FileVault volumes: do not attempt unlock in v1
├── UI message: “This drive looks encrypted. Unlock or decrypt it in your
│   old system (or with recovery keys), then run Move My Files again.”
├── Failing hardware: recommend Rescue My Files / professional recovery
└── Partial success is success — always show what was saved
```

## Accessibility And Golden Rules

```
├── Golden Rule 1: no asking users to edit config files
├── Golden Rule 2: no terminal required for the happy path
├── Keyboard navigable wizard; large primary actions
├── Works on Celeron N4020-class hardware (streaming SHA256, modest RAM)
└── Plain language; avoid filesystem jargon in primary UI
```

## Out Of Scope / Later

```
├── Guided browser bookmark/password import UI
├── Mail profile migration wizard
├── Application config import
├── Dual-boot assistants
├── Cloud pull (OneDrive/Google Drive sync clients)
└── In-tool BitLocker/FileVault unlock
```

These may appear as post-1.0 enhancements if Rescue + Migration personal-file paths are solid. See `ROADMAP.md`.

## Implementation Phasing

```
├── Phase 0: This specification + SPIKE-RECOVERY-TOOL-GENERAL.md
├── Phase 1: Spike Rescue MVP stable on live ISO (done / hardening)
├── Phase 2: Shared engine extraction (optional but preferred)
├── Phase 3: spike-migration Mode A (live pre-install)
├── Phase 4: Mode B (post-install import) + first-boot offer
└── Phase 5: Installer deep-links (“Open Move My Files”) polish
```

Do not block the Alpha installer gate on spike-migration. Manual Files copy and Rescue remain valid paths.

## Related Documents

```
├── SPIKE-RECOVERY-TOOL-GENERAL.md → When to use Rescue vs Migration
├── DISASTER-RECOVERY.md           → Rescue Layer 3; SpikeBackup/ restore
├── INSTALLER.md                   → Step 7 backup; reinstall with restore
├── MIGRATION-GUIDE.md             → User guide for switching OSes
├── PHILOSOPHY.md                  → Personal-files recovery promise
├── HARDWARE.md                    → Target machines / constraints
└── ROADMAP.md                     → Scheduling relative to rescue
```

## What This Document Does Not Cover

- Step-by-step screenshots for end users: future `user-guide/` backup/migration pages  

- Installer partitioning and wipe behavior: `INSTALLER.md`  

- Boot failure / GRUB recovery: `BOOT-PROCESS.md`, `DISASTER-RECOVERY.md` Layers 1–2  

- Spike Rescue implementation details already shipped: `src/spike-rescue/`, `DISASTER-RECOVERY.md`  

🐕 BigRangaTech
