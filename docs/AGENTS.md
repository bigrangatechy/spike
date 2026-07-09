AGENTS.md — Spike Project Master Reference
Purpose

This document is the complete project reference. It contains everything needed to understand, maintain, contribute to, or extend the Spike Linux distribution. It is written for:

    AI agents — to understand the project context and provide consistent guidance
    Human contributors — to understand architecture, constraints, and workflows
    Future maintainers — to inherit a well-documented system

This document supersedes chat context. If something conflicts between this document and other communication channels, this document wins.
Table of Contents

    Project Identity
    Core Philosophy
    Golden Rules
    Architecture Overview
    Documentation Structure
    Codebase Layout
    Critical Constraints
    Hardware Targets
    Component Specifications
    Development Workflow
    Testing Requirements
    Troubleshooting Guide
    Common Mistakes
    References

1. Project Identity
Project Name

    Name: Spike
    Organization: BigRangaTech
    Slogan: "Let's Make Tech Repairable Again"
    License: GPLv2+ (code), CC-BY-SA 4.0 (docs/artwork)
    Domain: https://spike.bigrangatech.com/
    Parent Domain: https://bigrangatech.com/

Origin Story

Spike is named after a real dog — loyal, scrappy, doesn't quit. The logo depicts a herding dog with circuit-board flanks, representing the mission to bring technology back to life. The dog symbolizes endurance and care; the circuits represent the technology being saved.
Mission

Build a Linux distribution that rescues older/cheap laptops (specifically dual-core Celeron with 4GB RAM) from landfill. Transform machines deemed "dead" by modern software into usable, dignified systems.
Target Users

    People with old laptops who don't know what Linux is
    Beginners who want their computer to work without learning technical skills
    Anyone who believes old hardware deserves a second life
    NOT for power users, developers, or those seeking terminal-heavy workflows

Core Values

    Respect the hardware — adapt to what's available, don't demand more
    Simplicity is a feature — choose simple over powerful when uncertain
    Every notification matters — never lose a notification, ever
    Old hardware deserves dignity — treat abandoned machines with respect
    Built to outlast — design for longevity, documentation before code
    Kindness is mandatory — community has zero tolerance for bullying beginners

Timeline

    Alpha: 8 months from start
    Beta: 18 months from start
    Production: After beta stabilization
    End of Life: Defined in EOL-Policy.md (minimum 5 years per release)

2. Core Philosophy
The Two Golden Rules
Rule 1: Users Never Edit Config Files

Every user-facing setting has a graphical interface in Settings. Config files exist on disk but are managed by the system. A user should never need to open a text editor to change how their system behaves.

What this means:

    If a setting has no GUI, it is not user-facing
    Settings are stored in /etc and /usr/share, not edited by users
    Advanced settings (swappiness, GRUB timeout, etc.) have GUI controls
    Kernel parameters and module blacklists are view-only, not editable
    If you add a feature, you must add its GUI setting

Rule 2: Users Never Touch A Terminal Unless They Want To

The terminal exists (Konsole) and works. But no normal system task — installing software, changing settings, troubleshooting, connecting to Wi-Fi, updating, recovering files — should ever require opening a terminal.

What this means:

    If the only way to accomplish a task requires terminal, that is a BUG
    Every task must have a GUI alternative
    Power users may opt in to terminal, but normal users should never need it
    Documentation never instructs users to use the terminal
    Settings pages include a "Report a Problem" button, not terminal instructions

Why These Rules Matter

They preserve dignity and accessibility for beginners. A user who needs to open Konsole to fix their Wi-Fi is a user who has failed the mission. Every design decision must enforce these rules.
3. Golden Rules Enforcement Checklist

Before merging any feature, verify:

Feature checklist:
□ Does this feature require users to edit config files?
  → If yes, redesign or mark as non-user-facing
□ Does this feature require users to use the terminal?
  → If yes, redesign or mark as non-user-facing
□ Is there a GUI setting for every configurable aspect?
  → If no, add it before merge
□ Is this documented in the user guide?
  → If no, write documentation before merge
□ Does this violate any critical constraints in Section 7?
  → If yes, reconsider design
□ Does this work on Tier 1 hardware (Celeron N4020, 4GB RAM)?
  → If no, optimize or defer
□ Will this increase idle memory beyond 400MB (Standard) / 800MB (Plus)?
  → If yes, benchmark and justify or refactor

4. Architecture Overview
Base System

    Base: Ubuntu Server LTS (next version, currently targeting 26.04 "Resolute Raccoon")
    Why not Debian? Stale kernel and Mesa versions hurt old Intel graphics
    Why not Alpine? musl libc causes compatibility issues with proprietary firmware
    Why not Fedora? Short support cycle, not suitable for long-term stability
    Why not Arch? Breakage risk too high for beginner users
    Why not Buildroot? No package manager, no upgrade path, security nightmare
    Why not Ubuntu Desktop? Snap bloat, unnecessary services, heavier default

Kernel

    Version: Ubuntu LTS kernel (Linux 7.0 at launch target)
    No custom kernel — use upstream Ubuntu kernel
    No kernel recompilation — just configure parameters and modules
    Module blacklisting: Done at install time based on detected hardware
    NVIDIA support: nouveau by default, proprietary driver optional post-install

