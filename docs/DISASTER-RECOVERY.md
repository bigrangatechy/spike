Spike Disaster Recovery
Purpose

This document specifies how Spike handles system failures — from minor issues like a broken update to catastrophic failures like a corrupted filesystem. The goal is simple: the user never loses their personal files, no matter what happens to the system.
Design Philosophy
The Recovery Promise

Spike makes one unconditional guarantee to the user:

  Your personal files are always recoverable.

This means:
├── If the system won't boot: files can be recovered
├── If an update breaks the desktop: files can be recovered
├── If the filesystem is corrupted: files can be recovered (mostly)
├── If the user forgets their password: files can be recovered
├── If the user wants to reinstall: files can be restored
└── If the hardware is dying: files can be recovered before it dies

The recovery path is always:
  1. Boot from Spike USB (live ISO)
  2. Run Spike Rescue tool (GUI — no terminal)
  3. Copy files to external USB drive
  4. Reinstall Spike (optional)
  5. Restore files (optional, during reinstall)

Recovery Layers

Spike has multiple layers of protection, from most-preferred
to last-resort:

Layer 1: Boot failure counter + previous kernel
├── Automatic, no user action needed
├── Handles: bad kernel update, driver crash
└── Recovery time: 1 reboot

Layer 2: Recovery mode (GRUB recovery entry)
├── Semi-automatic, requires GRUB menu interaction
├── Handles: broken service, misconfiguration
└── Recovery time: 5-10 minutes

Layer 3: Spike Rescue Tool (live ISO)
├── Manual, requires USB drive
├── Handles: unbootable system, corrupted desktop
└── Recovery time: 10-30 minutes (depends on file count)

Layer 4: Reinstall with restore
├── Manual, requires USB drive + time
├── Handles: total system failure, hopeless corruption
└── Recovery time: 20-40 minutes (install + restore)

Layer 5: Professional data recovery
├── Out of scope for Spike
├── Handles: physical drive failure (clicking, dead)
└── Spike acknowledges this exists and provides guidance

What Spike Does NOT Guarantee

Spike cannot guarantee recovery of:
├── System settings (theme, network configs, keyboard layout)
├── Application configurations (.config/, .local/)
├── Browser history, bookmarks, saved passwords
├── Installed applications (Flatpak or apt)
├── Custom wallpapers or personalizations
└── Files in non-standard locations (outside home directory)

These exclusions are intentional:
├── Restoring system configs from a broken system could
│   reintroduce the problem that caused the breakage
├── Application configs may reference missing packages
├── Saved passwords are encrypted and user-specific
└── Only personal files (documents, photos, media) are restored

If a user loses browser bookmarks, that is unfortunate but
acceptable. If a user loses family photos, that is a failure.
Spike optimizes for the latter, never the former.

Layer 1: Boot Failure Recovery
How It Works

The boot failure counter is the first line of defense.
See BOOT-PROCESS.md for full technical details.

Summary:
├── Counter stored at /boot/.spike/boot-count
├── Incremented at initramfs stage (before systemd)
├── Cleared when spike-session fully starts
├── After 3 consecutive failures: GRUB menu appears
└── Previous kernel highlighted automatically

What constitutes a "failure":
├── Kernel panic (boot aborts)
├── Initramfs cannot mount root (filesystem issue)
├── systemd reaches emergency target (critical service failure)
├── spike-session crashes before clearing counter
├── System hangs during boot (user hard-powers off)
├── KWin crashes repeatedly (black screen after login)
└── Any condition where spike-session does not reach "ready" state

What does NOT constitute a failure:
├── Successful boot followed by app crash (counter already cleared)
├── Slow boot (eventually succeeds)
├── Display flicker during Plymouth (cosmetic)
├── Network connection failure (post-boot issue)
└── User-initiated shutdown during boot

User Experience

Normal boot (no issues):
├── User presses power button
├── Plymouth splash shows
├── SDDM login appears
├── Desktop loads
└── User never knows boot counter exists

