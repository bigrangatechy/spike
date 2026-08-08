# Spike — AGENTS.md

## Master Reference & Agent Guide

This document is the single source of truth for AI agents (and human contributors) working on the Spike Linux distribution. It summarizes every architectural decision, every subsystem, every document, and every cross-reference. If a detail is not in this document, it should be in one of the linked specification documents. If it's in neither, the decision hasn't been made yet.

## 1. Project Overview

| Property | Value |
| :-: | :-: |
| **Project** | Spike |
| **Organization** | BigRangaTech |
| **Type** | Linux distribution (Ubuntu Server LTS base) |
| **Mission** | "Let's Make Tech Repairable Again" |
| **Target** | Old/disposable laptops — specifically dual-core Celeron N4020 (Gemini Lake Refresh, 2019) with 4GB RAM |
| **Goal** | A lightweight OS that still feels like a complete, polished, "real" operating system — not a stripped-down afterthought |
| **Named after** | The creator's real dog, Spike (a herding dog) |
| **License** | GPLv2+ (code), CC-BY-SA 4.0 (documentation and artwork) |
| **Governance** | BDFL model initially, with documented succession plan |
| **Philosophy** | Build software that outlasts the creator. Documentation-first methodology: discuss until convergent, write docs, then code. |

## 2. Mission & Philosophy

Spike exists to save cheap, "disposable" laptops from landfill/recycling.

Core values:

```
├── Repairable: Hardware should be serviceable, software should be understandable
├── Accessible: Beginners should feel competent, not overwhelmed
├── Complete: A lightweight OS that doesn't feel lightweight to use
├── Private: Zero telemetry, zero data collection — privacy is the default
├── Long-lived: Built to outlast its creator, with documented succession
└── Honest: No dark patterns, no upsells, no hidden data collection
```

Golden Rules (from `PHILOSOPHY.md` — authoritative source):

```
 1. Users never edit config files
 2. CLI tools exist for developers only
 3. If it's not documented, it doesn't exist
 4. The user never has to touch the terminal
 5. If a GUI doesn't exist either make one or it doesn't exist
 6. Privacy is the default, not a toggle
 7. Your personal files are always recoverable
 8. The laptop belongs to the user. The data belongs to the user.
 9. Build software that outlasts the creator
10. Lightweight doesn't mean stripped down
```

> **Note:** These rules may expand as the project evolves. Any addition must be logged in `agent-ops/DECISIONS.md` and require explicit BDFL approval.

### GUI Requirement (Golden Rules 4 & 5)

- **GUI required for:** Every built-in Spike system tool and every system setting 

- **GUI NOT required for:** Developer tools (spike-config CLI, debugging utilities, dev-guide tooling) 

- **GUI NOT required for:** User-installed applications (Flatpak apps manage their own UI) 

- **If a system tool/setting has no GUI:** Either build a GUI for it in Settings, or the feature doesn't ship 

- **Goal:** Cohesive, complete desktop experience — no terminal required for any system task 

## 3. Target Hardware

Primary target:

```
├── CPU: Intel Celeron N4020 (Gemini Lake Refresh, 2019)
│   ├── 2 cores, 2 threads
│   ├── Base: 1.10 GHz, Burst: 2.80 GHz
│   ├── Bogomips: ~2300 per core
│   └── Classification: "capable" (dual-core, acceptable bogomips)
├── RAM: 4GB
├── GPU: Intel UHD Graphics 600
│   ├── Driver: i915
│   ├── VA-API driver: iHD (intel-media-va-driver-non-free)
│   ├── Hardware decode: H.264, H.265 (8-bit), VP8, VP9
│   └── Does NOT support: AV1
├── Storage: SATA SSD (primary), HDD, SD card, USB 3.0 (all supported)
│   ├── Minimum: 128GB
│   └── eMMC: NOT SUPPORTED (wear-out risk, based on user experience)
└── Network: Wi-Fi (various chipsets), Ethernet
```

Development/test machines:

```
├── ThinkPad P50 (Xeon, M2000M) — dev/regression machine
├── AMD A4 Lenovo — worst-case validation (low bogomips, ZRAM skipped)
└── Celeron N4020 laptop — primary target validation
```

CPU classification thresholds:

```
├── "low-end": bogomips < 2000/core (e.g., AMD A4) → ZRAM skipped, swap only
├── "capable": bogomips >= 2200/core, dual-core+ (e.g., Celeron N4020) → ZRAM enabled
├── "modern": Core i3/i5/i7/i9, Ryzen, Athlon (post-2018) → full capabilities
└── "Celeron/Pentium class": model name contains Celeron/Pentium/Atom (pre-2018)
```

## 4. Architecture Overview

### Base System

| Property | Value |
| :-: | :-: |
| **Base** | Ubuntu Server 26.04 LTS (Resolute) — not Desktop; stripped per architecture/privacy specs |
| **Kernel** | Ubuntu LTS kernel (fresher than Debian Stable, 10-year ESM) |
| **Architecture** | x86_64 only (no 32-bit — Y2038 acknowledged for future i386 fork) |
| **Display** | Wayland (KWin) + XWayland fallback |
| **Desktop** | Custom lightweight KDE-feel shell (panel, launcher, session) — NOT full KDE Plasma — standalone KWin + custom shell |
| **Init system** | systemd |
| **Package mgr** | apt (system) + Flatpak (user applications via Discover) |
| **Snap** | REMOVED entirely (snapd not installed) |

### Variant System

Single ISO. Two variants. Same codebase, different resource allocation.

Variant is selected at install time based on hardware detection. User can override (warning shown if mismatched). Post-install variant switching available (no reinstall needed).

See: `VARIANT-DIFFERENCES.md` (authoritative — 14 differences documented)

### Complete Variant Comparison (14 Differences)

| # | Feature | Spike Standard | Spike Plus |
| :-: | :-: | :-: | :-: |
| 1 | ZRAM cap | 4GB max | Uncapped (up to RAM size) |
| 2 | Animations | Off | On (150-200ms transitions) |
| 3 | Compositor effects | Minimal (no blur/transparency/shadows) | Full (blur, transparency, shadows) |
| 4 | Plymouth theme | spike-minimal (static logo, progress dots) | spike-full (animated logo, progress bar) |
| 5 | CPU governor | powersave | schedutil |
| 6 | Bluetooth codecs | SBC only | SBC, AAC, LDAC, aptX |
| 7 | Flatpak runtimes | KDE + GNOME pre-seeded | All common runtimes pre-seeded |
| 8 | Applet polling | Conservative intervals (10s/30s) | Standard intervals (5s/15s) |
| 9 | Qt rendering | Software rasterizer preferred | OpenGL preferred |
| 10 | Spike button hover | No glow effect | Subtle purple glow |
| 11 | Hybrid sleep | ON (safety net for low RAM) | OFF (sufficient RAM) |
| 12 | Idle RAM target | <400MB | <800MB |
| 13 | Background services | Minimal set | Standard set (additional hw-conditional services) |
| 14 | Memory budget | Tighter allocation (~280-370MB idle) | Relaxed allocation (~420-650MB idle) |

Everything NOT listed above is identical between variants.

## 5. Documentation Structure

72 total documents across three tiers, plus agent operations:

### TIER 1 — Top-Level Specifications (36 files)