Display Stack

    Compositor: Wayland native (KWin standalone, NOT Plasma)
    Fallback: XWayland (not full X11)
    No Akonadi — contact database not needed
    No Baloo — file indexer adds overhead
    No KIO slaves — standard file access only
    No plasma-session/plasma-workspace — replaced by custom Spike Shell

Desktop Shell

    Name: Spike Shell
    Written in: Qt6 C++ (Qt Widgets, not QML — lower memory)
    Components:
        Panel (applet host, layout manager)
        Launcher (Kickoff-style app menu)
        Notification Daemon (bulletproof history, disk-before-display)
        Settings Panel (hybrid: custom pages + KDE KCM modules)
        Session Manager (login, logout, shutdown, autostart)
        Theme Engine (colors, styles, icons, fonts, decorations)
        14 Tray Applets (network, volume, battery, brightness, notifications, devices, updates, clock, session, night light, Bluetooth, airplane mode, keyboard layout, 1 TBD)

Software Management

    Package manager: apt (from Ubuntu base)
    Additional: Flatpak via Discover (KDE Software Center)
    NO Snap — stripped entirely from ISO
    Repositories: Ubuntu official repos + Flathub
    Updates: Security automatic, non-security via Discover notification only

Filesystem

    Type: ext4 everywhere (no Btrfs — rescue/restore loop replaces snapshot need)
    No LVM, no ZFS, no f2fs
    Automatic partitioning only — no manual option, no dual boot
    Minimum 128GB storage
    Supported: SATA SSD, NVMe, SATA HDD, SD card (UHS-I+), USB 3.0+
    NOT supported: eMMC, USB 2.0
    Adaptive mount flags:
        SSD: noatime, discard
        HDD: noatime, commit=60
        SD/USB: noatime, commit=60, discard

Memory Management

    ZRAM: Adaptive, enabled only on capable CPUs (bogomips ≥ ~2200/core), zstd compression
    ZRAM size: Equal to RAM, max 4GB on Standard, uncapped on Plus
    Swap file: 8GB (SSD/HDD), 4GB (SD/USB), priority 10
    Swappiness: SSD=15, HDD=5, SD/USB=10, adjustable slider for SSD only (range 15-60)
    Earlyoom: 10% memory/swap threshold, protects essential processes
    Transparent hugepages: madvise (not always)
    zswap.enabled=0 kernel param (prevent conflict with ZRAM)

Boot Process

    Bootloader: GRUB2 (migration to Limine planned for future)
    Hidden menu: Default, 3-5 second ESC window
    3 consecutive boot failures → menu appears with recovery highlighted
    Recovery entry always present when menu shown
    Boot failure counter: /boot/.spike/boot-count
    Themed GRUB: Spike branding, dark background with circuit traces

Audio/Video

    Audio: PipeWire + WirePlumber (no PulseAudio)
    Settings: 44100Hz, stereo-only, medium resampling, logging suppressed, no JACK
    Video acceleration: VA-API
        Intel: intel-media-va-driver-non-free (LIBVA_DRIVER_NAME=iHD)
        AMD: mesa-va-drivers (LIBVA_DRIVER_NAME=radeonsi)
    Firefox tuning: AV1 disabled, VP9 hardware decode forced (Gemini Lake supports VP9, not AV1)

Networking

    Stack: NetworkManager
    Firmware: ALL Wi-Fi firmware on ISO (Intel iwlwifi, Atheros ath, Realtek rtl, Broadcom bcmwl-kernel-source)
    Bluetooth: Conditional on hardware detection (udev-triggered for dongles)
    Firewall: ufw, deny incoming by default, allow outgoing
    No telemetry, no data collection

Variants

Single ISO, two configurations selected at install time:
Feature	Spike Standard	Spike Plus
Target RAM	4GB	8GB+
Idle memory	<400MB	<800MB
Animations	Off	On
Services	Minimal	Standard
Plymouth	Minimal/static	Animated
ZRAM cap	4GB max	Uncapped
CPU governor	powersave	schedutil
Recommends	Celeron/Pentium, ≤4GB	Modern i3/i5/i7/Ryzen, 8GB+
5. Documentation Structure
Total Count: 69 Documents