Boot failure scenario:
├── Boot attempt 1: fails (black screen, hang, or crash)
├── User holds power button to force shutdown
├── Boot attempt 2: fails
├── User holds power button to force shutdown
├── Boot attempt 3: fails
├── User holds power button to force shutdown
├── Boot attempt 4: GRUB menu appears automatically
│
│  ┌────────────────────────────────────────────────┐
│  │                                                │
│  │  ⚠️  Spike failed to start 3 times.            │
│  │                                                │
│  │  Try using the previous kernel version,        │
│  │  or select Recovery to rescue your files.      │
│  │                                                │
│  │  > Spike, with previous kernel                 │
│  │    Spike                                       │
│  │    Spike Recovery                              │
│  │                                                │
│  │  Booting selected entry in 15 seconds...       │
│  │                                                │
│  └────────────────────────────────────────────────┘
│
├── User selects "Spike, with previous kernel"
├── System boots with old kernel
├── Desktop loads normally
├── Counter cleared
└── User continues working

What the user should do after successful fallback boot:
├── Notification appears: "Spike recovered from a boot failure.
│   The previous kernel is in use. The new kernel has been
│   preserved for investigation. You can remove it via
│   Settings → Advanced → Boot."
├── Settings → Advanced → Boot shows:
│   ├── Boot failure count: 0 (reset after successful boot)
│   ├── Current kernel: 7.0.0-41-generic (previous)
│   ├── Failed kernel: 7.0.0-42-generic (still installed)
│   └── [Remove failed kernel] button
└── User clicks "Remove failed kernel" to prevent re-selection

What If Previous Kernel Isn't Available?

If this is the first kernel install (no previous version):

├── GRUB menu still appears after 3 failures
├── Only "Spike" (same broken kernel) and "Recovery" available
├── "Spike, with previous kernel" entry absent
├── User selects "Spike Recovery"
└── Recovery mode provides:
    ├── Root shell (for advanced users)
    └── Guidance to use live USB rescue (for beginners)

This scenario is unlikely because:
├── Kernel updates always retain the previous version
├── Only on a fresh install with no updates would this occur
└── Fresh installs rarely have boot failures (nothing to break yet)

Layer 2: Recovery Mode
What It Is

Recovery mode is a minimal boot environment accessible from
the GRUB menu. It boots into rescue.target instead of
graphical.target.

No GUI. No audio. No network (unless user enables).
Just a root shell with the root filesystem mounted.

This is a developer/power-user tool. For non-technical users,
Spike always recommends the live USB rescue path instead.

Accessing Recovery Mode

Two ways to access recovery mode:

Method 1: Boot failure counter triggered
├── After 3 boot failures, GRUB menu appears
├── "Spike Recovery" entry is visible
└── User selects it

Method 2: Manual GRUB menu access
├── Press ESC during boot (within 3-second window)
├── GRUB menu appears
├── "Spike Recovery" entry is visible
└── User selects it

Recovery Environment

When recovery mode boots, the user sees:

┌──────────────────────────────────────────────────┐
│                                                  │
│  Spike Recovery Mode                             │
│                                                  │
│  Your files are safe on your disk.               │
│  You can recover them using a Spike USB drive     │
│  and the "Rescue My Files" tool.                  │
│                                                  │
│  Available options:                               │
│                                                  │
│  1. [Drop to root shell]                         │
│     Opens a terminal for advanced repair.        │
│     Requires terminal knowledge.                  │
│                                                  │
│  2. [Remount disk as read-write]                 │
│     Allows modifying files on the system.        │
│                                                  │
│  3. [Enable networking]                          │
│     Connect to Wi-Fi for downloading fixes.      │
│                                                  │
│  4. [Run filesystem check]                       │
│     Check and repair disk errors.                │
│                                                  │
│  5. [Reboot]                                     │
│                                                  │
│  Recommended: Create a Spike USB drive on another │
│  computer, then use "Rescue My Files" to safely   │
│  recover your data.                               │
│                                                  │
└──────────────────────────────────────────────────┘

This screen replaces Ubuntu's standard recovery menu.
It prioritizes the live USB rescue path and de-emphasizes
the root shell, in accordance with Golden Rule 2.

Recovery Mode Capabilities

Root shell:
├── Provides bash as root (no password — standard Ubuntu behavior)
├── Root filesystem mounted read-only by default
├── User must select "Remount as read-write" to make changes
├── Can run: apt, dpkg, fsck, spike-rescue (CLI), manual repairs
├── Can edit config files (but this violates Golden Rule 1 for
│   normal users — recovery mode is explicitly the exception)
└── Journal available: journalctl -xb

Networking:
├── User can enable Wi-Fi (selects last connected network)
├── Allows apt update / apt install for repairing packages
├── Allows downloading drivers or firmware
└── If no Wi-Fi: Ethernet works if cable connected

