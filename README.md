# Spike Linux

## Purpose

This is the Spike Linux project README. It is the front door to the repository. It tells you what Spike is, who it's for, where things are, and how to get involved.

Everything else lives in the documentation. This file points you there.

## What Is Spike?

Spike is a Linux distribution for old laptops.

Specifically: laptops with Intel Celeron N4020-class processors
(dual-core, 4GB RAM, Intel UHD 600) that can no longer run
Windows 10 and cannot upgrade to Windows 11.

Spike gives these machines a second life.

```
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│  What Spike IS:                                              │
│  ├── A beginner-friendly Linux desktop                       │
│  ├── Built for non-technical users                           │
│  ├── Designed for old, slow hardware                         │
│  ├── Free and open source (GPLv2+)                          │
│  └── Made by BigRangaTech                                    │
│                                                              │
│  What Spike is NOT:                                          │
│  ├── A power-user distribution                               │
│  ├── A Windows replacement for everyone                      │
│  ├── A gaming platform                                       │
│  ├── A server operating system                               │
│  └── An enthusiast tinker-toy                                │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## Key Features

```
├── No terminal required for any user task (Golden Rule)
├── No config file editing for any user setting (Golden Rule)
├── Zero telemetry — nothing is collected, ever
├── Custom Qt6/Wayland desktop (Spike Shell)
├── Flatpak-only application model (one app store, one source)
├── ZRAM memory management (effective RAM exceeds physical RAM)
├── Two variants: Standard (minimal effects) and Plus (full effects)
├── Automatic hardware detection and configuration at install time
├── Boot failure recovery (3 failed boots → GRUB menu → recovery)
├── Accessible by default (screen reader, magnifier, on-screen keyboard)
├── Based on Ubuntu Server LTS (stable, long-term support base)
└── 11 languages supported
```

## Hardware Requirements

```
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│  Minimum:                  Recommended:                      │
│                                                              │
│  64-bit CPU (x86-64)       Intel Celeron N4020 or similar   │
│  2+ CPU cores              4GB RAM                           │
│  1.0 GHz base clock        240GB+ SATA SSD                   │
│  4GB RAM                   Intel UHD 600 GPU                 │
│  128GB storage (SSD)       USB 3.0 port                      │
│  1366×768 display          1440×900+ display                 │
│  USB 3.0 port              UEFI boot                         │
│                                                              │
│  Not supported:                                              │
│  ├── eMMC storage (too slow, limited lifespan)               │
│  ├── 32-bit processors                                      │
│  ├── Single-core CPUs                                       │
│  ├── ARM devices (Raspberry Pi, Chromebooks)                 │
│  └── Less than 4GB RAM                                       │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

See [docs/HARDWARE.md](docs/HARDWARE.md) for the complete hardware specification,
detection logic, and tier classification.

## Project Status

**CURRENT PHASE: Pre-alpha** (implementation underway)

Spike remains **pre-alpha** until the **installer works end-to-end**. Live ISO / shell work continues under that label. Opening **Alpha** is a BDFL gate when install → reboot to installed desktop succeeds.

```
├── Live ISO and Spike Shell are in active pre-alpha development
├── No public “download Spike Alpha” yet
├── Specs in docs/ guide the build
└── Phase flip: pre-alpha → Alpha when installer E2E works (DECISIONS.md)
```

Timeline:

```
├── Pre-alpha (current): docs + early ISO/shell; ends at installer E2E
├── Alpha: installer works; expand desktop, hardware testing
├── Beta: feature-complete, hardware testing, bug fixing
├── Release Candidate: final testing, polish, documentation review
└── 1.0: first stable release (~18 months from alpha start)
```

See [docs/ROADMAP.md](docs/ROADMAP.md) for the complete development timeline.
See [docs/CHANGELOG.md](docs/CHANGELOG.md) for what has been done so far.

## Repository Structure