docs/
├── Top-level (33 files) — Architecture, policies, references
│   ├── README.md
│   ├── INDEX.md
│   ├── PHILOSOPHY.md (with Golden Rules)
│   ├── ARCHITECTURE.md
│   ├── DESIGN-DECISIONS.md
│   ├── KERNEL.md
│   ├── MEMORY.md
│   ├── DESKTOP.md
│   ├── INSTALLER.md
│   ├── DISASTER-RECOVERY.md
│   ├── CONFIGURATION.md
│   ├── BOOT-PROCESS.md
│   ├── POWER-MANAGEMENT.md
│   ├── MULTIMEDIA.md
│   ├── NETWORKING.md
│   ├── SECURITY.md
│   ├── PRIVACY.md
│   ├── AGENTS.md (this document — master reference)
│   ├── PERFORMANCE-BASELINES.md
│   ├── CHANGELOG.md
│   ├── ROADMAP.md
│   ├── GOVERNANCE.md
│   ├── END-OF-LIFE-POLICY.md
│   ├── TROUBLESHOOTING.md
│   ├── FAQ.md
│   ├── MIGRATION-GUIDE.md
│   ├── SUPPORT.md
│   ├── ACCESSIBILITY.md
│   ├── CODE_OF_CONDUCT.md
│   ├── CONTRIBUTING.md
│   ├── BRANDING.md
│   ├── HARDWARE.md
│   └── LICENSE
│
├── user-guide/ (17 files + screenshots/) — Beginner-focused
│   ├── INDEX.md
│   ├── 01-getting-started.md
│   ├── 02-connecting-wifi.md
│   ├── 03-managing-files.md
│   ├── 04-installing-apps.md
│   ├── 05-customizing-appearance.md
│   ├── 06-managing-system.md
│   ├── 07-system-settings.md
│   ├── 08-backing-up-data.md
│   ├── 09-troubleshooting.md
│   ├── 10-using-terminal.md (educational only, not required for any task)
│   ├── 11-accessibility.md
│   ├── 12-printing.md
│   ├── 13-bluetooth.md
│   ├── 14-recovery.md
│   ├── 15-glossary.md
│   ├── 16-tips.md
│   └── screenshots/ (organized by topic)
│
└── dev-guide/ (19 files) — Developer-focused
    ├── INDEX.md
    ├── 01-getting-started.md
    ├── 02-repo-structure.md
    ├── 03-build-environment.md
    ├── 04-building-spike.md
    ├── 05-building-components.md
    ├── 06-spike-shell-architecture.md
    ├── 07-installer-internals.md
    ├── 08-rescue-tool-internals.md
    ├── 09-spike-config-internals.md
    ├── 10-branding-and-theming.md
    ├── 11-testing.md
    ├── 12-debugging.md
    ├── 13-git-workflow.md
    ├── 14-contribution-phases.md
    ├── 15-coding-conventions.md
    ├── 16-translation-workflow.md
    ├── 17-release-process.md
    ├── 18-accessibility-development.md
    └── 19-appendix-reference.md

Documentation Rules
User Guide

    Written for absolute beginners
    No jargon without glossary link
    No terminal instructions (ever)
    Every step has a screenshot
    Every document independently maintainable
    Cross-referenced to related docs
    Same content powers Settings app reader and website

Dev Guide

    Technical, precise, exact commands
    Tested examples (verified on P50 dev machine)
    Developer-standard English, jargon acceptable WITH context
    Numbered steps, expected output, common errors
    Cross-references to DESIGN-DECISIONS.md and AGENTS.md
    Every feature requires corresponding doc update

Writing Standards

    User guide: Plain English, one concept per paragraph, active voice
    Dev guide: Precise, imperative mood for commands, include copyable code
    Both: Spell-check, consistent terminology, no Wikipedia-length definitions
    Glossary: One-word or one-sentence definitions, no essays

6. Codebase Layout
Repository Structure (Monorepo)

spike/
├── .github/                          → GitHub metadata, issue templates, workflows
│   ├── ISSUE_TEMPLATE/
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── workflows/
├── docs/                             → All 69 documentation files
│   ├── (top-level docs)
│   ├── user-guide/
│   │   ├── INDEX.md
│   │   ├── *.md (17 files)
│   │   └── screenshots/
│   └── dev-guide/
│       ├── INDEX.md
│       └── *.md (19 files)
├── src/                              → All source code
│   ├── spike-shell/                  → Custom desktop shell
│   │   ├── ui/                       → Qt Widgets frontend
│   │   ├── panel/                    → Panel and applets
│   │   ├── launcher/                 → Application menu
│   │   ├── notify/                   → Notification daemon
│   │   ├── settings/                 → Settings window
│   │   ├── session/                  → Session management
│   │   ├── theme/                    → Theme engine
│   │   ├── tray/                     → Tray applet framework
│   │   └── CMakeLists.txt
│   ├── spike-installer/              → Graphical installer
│   │   ├── ui/                       → Installation screens
│   │   ├── detect/                   → Hardware detection modules
│   │   ├── partition/                → Partitioning logic
│   │   ├── backup/                   → Data backup
│   │   ├── restore/                  → Data restore
│   │   └── CMakeLists.txt
│   ├── spike-rescue/                 → Live ISO rescue tool
│   │   ├── scan/                     → Mount and scan partitions
│   │   ├── copy/                     → File copying with verification
│   │   └── CMakeLists.txt
│   ├── spike-config/                 → Configuration generators
│   │   ├── memory/                   → ZRAM, swap, earlyoom
│   │   ├── boot/                     → GRUB, kernel params
│   │   ├── security/                 → AppArmor, ufw
│   │   ├── network/                  → NetworkManager config
│   │   ├── multimedia/               → PipeWire, VA-API
│   │   └── storage/                  → Mount options, fstab
│   └── spike-branding/               → Visual identity
│       ├── grub-theme/               → GRUB2 theme
│       ├── plymouth/                 → Boot splash
│       ├── kwin-decorations/         → Window decorations
│       ├── wallpapers/               → Wallpapers
│       ├── icons/                    → Icon overrides
│       └── qt-stylesheets/           → Qt styling
├── build/                            → Build configuration
│   ├── iso-build/
│   │   ├── spike-standard.conf       → Standard variant config
│   │   ├── spike-plus.conf           → Plus variant config
│   │   └── shared-packages.conf      → Common packages
│   ├── package-configs/              → Package selection configs
│   ├── live-environment/             → Live ISO base environment
│   └── signing/                      → Release signing keys
├── scripts/                          → Utility scripts
│   ├── build-iso.sh
│   ├── test-installer.sh
│   ├── hardware-test.sh
│   ├── install-dev-env.sh
│   ├── mirror-sync.sh                → GitLab → GitHub sync
│   └── changelog-gen.sh
├── ci/                               → CI/CD configuration
│   ├── dockerfiles/
│   └── jobs/
├── .editorconfig                     → Editor settings
├── .gitignore                        → Git ignore rules
├── .gitattributes                    → Git attributes
├── CMakeLists.txt                    → Root CMake config
├── CODE_OF_CONDUCT.md                → Pointer to docs/CODE_OF_CONDUCT.md
├── CONTRIBUTING.md                   → Pointer to docs/CONTRIBUTING.md
├── LICENSE                           → GPLv2+ license
└── README.md                         → Project overview + download links

