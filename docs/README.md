# Spike Linux

A Linux distribution for old laptops.

Spike gives a second life to machines that can no longer run Windows 10 and can't upgrade to Windows 11 — specifically laptops with Intel Celeron N4020-class processors, 4GB of RAM, and Intel UHD 600 graphics.

Built for non-technical users. No terminal required. No config files. Zero telemetry.

## What Spike Is

```
What Spike IS:
- A beginner-friendly Linux desktop
- Built for non-technical users
- Designed for old, slow hardware
- Free and open source (GPLv2+)
- Made by BigRangaTech
What Spike is NOT:
- A power-user distribution
- A Windows replacement for everyone
- A gaming platform
- A server operating system
- An enthusiast tinker-toy
```

## Key Features

- **No terminal required** for any user task (Golden Rule) 

- **No config file editing** for any user setting (Golden Rule) 

- **Zero telemetry** — nothing is collected, ever 

- **Custom Qt6/Wayland desktop** (Spike Shell) 

- **Flatpak-only application model** (one app store, one source) 

- **ZRAM memory management** (effective RAM exceeds physical RAM) 

- **Two variants**: Standard (minimal effects) and Plus (full effects) 

- **Automatic hardware detection** and configuration at install time 

- **Boot failure recovery** (3 failed boots triggers GRUB menu with recovery) 

- **Accessible by default** (screen reader, magnifier, on-screen keyboard) 

- **Based on Ubuntu Server LTS** (stable, long-term support base) 

- **11 languages supported** 

## Hardware Requirements

| **Requirement** | **Minimum** | **Recommended** |
| :-: | :-: | :-: |
| CPU | 64-bit (x86-64), 2+ cores, 1.0 GHz | Intel Celeron N4020 or similar |
| RAM | 4 GB | 4 GB (Standard) / 8 GB (Plus) |
| Storage | 128 GB SSD | 240 GB+ SATA SSD |
| Display | 1366×768 | 1440×900+ |
| Boot | UEFI or BIOS | UEFI |
| USB | USB 3.0 port | USB 3.0+ |

**Not supported:** eMMC storage, 32-bit processors, single-core CPUs, ARM devices, less than 4GB RAM.

See [HARDWARE.md](HARDWARE.md) for the complete hardware specification, detection logic, and tier classification.

## Project Status

```
CURRENT PHASE: Pre-alpha (documentation)
Spike follows a "docs-first" methodology. The entire system is
specified in documentation before any code is written.
- You cannot download or install Spike yet
- There is no ISO image yet
- There is no working desktop yet
- What exists: a complete specification of what Spike will be
- When code begins, it will follow these specifications
Timeline:
- Pre-alpha (current): documentation, architecture, design
- Alpha: first bootable ISO, basic desktop, installer
- Beta: feature-complete, hardware testing, bug fixing
- Release Candidate: final testing, polish, documentation review
- 1.0: first stable release (~18 months from alpha start)
```

See [ROADMAP.md](ROADMAP.md) for the complete development timeline.  
See [CHANGELOG.md](CHANGELOG.md) for what has been done so far.

## Repository Structure

```
spike/
├── README.md                      → Project front door (repo root)
├── CODE_OF_CONDUCT.md             → Pointer → docs/CODE_OF_CONDUCT.md
├── CONTRIBUTING.md                → Stub — see docs/CONTRIBUTING.md
├── licences/                      → Organized copies of license texts
├── LICENSE                        → GPLv2+ (code)
├── LICENSE-CC-BY-SA-4.0           → CC-BY-SA 4.0 (docs/artwork)
│
└── docs/                          → You are here
    ├── README.md                  → Docs introduction
    ├── INDEX.md                   → Document index
    ├── AGENTS.md                  → Master project reference
    ├── CONTRIBUTING.md            → How to contribute
    ├── CODE_OF_CONDUCT.md         → Community standards (canonical)
    ├── ARCHITECTURE.md
    ├── DESIGN-DECISIONS.md
    ├── PHILOSOPHY.md
    ├── KERNEL.md
    ├── MEMORY.md
    ├── BOOT-PROCESS.md
    ├── DESKTOP.md
    ├── INSTALLER.md
    ├── CONFIGURATION.md
    ├── VARIANT-DIFFERENCES.md
    ├── HARDWARE.md
    ├── POWER-MANAGEMENT.md
    ├── MULTIMEDIA.md
    ├── NETWORKING.md
    ├── SECURITY.md
    ├── PRIVACY.md
    ├── ACCESSIBILITY.md
    ├── BRANDING.md
    ├── DISASTER-RECOVERY.md
    ├── SPIKE-RECOVERY-TOOL-GENERAL.md
    ├── SPIKE-MIGRATION.md
    ├── PERFORMANCE-BASELINES.md
    ├── GOVERNANCE.md
    ├── END-OF-LIFE-POLICY.md
    ├── ROADMAP.md
    ├── CHANGELOG.md
    ├── TROUBLESHOOTING.md
    ├── FAQ.md
    ├── MIGRATION-GUIDE.md
    ├── SUPPORT.md
    ├── agent-ops/                 → Agent/contributor operational docs
    ├── user-guide/                → End-user docs (stubs — filled as desktop ships)
    └── dev-guide/                 → Developer docs (living — see `dev-guide/INDEX.md`)
```