```
├── README.md                 → Project introduction and quick start
├── INDEX.md                  → Master document index (this structure)
├── LICENSE                   → GPLv2+ (code), CC-BY-SA 4.0 (docs/artwork)
├── AGENTS.md                 → Master reference for AI agents (THIS FILE)
├── PHILOSOPHY.md             → Mission, values, golden rules
├── GOVERNANCE.md             → BDFL model, succession plan, decision authority
├── ROADMAP.md                → Development timeline, milestones, release plan
├── END-OF-LIFE-POLICY.md     → EOL schedule, LTS commitment, support tiers
│
├── ARCHITECTURE.md           → System overview, component diagram, data flow
├── DESIGN-DECISIONS.md       → The "why" document — rationale for every decision
├── MEMORY.md                 → Adaptive ZRAM/swap, memory ladder, earlyoom
├── DESKTOP.md                → Spike Shell: panel, launcher, notifications, session
├── BOOT-PROCESS.md           → Boot sequence, Plymouth, SDDM, systemd services
├── POWER-MANAGEMENT.md       → Power profiles, CPU governor, suspend, battery
├── MULTIMEDIA.md             → PipeWire, VA-API, Bluetooth audio, Firefox tuning
├── NETWORKING.md             → NetworkManager, Wi-Fi firmware, firewall, VPN
├── KERNEL.md                 → Module blacklisting, boot params, GPU drivers, sysctl
├── VARIANT-DIFFERENCES.md   → Exhaustive Standard vs Plus comparison (14 diffs)
│
├── INSTALLER.md              → 10-step installer, detection, partitioning, backup
├── DISASTER-RECOVERY.md      → 4-layer recovery model, Spike Rescue, restore
├── SPIKE-RECOVERY-TOOL-GENERAL.md → Rescue vs Migration vs installer map
├── SPIKE-MIGRATION.md        → spike-migration (Move My Files) product/tech spec
├── CONFIGURATION.md          → spike-config: state store, templates, changelog
│
├── SECURITY.md               → Threat model, AppArmor, firewall, updates, hardening
├── PRIVACY.md               → Zero telemetry, app permissions, data retention, browser
│
├── PERFORMANCE-BASELINES.md  → Boot time, memory, app launch benchmarks (alpha+)
├── BRANDING.md               → Logo, splash, GRUB theme, colors, typography
│
├── USER-GUIDE.md             → Getting started, daily use, settings walkthrough
├── TROUBLESHOOTING.md        → Common problems and solutions
├── FAQ.md                    → Frequently asked questions
├── MIGRATION-GUIDE.md        → Coming from Windows/macOS/other Linux
├── SUPPORT.md                → How to get help, report bugs, contribute feedback
│
├── ACCESSIBILITY.md          → Screen reader, keyboard nav, contrast, font scaling
├── TRANSLATIONS.md           → i18n framework, translation contribution
│
├── CODE_OF_CONDUCT.md        → Community standards
├── CONTRIBUTING.md           → How to contribute (phased model, DCO)
└── CHANGELOG.md              → Version history (per release)
```

### TIER 2 — User Guide Sub-documents (17 files)

```
├── user-guide/
│   ├── 01-getting-started.md
│   ├── 02-desktop-tour.md
│   ├── 03-installing-apps.md
│   ├── 04-file-management.md
│   ├── 05-settings-overview.md
│   ├── 06-network-and-wifi.md
│   ├── 07-bluetooth.md
│   ├── 08-sound-and-video.md
│   ├── 09-power-and-battery.md
│   ├── 10-printing.md
│   ├── 11-backup-and-recovery.md
│   ├── 12-users-and-accounts.md
│   ├── 13-keyboard-shortcuts.md
│   ├── 14-software-updates.md
│   ├── 15-accessibility.md
│   ├── 16-troubleshooting.md
│   └── 17-glossary.md
```

### TIER 3 — Developer Guide Sub-documents (19 files)

```
├── dev-guide/
│   ├── 01-dev-environment.md
│   ├── 02-build-system.md
│   ├── 03-iso-construction.md
│   ├── 04-package-management.md
│   ├── 05-testing-framework.md
│   ├── 06-ci-cd.md
│   ├── 07-release-process.md
│   ├── 08-spike-config-internals.md
│   ├── 09-installer-internals.md
│   ├── 10-shell-development.md
│   ├── 11-theme-engine.md
│   ├── 12-hardware-detection.md
│   ├── 13-translation-workflow.md
│   ├── 14-security-review.md
│   ├── 15-performance-profiling.md
│   ├── 16-debugging-guide.md
│   ├── 17-coding-standards.md
│   ├── 18-git-workflow.md
│   └── 19-architecture-deep-dive.md
```

### Agent Operations (23 files)

Not counted in the 70 specification documents. These are operational reference files for AI agents and human contributors working on Spike.

```
├── agent-ops/
│   ├── README.md              → Folder index, reading order, orientation
│   ├── RULES.md               → Workflow rules (documentation-first mandate)
│   ├── CONSTRAINTS.md         → Hard technical constraints (never do X)
│   ├── PREFERENCES.md         → Soft preferences (prefer X over Y)
│   ├── CONVENTIONS.md         → Code style, naming, commit messages
│   ├── PROTOCOLS.md           → Step-by-step procedures for common situations
│   ├── PATTERNS.md            → Approved implementation patterns
│   ├── TEMPLATES.md           → Fill-in structures for docs/specs/entries
│   ├── GLOSSARY.md            → Project terminology
│   ├── GOTCHAS.md             → Known traps, easy mistakes
│   ├── SELF-CHECK.md          → Pre-completion checklist
│   ├── QUICK-REF.md           → One-page cheat sheet of critical info
│   ├── FAQ.md                 → Common agent questions with answers
│   ├── TROUBLESHOOTING.md     → What to try when the agent gets stuck
│   ├── SECURITY-CHECKLIST.md  → Agent-specific security review items
│   ├── MISTAKES.md            → Catalog of mistakes (grows over time)
│   ├── CHANGELOG.md           → Changelog for agent-ops folder itself
│   │
│   ├── STATE.md               → Dynamic: progress tracker
│   ├── DECISIONS.md           → Dynamic: decision log
│   ├── SESSION_LOG.md         → Dynamic: session history
│   │
│   └── adapters/
│       ├── .cursorrules       → Cursor IDE adapter
│       ├── CLAUDE-CODE.md     → Future: Claude Code adapter
│       └── AIDER.md           → Future: Aider adapter
```

> **For agents:** Read `agent-ops/README.md` first. It defines reading order and how to use the agent-ops folder.

### Phase 2 Document Status

**COMPLETED — Core technical specs (8 documents, drafted during Phase 2):**

```
├── ✅ POWER-MANAGEMENT.md     → Power profiles, CPU, suspend, battery
├── ✅ MULTIMEDIA.md            → PipeWire, VA-API, Firefox, Bluetooth audio
├── ✅ NETWORKING.md            → NetworkManager, firmware, firewall, VPN
├── ✅ DISASTER-RECOVERY.md     → 4-layer recovery, Spike Rescue, restore
├── ✅ SECURITY.md              → Threat model, AppArmor, updates, hardening
├── ✅ PRIVACY.md               → Zero telemetry, permissions, data retention
├── ✅ CONFIGURATION.md         → spike-config architecture, state store
└── ✅ VARIANT-DIFFERENCES.md   → Exhaustive Standard vs Plus comparison
```

**COMPLETED — Additional top-level docs (formatted for GitLab Markdown):**

```
├── ✅ README.md                → Project introduction and quick start
├── ✅ PHILOSOPHY.md            → Mission, values, golden rules (10 rules)
├── ✅ GOVERNANCE.md            → BDFL model, succession plan
├── ✅ ARCHITECTURE.md          → System overview, component diagram
├── ✅ DESKTOP.md               → Spike Shell: panel, launcher, notifications
├── ✅ BOOT-PROCESS.md          → Boot sequence, Plymouth, SDDM, services
├── ✅ KERNEL.md                → Modules, boot params, GPU drivers, sysctl
├── ✅ PERFORMANCE-BASELINES.md → Benchmarks (estimates until alpha)
├── ✅ INSTALLER.md             → 10-step installer, detection, partitioning
├── ✅ DISASTER-RECOVERY.md     → (also in Phase 2 — fully drafted)
├── ✅ SPIKE-RECOVERY-TOOL-GENERAL.md → Rescue vs Migration tool map
├── 📝 SPIKE-MIGRATION.md       → Move My Files (spec; not implemented yet)
├── ✅ BRANDING.md              → Logo, splash, GRUB theme, colors, typography
├── ✅ TROUBLESHOOTING.md       → Common problems and solutions
├── ✅ FAQ.md                   → Frequently asked questions
├── ✅ MIGRATION-GUIDE.md       → Coming from Windows/macOS/other Linux
├── ✅ SUPPORT.md               → How to get help, report bugs, feedback
├── ✅ END-OF-LIFE-POLICY.md    → EOL schedule, LTS commitment
├── ✅ ROADMAP.md               → Development timeline, milestones
├── ✅ CHANGELOG.md             → Version history format and milestones
├── ✅ CODE_OF_CONDUCT.md       → Community standards and enforcement
├── ✅ CONTRIBUTING.md          → How to contribute (phased model, DCO)
├── ✅ AGENTS.md                → This file (master reference)
├── ✅ MEMORY.md                → ZRAM/swap spec, memory ladder, earlyoom
├── ✅ INDEX.md                 → Master document index
├── ✅ DESIGN-DECISIONS.md      → Rationale for every major decision
├── ✅ ACCESSIBILITY.md         → Screen reader, keyboard nav, contrast
├── ✅ SECURITY.md              → (also in Phase 2 — fully drafted)
├── ✅ PRIVACY.md               → (also in Phase 2 — fully drafted)
├── ✅ NETWORKING.md            → (also in Phase 2 — fully drafted)
├── ✅ POWER-MANAGEMENT.md     → (also in Phase 2 — fully drafted)
├── ✅ MULTIMEDIA.md            → (also in Phase 2 — fully drafted)
├── ✅ VARIANT-DIFFERENCES.md  → (also in Phase 2 — fully drafted)
└── ✅ HARDWARE.md              → Hardware tiers, detection logic, support
```