Component Responsibilities
Component	Responsibility	Safe to Modify	Dangerous Areas
spike-shell/ui	Panel, launcher, tray applets	Qt widget code, CSS	Main event loop
spike-shell/notify	Notification daemon	History logic, UI	Disk-before-display invariant
spike-shell/session	Session manager	Autostart logic	Shutdown flow, GRUB failure counter
spike-installer/detect	Hardware detection	Detection logic	Module blacklist generation
spike-installer/partition	Partitioning	Partition sizes	Data destruction
spike-rescue/copy	File recovery	Checksum logic	Read-only mount enforcement
spike-config/*	System configuration	Tunable values	Critical kernel params
spike-branding/*	Visual identity	Colors, logos	GRUB theme syntax, KWin config
build/*	ISO generation	Package lists	Signing procedures
7. Critical Constraints
NEVER VIOLATE THESE

These constraints are non-negotiable. Any code change that violates them will be rejected.
Memory Constraints

□ Never exceed 400MB idle on Spike Standard (4GB RAM target)
□ Never exceed 800MB idle on Spike Plus (8GB+ RAM target)
□ Never add Akonadi, Baloo, or cloud-init
□ Never enable full Plasma workspace (only KWin standalone)
□ Never load Settings pages at startup (on-demand only)
□ Never cache more than 100 notification history entries in memory
□ Always check memory pressure before background tasks

Golden Rule Violations

□ Never require terminal for normal tasks
□ Never require config file editing for user-facing settings
□ Never expose a setting without providing its GUI control
□ Never use terminal commands in user guide documentation
□ Always provide GUI alternative for any CLI tool

Hardware Constraints

□ Never support eMMC storage (wearout issue)
□ Never support USB 2.0 drives (too slow)
□ Never require manual partitioning (automatic only)
□ Never enable dual boot option (single drive, single OS)
□ Never require 32-bit x86 support (x86_64 only)

Security Constraints

□ Never disable firewall (ufw deny incoming is mandatory)
□ Never disable AppArmor (Ubuntu defaults are minimum)
□ Never store passwords in plaintext
□ Never collect telemetry or usage data
□ Never force automatic reboots (gentle notification only)
□ Never allow root SSH login

Privacy Constraints

□ No network access without explicit user consent
□ No background data collection
□ No remote phone home features
□ No crash reporting that uploads user data
□ All logs stay local, never transmitted

Stability Constraints

□ Never break ABI without major version bump
□ Never remove user-visible features without migration path
□ Never change default behavior without explicit opt-in
□ Always maintain backward compatibility within major version
□ Document breaking changes in CHANGELOG.md and MIGRATION-GUIDE.md

8. Hardware Targets
Primary Target (Tier 1 — Must Be Daily-Driver Usable)

CPU:       Intel Celeron N4020 (Gemini Lake Refresh, 2019)
Cores:     2
Threads:   2
Base:      1.10 GHz
Turbo:     2.80 GHz
Cache:     4MB
RAM:       4GB DDR4-2400 (soldered)
GPU:       Intel UHD Graphics 600 (12 EU, 300-650 MHz)
Storage:   240GB SATA SSD (or HDD/SD/USB as fallback)
Display:   1440×900 or 1366×768 (typical 14" laptop)

Worst Case Test (Tier 2 — Must Boot and Function)

CPU:       AMD A4 (older, slower than Celeron)
RAM:       4GB
GPU:       AMD Radeon HD (integrated)
Storage:   500GB SATA HDD
Machine:   Lenovo ThinkPad with AMD A4

Dev Machine (Tier 3 — Regression Check Only)

CPU:       Intel Xeon (ThinkPad P50)
RAM:       32GB
GPU:       NVIDIA Quadro M2000M
Storage:   512GB NVMe
Use:       Development and regression testing only

Hardware Detection Requirements

The installer must detect and classify:

CPU:
├── Bogomips per core
├── Number of cores
├── CPU family/model (for driver selection)
└── Classification: capable (ZRAM) vs low-end (skip ZRAM)

Storage:
├── Type: SSD/HDD/NVMe/SD/USB/eMMC
├── Capacity
├── Rotational flag (/sys/block/[dev]/queue/rotational)
├── USB version (if USB device)
└── Reject: eMMC, USB 2.0

GPU:
├── Vendor: Intel/AMD/NVIDIA
├── Model name
├── VA-API driver to install
└── Enable/disable specific features (AV1 disabled on unsupported)

Memory:
├── Total RAM
├── Classification: Standard (≤4GB) vs Plus (≥8GB)
└── Recommend variant accordingly

Network:
├── Wi-Fi adapter present?
├── Ethernet adapter present?
├── Wi-Fi interface name
└── Load appropriate firmware

Bluetooth:
├── Bluetooth adapter present?
└── Conditionally load Bluetooth applet

9. Component Specifications
Spike Shell
Panel

Position: Bottom (default) or Top (Settings)
Height: 32px default (24-48px adjustable)
Auto-hide: Optional (Settings → Appearance → Panel)
Layout: Three zones (left, center, right)
Technology: wlr-layer-shell for overlay positioning

Left zone applets:
├── Spike button (opens launcher)
├── Launcher button (favorites)
└── Window list (running apps)

Right zone applets:
├── Notifications (badge)
├── Network
├── Volume
├── Battery (conditional)
├── Brightness (conditional)
├── Bluetooth (conditional)
├── Update Notifier
├── Removable Devices
├── Clock / Calendar
├── Session Menu
├── Night Light
├── Keyboard Layout (conditional)
└── Airplane Mode (conditional)

Memory budget: 15-20MB (panel itself, not counting applets)

Launcher

Style: Kickoff-style (KDE-inspired)
Sections:
├── Favorites (pre-populated, user-editable)
├── Recently Used (last 5, cleared on logout)
├── All Applications (by category)
└── Search (live, case-insensitive)

Categories:
├── Internet
├── Office
├── Media
├── Graphics
├── Games
├── System
├── Accessories
└── Development (conditional)

Database sources:
├── /usr/share/applications/
├── ~/.local/share/applications/
└── Flatpak exports (.local and /var/lib/flatpak/exports/share/applications/)

Features:
├── Single-click launch
├── Focus existing window if app is running
├── Recent app badge (🟢)
├── Keyboard navigation (Super, Escape, arrows, Tab)
└── inotify-watch for live updates

Memory budget: 15-25MB

Notification Daemon

Protocol: freedesktop.org Notifications DBus
Storage: ~/.local/share/spike/notifications/history.json
Retention: 3 days default (adjustable 1-31 days)
Max count: 500 default (adjustable 10-1000)

Invariant: DISK-BEFORE-DISPLAY
Every notification is written to history.json atomically BEFORE being shown.
If display layer crashes, notification is still in history.

Atomic write process:
├── Write to history.json.tmp
├── fsync(history.json.tmp)
├── rename(history.json.tmp, history.json)
├── fsync(directory)

Tray badge persists until user opens history viewer
History viewer accessible by clicking notification applet
Do Not Disturb mode suppresses popups (not history)
Transient hint ignored — all notifications persist to disk

Crash recovery:
├── systemd restarts daemon (Restart=always)
├── On restart: read history from disk, restore state
├── Counter continues from last persisted value
└── Notifications sent during downtime are lost (acceptable)

Memory budget: 5-8MB

Settings Panel

Type: Hybrid (custom pages + KDE KCM modules)
Load: On-demand only (never at startup)

Custom pages (Spike-specific):
├── Appearance
├── Notifications
├── Memory
├── Boot
├── Storage
├── Diagnostics
├── Users (subset)
└── About (with integrated user guide reader)

KDE KCM modules loaded (standard system settings):
├── Display
├── Sound
├── Power
├── Keyboard
├── Mouse/Touchpad
├── Bluetooth
├── Printer
├── Network
├── VPN
├── Date & Time
├── Accessibility
└── Software Sources

Search: Live search across all page titles and keywords
Help: Context-aware (?) button → opens integrated user guide
User guide location: /usr/share/spike/user-guide/
Offline reader: Integrated into Settings → About

Memory budget: 0MB (not running), ~10-20MB when open

Spike Installer
Flow (10 Steps)

1. Welcome + Language selection
2. Timezone selection (IP geolocation if online)
3. Wi-Fi connection (optional)
4. Username + password creation
5. Computer name (hostname suggestion)
6. Variant selection (Standard/Plus, auto-recommended)
7. Data backup to USB (optional, scans for personal files)
8. Storage confirmation + wipe warning (type "ERASE" to confirm)
9. Installation (automated, progress bar)
10. Reboot to desktop

Average time: 8-12 minutes on Celeron N4020 with SSD

Hardware Detection Modules

detect/cpu.cpp → bogomips, cores, classification
detect/storage.cpp → type, size, rejection (eMMC/USB 2.0)
detect/gpu.cpp → vendor, model, driver selection
detect/network.cpp → Wi-Fi/Ethernet adapter detection
detect/bluetooth.cpp → Bluetooth adapter detection
detect/modem.cpp → Mobile broadband detection

Installation Tasks (20 Steps)

1. Partition drive (UEFI or BIOS)
2. Create swap file
3. Configure ZRAM (if CPU capable)
4. Configure swappiness
5. Install base system packages
6. Install GPU drivers
7. Install audio (PipeWire + WirePlumber)
8. Install networking (NetworkManager + all firmware)
9. Generate module blacklist
10. Pre-seed Flatpak runtimes
11. Install Spike Shell
12. Apply GRUB2 configuration
13. Apply Plymouth theme
14. Apply Spike theme (Qt/KWin)
15. Configure security (AppArmor, ufw)
16. Create user account
17. Configure CPU governor
18. Write kernel tunables
19. Final configuration (locale, timezone, keyboard)
20. Unmount and finalize

Data Backup

Scans for: Windows/Linux/macOS user directories
Copies: Documents, Photos, Videos, Music, Downloads, Desktop
Checksum: SHA256 verification on every file
Partial corruption: Log failed files, continue with rest
Size requirement: USB must have ≥ total file size
Folder preservation: Exact structure replicated on USB
Output: /run/media/usb/SpikeBackup/

Data Restore

Trigger: During installation, if "Fresh install and restore" selected
Mount: USB drive containing SpikeBackup/
Copy: To /home/[new_username]/
Ownership: chown -R [new_user]:[new_user]
Verification: SHA256 checksum on all files
Not restored: System settings, app configs, passwords (security measure)
Only restored: Personal files (documents, photos, media)

Spike Rescue Tool
Purpose

Recover personal files from a broken Spike installation via live ISO.
Flow

1. Boot from Spike ISO
2. Click "Rescue My Files" icon on desktop
3. Scan for installed operating systems
4. If Spike detected: offer recovery or reinstall-with-restore
5. Mount source partition READ-ONLY
6. Scan for user data
7. Prompt for USB destination
8. Copy files with SHA256 verification
9. Log any failed files
10. Report summary: N files recovered, M files failed

Safety Guarantees

Read-only mount prevents further damage to filesystem
Failed files logged but don't abort operation
Checksum verification ensures integrity
User can preview file counts before committing
No system modifications made (data recovery only)

Spike Config System
Generates At Install Time

/etc/modprobe.d/spike-blacklist.conf → Module blacklist
/etc/default/grub → Boot parameters
/etc/systemd/zram-generator.conf → ZRAM configuration
/etc/sysctl.d/99-spike-memory.conf → Swappiness, VM tuning
/etc/sysctl.d/99-spike-fs.conf → Filesystem tuning
/etc/sysctl.d/99-spike-network.conf → Network tuning
/etc/udev/rules.d/99-spike-zram.rules → ZRAM udev trigger
/etc/ld.so.conf.d/spike.conf → Library paths
/usr/share/spike/themes/qt-stylesheet.qss → UI styling
/usr/share/spike/branding/kwin-decoration.rc → Window decoration

No Post-Install Editing Required

All settings accessible via Settings GUI. Config files exist on disk but users never edit them directly.
10. Development Workflow
GitLab CE Primary, GitHub Mirror

Primary:     git.bigrangatech.com (GitLab CE, self-hosted)
Mirror:      github.com/bigrangatech/spike (read-only mirror)

Workflow:
├── Developers fork from GitLab (not GitHub)
├── Merge requests submitted to GitLab
├── Issues tracked on GitLab
├── CI/CD pipelines run on GitLab CI
├── Releases published on GitLab
├── GitHub mirrors automatically via scripts/mirror-sync.sh
└── GitHub README states: "This is a mirror. Contribute at git.bigrangatech.com"

Contribution Phases

Pre-alpha (now through alpha start):
├── Read-only repository
├── Issues open (feedback welcome)
├── No code contributions accepted yet
└── Translations accepted

Alpha (months 1-8):
├── Bug reports welcome
├── Hardware testing encouraged
├── Translations open
├── Code patches: case-by-case, must include DCO
└── Review: BDFL only, no formal reviews

Beta (months 9-18):
├── Full contributions open
├── DCO mandatory for all code
├── Code review required
├── Branch protection on main
└── Contributors form team, BDFL breaks ties

Coding Conventions

C++ (Qt):
├── Follow Qt naming conventions
├── camelCase for methods/variables
├── PascalCase for classes
├── Member variables: m_prefix or trailing underscore
├── Comments: Qt Doc style (/** */ for public APIs)
├── No raw pointers, use QPointer/QSharedPointer/unique_ptr
├── Connect signals using function pointers (not QString)
└── Use Qt container types (QList, QMap, QString)