Filesystem check:
├── Runs fsck on root filesystem
├── Automatically fixes safe errors
├── Prompts for unsafe repairs (potential data loss)
├── Reports results
└── After fsck: user can attempt normal boot

Reboot:
├── Reboots the system normally
├── If repair was successful: normal boot succeeds
├── If repair failed: boot still fails, counter increments
└── After 3 more failures: GRUB menu appears again

What Recovery Mode Cannot Do

Recovery mode limitations:
├── No graphical interface (no KWin, no Spike Shell)
├── No audio (no PipeWire)
├── No Bluetooth
├── No GUI file manager (Dolphin)
├── No GUI settings (Settings panel unavailable)
├── No Flatpak application access
├── Requires terminal knowledge for most operations
└── Cannot restore files to USB (no GUI Spike Rescue tool)

The GUI Spike Rescue tool only runs from the live ISO,
not from recovery mode. This is by design:
├── Live ISO has a full environment (GUI, audio, file managers)
├── Recovery mode is minimal (damaged system may be unstable)
├── Running GUI from a damaged system risks further issues
└── Live ISO is a known-good, stable environment

Layer 3: Spike Rescue Tool
Overview

Spike Rescue is a GUI application that runs from the live ISO.
It is the primary recovery tool for non-technical users.

What it does:
├── Mounts the broken system's disk (read-only)
├── Scans for user data
├── Copies user data to an external USB drive
├── Verifies every file with SHA256 checksums
└── Reports what was recovered and what couldn't be read