**REMAINING (top-level, pending drafting):**

```
├── 🔲 TRANSLATIONS.md          → i18n framework, translation contribution
└── 🔲 USER-GUIDE.md            → Getting started overview (sub-docs filled as desktop ships)
```

**Licenses (repo root):**

```
├── ✅ LICENSE                   → GPLv2+ (code)
└── ✅ LICENSE-CC-BY-SA-4.0      → documentation and artwork (copies also in licences/)
```

**REMAINING (sub-document directories):**

```
├── 🔲 All 17 user-guide/ sub-documents (stubs — fill as desktop ships)
└── 🔲 Remaining 16 of 19 `dev-guide/` stubs (03/04/07 drafted — build + installer)
```

**AGENT OPERATIONS:**

```
├── ✅ README.md               → Folder index, reading order
├── ✅ RULES.md                → Workflow rules, documentation-first mandate
├── ✅ CONSTRAINTS.md         → Hard technical constraints
├── ✅ GLOSSARY.md             → Project terminology
├── ✅ CONVENTIONS.md         → Code style, naming, commits
├── ✅ PROTOCOLS.md           → Step-by-step procedures
├── ✅ STATE.md               → Dynamic: progress tracker (initialized)
├── ✅ DECISIONS.md           → Dynamic: decision log (initialized)
├── ✅ SESSION_LOG.md         → Dynamic: session history (initialized)
├── 🔲 PATTERNS.md            → Implementation patterns
├── 🔲 TEMPLATES.md           → Fill-in structures
├── 🔲 SELF-CHECK.md          → Pre-completion checklist
├── 🔲 SECURITY-CHECKLIST.md → Agent-specific security review
├── 🔲 GOTCHAS.md             → Known traps
├── 🔲 PREFERENCES.md        → Soft preferences
├── 🔲 QUICK-REF.md          → One-page cheat sheet
├── 🔲 FAQ.md                → Common agent questions
├── 🔲 TROUBLESHOOTING.md    → What to try when stuck
├── 🔲 MISTAKES.md           → Common mistakes catalog
├── 🔲 CHANGELOG.md          → agent-ops changelog
├── 🔲 adapters/.cursorrules → Cursor adapter
├── 🔲 adapters/CLAUDE-CODE.md → Future
└── 🔲 adapters/AIDER.md     → Future
```

## 6. Organization & Branding