Bash/Shell:
├── Use strict mode (set -euo pipefail)
├── Quote all variables
├── Meaningful variable names
├── Comments for complex operations
├── Error messages to stderr
└── Exit codes meaningful

Python (scripts):
├── Python 3 compatible
├── PEP 8 style
├── Docstrings for functions
├── Logging, not print()
└── Type hints where practical

DCO (Developer Certificate of Origin)

Required for all code contributions. Signed with:
git commit -s -m "Commit message"

DCO sign-off certifies:
├── You wrote this contribution or have right to submit it
├── You understand it will be licensed under GPLv2+
├── You agree to the terms of the DCO
└── Attribution will be preserved in commit history

Branch Protection Rules (Post-Beta)

main branch protected:
├── No direct pushes
├── Pull/Merge request required
├── Code review from at least 1 maintainer
├── CI/CD must pass
├── DCO signature required
└── Squash merge preferred (clean history)

11. Testing Requirements
Hardware Testing Matrix

Tier 1 (Daily-driver usable, MUST PASS):
├── Intel Celeron N4020 (Gemini Lake, 4GB RAM)
├── Must boot, install, and run smoothly
└── All features functional

Tier 2 (Must boot and function, SHOULD PASS):
├── AMD A4 Lenovo (worst case performance)
├── Boot and install succeed
└── Performance may be degraded, features functional