```
spike/
├── README.md                      → You are here
├── CODE_OF_CONDUCT.md             → Pointer → docs/CODE_OF_CONDUCT.md
├── CONTRIBUTING.md                → Stub — see docs/CONTRIBUTING.md
├── licences/                      → Organized copies of license texts
│   ├── GPL-2.0-or-later.md
│   └── CC BY-SA 4.0.md
├── LICENSE                        → GPLv2+ (code)
├── LICENSE-CC-BY-SA-4.0           → CC-BY-SA 4.0 (docs/artwork)
│
├── docs/                          → Project specifications
│   ├── README.md                  → Docs introduction
│   ├── INDEX.md                   → Document index
│   ├── AGENTS.md                  → Master project reference
│   ├── CONTRIBUTING.md            → How to contribute
│   ├── CODE_OF_CONDUCT.md         → Community standards (canonical)
│   ├── ARCHITECTURE.md
│   ├── DESIGN-DECISIONS.md
│   ├── PHILOSOPHY.md
│   ├── KERNEL.md
│   ├── MEMORY.md
│   ├── BOOT-PROCESS.md
│   ├── DESKTOP.md
│   ├── INSTALLER.md
│   ├── CONFIGURATION.md
│   ├── VARIANT-DIFFERENCES.md
│   ├── HARDWARE.md
│   ├── POWER-MANAGEMENT.md
│   ├── MULTIMEDIA.md
│   ├── NETWORKING.md
│   ├── SECURITY.md
│   ├── PRIVACY.md
│   ├── ACCESSIBILITY.md
│   ├── BRANDING.md
│   ├── DISASTER-RECOVERY.md
│   ├── PERFORMANCE-BASELINES.md
│   ├── GOVERNANCE.md
│   ├── END-OF-LIFE-POLICY.md
│   ├── ROADMAP.md
│   ├── CHANGELOG.md
│   ├── TROUBLESHOOTING.md
│   ├── FAQ.md
│   ├── MIGRATION-GUIDE.md
│   ├── SUPPORT.md
│   ├── agent-ops/                 → Agent/contributor operational docs
│   ├── user-guide/                → End-user docs (stubs — filled as desktop ships)
│   └── dev-guide/                 → Developer docs (stubs — filled as desktop ships)
│
├── src/                           → Source code (future)
├── build/                         → Build system (future)
├── ci/                            → CI configuration (future)
└── scripts/                       → Helper scripts (future)
```

## Getting Started

### For Users

Spike is not yet available for download.

When it is ready:

```
├── Download: spike.bigrangatech.com/download
├── Minimum USB drive: 8GB, USB 3.0
├── Installation guide: docs/MIGRATION-GUIDE.md
├── Hardware check: docs/HARDWARE.md
└── FAQ: docs/FAQ.md
```

In the meantime:

```
├── Read docs/PHILOSOPHY.md to understand what Spike is about
├── Read docs/FAQ.md for common questions
├── Visit the forum: forum.bigrangatech.com
└── Watch the website: spike.bigrangatech.com
```

### For Contributors

Spike welcomes contributors of all skill levels.

Documentation contributors (no coding required):

```
├── Read docs/AGENTS.md first (master project reference)
├── Read docs/CONTRIBUTING.md for the contribution process
├── Documentation is in Markdown — editable in any text editor
├── Submit changes via GitLab merge request
├── DCO (Developer Certificate of Origin) required — no CLA
├── House style: see docs/AGENTS.md for document conventions
└── Start with: reviewing existing docs for gaps, errors, or
    unclear wording
```

Code contributors:

```
├── Read docs/ARCHITECTURE.md for system overview
├── Read docs/DESIGN-DECISIONS.md for rationale
├── Read docs/dev-guide/ for technical specifications (to be written)
├── All code is GPLv2+
├── DCO required (git commit -s)
├── No CLA — you retain your copyright
├── Code follows the specs in the documentation
│   └── If the spec is wrong, fix the spec first, then the code
└── Primary repository: git.bigrangatech.com
    GitHub mirror: github.com (read-only)
```