## Getting Started

### For Users

```
Spike is not yet available for download.
When it is ready:
- Download: spike.bigrangatech.com/download
- Minimum USB drive: 8GB, USB 3.0
- Installation guide: MIGRATION-GUIDE.md
- Hardware check: HARDWARE.md
- FAQ: FAQ.md
In the meantime:
- Read PHILOSOPHY.md to understand what Spike is about
- Read FAQ.md for common questions
- Visit the forum: forum.bigrangatech.com
- Watch the website: spike.bigrangatech.com
```

### For Contributors

```
Spike welcomes contributors of all skill levels.
Documentation contributors (no coding required):
- Read AGENTS.md first (master project reference)
- Read CONTRIBUTING.md for the contribution process
- Documentation is in Markdown — editable in any text editor
- Submit changes via GitLab merge request
- DCO (Developer Certificate of Origin) required — no CLA
- Start with: reviewing existing docs for gaps, errors, or unclear wording
Code contributors:
- Read ARCHITECTURE.md for system overview
- Read DESIGN-DECISIONS.md for rationale
- Read `dev-guide/` for implementation maps (`01-getting-started.md`, `02-repo-structure.md`, …)
- All code is GPLv2+
- DCO required (git commit -s)
- No CLA — you retain your copyright
- Code follows the specs in the documentation
  → If the spec is wrong, fix the spec first, then the code
- Primary repository: git.bigrangatech.com
  GitHub mirror: github.com (read-only)
Community contributors:
- Help on the forum: forum.bigrangatech.com
- Answer questions (see SUPPORT.md)
- Report bugs (see SUPPORT.md → Bug Reporting)
- Test on your hardware (when alpha is available)
- Spread the word
- See SUPPORT.md → "Supporting Spike" for more ideas
```

## Infrastructure

| **Resource** | **Location** |
| :-: | :-: |
| Primary repository | git.bigrangatech.com (GitLab CE) |
| GitHub mirror | github.com (read-only) |
| Website | spike.bigrangatech.com |
| Forum | forum.bigrangatech.com |
| Documentation | spike.bigrangatech.com/docs |
| Downloads | spike.bigrangatech.com/download |
| License | GPLv2+ (code), CC-BY-SA 4.0 (documentation and artwork) |
| Governance | BDFL (BigRangaTech) |
| DCO | Yes (no CLA) |
| Code of Conduct | [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) (canonical; root file points here) |
| Contributing | [CONTRIBUTING.md](CONTRIBUTING.md) |

## Golden Rules

```
Two rules are structural constraints. They cannot be overridden.
They apply to every design decision, every feature, and every
contribution.
Rule 1: No config file editing for any user setting.
  → If a user needs to change a setting, there must be a GUI
    for it in Settings. No "edit this file" instructions in
    user-facing documentation.
Rule 2: No terminal required for any user task.
  → If a user needs to do something, there must be a way to
    do it through the GUI. The terminal exists for developers
    and power users, but no user task should require it.
```

See [PHILOSOPHY.md](PHILOSOPHY.md) for the full value system.  
See [GOVERNANCE.md](GOVERNANCE.md) for structural constraint override process.

## License

```
Spike Linux is licensed under the GNU General Public License
version 2 or later (GPLv2+).
- All code: GPLv2+
- All documentation: CC-BY-SA 4.0
- Branding assets (logos, icons, themes): CC BY-SA 4.0
- Third-party components: retain their original licenses
  - Ubuntu base: various (GPL, MIT, BSD, etc.)
  - KDE components: LGPL/GPL
  - Flatpak: LGPL
  - Noto Sans font: SIL Open Font License 1.1
  - Breeze icons: LGPL-3.0
- Full license texts: `LICENSE` (GPLv2+), `LICENSE-CC-BY-SA-4.0` (docs/artwork); copies in `licences/`
The "Spike" name and dog mascot are trademarks of BigRangaTech.
See GOVERNANCE.md for trademark policy.
```

## Acknowledgments

```
Spike stands on the shoulders of giants:
- Ubuntu / Canonical — base system and package repository
- KDE / Plasma community — KWin, Dolphin, Konsole, Discover, Breeze
- GNOME community — Orca (accessibility), GLib, ATK
- freedesktop.org — standards (desktop entries, portals, etc.)
- Kernel.org / Linus Torvalds — the Linux kernel
- PipeWire project — audio/video server
- Wayland project — display protocol
- Mozilla — Firefox
- Document Foundation — LibreOffice
- Flathub — Flatpak application repository
- Google — Noto fonts
- Every contributor to every open-source package Spike uses
- Every person who ever installed Linux on an old laptop
  and refused to throw it away
Spike doesn't reinvent the wheel. It puts existing wheels together
in a configuration designed for people who need them most.
```

🐕 BigRangaTech