Tier 3 (Regression check only):
├── ThinkPad P50 (Xeon, NVIDIA Quadro)
├── Ensures nothing breaks on higher-end hardware
└── Not representative of target users

Performance Baselines (Measured During Alpha)

Cold boot time: <40 seconds (target on SSD)
Idle memory (Standard): <400MB
Idle memory (Plus): <800MB
Discover launch time: <2 seconds
Firefox launch time: <3 seconds
Application launch time: <2 seconds (average)
Shutdown time: <5 seconds
Hibernate resume: <10 seconds (if implemented)

Testing Checklist Per Feature

Before merging a feature:
□ Works on Tier 1 hardware (Celeron N4020)
□ Doesn't exceed memory budget
□ Has corresponding user guide update
□ Has corresponding dev guide update
□ Tested on physical hardware (not just VM)
□ No regression on existing features
□ DCO signed
□ Code review passed (post-beta)
□ CI/CD pipeline passed
□ Documentation in docs/ directory
□ Screenshot added to user-guide/screenshots/ (if GUI change)

12. Troubleshooting Guide
Common Issues and Solutions
Installer Fails Partway Through

Problem: Installer crashed, system partially installed
Solution: Reboot from live ISO, run "Rescue My Files" to recover data, then fresh install

Boot Fails After Installation