| Property | Value |
| :-: | :-: |
| **Organization** | BigRangaTech (existing org with multiple projects) |
| **Product** | Spike (dedicated project under BigRangaTech) |
| **Logo** | Herding dog (Spike the real dog) with circuit board flanks, purple/cyan glow, circular emblem |
| **Splash screen** | BigRangaTech branded, Spike dog, dark background with glowing circuit traces |
| **Slogan** | "Let's Make Tech Repairable Again" |
| **Colors** | Purple (#6d4aff) and cyan/teal as system accent colors |
| **GRUB theme** | Dark background, Spike emblem, BigRangaTech branding, purple/cyan accents |
| **Typography** | Noto Sans (default), Noto Sans Mono (monospace) |
| **Icon theme** | Breeze (KDE default) |

Each BigRangaTech project gets its own dedicated docs and eventually its own website.

## 7. Base & System Architecture

### Base Distribution

Ubuntu Server **26.04 LTS** was chosen over alternatives:

Considered and rejected:

```
├── Debian Stable — older kernel/Mesa, slower security updates
├── Fedora — 13-month lifecycle too short for rescue laptops
├── Arch — rolling release instability unsuitable for beginners
├── Alpine — musl libc breaks some Flatpaks and proprietary apps
├── Buildroot — no package manager, no upgrade path, KDE apps need real system
└── Ubuntu Desktop — includes Snap, Canonical branding, desktop bloat
```

Ubuntu Server LTS provides:

```
├── Fresher kernel and Mesa than Debian Stable
├── 10-year ESM support lifecycle
├── Clean starting point (no desktop, no Snap, no Canonical branding)
├── Massive package repository
├── Well-documented, well-tested base
└── Security updates flowing from Canonical
```

Architecture: x86_64 only

```
├── No 32-bit version (would require forking i386 libraries)
├── 32-bit deferred conditionally — only pursued if proven necessary
└── Y2038 problem acknowledged for potential future 32-bit project
```

### Display & Desktop

| Property | Value |
| :-: | :-: |
| **Display server** | Wayland (KWin as compositor) |
| **XWayland** | Available as fallback for X11-only applications |
| **Desktop shell** | Custom lightweight shell (Spike Shell) — NOT full KDE Plasma — standalone KWin + custom components |

Spike Shell components:

```
├── Panel (bottom, 3 zones: left/center/right, 32px height default)
├── Application launcher (Kickoff-style, categories, search, favorites)
├── Notification daemon (disk-before-display invariant)
├── Session manager (login, logout, autostart filtering)
├── Tray applet framework
├── Settings panel (hybrid: custom pages + KDE KCM modules)
└── First-run experience (welcome wizard)
```

KDE standalone apps included (NOT full Plasma):

```
├── Discover (software manager — Flatpak frontend)
├── Dolphin (file manager)
├── Konsole (terminal emulator)
├── Kate (text editor)
├── Ark (archive manager)
├── Spectacle (screenshot tool)
├── KCalc (calculator)
└── Selected Settings KCMs (kscreen, plasma-pa, powerdevil, bluedevil, print-manager)
    └── Not full System Settings; not plasma-nm; never plasma-desktop/workspace
```

Flatpak via Discover for user applications:

```
├── Pre-seeded KDE + GNOME Flatpak runtimes on ISO (Standard)
├── Pre-seeded all common runtimes (Plus)
├── Firefox (Flatpak, Spike-tuned prefs)
├── LibreOffice (Flatpak)
└── User installs any additional apps via Discover
```

### Bootloader

| Property | Value |
| :-: | :-: |
| **Bootloader** | GRUB2 (current) |
| **Future** | Documented migration path to Limine (not imminent) |

GRUB configuration:

```
├── Theme: Spike/BigRangaTech logo on dark background, purple/cyan accents
├── Boot menu hidden by default
├── 3-5 second ESC window to show menu
├── Recovery entry always present
├── Boot failure counter: after 3 consecutive failures, GRUB menu shows
│   automatically with recovery entry highlighted, 10-15 second timeout
├── GRUB_DISABLE_OS_PROBER=true (no dual boot detection)
├── GRUB_DISABLE_SUBMENU=y (flat kernel list)
└── GRUB_TERMINAL_OUTPUT=gfxterm (themed graphical output)
```

Boot parameters:

```
├── quiet splash zswap.enabled=0 transparent_hugepage=madvise
├── zswap disabled (ZRAM is used instead — they conflict)
└── THP set to madvise (prevents khugepaged overhead)
```

### Partitioning & Storage

| Property | Value |
| :-: | :-: |
| **Partitioning** | Fully automatic, no manual option (targeting beginners) |
| **Filesystem** | ext4 everywhere (Btrfs considered and rejected for simplicity) |

Partitions:

```
├── /boot/efi — 512MB FAT32 (UEFI systems)
│   OR /boot — 1GB ext4 (BIOS/Legacy systems)
├── / — rest of disk, ext4
└── /swapfile — 8GB (SSD/HDD) or 4GB (SD/USB)
```

Mount flags (adaptive based on storage type):

```
├── SSD/NVMe:  defaults,noatime
├── HDD:       defaults,noatime,commit=60
├── SD/USB:    defaults,noatime,commit=60,discard
```

Storage detection: `lsblk -d -o NAME,ROTA` (rotational flag)

Minimum storage: 128GB

Supported storage:

```
├── SATA SSD ✅
├── SATA HDD ✅
├── NVMe SSD ✅
├── SD card (UHS-I+, 128GB+) ✅
├── USB 3.0 thumb drive (128GB+) ✅
└── eMMC ❌ (NOT SUPPORTED — wear-out risk)
```

No dual boot support. No disk encryption (LUKS) — deliberate decision for beginner recovery simplicity.

## 8. Memory Management

Full memory ladder:

```
Physical RAM (4GB) → ZRAM compressed (~8-10GB effective) → Swap file (8GB) → Earlyoom
```

ZRAM:

```
├── Compression: zstd
├── Disksize: Equal to RAM (Standard: capped at 4GB, Plus: uncapped)
├── Priority: 100
├── Only enabled on capable CPUs (dual-core+, bogomips >= ~2200/core)
├── Low-end CPUs (AMD A4): ZRAM skipped, swap only
└── zswap disabled on kernel command line (conflicts with ZRAM)
```

Swap file:

```
├── Size: 8GB (SSD/HDD), 4GB (SD/USB)
├── Priority: 10 (lower than ZRAM's 100)
├── Located at /swapfile
└── Created at install time
```

Swappiness (per storage type):

```
├── SSD: 15 (prefer RAM, use swap sparingly)
├── HDD: 5 (avoid swap — too slow)
├── SD/USB: 10 (balance wear and performance)
└── User-adjustable slider in Settings (SSD only, increase only, range 15-60)
```

Earlyoom:

```
├── Threshold: 10% of memory+swap remaining
├── Protected processes: spike-shell, kwin_wayland, systemd, pipewire, wireplumber
├── Preferred kill targets: firefox, chromium, libreoffice, gimp, blender
└── Safety valve to prevent system freeze on memory exhaustion
```

HDD warning message:

```
"Swap performance on HDD may be slow. Consider upgrading to a SATA SSD
 for significantly better performance."
```

See: `MEMORY.md` (full spec), `CONFIGURATION.md` (memory module)

## 9. Component Specifications

### Power Management

See: `POWER-MANAGEMENT.md` (complete)

Three power profiles:

```
├── Performance (AC power): CPU governor = schedutil/performance,
│   Wi-Fi/BT/USB autosuspend OFF, no dimming
├── Battery Saver (battery <50%): CPU governor = powersave,
│   Wi-Fi/BT/USB autosuspend ON, aggressive dimming
└── Critical (battery <20%): CPU governor = powersave, dim to 30%,
    urgent notifications, auto-shutdown countdown at 5%
```

CPU governor:

```
├── Standard variant: powersave (default)
├── Plus variant: schedutil (default)
├── Intel_pstate preferred for Intel CPUs
├── Performance mode available as temporary override (AC only)
└── Reverts to variant default when profile changes
```

Screen management:

```
├── Screen blank: 15 min (AC), 5 min (battery)
├── Dimming: 5 min idle (AC), 3 min (battery), target 50% brightness
├── Lid close: suspend (default, configurable)
├── Power button: suspend (default, configurable)
└── Night Light: manual schedule or sunset-to-sunrise (requires location)
```

Sleep states:

```
├── Suspend: Default sleep action
├── Hibernate: Available if swap >= RAM
├── Hybrid sleep: ON for Standard (safety net), OFF for Plus
└── Require password on wake: YES (default)
```

Battery health:

```
├── Monitoring: percentage, capacity, cycle count, charge limiting (if supported)
├── Warnings: 20% (notification), 10% (urgent), 5% (auto-shutdown countdown)
├── Health history: monthly readings stored in /var/lib/spike/power/
└── Battery data never transmitted off-device
```

Peripheral power saving:

```
├── Wi-Fi power saving: adaptive (off on AC, on on battery saver)
├── Bluetooth power saving: adaptive (off on AC, on on battery saver)
├── USB autosuspend: adaptive (off on AC, on on battery saver)
└── HDD spindown: 10 min (battery), 15 min (AC), HDD only
```

### Multimedia

See: `MULTIMEDIA.md` (complete)

Audio stack:

```
├── PipeWire (daemon) + WirePlumber (session manager, minimal config)
├── Sample rate: 44100Hz (locked, prevents dynamic switching overhead)
├── Channels: 2 (stereo only)
├── Resampler quality: 1 (low, saves CPU — same for both variants)
├── Logging: suppressed (level 1, warnings only)
├── No JACK / pro audio features
└── Volume: 0-150% (boost above 100% is software amplification)
```

Bluetooth audio:

```
├── Conditional on hardware detection (udev)
├── Codecs:
│   ├── Standard: SBC only
│   └── Plus: SBC, AAC, LDAC, aptX
├── Codec negotiation: PipeWire/WirePlumber (best available per device)
├── User can override codec per device
└── Audio routing priority: Bluetooth → HDMI → headphone jack → speakers
```

Video acceleration (VA-API):

```
├── Intel: intel-media-va-driver-non-free (LIBVA_DRIVER_NAME=iHD)
├── AMD: mesa-va-drivers (LIBVA_DRIVER_NAME=radeonsi)
├── NVIDIA: vdpau-va-driver (or proprietary via VDPAU)
├── N4020 hardware decode: H.264, H.265 (8-bit), VP8, VP9
├── AV1: NOT supported on Gemini Lake → disabled in Firefox
└── VA-API config is IDENTICAL between variants
```

Firefox tuning:

```
├── media.ffmpeg.vaapi.enabled = true
├── media.ffmpeg.vaapi.force-enabled = true
├── media.ffvpx.enabled = false (disable FFmpeg VPX software decoder)
├── media.av1.enabled = false (force VP9 fallback → hardware decode)
├── gfx.webrender.all = true
└── Prefs stored in: /usr/share/spike/firefox/spike-prefs.js
```

Screen capture:

```
├── Spectacle (KDE) — full screen, region, window, active screen
├── Wayland portal API for app-initiated capture
└── Print Screen key → Spectacle region capture
```

WebRTC:

```
├── Video calls supported in Firefox
├── Camera: uvcvideo (universal USB camera driver)
├── Microphone: via PipeWire
├── Echo cancellation + noise suppression: enabled (PipeWire)
├── Camera/mic access via portal permissions (ask by default)
└── Camera/mic indicators in panel when active (non-negotiable)
```

Keyboard shortcuts:

```
├── Print Screen → Spectacle region capture
├── Volume Up/Down/Mute → native keys
├── Brightness Up/Down → native keys
├── Play/Pause/Next/Prev → media keys
└── Super key → Open launcher
```

### Networking

See: `NETWORKING.md` (complete)

| Property | Value |
| :-: | :-: |
| **Network manager** | NetworkManager (sole manager) |
| **DHCP client** | dhclient |
| **DNS resolver** | systemd-resolved (caches locally, no app can bypass) |

Wi-Fi firmware (ALL pre-installed on ISO):

```
├── Intel: iwlwifi
├── Atheros: ath9k, ath10k
├── Realtek: rtl8723de (ant_sel=2 for improved signal), rtl88xx
├── Broadcom: bcmwl (requires MOK enrollment if Secure Boot enabled;
│   b43/ssb/bcma/brcmsmac blacklisted)
└── No firmware downloads needed post-install
```

Wi-Fi power saving: follows power profile (Performance: off, Battery Saver: on)

DNS configuration:

```
├── Default: router-provided via DHCP
├── Fallback: 1.1.1.1, 8.8.8.8
├── DNS-over-TLS: supported (disabled by default, user can enable)
├── DNS-over-HTTPS: available in Firefox (disabled system-wide)
└── No app can bypass system DNS (prevents DNS leaks)
```

Firewall (ufw):

```
├── Default incoming: DENY
├── Default outgoing: ALLOW
├── Pre-configured rule: 5353/udp allow (mDNS for printer discovery)
├── GUI in Settings → Network → Firewall
└── ufw enabled at boot
```

VPN support: OpenVPN, WireGuard, IPSec/L2TP (via NetworkManager)

Mobile broadband: conditional on modem detection (ModemManager)

Captive portal: automatic detection, auto-opens Firefox to login page

Wi-Fi hotspot: supported (Settings → Network → Hotspot)

Airplane mode: via rfkill (disables Wi-Fi and Bluetooth)

Network diagnostics: GUI with ping/DNS/speed tests, exportable report

Network tray applet: signal strength, connection status, quick connect

MAC randomization: off by default (optional per-network for public Wi-Fi)

See also: `PRIVACY.md` (DNS privacy, MAC randomization, VPN privacy)

### Disaster Recovery

See: `DISASTER-RECOVERY.md` (complete)

4-Layer Recovery Model:

**Layer 1 — Boot Failure Counter (automatic):**

```
├── Initramfs increments counter on each boot
├── After 3 consecutive failures: GRUB menu shows automatically
├── Recovery entry highlighted, 10-15 second timeout
├── Successful boot resets counter to 0
└── Previous kernel fallback available
```

**Layer 2 — Recovery Mode (GRUB entry):**

```
├── Recovery entry always present in GRUB menu
├── Boots into minimal environment (single-user root shell)
├── No graphics, no network, minimal services
├── User can run diagnostic commands, fix configs, rollback
└── spike-config --rollback available
```

**Layer 3 — Spike Rescue Tool (GUI on live ISO):**

```
├── Boot from Spike USB installer
├── "Rescue my data" option (instead of install)
├── Mounts broken system READ-ONLY
├── Scans for user data (Linux/Windows/macOS partitions)
├── Copies to USB with SHA256 verification
├── Preserves folder structure
└── Handles filesystem errors gracefully
```

**Layer 4 — Reinstall with Restore:**

```
├── Installer detects existing Spike installation
├── Offers "Fresh install" or "Fresh install and restore my data"
├── If SpikeBackup folder found on USB: restores after install
├── Fixes file ownership/permissions (chown to new user)
├── Restores personal files only:
│   Documents, Pictures, Videos, Music, Downloads, Desktop
├── Does NOT restore: system configs, app configs, browser data
└── Fresh system with user's files back in place
```

Recovery promise: "Your personal files are always recoverable"

7 failure scenarios documented:

1. Kernel update broke boot → Layer 1 (previous kernel) 

2. Config change broke boot → Layer 2 (recovery mode + rollback) 

3. System won't boot at all → Layer 3 (live ISO rescue) 

4. Disk corruption → Layer 3 (read-only mount, copy what's salvageable) 

5. Forgot password → Layer 2 (root shell, passwd command) 

6. Failed upgrade → Layer 4 (reinstall + restore) 

7. Dead hard drive → Layer 3 (boot from USB, recover to new drive) 

### Security

See: `SECURITY.md` (complete)

Threat model: Protecting a beginner user's laptop on untrusted networks (NOT enterprise, NOT nation-state, NOT physical tamper resistance)

Authentication:

```
├── Standard user/sudo model (Ubuntu default)
├── Root account locked
├── One user password for login, sudo, polkit, screen unlock
├── Min 6 characters, no complexity requirements, no expiration
└── No auto-login by default (optional in settings)
```

AppArmor: Enabled with Ubuntu defaults, no custom profiles

Firewall (ufw):

```
├── Deny incoming, allow outgoing
├── Pre-configured mDNS rule (5353/udp)
└── See NETWORKING.md for full firewall spec
```

Automatic updates:

```
├── Security updates: auto-install (background, idle 10+ min, low CPU)
│   Via spike-update.timer every 6 hours
├── Non-security updates: notify via Discover (user installs)
├── Flatpak updates: notify only (user installs via Discover)
└── NEVER force reboot — gentle notification only
```

Secure Boot: Supported but not required

```
├── Works with Secure Boot on or off
├── Broadcom Wi-Fi drivers need MOK enrollment if Secure Boot enabled
└── MOK enrollment prompt at first boot if Broadcom detected
```

Browser security (Firefox):

```
├── Enhanced Tracking Protection: Strict
├── HTTPS-Only mode: Enabled
├── Popup blocking: Enabled
├── No Flash/Java
├── DRM: Enabled (user choice for streaming)
└── Prefs managed by spike-config (see CONFIGURATION.md)
```

No SSH server installed (client only)

No antivirus (defense in depth via sandboxing, AppArmor, firewall, no auto-execution)

Kernel security:

```
├── sysctl network hardening (rp_filter, accept_redirects, etc.)
├── Module blacklisting for unused/non-hotpluggable modules
├── Never disable: USB, network, audio, Bluetooth, hot-pluggable bus drivers
└── Kernel updates via standard Ubuntu LTS security updates
```

No telemetry whatsoever:

```
├── ubuntu-report: removed
├── apport: removed
├── whoopsie: removed
├── popularity-contest: removed
├── landscape: removed
├── motd-news: removed
└── snapd: removed (includes Canonical telemetry)
```

Screen locking:

```
├── Auto-lock on screen blank
├── Require password on wake
├── Lock on user switch
└── Cannot be disabled below 1 minute (security floor)
```

Contributor security checklist:

```
├── No secrets in code
├── No hardcoded credentials
├── Dependencies checked for known vulnerabilities
├── Config files validated before write
└── No network calls from system services except documented ones
```

### Privacy

See: `PRIVACY.md` (complete)

Core principle: "The laptop belongs to the user. The data belongs to the user. Spike exists to serve, not to observe."

Privacy is not a toggle — it is the baseline. There is no "privacy mode" because privacy is always on.

Data collection: ZERO. Absolute and non-negotiable.

```
├── No telemetry, no analytics, no crash reports, no usage stats
├── No "anonymous" statistics, no "opt-out" telemetry
├── BDFL-level decision recorded in DESIGN-DECISIONS.md
└── If a future contributor proposes adding telemetry: the answer is no
```

Removed Ubuntu components (stripped at ISO build):

```
├── ubuntu-report, apport, whoopsie, popularity-contest,
│   landscape-client, ubuntu-advantage-tools, motd-news, cloud-init, snapd
└── None can be accidentally enabled
```

Complete list of network connections Spike makes:

1. apt update → archive.ubuntu.com (package lists, no user data, every 6 hrs) 

2. Flatpak remote check → flathub.org (app metadata, no user data, every 6 hrs) 

3. NTP sync → pool.ntp.org (time request, no user data, on boot + periodic) 

4. Connectivity check → connectivity-check.ubuntu.com (HTTP GET, no body) 

5. Captive portal detection → redirect target (HTTP GET, no body) 

That is the COMPLETE list. Nothing else connects anywhere.

Application permissions (portal-based):

```
├── xdg-desktop-portal (Wayland-native permission system)
├── Categories: files, camera, microphone, screen, notifications, background
├── Permission states: Allowed, Ask, Denied, N/A
├── Dialog: [Allow] [Always Allow] [Deny] [Always Deny]
├── Stored per-application, revocable in Settings → Privacy
├── New apps default to: Ask
└── Pre-configured defaults for Firefox, VLC, LibreOffice, Spectacle
```

Camera/microphone indicators:

```
├── Panel icon appears when camera or mic is actively in use
├── Shows which app is using the device
├── Click to disable/mute immediately
├── INDICATORS ARE NON-NEGOTIABLE (cannot be hidden or disabled)
└── Kill switch available in Settings → Privacy → Camera & Microphone
```

Camera/microphone kill switch:

```
├── Disable camera: unloads uvcvideo module (apps get "not available")
├── Disable microphone: mutes all capture devices in PipeWire (apps get silence)
├── State persists across reboots
├── Panel shows disabled indicator
└── Separate from Airplane Mode (not radio-based)
```

Firefox privacy (Spike-managed):

```
├── Enhanced Tracking Protection: Strict (blocks trackers, fingerprinting, crypto miners)
├── HTTPS-Only: Enabled (upgrade all HTTP to HTTPS, warn on no-HTTPS sites)
├── Do Not Track: Enabled (supplemental, most sites ignore)
├── Third-party cookies: Blocked (via ETP Strict)
├── URL tracking parameter stripping: Enabled (utm_source, fbclid, gclid, etc.)
├── DNS-over-HTTPS: Disabled by default (user can enable in Firefox)
├── Cookie deletion on close: Disabled (convenience, user can enable)
└── Firefox data isolated by Flatpak sandbox
```

Location services:

```
├── Opt-in (disabled by default)
├── Accuracy: Approximate (city-level, IP-based — no GPS on most target laptops)
├── Used for: installer timezone detection, Night Light sunset/sunrise
├── geoclue2 daemon (does not start if disabled)
├── No street-level tracking possible
└── Apps cannot request location if disabled at portal level
```

Data retention:

```
├── System logs: 7 days (journald, configurable 1-30)
├── Spike connection log: 7 days (configurable 1-30)
├── Notification history: 3 days (configurable 1-31, max 500 entries)
├── Launcher recently used: cleared on every logout
├── Battery health history: unlimited (until user resets)
├── Clipboard: NOT stored (in-memory only, cleared on logout)
├── All data stays local, never transmitted off-device
└── User can clear all in Settings → Privacy → Data Retention
```

Multi-user isolation:

```
├── Home directories: 750 permissions (other users cannot access)
├── Notifications: per-session (not visible to other users)
├── Launcher: per-user (favorites, recently used)
├── Settings: user prefs in ~/.config/spike/, system-wide = admin only
├── Browser: per-user Firefox data (isolated by Flatpak)
├── Clipboard: cleared on user switch
└── No guest session available (deliberate — complexity + data leakage risk)
```

Cloud services:

```
├── NO cloud integration whatsoever
├── No cloud storage sync, no cloud backup, no cloud auth, no cloud settings sync
├── No Spike cloud account exists
├── No "sign in with..." prompts anywhere
├── Users can install cloud clients via Discover (standard Flatpak sandbox)
└── Spike does not recommend or endorse cloud providers
```

MAC randomization: off by default (per-network randomization optional for public Wi-Fi)

DNS privacy: DoT available (disabled by default), DoH available in Firefox

Network audit trail:

```
├── Active connections viewable in Settings → Diagnostics → Network Connections
├── Connection log: /var/log/spike/connections.log (7-day retention)
├── Data source: ss -tunp (parsed, displayed in GUI)
└── Users can verify no unexpected connections
```

### Configuration (spike-config)

See: `CONFIGURATION.md` (complete)

spike-config is the configuration engine for Spike. NOT a daemon — runs on-demand (invoked by Settings GUI or systemd triggers).

Architecture:

```
┌─────────────┐     DBus      ┌──────────────────┐     generates     ┌──────────┐
│ Settings GUI │ ──────────▶  │  spike-config    │ ────────────────▶ │ Config   │
│ (thin client)│              │  (state store +  │                   │ Files    │
└─────────────┘              │   template engine)│                   │ (/etc/)  │
                              └────────┬─────────┘                   └──────────┘
                                       │
                              ┌────────┴────────┐
                              │  Change Log      │
                              │  (changelog.json) │
                              └─────────────────┘
```

State store:

```
├── Location: /var/lib/spike/config/state.json
├── Format: JSON (versioned)
├── Single source of truth for all configuration
├── Contains: hardware detection, memory, boot, security, updates,
│   multimedia, power, network, privacy, installer, desktop settings
├── Permissions: 644 (root:root)
└── If lost/corrupted: regenerated from hardware detection + defaults.json
```

Template engine:

```
├── Templates: /usr/lib/spike/config/templates/*.tpl
├── Variables: {{variable}} substituted from state store
├── Validation: syntax check, no leftover {{}}, no empty critical fields
├── Atomic write: write to .tmp → fsync → rename → fsync(dir)
└── Guarantee: config file is always OLD or NEW version, never corrupted
```

Change log:

```
├── Location: /var/lib/spike/config/changelog.json
├── Append-only JSON array
├── Records: timestamp, module, setting, old_value, new_value, source,
│   files_regenerated, services_reloaded
├── Max 5000 entries (oldest pruned)
└── Enables rollback (spike-config --rollback <entry-id>)
```

Modules (config file generators):

```
├── memory     → sysctl.d/99-spike-memory.conf, zram-generator.conf,
│               udev zram rules, earlyoom config
├── boot       → /etc/default/grub, boot-count, Plymouth theme
├── security   → ufw rules, sudoers, module blacklist
├── network    → NetworkManager.conf, resolved.conf, /etc/hosts
├── multimedia → pipewire.conf, wireplumber, environment, Firefox prefs,
│               GPU driver modprobe
├── power      → logind.conf, battery health tmpfiles, cron weekly
├── privacy    → journald.conf, audit rules
├── desktop    → sddm.conf, spike-shell.conf, Qt stylesheet, KWin decoration,
│               dconf (GTK apps)
└── updates    → apt.conf.d/99-spike-updates, unattended-upgrades config,
                spike-update.timer
```

Idempotent: running `spike-config --generate-all` produces identical output every time given the same state store. No duplicates, no stale settings.

CLI interface (DEVELOPERS ONLY — not in user guide):

```
├── spike-config --generate-all
├── spike-config --generate <module>
├── spike-config --detect (hardware detection)
├── spike-config --state (dump state store)
├── spike-config --state-set <module> <key> <value>
├── spike-config --changelog
├── spike-config --rollback <entry-id>
└── spike-config --validate
```

State store recovery:

```
├── If state.json missing/corrupted: regenerate from defaults.json + hardware detect
├── defaults.json: /usr/lib/spike/config/defaults.json (per-variant defaults)
├── All config files regenerated based on new state
├── User notified: "Configuration reset to defaults. Please review settings."
└── No "unrecoverable config" state ever exists
```

DBus interface: `org.spike.Config`

```
├── GetState(), GetModuleState(module), SetSetting(module, key, value)
├── GenerateAll(), GenerateModule(module), DetectHardware()
├── GetChangelog(count), Rollback(entry_id), ValidateAll()
├── Signals: StateChanged, ConfigRegenerated
└── Settings GUI never writes config files directly — always via spike-config
```

### Variant Differences

See: `VARIANT-DIFFERENCES.md` (complete — authoritative source)

Single ISO. Two variants. 14 differences. Everything else identical.

The 14 differences:

```
 1. ZRAM cap: 4GB (Standard) vs uncapped (Plus)
 2. Animations: Off (Standard) vs On (Plus)
 3. Compositor effects: Minimal (Standard) vs Full (Plus)
 4. Plymouth theme: spike-minimal (Standard) vs spike-full (Plus)
 5. CPU governor: powersave (Standard) vs schedutil (Plus)
 6. Bluetooth codecs: SBC only (Standard) vs SBC+AAC+LDAC+aptX (Plus)
 7. Flatpak runtimes: KDE+GNOME (Standard) vs all common (Plus)
 8. Applet polling: Conservative (Standard) vs Standard (Plus)
 9. Qt rendering: Software preferred (Standard) vs OpenGL preferred (Plus)
10. Spike button hover: No glow (Standard) vs Glow (Plus)
11. Hybrid sleep: ON (Standard) vs OFF (Plus)
12. Idle RAM target: <400MB (Standard) vs <800MB (Plus)
13. Background services: Minimal set (Standard) vs Standard set (Plus)
14. Memory budget: Tighter ~280-370MB (Standard) vs Relaxed ~420-650MB (Plus)
```

Identical between variants: kernel, filesystem, partitioning, installer, desktop shell, networking, security, privacy, disaster recovery, configuration system, updates, community model, ISO, packages.

Variant selection:

```
├── Installer recommends based on RAM + CPU detection
├── User can override (warning if mismatched)
├── Post-install switching available (no reinstall)
├── Switch calls: spike-config --state-set system variant <plus|standard>
├── Then: spike-config --generate-all
├── Some changes immediate, some require re-login, some require reboot
└── See VARIANT-DIFFERENCES.md for full selection algorithm
```

## 10. Installer

10-step installer flow:

1. Language 

2. Timezone (IP-based geolocation, user confirms) 

3. Wi-Fi (connect to download firmware/updates if needed) 

4. Username 

5. Password (with reminder hint field) 

6. Hostname (suggests spike-laptop, user can change) 

7. Storage confirmation (data backup option, wipe warning) 

8. Variant selection (recommended based on hardware, user can override) 

9. Install (automated, progress bar, no user interaction needed) 

10. Reboot to desktop 

Data backup (Step 7):

```
├── Installer scans for user data (Windows/Linux/macOS personal files)
├── Offers backup to USB before wiping
├── Verifies copy with checksums (SHA256)
├── Preserves folder structure
└── If existing Spike installation detected:
    "Fresh install" or "Fresh install and restore my data"
```

Partitioning (Step 9):

```
├── Fully automatic, no manual option
├── ext4 everywhere
├── /boot/efi (512MB FAT32) or /boot (1GB ext4) + / (rest, ext4) + /swapfile
├── Mount flags adaptive per storage type
└── Minimum 128GB
```

Hardware detection (runs during Step 9):

```
├── detect/cpu.cpp → CPU model, cores, bogomips, classification
├── detect/storage.cpp → Storage type, size, rotational, eMMC/USB check
├── detect/gpu.cpp → GPU vendor, model, driver, VA-API driver
├── detect/network.cpp → Wi-Fi/Ethernet, driver module
├── detect/bluetooth.cpp → Bluetooth adapter presence
└── detect/modem.cpp → Mobile broadband modem presence
```

Installation tasks (automated during Step 9):

```
 1. Partition disk
 2. Format partitions
 3. Mount filesystems
 4. Copy base system (from ISO, not debootstrap — faster)
 5. Install kernel
 6. Install GRUB2 (with Spike theme)
 7. Create user account
 8. Lock root account
 9. Configure ZRAM/swap (based on CPU + storage + variant)
10. Install Flatpak + pre-seed runtimes (based on variant)
11. Install KDE standalone apps
12. Install Firefox (with Spike-tuned prefs)
13. Install Plymouth theme (based on variant)
14. Apply desktop theme (animations, compositor, based on variant)
15. Enable/disable services (based on hardware detection + variant)
16. Set CPU governor (based on variant)
17. Configure ufw firewall
18. Configure automatic updates
19. Strip telemetry components
20. Generate initial state store (spike-config --detect + --generate-all)
```

## 11. Authentication

Model: Standard user/sudo (Ubuntu default)

```
├── Root account: locked
├── One user password for: login, sudo, polkit, screen unlock
├── Password policy: min 6 chars, no complexity requirements, no expiration
├── Auto-login: off by default (optional in settings)
└── sudo lecture: first-use message explaining what sudo does
```

No SSH server installed (client only — openssh-client, not openssh-server)

No disk encryption (LUKS) — deliberate for beginner recovery simplicity

No dual boot support

No guest session

## 12. Licensing & Contribution

License:

```
├── Code: GPLv2+ (compatible with KWin, KDE Frameworks, Flatpak, Ubuntu packages)
├── Documentation: CC-BY-SA 4.0
└── Artwork (logo, splash, GRUB theme): CC-BY-SA 4.0
```

Contribution model (phased):

```
├── Pre-alpha: public read-only repo, issues only
├── Alpha: bug reports, hardware submissions, translations, case-by-case
│           patches with DCO (Developer Certificate of Origin)
├── Beta: full contributions, code review, branch protection
└── DCO chosen over CLA (lighter barrier to entry)
```

Governance:

```
├── BDFL (Benevolent Dictator For Life) model initially
├── Creator holds final decision authority
├── Documented succession plan (in GOVERNANCE.md)
└── Canonical trademark compliance documented (using Ubuntu base)
```

Timeline:

```
├── 8 months to alpha release
├── 18 months to beta release
└── Documentation-first: discuss → converge → write docs → then code
```

## 13. Spike Shell (Desktop) — Summary

> **Note:** Full specification is in `DESKTOP.md`. This section is a summary of decided architecture.

Spike Shell is a custom lightweight desktop shell built on:

```
├── KWin (Wayland compositor — standalone, not plasma-session)
├── Qt/KDE Frameworks (for standalone apps and KCM modules)
├── SDDM (display manager, Spike-themed)
└── Custom shell binary (spike-shell) providing:
```

Panel:

```
├── Position: bottom (default) or top
├── Height: 32px default (adjustable 24-48px)
├── Auto-hide: optional (instant slide on Standard, animated on Plus)
├── 3 zones: left (launcher button + pinned apps), center (clock),
│           right (system tray applets)
├── 14 applets (always present) + 4 conditional (BT, modem, etc.)
└── Applets: clock, network, battery, brightness, volume, bluetooth
            (conditional), updates, notifications, removable devices,
            keyboard layout, input method, night light, power profiles,
            camera/mic indicator, spike-menu (launcher button)
```

Launcher:

```
├── Kickoff-style (3-pane: favorites, categories, search results)
├── Categories: System, Internet, Office, Media, Games, Settings, Other
├── Search: fuzzy match, launches on Enter
├── Favorites: user-pinned apps (per-user)
├── Recently used: cleared on each logout
├── New app badge: apps installed since last launch
└── Super key opens launcher
```

Notification daemon:

```
├── INVARIANT: disk before display (write to disk, THEN show on screen)
├── Never loses notifications due to crash or power loss
├── History: ~/.local/share/spike/notifications/history.json
├── Retention: 3 days (configurable), max 500 (configurable)
├── Per-session (not visible to other users)
└── Do Not Disturb mode (suppresses non-critical notifications)
```

Session manager:

```
├── Login: SDDM → spike-session → spike-shell + KWin + services
├── Logout: confirm dialog → stop services → return to SDDM
├── Autostart: filtered (only approved autostart entries)
├── Environment: QT_QPA_PLATFORM=wayland, LIBVA_DRIVER_NAME, MOZ_DISABLE_AV1
└── First-run: welcome wizard (timezone confirm, Wi-Fi check, tour)
```

Settings panel:

```
├── Hybrid: custom Spike pages + selected standalone KDE KCMs
├── Custom: Appearance, Notifications, Network (NetworkManager), Language,
│           Keyboard/Mouse, Users, Date & Time, Accessibility, Advanced…
├── KCM (standalone pkgs only): Display, Sound, Power, Bluetooth, Printer
├── Forbidden: plasma-desktop, plasma-workspace, plasma-nm
├── spike-config settings via org.spike.Config D-Bus
├── Live network connections via NetworkManager D-Bus (not spike-config keyfiles)
└── Never writes config files directly from the GUI
```

Theme engine:

```
├── Accent color: purple (#6d4aff)
├── Secondary: cyan/teal
├── Font: Noto Sans 10pt
├── Icon theme: Breeze
├── Cursor: Breeze, 24px
├── Wallpapers: /usr/share/spike/wallpapers/
└── KWin decorations: minimal (Standard) or with effects (Plus)
```

## 14. Cross-Reference Index

| Topic | Document(s) |
| :-: | :-: |
| Adaptive memory (ZRAM/swap) | `MEMORY.md`, `CONFIGURATION.md` |
| Animations (variant difference) | `VARIANT-DIFFERENCES.md` |
| AppArmor | `SECURITY.md` |
| Application permissions (portals) | `PRIVACY.md` |
| Atomic config writes | `CONFIGURATION.md` |
| Audio (PipeWire/WirePlumber) | `MULTIMEDIA.md` |
| Authentication (passwords, sudo) | `SECURITY.md`, `INSTALLER.md` |
| Automatic updates | `SECURITY.md`, `CONFIGURATION.md` |
| Battery health | `POWER-MANAGEMENT.md` |
| Bluetooth audio codecs | `MULTIMEDIA.md`, `VARIANT-DIFFERENCES.md` |
| Bluetooth activation (conditional) | `NETWORKING.md` |
| Boot failure counter | `DISASTER-RECOVERY.md`, `BOOT-PROCESS.md` |
| Boot parameters | `BOOT-PROCESS.md`, `KERNEL.md` |
| Branding (logo, splash, GRUB) | `BRANDING.md` |
| Broadcom Wi-Fi (MOK enrollment) | `NETWORKING.md`, `SECURITY.md` |
| Camera/microphone indicators | `PRIVACY.md` |
| Camera/microphone kill switch | `PRIVACY.md` |
| Change log (config) | `CONFIGURATION.md` |
| CPU classification | `KERNEL.md`, `VARIANT-DIFFERENCES.md` |
| CPU governor | `POWER-MANAGEMENT.md`, `VARIANT-DIFFERENCES.md` |
| Captive portal detection | `NETWORKING.md` |
| Clipboard (no persistence) | `PRIVACY.md` |
| Configuration system (spike-config) | `CONFIGURATION.md` |
| Data backup (installer) | `INSTALLER.md`, `DISASTER-RECOVERY.md` |
| Data migration (Move My Files) | `SPIKE-MIGRATION.md`, `SPIKE-RECOVERY-TOOL-GENERAL.md` |
| Data retention policies | `PRIVACY.md` |
| Data sources | `CONFIGURATION.md` |
| Desktop shell | `DESKTOP.md` |
| Disk encryption (none — rationale) | `DESIGN-DECISIONS.md` |
| DNS privacy (DoT/DoH) | `PRIVACY.md`, `NETWORKING.md` |
| eMMC (not supported — rationale) | `DESIGN-DECISIONS.md` |
| Earlyoom | `MEMORY.md`, `CONFIGURATION.md` |
| ext4 everywhere (rationale) | `DESIGN-DECISIONS.md` |
| Filesystem (partitioning, mount flags) | `INSTALLER.md` |
| Firefox tuning (VA-API, AV1) | `MULTIMEDIA.md` |
| Firewall (ufw) | `NETWORKING.md`, `SECURITY.md` |
| Flatpak runtimes (pre-seeded) | `VARIANT-DIFFERENCES.md`, `INSTALLER.md` |
| GRUB2 bootloader | `BOOT-PROCESS.md` |
| Hardware detection | `CONFIGURATION.md`, `INSTALLER.md` |
| Hybrid sleep | `POWER-MANAGEMENT.md`, `VARIANT-DIFFERENCES.md` |
| Idle RAM targets | `VARIANT-DIFFERENCES.md` |
| Installer flow (10 steps) | `INSTALLER.md` |
| Kernel module blacklisting | `KERNEL.md` |
| Location services | `PRIVACY.md` |
| MAC randomization | `PRIVACY.md` |
| Memory budget | `VARIANT-DIFFERENCES.md` |
| Multi-user isolation | `PRIVACY.md` |
| Network connections (complete list) | `PRIVACY.md` |
| Networking (general) | `NETWORKING.md` |
| Night Light | `POWER-MANAGEMENT.md`, `PRIVACY.md` |
| Notifications (disk-before-display) | `DESKTOP.md` |
| Panel and applets | `DESKTOP.md` |
| Partitioning | `INSTALLER.md` |
| Plymouth (boot splash) | `BOOT-PROCESS.md`, `VARIANT-DIFFERENCES.md` |
| Power profiles | `POWER-MANAGEMENT.md` |
| Privacy (general) | `PRIVACY.md` |
| Recovery (4-layer model) | `DISASTER-RECOVERY.md` |
| Root account (locked) | `SECURITY.md` |
| Screen capture | `MULTIMEDIA.md` |
| Screen locking | `SECURITY.md` |
| Secure Boot | `SECURITY.md` |
| Spike Rescue tool | `DISASTER-RECOVERY.md`, `SPIKE-RECOVERY-TOOL-GENERAL.md` |
| Spike Migration tool | `SPIKE-MIGRATION.md`, `SPIKE-RECOVERY-TOOL-GENERAL.md` |
| Spike Shell | `DESKTOP.md` |
| State store | `CONFIGURATION.md` |
| Swappiness | `MEMORY.md`, `CONFIGURATION.md` |
| sysctl tunables | `KERNEL.md`, `SECURITY.md` |
| Telemetry (zero — absolute) | `PRIVACY.md`, `SECURITY.md` |
| Ubuntu components removed | `PRIVACY.md`, `SECURITY.md` |
| Update timer | `SECURITY.md`, `CONFIGURATION.md` |
| VA-API (video acceleration) | `MULTIMEDIA.md` |
| Variant differences (all 14) | `VARIANT-DIFFERENCES.md` |
| Variant selection algorithm | `VARIANT-DIFFERENCES.md` |
| Variant switching (post-install) | `VARIANT-DIFFERENCES.md` |
| VPN | `NETWORKING.md`, `PRIVACY.md` |
| Wayland/KWin | `BOOT-PROCESS.md`, `DESKTOP.md` |
| WebRTC | `MULTIMEDIA.md` |
| Wi-Fi firmware/drivers | `NETWORKING.md` |

## 15. Key Decisions Log

| Decision | Choice | Rationale |
| :-: | :-: | :-: |
| Base distribution | Ubuntu Server LTS | Fresher kernel/Mesa, 10yr ESM, clean base |
| Architecture | x86_64 only | 32-bit requires library forking |
| Display server | Wayland (KWin) | Modern, XWayland fallback validated |
| Desktop shell | Custom (not Plasma) | Lightweight, cohesive UX |
| Package manager (user apps) | Flatpak | Sandboxed, easy updates via Discover |
| Filesystem | ext4 everywhere | Simplicity, consistency, reliability |
| Encryption | None (deliberate) | Beginner recovery simplicity |
| Bootloader | GRUB2 | Reliable, well-understood, Limine later |
| Swap strategy | ZRAM + swap file | Compressed RAM first, disk second |
| Audio server | PipeWire | Modern, low overhead, PulseAudio compat |
| Video accel | VA-API | Hardware decode for H.264/VP9 on N4020 |
| AV1 in Firefox | Disabled | N4020 lacks AV1 decode, force VP9 |
| eMMC | Not supported | Wear-out risk (user experience) |
| No dual boot | Yes | Simplify for beginners |
| No guest session | Yes | Complexity + data leakage risk |
| No telemetry | Absolute | BDFL-level decision, non-negotiable |
| No cloud integration | Yes | Privacy by default |
| License (code) | GPLv2+ | Compatible with KDE, Flatpak, Ubuntu |
| License (docs/artwork) | CC-BY-SA 4.0 | Free documentation |
| DCO over CLA | DCO | Lighter barrier to contributions |
| Governance | BDFL | Clear decision authority initially |
| Documentation-first | Yes | Discuss → converge → docs → code |
| Golden Rules expanded | 10 rules (from 3) | Rules 4-10 added: no terminal, GUI completeness, privacy default, recovery guarantee, user sovereignty, outlast creator, lightweight not stripped |


## 16. Development Phases

**Phase 1 — Architecture Design** ✅ COMPLETE

```
├── All major architectural decisions resolved
├── All open questions closed
└── Documentation structure designed (70 files)
```

**Phase 2 — Specification Documents** ✅ NEARLY COMPLETE

```
├── ✅ Core top-level product/system specs drafted and formatted
├── ✅ INDEX.md, AGENTS.md, PHILOSOPHY.md current for phase tracking
├── ✅ LICENSE + LICENSE-CC-BY-SA-4.0 at repo root (copies in licences/)
├── ✅ agent-ops: README, RULES, CONSTRAINTS, GLOSSARY, CONVENTIONS, PROTOCOLS
├── ✅ agent-ops dynamic files initialized: STATE, DECISIONS, SESSION_LOG
├── 🔲 TRANSLATIONS.md (i18n framework)
├── 🔲 USER-GUIDE.md (top-level overview; sub-docs as desktop ships)
├── 🔲 user-guide/ stubs; remaining 16 of 19 `dev-guide/` stubs (03/04/07 drafted)
└── 🔲 Remaining agent-ops static helpers (PATTERNS, GOTCHAS, etc.)
```

**Phase 3 — Prototyping** 🔲 NOT STARTED

```
├── Build Spike base ISO (live-build; one hybrid ISO — see agent-ops/DECISIONS.md, dev-guide/04-building-spike.md)
├── Implement spike-config
├── Implement Spike Shell
├── Implement installer
├── Implement Spike Rescue
└── Test on Celeron N4020, AMD A4, ThinkPad P50
```

> **Note:** Project release label remains **pre-alpha** until the **installer works end-to-end** (then **Alpha**). Prototyping (ISO, shell, spike-config, installer) continues under pre-alpha until that gate. See `docs/agent-ops/DECISIONS.md`.

**Phase 4 — Alpha Release (8 months from start)** 🔲 NOT STARTED

```
├── Functional system on target hardware
├── Public read-only repo
├── Bug reports and hardware submissions accepted
└── PERFORMANCE-BASELINES.md populated with real numbers
```

**Phase 5 — Beta Release (18 months from start)** 🔲 NOT STARTED

```
├── Feature-complete
├── Full contributions accepted (with code review)
├── Branch protection enforced
└── Translation contributions accepted
```

Validation priorities (during alpha):

```
├── VA-API performance on N4020
├── PipeWire overhead on 4GB RAM
├── Discover outside Plasma (KDE standalone context)
├── KWin Wayland session without plasma-session
├── ZRAM compression ratio and CPU impact on Celeron
├── Boot time on HDD vs SSD
├── Idle memory usage on Standard variant
└── Firefox with AV1 disabled (VP9 fallback performance)
```


🐕 BigRangaTech