What it does NOT do:
├── Repair the system (that's reinstall territory)
├── Modify the source disk (read-only mount enforced)
├── Recover system settings or app configurations
├── Guess where files are stored (uses known locations only)
└── Require any terminal usage (100% GUI)

Prerequisites

To use Spike Rescue, the user needs:
├── A Spike USB drive (the same one used for installation)
├── A separate USB drive (or the same one, if large enough)
│   for storing recovered files
└── The broken laptop

If the user doesn't have their installation USB:
├── They can create one on another computer
├── Download from https://spike.bigrangatech.com/download
├── Write to USB using:
│   ├── Another Spike system (Settings → Create Installation USB)
│   ├── Another Linux (balenaEtcher or dd)
│   ├── Windows (Rufus or balenaEtcher)
│   └── macOS (balenaEtcher)
└── User guide has step-by-step instructions with screenshots

Rescue Flow (Detailed)

Step 1: Boot from Spike USB
├── User inserts Spike USB drive
├── Powers on laptop
├── (May need to change boot order in firmware — user guide covers this)
├── Spike live environment boots
└── Desktop appears (same as installation environment)

Step 2: Launch Spike Rescue
├── Desktop icon: "Rescue My Files" (prominent, alongside "Install Spike")
├── Double-click to launch
└── Spike Rescue opens

Step 3: Disk Scan
┌──────────────────────────────────────────────────┐
│  Spike Rescue                                     │
│                                                  │
│  Scanning for installed systems...                │
│  ████████████████████████████████░░░░  80%          │
│                                                  │
│  Checking /dev/sda...                             │
│  Checking /dev/sdb...                             │
│                                                  │
└──────────────────────────────────────────────────┘

Scan process:
├── Enumerate all block devices (lsblk)
├── For each partition:
│   ├── Detect filesystem type (ext4, ntfs, hfsplus, btrfs)
│   ├── Attempt read-only mount
│   ├── If mount succeeds: check for recognizable OS
│   │   ├── Linux: check for /etc/os-release
│   │   ├── Windows: check for /Windows/System32
│   │   ├── macOS: check for /System/Library/CoreServices
│   │   └── Generic: check for /home/ or /Users/
│   ├── If OS detected: add to results list
│   └── Unmount (will remount during recovery)
└── Results presented to user

Step 4: Select Source
┌──────────────────────────────────────────────────┐
│  Spike Rescue                                     │
│                                                  │
│  Found the following systems on your computer:     │
│                                                  │
│  ● Spike Linux on /dev/sda2 (ext4)                │
│    User: john — 1,531 files found                 │
│                                                  │
│  ○ Windows 10 on /dev/sda3 (ntfs)                 │
│    User: John — 847 files found                   │
│                                                  │
│  Select a system to recover files from.            │
│                                                  │
│  [Back]  [Recover files]                          │
│                                                  │
└──────────────────────────────────────────────────┘

Step 5: File Scan Results
┌──────────────────────────────────────────────────┐
│  Spike Rescue                                     │
│                                                  │
│  Files found on Spike Linux (/dev/sda2):           │
│                                                  │
│  📁 Documents   — 124 files, 1.2 GB               │
│  📁 Pictures    — 1,203 files, 3.1 GB              │
│  📁 Videos      — 47 files, 2.8 GB                │
│  📁 Music       — 89 files, 0.3 GB                 │
│  📁 Downloads   — 56 files, 0.4 GB                │
│  📁 Desktop     — 12 files, 0.4 GB                │
│                                                  │
│  Total: 1,531 files (8.2 GB)                      │
│                                                  │
│  ⚠️ 3 files could not be read (may be damaged).    │
│                                                  │
│  [Back]  [Continue to recovery]                   │
│                                                  │
└──────────────────────────────────────────────────┘

Step 6: Connect Destination USB
┌──────────────────────────────────────────────────┐
│  Spike Rescue                                     │
│                                                  │
│  Connect a USB drive to save your recovered        │
│  files. The drive must have at least 8.2 GB        │
│  of free space.                                   │
│                                                  │
│  Waiting for USB drive...                         │
│  [Insert USB drive now]                           │
│                                                  │
│  ── USB drive detected ──                         │
│  Drive: SanDisk Ultra 32GB                        │
│  Free space: 28.4 GB                             │
│  ✓ Sufficient space                               │
│                                                  │
│  Files will be saved to:                          │
│  /run/media/usb/SpikeBackup/                      │
│                                                  │
│  [Back]  [Start recovery]                          │
│                                                  │
└──────────────────────────────────────────────────┘

Step 7: Recovery Progress
┌──────────────────────────────────────────────────┐
│  Spike Rescue                                     │
│                                                  │
│  Recovering files...                              │
│  ████████████████████░░░░░░░░░░░░░░  62%          │
│                                                  │
│  Currently copying: Pictures/IMG_0847.jpg          │
│                                                  │
│  Files copied: 949 / 1,531                        │
│  Data transferred: 5.1 GB / 8.2 GB                │
│  Elapsed: 4 minutes 12 seconds                    │
│  Estimated remaining: 2 minutes 38 seconds         │
│                                                  │
│  ✓ Verify each file (SHA256 checksum)             │
│                                                  │
│  [Cancel]                                         │
│                                                  │
└──────────────────────────────────────────────────┘

Cancel behavior:
├── User clicks Cancel
├── Confirmation: "Stop recovery? Files already copied
│   will remain on the USB drive."
├── If confirmed: stop copying, unmount source, show partial results
└── If declined: continue recovery

Step 8: Recovery Complete
┌──────────────────────────────────────────────────┐
│  Spike Rescue                                     │
│                                                  │
│  ✅ Recovery complete!                            │
│                                                  │
│  Files recovered: 1,528                           │
│  Files that could not be read: 3                  │
│  Data recovered: 8.19 GB                          │
│  Checksum verification: All passed                │
│                                                  │
│  Your files are saved on the USB drive in:         │
│  SpikeBackup/                                     │
│                                                  │
│  ⚠️ 3 files could not be read:                    │
│  [Show details ▼]                                │
│    • Pictures/Vacation/beach.jpg (read error)      │
│    • Documents/Work/report.docx (read error)       │
│    • Downloads/archive.zip (read error)           │
│                                                  │
│  These files may be damaged on your disk. You can  │
│  try recovering them with specialized data recovery │
│  tools, but there is no guarantee of success.       │
│                                                  │
│  What would you like to do next?                   │
│                                                  │
│  [Reinstall Spike]                                │
│  [Exit to desktop]                                │
│                                                  │
└──────────────────────────────────────────────────┘

File Scanning Details

Directories scanned (per OS type):

Spike / Linux:
├── /home/[username]/Documents/
├── /home/[username]/Pictures/
├── /home/[username]/Videos/
├── /home/[username]/Music/
├── /home/[username]/Downloads/
├── /home/[username]/Desktop/
├── /home/[username]/*.odt, *.ods, *.odp (root of home)
├── /home/[username]/*.pdf, *.docx, *.xlsx (root of home)
└── If multiple users: all home directories scanned

Windows:
├── /Users/[username]/Documents/
├── /Users/[username]/Pictures/
├── /Users/[username]/Videos/
├── /Users/[username]/Music/
├── /Users/[username]/Downloads/
├── /Users/[username]/Desktop/
└── If multiple users: all user directories scanned

macOS:
├── /Users/[username]/Documents/
├── /Users/[username]/Pictures/
├── /Users/[username]/Movies/
├── /Users/[username]/Music/
├── /Users/[username]/Downloads/
├── /Users/[username]/Desktop/
└── If multiple users: all user directories scanned

NOT scanned (intentionally excluded):
├── System directories (/usr, /etc, /var, /opt, /sys, /proc)
├── Application directories (.config, .local, .cache)
├── Hidden directories (.* in home — dotfiles)
├── Browser profiles
├── Email databases
└── Application data

Reason: These contain system-specific configs that shouldn't
be restored. Personal files are in the visible directories.

Checksum Verification

Every file recovered goes through SHA256 verification:

Process:
1. Read file from source disk
2. Compute SHA256 hash of source data
3. Write file to USB drive
4. Read file back from USB drive
5. Compute SHA256 hash of destination data
6. Compare hashes

If hashes match: file verified, continue to next file
If hashes differ:
├── Retry copy (up to 3 attempts)
├── If still failing:
│   ├── Log as "verification failed"
│   ├── File remains on USB (may be usable despite mismatch)
│   └── Report in summary: "2 files failed verification"
└── Verification failures indicate:
    ├── Source disk has bad sectors (file is corrupt at source)
    ├── USB drive has issues (try different USB drive)
    └── Rare: memory (RAM) corruption (run memtest)

Memory overhead:
├── SHA256 computed in streaming fashion (not loading entire file)
├── Buffer size: 64KB per file
├── Negligible memory impact even on 4GB systems
└── Slower than blind copy, but integrity is worth the trade-off

Read-Only Mount Enforcement

Spike Rescue mounts source partitions read-only. This is
NON-NEGOTIABLE. The source disk is never modified.

Implementation:
├── mount -o ro,ro /dev/sda2 /mnt/source
├── Double "ro" prevents remount as read-write
├── Even if the tool crashes: mount stays read-only
├── Kernel enforces read-only (process cannot override)
└── Only root can remount (and Spike Rescue doesn't)

Why this matters:
├── Damaged filesystems can be further damaged by writes
├── Read-only mount prevents accidental modification
├── User confidence: "My files are safe — this tool can't
│   break anything"
└── Legal/ethical: recovery tool should never modify source

NTFS handling:
├── NTFS mounted via ntfs3 (kernel driver) or ntfs-3g (FUSE)
├── Read-only mount: ntfs-3g -o ro
├── If NTFS is "dirty" (not cleanly unmounted):
│   ├── Read-only mount still works
│   ├── No repair attempted (that would require write access)
│   └── User warned: "Windows was not shut down properly.
│       Some files may not be accessible."
└── Hibernated Windows (fast startup):
    ├── ntfs-3g refuses to mount (hibernation file active)
    ├── ntfs3 (kernel) may mount read-only
    ├── If mount fails: user advised to disable fast startup
    │   in Windows and try again
    └── User guide covers this scenario with screenshots

Layer 4: Reinstall With Restore
When To Use

Reinstall with restore is the nuclear option:
├── System is unbootable and recovery mode didn't help
├── Filesystem corruption is severe
├── User wants a clean start
├── User is selling/giving away the laptop (wipe + reinstall)
└── Spike Rescue recovered files, now user wants Spike back

The installer detects an existing Spike installation and
offers two options:
├── Fresh install (erase everything, install from scratch)
└── Fresh install and restore my data (erase, install, restore from USB)

Detection

The installer scans the target drive during the hardware
detection phase (Step 6 in INSTALLER.md):

Detection logic:
├── Scan all partitions for /etc/os-release
├── If os-release contains "Spike" or ID=spike:
│   ├── Flag as "existing Spike installation"
│   ├── Scan for user home directories
│   ├── Count personal files (same scan as Spike Rescue)
│   └── Check for SpikeBackup/ on connected USB drives
├── If no Spike installation found: normal install path
└── If Spike found: present recovery options

Detection happens automatically — user doesn't need to
select anything special. The installer knows.

Restore During Installation

If user selects "Fresh install and restore my data":

1. Check for SpikeBackup/ on connected USB
   ├── If found: proceed with install + scheduled restore
   └── If not found:
       ┌──────────────────────────────────────────────┐
       │  No backup found                               │
       │                                                │
       │  No SpikeBackup folder was found on any        │
       │  connected USB drive.                          │
       │                                                │
       │  You can still install Spike without           │
       │  restoring files.                              │
       │                                                │
       │  [Fresh install]  [Try different USB]          │
       └──────────────────────────────────────────────┘

2. Normal installation proceeds (Steps 1-9 from INSTALLER.md)
   ├── Step 7 (Data backup) is SKIPPED — user already has backup
   └── All other steps proceed normally

3. After installation completes, before reboot:
   ├── Mount USB drive containing SpikeBackup/
   ├── Copy SpikeBackup/ to /home/[new_username]/
   │   ├── Preserve folder structure (Documents/, Pictures/, etc.)
   │   ├── Checksum verification on every file (SHA256)
   │   └── Progress bar: "Restoring your files... 45%"
   ├── Set ownership: chown -R [user]:[user] /home/[user]/
   ├── Set permissions: find /home/[user]/ -type d -exec chmod 755 {}
   │                   find /home/[user]/ -type f -exec chmod 644 {}
   ├── Unmount USB drive
   └── Proceed to reboot (Step 10)

4. First boot:
   ├── User finds their files in their home directory
   ├── Welcome message includes: "Your files have been restored
   │   from backup. Check your Documents, Pictures, and other
   │   folders."
   └── Applications need to be reinstalled via Discover

What Gets Restored

Restored from SpikeBackup/:
├── Documents/         → ~/Documents/
├── Pictures/          → ~/Pictures/
├── Videos/            → ~/Videos/
├── Music/             → ~/Music/
├── Downloads/         → ~/Downloads/
├── Desktop/           → ~/Desktop/
└── Any other folders present in the backup

NOT restored:
├── System settings (theme, panel config, wallpaper)
├── Network configurations (saved Wi-Fi passwords)
├── Application configs (.config/, .local/)
├── Browser data (history, bookmarks, passwords)
├── SSH keys
├── GPG keys
├── Installed applications (must reinstall via Discover)
└── Flatpak app data (app-specific configs)

This is the same exclusion list as Spike Rescue.
Consistency between tools is intentional.

Common Failure Scenarios And Recovery Paths
Scenario 1: Bad Kernel Update

Symptom: System was working, kernel update installed, reboot
resulted in black screen or kernel panic.

Recovery path:
1. Let boot fail 3 times → GRUB menu appears
2. Select "Spike, with previous kernel"
3. System boots normally
4. Settings → Advanced → Boot → Remove failed kernel
5. Done — system operational

Time: 5-10 minutes (3 reboots + cleanup)
Data loss: None
Tools needed: None (all automatic)

Scenario 2: GPU Driver Crash

Symptom: System boots, but after login: black screen, frozen
cursor, or KWin crash loop. Desktop never appears.

Recovery path:
1. Let boot fail 3 times → GRUB menu appears
2. Select "Spike Recovery"
3. In recovery mode:
   a. Enable networking
   b. Drop to root shell
   c. apt purge [problematic-driver-package]
   d. update-initramfs -u
   e. reboot
4. If recovery mode is too technical:
   a. Reboot to GRUB menu
   b. Boot from Spike USB
   c. Use Spike Rescue to recover files
   d. Reinstall Spike with restore

Time: 15-45 minutes (depending on path chosen)
Data loss: None (files recoverable)
Tools needed: Spike USB (for reinstall path)

Scenario 3: Corrupted Filesystem (Power Loss)

Symptom: System was abruptly powered off (battery died, power
outage). Now system won't boot — GRUB loads but kernel panics
or initramfs reports errors.

Recovery path:
1. Boot fails → initramfs may attempt fsck
2. If fsck succeeds: system boots (minor corruption)
3. If fsck fails: initramfs shows Spike recovery screen
4. From recovery screen:
   a. Boot from Spike USB
   b. Use Spike Rescue to recover files
   c. Reinstall Spike with restore

Time: 20-40 minutes
Data loss: Possible (corrupted files may not be readable)
Tools needed: Spike USB + spare USB for backup

Scenario 4: Full Disk (No Space)

Symptom: System boots but runs extremely slowly. Applications
crash. Notifications fail. Disk is 100% full.

Recovery path:
1. If desktop is usable (barely):
   a. Settings → Advanced → Storage → identify large files
   b. Delete unnecessary files
   c. Empty trash
   d. Clear Discover cache (Settings → Software Sources → Clear cache)
   e. Remove unused Flatpak apps via Discover
2. If desktop is not usable:
   a. Boot from Spike USB
   b. Mount system disk (read-write via file manager)
   c. Delete files manually (Dolphin)
   d. Reboot normally
3. If still not enough space:
   a. Backup files via Spike Rescue
   b. Reinstall Spike (fresh filesystem has plenty of space)
   c. Restore files

Time: 10-60 minutes (depending on approach)
Data loss: Intentional (user deletes files to free space)
Tools needed: Spike USB (for live USB path)

Scenario 5: Forgotten Password

Symptom: User forgot their login password. Cannot access system.

Recovery path:
1. Reboot
2. Press ESC during boot → GRUB menu
3. Select "Spike Recovery"
4. In recovery mode:
   a. Drop to root shell
   b. Remount as read-write
   c. passwd [username]
   d. Enter new password
   e. reboot
5. Login with new password

Note: This requires terminal usage (violates Golden Rule 2).
However, password recovery fundamentally requires elevated
access. There is no way to change a password from a GUI without
knowing the current password.

Alternative (GUI path):
1. Boot from Spike USB
2. Mount system disk read-write (via Dolphin)
3. chroot into the system (advanced — developer guide covers this)
4. Change password
5. Reboot

The recovery mode path is documented in the user guide with
step-by-step instructions and screenshots. While it involves
a terminal, the user is guided through 4 simple commands.

Future consideration: A "Password Reset" tool on the live ISO
that automates this via GUI is a planned post-beta feature.

Scenario 6: Dying Hard Drive

Symptom: System takes very long to boot. Files open slowly.
Strange clicking sounds (HDD). SMART warnings. Random freezes.

Recovery path:
1. IMMEDIATELY back up files:
   a. Boot from Spike USB (don't boot from dying disk)
   b. Use Spike Rescue to recover files
   c. If files are recovering slowly: be patient
   d. If read errors increase: stop and accept partial recovery
2. Replace the hard drive (user guide covers replacement basics)
3. Install Spike on new drive
4. Restore files from USB backup

Time: 30 minutes - several hours (depends on drive health)
Data loss: Possible (unreadable sectors)
Tools needed: Spike USB + spare USB + replacement drive

SMART warning integration:
├── Settings → Advanced → Storage shows SMART status
├── If SMART indicates failure: red warning
│   "Your hard drive may be failing. Back up your files
│    immediately."
├── Notification appears on every boot if SMART is failing
├── Spike Rescue logs SMART data for support purposes
└── User guide: "What to do if your hard drive is failing"

Scenario 7: Broken Desktop (Spike Shell Crash)

Symptom: System boots, SDDM login appears, user logs in,
but Spike Shell crashes immediately or repeatedly. No panel,
no desktop, just a wallpaper or black screen.

Recovery path:
1. Switch to TTY: Ctrl+Alt+F2
2. Log in with username and password
3. Try restarting Spike Shell:
   systemctl --user restart spike-session
4. If that fails:
   a. Check logs: journalctl --user -u spike-session -b
   b. If configuration issue: reset Spike config
      rm -rf ~/.config/spike/
      systemctl --user restart spike-session
   c. If still fails: the system installation may be damaged
5. If terminal recovery is too technical:
   a. Boot from Spike USB
   b. Use Spike Rescue to recover files
   c. Reinstall Spike with restore

Time: 5-30 minutes
Data loss: None (resetting Spike config doesn't affect personal files)
Tools needed: Spike USB (for reinstall path)

Note: Resetting ~/.config/spike/ only affects Spike Shell settings
(panel position, applet config, theme overrides). Personal files
in ~/Documents, ~/Pictures, etc. are untouched.

Creating A Spike USB Drive
From Within Spike

Settings → Advanced → Create Installation USB

┌──────────────────────────────────────────────────┐
│  Create Spike USB                                 │
│                                                  │
│  This tool creates a Spike installation USB drive │
│  that can be used to install Spike or recover      │
│  files from a broken system.                      │
│                                                  │
│  USB drive: [SanDisk Ultra 32GB] ▼                │
│  ISO image: Spike 0.1.0-alpha (downloaded)        │
│                                                  │
│  ⚠️ All data on this USB drive will be erased.     │
│                                                  │
│  [Download latest ISO]  [Create USB]              │
│                                                  │
└──────────────────────────────────────────────────┘

Behavior:
├── Downloads latest ISO from spike.bigrangatech.com
├── Verifies ISO checksum (SHA256)
├── Writes ISO to USB drive (dd under the hood)
├── Verifies written data
├── Progress bar: "Creating Spike USB... 45%"
└── Completion: "Spike USB created successfully.
    You can use this to install Spike or recover files."

This tool exists so users can create recovery media BEFORE
they need it. Encouraged during first-run setup.

From Other Operating Systems

Windows:
├── Download ISO from spike.bigrangatech.com/download
├── Use Rufus (free, open source): https://rufus.ie
├── Or balenaEtcher: https://balena.io/etcher
└── User guide has step-by-step with screenshots

macOS:
├── Download ISO from spike.bigrangatech.com/download
├── Use balenaEtcher: https://balena.io/etcher
└── User guide has step-by-step with screenshots

Other Linux:
├── Download ISO from spike.bigrangatech.com/download
├── Use balenaEtcher
├── Or dd: dd if=spike.iso of=/dev/sdX bs=4M status=progress
├── Or gnome-disk-utility (GNOME Disks: Restore Disk Image)
└── User guide has step-by-step with screenshots

Recovery Best Practices
For Users

1. Create a Spike USB drive NOW (before you need it)
   └── Settings → Advanced → Create Installation USB

2. Back up important files regularly
   └── Copy to external USB drive every few weeks

3. Let updates install when prompted
   └── Security updates happen automatically, but non-security
       updates need your approval via Discover

4. If your system feels sick (slow, crashing):
   └── Back up files immediately, then investigate

5. If your system won't boot:
   └── Don't panic. Your files are safe. Use Spike Rescue.

6. If you hear clicking from your hard drive:
   └── Stop using it immediately. Boot from Spike USB and
       recover files. The drive is dying.

For Support Staff

When helping a user with a broken system:

1. Always ask: "Do you have a Spike USB drive?"
   └── If not: guide them to create one on another computer

2. Always recommend Spike Rescue first
   └── It's the safest, easiest recovery path

3. Never instruct users to use terminal commands
   └── Golden Rule 2. Use Spike Rescue GUI instead

4. Only suggest recovery mode if:
   ├── Spike Rescue can't read the disk (filesystem too damaged)
   ├── User needs to remove a specific package
   └── User is comfortable with terminal

5. If all else fails: reinstall with restore
   └── Files are already backed up (Spike Rescue step)

6. Ask for diagnostic report
   └── Settings → Advanced → Diagnostics → Export Report
       (if system is bootable enough to access Settings)

Integration With Other Systems
Boot Failure Counter

Referenced in:
├── BOOT-PROCESS.md (technical implementation)
├── KERNEL.md (kernel update and rollback flow)
├── INSTALLER.md (counter initialized at install)
└── DESKTOP.md (Settings → Boot page shows counter)

Disaster Recovery relationship:
├── Counter is Layer 1 of recovery
├── Failure to clear counter triggers recovery cascade
└── This document describes the user-facing recovery flow
    that the counter initiates

Spike Rescue Tool

Referenced in:
├── INSTALLER.md (brief mention, "Rescue My Files" desktop icon)
├── BOOT-PROCESS.md (initramfs recovery screen mentions live USB)
└── This document (full specification)

Disaster Recovery relationship:
├── Spike Rescue is Layer 3 of recovery
├── Primary tool for non-technical users
├── Runs from live ISO (same ISO as installer)
└── Specified in full detail here

Installer Restore

Referenced in:
├── INSTALLER.md (Step 7: Data Backup, Reinstall with Restore)
└── This document (Layer 4: Reinstall with Restore)

Disaster Recovery relationship:
├── Reinstall with restore is Layer 4 of recovery
├── Last resort before professional data recovery
├── Integrated into the installer (user doesn't need separate tool)
└── Detects existing Spike installation automatically

What This Document Does Not Cover

    Boot failure counter technical implementation: See BOOT-PROCESS.md (Section: Boot Failure Counter)
    GRUB configuration and recovery entry: See BOOT-PROCESS.md (Section: GRUB2 Bootloader)
    Installer installation steps: See INSTALLER.md (Full 10-step flow)
    Kernel update and rollback: See KERNEL.md (Section: Kernel Updates)
    Spike Shell crash recovery (spike-session restart): See DESKTOP.md (Session Manager)
    SMART monitoring and disk health: See DESKTOP.md (Settings → Storage)
    Settings → Boot page (counter view/reset): See BOOT-PROCESS.md (Settings Integration)
    Firewall and security during recovery: See SECURITY.md
    Privacy of recovered files: See PRIVACY.md
    User guide recovery instructions: See docs/user-guide/14-recovery.md

🐕 BigRangaTech