Problem: System won't boot, GRUB shows error or black screen
Solution:
├── 3 consecutive failures trigger GRUB menu (automatic)
├── Select recovery entry from GRUB menu
├── Boot into recovery mode
├── Run spike-rescue to recover data to USB
└── Fresh install and restore from backup

Memory Pressure / Swap Thrashing

Problem: System is slow, high disk usage
Cause: Swap thrashing, ZRAM not enabled, or insufficient RAM
Check:
├── free -h (check swap usage)
├── zramctl (verify ZRAM active)
├── systemctl status earlyoom
└── dmesg | grep -i oom (check for OOM killer events)

Fix:
├── Close memory-intensive applications (Firefox tabs, etc.)
├── Verify ZRAM is enabled (should be on Celeron)
├── Check if earlyoom is protecting key processes
├── Consider hardware upgrade (more RAM) if persistent
└── Adjust swappiness if SSD (Settings → Advanced → Memory)

Network Manager Won't Connect

Problem: Wi-Fi won't connect, or no networks detected
Check:
├── nmcli device (verify Wi-Fi adapter detected)
├── lsmod | grep iwlwifi (Intel) / ath9k (Atheros) / rtl (Realtek)
├── systemctl status NetworkManager
└── dmesg | grep -i firmware (check for missing firmware)

Fix:
├── Restart NetworkManager: sudo systemctl restart NetworkManager
├── Reload driver module (sudo modprobe -r iwlwifi; sudo modprobe iwlwifi)
├── Verify firmware present: ls /lib/firmware/intel/
└── Reinstall firmware package if missing

Notification History Lost

Problem: Notifications disappeared, can't find them in history
This should NEVER happen. If it does, it's a critical bug.

Investigation:
├── Check ~/.local/share/spike/notifications/history.json exists
├── Check file permissions (should be readable by user)
├── Check disk space (full disk might prevent writes)
├── Check spike-notify service status
└── Look for segmentation faults in journalctl

Report: File issue on GitLab with full logs attached

Display Resolution Wrong

Problem: Screen resolution incorrect or stretched
Check:
├── xrandr --query (list available modes)
├── /usr/share/spike-branding/display-profiles/ (custom profiles)
└── Settings → Display

Fix:
├── Select correct resolution in Settings → Display
├── Check if custom profile exists for detected monitor
└── If persistent, report with xrandr output attached

Audio Not Working

Problem: No sound from speakers or headphones
Check:
├── pactl info (verify PipeWire active)
├── wpctl status (check WirePlumber state)
├── pactl list sinks (list audio outputs)
└── pavucontrol (volume control GUI)