Community contributors:

```
├── Help on the forum: forum.bigrangatech.com
├── Answer questions (see docs/SUPPORT.md)
├── Report bugs (see docs/SUPPORT.md → Bug Reporting)
├── Test on your hardware (when alpha is available)
├── Spread the word
└── See docs/SUPPORT.md → "Supporting Spike" for more ideas
```

## Infrastructure

```
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│  RESOURCE              LOCATION                              │
│  ───────────────────── ─────────────────────────────────────  │
│  Primary repository    git.bigrangatech.com (GitLab CE)      │
│  GitHub mirror         github.com (read-only)                │
│  Website               spike.bigrangatech.com                │
│  Forum                 forum.bigrangatech.com                │
│  Documentation         spike.bigrangatech.com/docs           │
│  Downloads             spike.bigrangatech.com/download       │
│                                                              │
│  License:              GPLv2+ (code)                          │
│                        CC-BY-SA 4.0 (documentation & artwork) │
│                                                              │
│  Governance:           BDFL (BigRangaTech)                   │
│  DCO:                  Yes (no CLA)                           │
│  Contributing:         docs/CONTRIBUTING.md                   │
│  Code of Conduct:      docs/CODE_OF_CONDUCT.md (root points here) │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## Golden Rules

Two rules are structural constraints. They cannot be overridden.
They apply to every design decision, every feature, and every
contribution.

**Rule 1: No config file editing for any user setting.**

```
└── If a user needs to change a setting, there must be a GUI
    for it in Settings. No "edit this file" instructions in
    user-facing documentation.
```

**Rule 2: No terminal required for any user task.**

```
└── If a user needs to do something, there must be a way to
    do it through the GUI. The terminal exists for developers
    and power users, but no user task should require it.
```

See [docs/PHILOSOPHY.md](docs/PHILOSOPHY.md) for the full value system.
See [docs/GOVERNANCE.md](docs/GOVERNANCE.md) for structural constraint override process.

## License

Spike Linux is licensed under the GNU General Public License
version 2 or later (GPLv2+).

```
├── All code: GPLv2+
├── All documentation: CC-BY-SA 4.0
├── Branding assets (logos, icons, themes): CC-BY-SA 4.0
├── Third-party components: retain their original licenses
│   ├── Ubuntu base: various (GPL, MIT, BSD, etc.)
│   ├── KDE components: LGPL/GPL
│   ├── Flatpak: LGPL
│   ├── Noto Sans font: SIL Open Font License 1.1
│   └── Breeze icons: LGPL-3.0
└── Full license texts: LICENSE (code), LICENSE-CC-BY-SA-4.0 (docs/artwork);
    organized copies also in licences/
```

The "Spike" name and dog mascot are trademarks of BigRangaTech.
See [docs/GOVERNANCE.md](docs/GOVERNANCE.md) for trademark policy.

## Acknowledgments

Spike stands on the shoulders of giants:

```
├── Ubuntu / Canonical — base system and package repository
├── KDE / Plasma community — KWin, Dolphin, Konsole, Discover, Breeze
├── GNOME community — Orca (accessibility), GLib, ATK
├── freedesktop.org — standards (desktop entries, portals, etc.)
├── Kernel.org / Linus Torvalds — the Linux kernel
├── PipeWire project — audio/video server
├── Wayland project — display protocol
├── Mozilla — Firefox
├── Document Foundation — LibreOffice
├── Flathub — Flatpak application repository
├── Google — Noto fonts
├── Every contributor to every open-source package Spike uses
└── Every person who ever installed Linux on an old laptop
    and refused to throw it away
```

Spike doesn't reinvent the wheel. It puts existing wheels together
in a configuration designed for people who need them most.

🐕 BigRangaTech