Fix:
├── Restart PipeWire: systemctl --user restart pipewire pipewire-pulse wireplumber
├── Check muted status in pavucontrol
├── Verify correct output device selected
└── Reinstall PipeWire packages if needed

Black Screen After Login

Problem: SDDM shows login, but after login: black screen or frozen cursor
This is usually a compositor or shell crash.

Diagnosis:
├── Boot into recovery mode (GRUB menu)
├── SSH into system or use terminal console (Ctrl+Alt+F2)
├── journalctl -xb | grep -i spike (look for shell errors)
├── journalctl -xb | grep -i kwin (check compositor status)
└── cat /tmp/spike-shell.log (if exists)

Quick fix:
├── Reboot, try different TTY (Ctrl+Alt+F3) and kill/start spike-session
├── Or: Reboot into live ISO, backup data, fresh install
└── Preventative: Check for kernel panics or driver issues before merge

13. Common Mistakes
What NOT to Do

❌ Don't add features that require terminal usage
→ Always provide GUI alternative first

❌ Don't increase idle memory beyond budget
→ Measure before and after changes

❌ Don't modify GRUB without testing on all bootloader modes
→ Test UEFI and BIOS modes

❌ Don't skip hardware testing on Tier 1 machine
→ Celeron N4020 is non-negotiable

❌ Don't add dependencies without justification
→ Every package increases memory and attack surface

❌ Don't edit config files directly in code
→ Use template + substitution during install

❌ Don't assume users have internet access
→ Everything must work offline

❌ Don't prioritize aesthetics over performance
→ If animations hurt performance, disable them

❌ Don't change default behavior without opt-in
→ Preserve predictability for users

❌ Don't forget to update documentation
→ Every feature requires corresponding docs

What TO Do

✅ DO test on actual target hardware
✅ DO document every design decision in DESIGN-DECISIONS.md
✅ DO write tests for critical code paths
✅ DO follow Golden Rules religiously
✅ DO keep changes small and focused
✅ DO provide clear error messages
✅ DO include recovery paths for all operations
✅ DO benchmark performance changes
✅ DO consider edge cases (what if disk is full?)
✅ DO ask for clarification if unsure (better than guessing wrong)

14. References
Documentation Links

Full documentation tree:
├── docs/README.md → Entry point
├── docs/INDEX.md → Routing document
├── docs/PHILOSOPHY.md → Mission and values
├── docs/ARCHITECTURE.md → System overview
├── docs/DESIGN-DECISIONS.md → Rationale for every decision
├── docs/kernel.md → Kernel parameters and module config
├── docs/memory.md → ZRAM, swap, earlyoom
├── docs/desktop.md → Spike Shell specification
├── docs/installer.md → Installer design
├── docs/dev-guide/index.md → Developer manual
└── docs/user-guide/index.md → User manual (offline)

Website:
└── https://spike.bigrangatech.com/docs/

External Resources

Qt Documentation:
├── https://doc.qt.io/qt-6/
└── https://doc.qt.io/qt-6/qml-overview.html (reference, not for use)

KDE Frameworks:
├── https://develop.kde.org/
└── https://api.kde.org/

Wayland Protocols:
├── https://wayland.freedesktop.org/
└── https://gitlab.freedesktop.org/wayland/wlr-protocols

Linux Kernel Docs:
├── https://www.kernel.org/doc/html/latest/
└── https://docs.kernel.org/admin-guide/

Flatpak Docs:
├── https://docs.flatpak.org/
└── https://www.flatpak.org/doc/

PipeWire Docs:
├── https://pipewire.pages.freedesktop.org/
└── https://docs.pipewire.org/

NetworkManager Docs:
├── https://networkmanager.dev/docs/api/
└── https://developer.gnome.org/NetworkManager/stable/

PipeWire DBus API:
├── https://pipewire.org/docs/dbustopic.html
└── https://freedesktop.org/software/pipewire-doc/

Contact and Support

Primary support: https://spike.bigrangatech.com/support/
Issue tracker: https://git.bigrangatech.com/spike/-/issues
Discussion: https://bigrangatech.com/discussion (forum or Matrix room)
Abuse reports: abuse@bigrangatech.com
Documentation bugs: docs/issues on GitLab

Version History

0.1.0-alpha → Initial alpha, limited testing
0.2.0-alpha → Expanded testing, hardware matrix
0.3.0-beta → Public beta, broader testing
1.0.0-production → First production release
1.x.x → Point releases, bug fixes
2.0.0 → Major version, potential breaking changes

Document Maintenance

This document (AGENTS.md) must be kept synchronized with all other documentation. If something changes:

    Update this document FIRST
    Then update other affected documents (DESIGN-DECISIONS.md, component specs)
    Commit with clear description: "Update AGENTS.md with [change]"
    Update CHANGELOG.md with the modification

If this document becomes outdated, it is the PRIMARY source of truth for Spike project context. Other documents may lag, but this document should always reflect current state.

Last Updated: July 2026 Maintained By: BDFL (Ranga) Review Frequency: Monthly (during alpha), Quarterly (during beta) Status: Active and authoritative

🐕 BigRangaTech
