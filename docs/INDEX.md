# Spike Linux Document Index

## Purpose

This document is the master index for all Spike Linux documentation. Every document in the project is listed here with its location, status, a one-line description, and cross-references. If a document exists in the repository, it appears in this index. If a document appears in this index, it should exist in the repository.

This index is the map. The documents are the territory. When the map and the territory disagree, the territory wins — fix the index to match. Keeping this index accurate is a maintenance task, not a one-time effort. Every time a document is added, renamed, or completed, this index must be updated in the same merge request.

## How To Use This Index

Finding what you need:

```
├── Know the document name? → Jump to the relevant section below
├── Know the topic? → Check the "Topic Index" at the end
├── Know who you are?
│   ├── New user exploring Spike → README.md → PHILOSOPHY.md → FAQ.md
│   ├── Switching from Windows/macOS → MIGRATION-GUIDE.md
│   ├── Having a problem → TROUBLESHOOTING.md → FAQ.md → SUPPORT.md
│   ├── Want to contribute → CONTRIBUTING.md → AGENTS.md → CODE_OF_CONDUCT.md
│   ├── Developer building Spike → ARCHITECTURE.md → dev-guide/
│   └── Want to understand the project → PHILOSOPHY.md → DESIGN-DECISIONS.md
└── Looking for a specific feature? → Check "Topic Index" at the end
```

Status legend:

```
├── ✅ Complete — document written and reviewed
├── 📝 Drafted — document written, pending review or revision
├── 🔲 Not started — document planned but not written
└── ⛔ Deprecated — document exists but is superseded (none currently)
```

> **NOTE:** All documents are specifications (pre-alpha phase). "Complete" means the specification is written, not that the feature is implemented. Implementation begins at alpha phase.

## Document Counts

| **Tier** | **Documents** | **Status** |
| :-: | :-: | :-: |
| Top-level | 34 | ✅ 33 complete + 🔲 1 pending (`TRANSLATIONS.md`); `LICENSE` at repo root |
| User guide | 17 | 🔲 17 not started |
| Dev guide | 19 | 🔲 19 not started |
| **TOTAL** | **70** |  |

> **NOTE:** `AGENTS.md` is the master reference document. It is not counted in the 70 but governs all of them.

## Top-Level Documents

### Project Overview

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `README.md` | Front door of the repository — what Spike is |
| ✅ | `INDEX.md` | This document — master index of all docs |
| ✅ | `AGENTS.md` | Master project reference — phases, conventions, tracking |
| ✅ | `LICENSE` | GPLv2+ full license text (repo root; copy in `licences/`) |
| ✅ | `LICENSE-CC-BY-SA-4.0` | CC-BY-SA 4.0 for documentation and artwork (repo root; copy in `licences/`) |

### Vision And Governance

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `PHILOSOPHY.md` | Project values, mission, and the "why" behind Spike |
| ✅ | `DESIGN-DECISIONS.md` | Rationale for every major technical and UX decision |
| ✅ | `GOVERNANCE.md` | BDFL model, structural constraints, succession, trademark |
| ✅ | `CODE_OF_CONDUCT.md` | Community behavior standards and enforcement model |
| ✅ | `CONTRIBUTING.md` | How to contribute — DCO, MRs, review process, conventions |
| ✅ | `ROADMAP.md` | Development timeline from pre-alpha through 1.0 and beyond |
| ✅ | `CHANGELOG.md` | Release history and changelog conventions (no releases yet) |
| ✅ | `END-OF-LIFE-POLICY.md` | Release lifecycle, support duration, EOL notification cascade |

### System Architecture

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `ARCHITECTURE.md` | System architecture overview — layers, components, data flow |
| ✅ | `KERNEL.md` | Kernel configuration, module management, boot parameters |
| ✅ | `MEMORY.md` | ZRAM, swap, earlyoom, memory budget, RAM classification |
| ✅ | `BOOT-PROCESS.md` | Boot sequence — GRUB, initramfs, spike-session, Plymouth handoff |
| ✅ | `CONFIGURATION.md` | spike-config system — state store, state.json, no config files |
| ✅ | `VARIANT-DIFFERENCES.md` | Spike Standard vs Plus — detection, differences, switching |

### Hardware

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `HARDWARE.md` | Hardware tiers, detection logic, GPU/CPU/RAM/storage support |
| ✅ | `PERFORMANCE-BASELINES.md` | Performance targets per tier — boot, memory, launch, FPS |

### Desktop And UI

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `DESKTOP.md` | Spike Shell — panel, launcher, notifications, window management |
| ✅ | `BRANDING.md` | Visual identity — colors, typography, logo, icons, sounds, themes |
| ✅ | `ACCESSIBILITY.md` | Vision, hearing, motor, cognitive features — performance budget |

### System Services

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `POWER-MANAGEMENT.md` | Battery, suspend, hibernate, CPU governors, thermal management |
| ✅ | `MULTIMEDIA.md` | PipeWire, VA-API, codecs, Bluetooth audio, screen recording |
| ✅ | `NETWORKING.md` | Wi-Fi, ethernet, firewall, DNS, VPN, Bluetooth (network side) |
| ✅ | `SECURITY.md` | Security model — AppArmor, Secure Boot, firewall, sandboxing |
| ✅ | `PRIVACY.md` | Privacy policy — zero telemetry, data collection stance, analytics ban |

### Installation And Recovery

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `INSTALLER.md` | Installation process — hardware detection, partitioning, variant |
| ✅ | `DISASTER-RECOVERY.md` | Recovery boot, rollback, data rescue, spike-reset, reinstall |

### User Support

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| ✅ | `MIGRATION-GUIDE.md` | Switching to Spike from Windows, macOS, or another Linux |
| ✅ | `TROUBLESHOOTING.md` | Symptom-based problem solving — 12 sections, GUI + advanced fixes |
| ✅ | `FAQ.md` | Common questions in plain language — 12 sections for non-technical users |
| ✅ | `SUPPORT.md` | Support channels, bug reporting, feature requests, community help |

## User Guide (17 Documents)

The user guide provides step-by-step instructions for everyday tasks. Target audience: non-technical users who have never used Linux. Reading level: plain language, no jargon, no terminal, no prerequisites.

All user-guide documents are 🔲 NOT STARTED. File names below are proposed — final names confirmed when documents are written.

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| 🔲 | `user-guide/getting-started.md` | First boot wizard, initial setup, first steps |
| 🔲 | `user-guide/desktop-basics.md` | Desktop layout, windows, workspaces, navigation |
| 🔲 | `user-guide/panel-and-launcher.md` | Panel applets, launcher, searching, pinning |
| 🔲 | `user-guide/file-management.md` | Files app, browsing, copying, organizing, trash |
| 🔲 | `user-guide/settings-overview.md` | Every settings category explained in plain language |
| 🔲 | `user-guide/network-and-wifi.md` | Connecting to Wi-Fi, ethernet, managing connections |
| 🔲 | `user-guide/bluetooth.md` | Pairing devices, audio, file transfer, troubleshooting |
| 🔲 | `user-guide/sound-and-audio.md` | Volume, outputs, inputs, Bluetooth audio, sound settings |
| 🔲 | `user-guide/display.md` | Resolution, scaling, multi-monitor, night light |
| 🔲 | `user-guide/keyboard-and-input.md` | Layouts, shortcuts, touchpad settings, Fn keys |
| 🔲 | `user-guide/power-and-battery.md` | Battery settings, suspend, hibernate, power profiles |
| 🔲 | `user-guide/installing-apps.md` | Using Discover, finding apps, managing installed apps |
| 🔲 | `user-guide/updates.md` | How updates work, checking, scheduling, what to expect |
| 🔲 | `user-guide/backups-and-recovery.md` | Spike Rescue, data backup, restoring from backup |
| 🔲 | `user-guide/accessibility-guide.md` | Screen reader, magnifier, on-screen keyboard, all features |
| 🔲 | `user-guide/printing.md` | Setting up printers, IPP, managing print queues |
| 🔲 | `user-guide/troubleshooting-basics.md` | First steps when something goes wrong — before the forum |

## Dev Guide (19 Documents)

The dev guide provides technical implementation details for contributors building Spike. Target audience: developers with Linux and Qt6 experience. Reading level: technical, with code examples, API references, and internals.

All dev-guide documents are 🔲 NOT STARTED. File names below are proposed — final names confirmed when documents are written.

| **Status** | **Document** | **Description** |
| :-: | :-: | :-: |
| 🔲 | `dev-guide/build-system.md` | ISO build process, live-build config, build tools |
| 🔲 | `dev-guide/iso-construction.md` | How the Spike ISO is assembled, contents, boot config |
| 🔲 | `dev-guide/spike-shell-architecture.md` | Spike Shell internals — process model, IPC, rendering |
| 🔲 | `dev-guide/qml-components.md` | QML component library — panels, applets, dialogs |
| 🔲 | `dev-guide/spike-config-internals.md` | state.json schema, config modules, rebuild process |
| 🔲 | `dev-guide/installer-development.md` | Installer internals — Calamares config or custom |
| 🔲 | `dev-guide/theme-system.md` | Qt stylesheet (QSS), theme engine, variant theming |
| 🔲 | `dev-guide/icon-theme-spec.md` | Icon theme structure, freedesktop.org spec, overrides |
| 🔲 | `dev-guide/kwin-configuration.md` | KWin setup, window decorations, effects, rules |
| 🔲 | `dev-guide/sddm-theme-dev.md` | SDDM QML theme development, authentication flow |
| 🔲 | `dev-guide/plymouth-theme-dev.md` | Plymouth theme scripts, rendering modes, variants |
| 🔲 | `dev-guide/grub-theme-dev.md` | GRUB theme files, font compilation, boot parameters |
| 🔲 | `dev-guide/kernel-module-management.md` | Module loading, blacklisting, DKMS, firmware handling |
| 🔲 | `dev-guide/zram-implementation.md` | ZRAM setup, zram-generator config, compression testing |
| 🔲 | `dev-guide/flatpak-integration.md` | Flatpak permissions, portals, overrides, Discover |
| 🔲 | `dev-guide/ci-cd-pipelines.md` | GitLab CI config, DCO enforcement, build automation |
| 🔲 | `dev-guide/testing-framework.md` | Test types, test runners, coverage, hardware testing |
| 🔲 | `dev-guide/release-process.md` | Release checklist, ISO signing, publication, mirroring |
| 🔲 | `dev-guide/coding-standards.md` | Detailed per-language conventions, linting, formatting |

## Topic Index

Find documents by topic. A document may appear under multiple topics.

### Boot And Startup

```
├── Boot sequence (full):              BOOT-PROCESS.md
├── GRUB bootloader (theme):           BRANDING.md
├── GRUB bootloader (technical):       BOOT-PROCESS.md
├── GRUB theme development:            dev-guide/grub-theme-dev.md 🔲
├── Plymouth (splash screen):         BOOT-PROCESS.md, BRANDING.md
├── Plymouth theme development:        dev-guide/plymouth-theme-dev.md 🔲
├── SDDM (login screen):               BOOT-PROCESS.md, BRANDING.md
├── SDDM theme development:            dev-guide/sddm-theme-dev.md 🔲
├── Boot failure recovery:            BOOT-PROCESS.md, DISASTER-RECOVERY.md
├── Boot counter mechanism:           BOOT-PROCESS.md
└── Boot performance targets:          PERFORMANCE-BASELINES.md
```

### CPU And Performance

```
├── CPU detection and classification:  HARDWARE.md
├── CPU governors:                     POWER-MANAGEMENT.md
├── Performance baselines:             PERFORMANCE-BASELINES.md
├── Performance budget (accessibility): ACCESSIBILITY.md
├── Variant performance differences:   VARIANT-DIFFERENCES.md
└── ZRAM and memory:                   MEMORY.md
```

### Desktop And UI

```
├── Desktop overview:                  DESKTOP.md
├── Panel and launcher:                DESKTOP.md
├── Window management:                 DESKTOP.md
├── Notifications:                     DESKTOP.md
├── Settings app:                      DESKTOP.md, CONFIGURATION.md
├── Visual identity (colors/fonts):    BRANDING.md
├── Logo and mascot:                   BRANDING.md
├── Icon theme:                        BRANDING.md
├── System sounds:                     BRANDING.md
├── Wallpapers:                        BRANDING.md
├── Animations (variant differences):  BRANDING.md, VARIANT-DIFFERENCES.md
├── Qt stylesheet (QSS):               BRANDING.md
├── KWin window decorations:           BRANDING.md, DESKTOP.md
├── Cursor theme:                      BRANDING.md
└── Theme system development:          dev-guide/theme-system.md 🔲
```

### File Management

```
├── File manager (Dolphin):            DESKTOP.md
├── File system (ext4):                INSTALLER.md, HARDWARE.md
├── Partitioning scheme:               INSTALLER.md, HARDWARE.md
├── Swap file:                         HARDWARE.md, MEMORY.md
├── Flatpak app data:                  CONFIGURATION.md
└── User guide (files):               user-guide/file-management.md 🔲
```

### GPU And Graphics

```
├── GPU support (Intel/AMD/NVIDIA):    HARDWARE.md
├── GPU driver selection:              HARDWARE.md
├── NVIDIA hybrid (Optimus):          HARDWARE.md
├── VA-API (hardware video decode):    MULTIMEDIA.md, HARDWARE.md
├── OpenGL/Vulkan:                    HARDWARE.md
├── Wayland compositor (KWin):         ARCHITECTURE.md, DESKTOP.md
└── Multi-monitor:                    HARDWARE.md, BRANDING.md
```

### Hardware

```
├── Hardware tiers:                    HARDWARE.md
├── Hardware detection logic:          HARDWARE.md
├── Storage support:                  HARDWARE.md
├── Wi-Fi support:                    HARDWARE.md, NETWORKING.md
├── Bluetooth support:                HARDWARE.md, NETWORKING.md
├── Audio hardware:                   HARDWARE.md, MULTIMEDIA.md
├── Webcam support:                   HARDWARE.md
├── Peripherals (USB, displays):      HARDWARE.md
├── Printers:                        HARDWARE.md
├── Modems:                          HARDWARE.md
├── Hardware registry (opt-in):       HARDWARE.md, PRIVACY.md
├── Minimum requirements:             HARDWARE.md
└── Hardware compatibility checking:  MIGRATION-GUIDE.md, HARDWARE.md
```

### Installation

```
├── Installer process:                INSTALLER.md
├── Partitioning:                     INSTALLER.md, HARDWARE.md
├── Variant selection:                INSTALLER.md, VARIANT-DIFFERENCES.md
├── Hardware detection at install:    INSTALLER.md, HARDWARE.md
├── Driver installation:              HARDWARE.md
├── Post-install wizard:              INSTALLER.md
├── Creating installer USB:           MIGRATION-GUIDE.md
├── Booting from USB:                 MIGRATION-GUIDE.md
└── Installer development:            dev-guide/installer-development.md 🔲
```

### Memory

```
├── ZRAM architecture:                MEMORY.md
├── Swap file management:            MEMORY.md, HARDWARE.md
├── Earlyoom (OOM killer):            MEMORY.md
├── Swappiness:                      HARDWARE.md, MEMORY.md
├── Memory budget (components):      MEMORY.md
├── ZRAM implementation:              dev-guide/zram-implementation.md 🔲
└── RAM detection and classification: HARDWARE.md
```

### Networking

```
├── Wi-Fi configuration:              NETWORKING.md
├── Ethernet:                        NETWORKING.md, HARDWARE.md
├── Firewall (ufw):                  SECURITY.md, NETWORKING.md
├── DNS:                             NETWORKING.md
├── VPN:                             NETWORKING.md
├── NetworkManager:                  NETWORKING.md
├── Wi-Fi hardware support:          HARDWARE.md
└── User guide (network):            user-guide/network-and-wifi.md 🔲
```

### Privacy And Security

```
├── Zero telemetry policy:            PRIVACY.md
├── Security model overview:          SECURITY.md
├── AppArmor:                        SECURITY.md
├── Secure Boot:                     SECURITY.md
├── Flatpak sandboxing:               SECURITY.md
├── Portal permissions:              SECURITY.md, DESKTOP.md
├── Disk encryption (LUKS):          SECURITY.md, INSTALLER.md
├── Camera privacy:                  HARDWARE.md, PRIVACY.md
├── Hardware registry privacy:       HARDWARE.md, PRIVACY.md
└── Data collection stance:          PRIVACY.md
```

### Power

```
├── Battery management:               POWER-MANAGEMENT.md
├── Suspend and hibernate:            POWER-MANAGEMENT.md
├── CPU governors:                    POWER-MANAGEMENT.md
├── Thermal management:               POWER-MANAGEMENT.md
├── Power profiles:                   POWER-MANAGEMENT.md
├── Lid switch behavior:              POWER-MANAGEMENT.md
└── User guide (power):              user-guide/power-and-battery.md 🔲
```

### Accessibility

```
├── Accessibility overview:           ACCESSIBILITY.md
├── Screen reader (Orca):            ACCESSIBILITY.md
├── Magnifier:                       ACCESSIBILITY.md
├── On-screen keyboard:              ACCESSIBILITY.md
├── High contrast mode:              ACCESSIBILITY.md, BRANDING.md
├── Motor/input aids:                ACCESSIBILITY.md
├── Cognitive features:              ACCESSIBILITY.md
├── Login screen accessibility:      ACCESSIBILITY.md, BRANDING.md
├── Performance budget (<100MB):     ACCESSIBILITY.md, PERFORMANCE-BASELINES.md
└── User guide (accessibility):      user-guide/accessibility-guide.md 🔲
```

### Updates And Recovery

```
├── Update system:                    CONFIGURATION.md
├── Update scheduling:                CONFIGURATION.md
├── Disaster recovery overview:       DISASTER-RECOVERY.md
├── Recovery boot:                   DISASTER-RECOVERY.md, BOOT-PROCESS.md
├── Rollback (snapshot):             DISASTER-RECOVERY.md
├── Data rescue:                     DISASTER-RECOVERY.md
├── spike-reset:                     DISASTER-RECOVERY.md
├── Reinstallation:                  DISASTER-RECOVERY.md, INSTALLER.md
└── User guide (updates):            user-guide/updates.md 🔲
```

### Governance And Community

```
├── Governance model (BDFL):          GOVERNANCE.md
├── Structural constraints:           GOVERNANCE.md, CONTRIBUTING.md
├── Succession planning:              GOVERNANCE.md
├── Trademark policy:                 GOVERNANCE.md
├── Code of Conduct:                 CODE_OF_CONDUCT.md
├── Contributing guide:               CONTRIBUTING.md
├── Support channels:                SUPPORT.md
├── Forum structure:                 SUPPORT.md
├── Bug reporting:                   SUPPORT.md
├── Feature requests:                SUPPORT.md
├── End-of-life policy:              END-OF-LIFE-POLICY.md
└── Roadmap:                         ROADMAP.md
```

### Multimedia

```
├── Audio (PipeWire):                 MULTIMEDIA.md
├── Video playback:                  MULTIMEDIA.md
├── Codecs (H.264, HEVC, VP9, AV1):  MULTIMEDIA.md
├── VA-API (hardware decode):        MULTIMEDIA.md, HARDWARE.md
├── Bluetooth audio:                MULTIMEDIA.md, NETWORKING.md
├── Screen recording:               MULTIMEDIA.md
├── System sounds:                  BRANDING.md
└── User guide (sound):             user-guide/sound-and-audio.md 🔲
```

### Branding And Visual Design

```
├── Color palette:                    BRANDING.md
├── Typography (Noto Sans):           BRANDING.md
├── Logo (Spike dog):                 BRANDING.md
├── Boot visuals (GRUB, Plymouth, SDDM): BRANDING.md
├── Circuit trace motif:             BRANDING.md
├── Component specs (buttons, inputs): BRANDING.md
├── Wallpapers:                      BRANDING.md
├── Icon theme:                      BRANDING.md
├── Cursor theme:                    BRANDING.md
├── Animation guidelines:            BRANDING.md
├── Spacing and layout:              BRANDING.md
├── Asset file inventory:            BRANDING.md
├── Asset licensing (CC BY-SA):      BRANDING.md, GOVERNANCE.md
└── Branding compliance checklist:   BRANDING.md
```

### Migration

```
├── Migration overview:               MIGRATION-GUIDE.md
├── From Windows:                    MIGRATION-GUIDE.md
├── From macOS:                      MIGRATION-GUIDE.md
├── From another Linux:              MIGRATION-GUIDE.md
├── Browser migration:               MIGRATION-GUIDE.md
├── Email migration:                 MIGRATION-GUIDE.md
├── Software equivalents:            MIGRATION-GUIDE.md
├── Accounts and services:           MIGRATION-GUIDE.md
└── First week guide:                MIGRATION-GUIDE.md
```

## Document Dependency Map

This map shows which documents reference which other documents. It helps contributors understand the impact of changes.

Legend:

```
├── A → B means "A references B" (if you change B, check A)
├── ↔ means mutual reference (both reference each other)
└── →* means "references many" (not individually listed)
```

`ARCHITECTURE.md`

```
├── → PHILOSOPHY.md, DESIGN-DECISIONS.md, KERNEL.md, MEMORY.md,
│      BOOT-PROCESS.md, DESKTOP.md, CONFIGURATION.md,
│      VARIANT-DIFFERENCES.md, HARDWARE.md
└── →* user-guide/, dev-guide/
```

`PHILOSOPHY.md`

```
├── → DESIGN-DECISIONS.md, GOVERNANCE.md
└── (referenced by almost everything)
```

`DESIGN-DECISIONS.md`

```
├── → ARCHITECTURE.md, PHILOSOPHY.md, VARIANT-DIFFERENCES.md,
│      CONFIGURATION.md, DESKTOP.md, HARDWARE.md
└── (referenced by: CONTRIBUTING.md, many others)
```

`KERNEL.md`

```
├── → MEMORY.md, BOOT-PROCESS.md, HARDWARE.md
└── (referenced by: ARCHITECTURE.md)
```

`MEMORY.md`

```
├── → KERNEL.md, CONFIGURATION.md, HARDWARE.md,
│      PERFORMANCE-BASELINES.md, VARIANT-DIFFERENCES.md
└── (referenced by: HARDWARE.md, POWER-MANAGEMENT.md, ACCESSIBILITY.md)
```

`BOOT-PROCESS.md`

```
├── → KERNEL.md, DESKTOP.md, CONFIGURATION.md,
│      DISASTER-RECOVERY.md, BRANDING.md
└── (referenced by: ARCHITECTURE.md, DISASTER-RECOVERY.md)
```

`DESKTOP.md`

```
├── → ARCHITECTURE.md, CONFIGURATION.md, BRANDING.md,
│      ACCESSIBILITY.md, MULTIMEDIA.md, SECURITY.md
└── (referenced by: many user-guide docs)
```

`INSTALLER.md`

```
├── → HARDWARE.md, VARIANT-DIFFERENCES.md, CONFIGURATION.md,
│      SECURITY.md, BOOT-PROCESS.md
└── (referenced by: MIGRATION-GUIDE.md, DISASTER-RECOVERY.md)
```

`CONFIGURATION.md`

```
├── → ARCHITECTURE.md, VARIANT-DIFFERENCES.md
└── (referenced by: almost everything)
```

`VARIANT-DIFFERENCES.md`

```
├── → ARCHITECTURE.md, MEMORY.md, POWER-MANAGEMENT.md,
│      BRANDING.md, PERFORMANCE-BASELINES.md
└── (referenced by: INSTALLER.md, HARDWARE.md, many others)
```

`HARDWARE.md`

```
├── → MEMORY.md, KERNEL.md, BOOT-PROCESS.md, INSTALLER.md,
│      MULTIMEDIA.md, NETWORKING.md, POWER-MANAGEMENT.md,
│      ACCESSIBILITY.md, PERFORMANCE-BASELINES.md,
│      VARIANT-DIFFERENCES.md, PRIVACY.md, SECURITY.md
└── (referenced by: MIGRATION-GUIDE.md, SUPPORT.md, many others)
```

`POWER-MANAGEMENT.md`

```
├── → KERNEL.md, MEMORY.md, HARDWARE.md
└── (referenced by: VARIANT-DIFFERENCES.md)
```

`MULTIMEDIA.md`

```
├── → HARDWARE.md, NETWORKING.md, DESKTOP.md
└── (referenced by: BRANDING.md)
```

`NETWORKING.md`

```
├── → HARDWARE.md, SECURITY.md, MULTIMEDIA.md
└── (referenced by: HARDWARE.md)
```

`SECURITY.md`

```
├── → INSTALLER.md, CONFIGURATION.md, DESKTOP.md, PRIVACY.md
└── (referenced by: HARDWARE.md, NETWORKING.md, DISASTER-RECOVERY.md)
```

`PRIVACY.md`

```
├── → SECURITY.md, HARDWARE.md
└── (referenced by: HARDWARE.md, SUPPORT.md)
```

`ACCESSIBILITY.md`

```
├── → DESKTOP.md, BRANDING.md, PERFORMANCE-BASELINES.md
└── (referenced by: BRANDING.md, DESKTOP.md)
```

`BRANDING.md`

```
├── → ACCESSIBILITY.md, PERFORMANCE-BASELINES.md, DESKTOP.md,
│      BOOT-PROCESS.md, MULTIMEDIA.md, GOVERNANCE.md
└── (referenced by: many user-guide docs)
```

`DISASTER-RECOVERY.md`

```
├── → BOOT-PROCESS.md, INSTALLER.md, CONFIGURATION.md
└── (referenced by: SUPPORT.md, MIGRATION-GUIDE.md)
```

`PERFORMANCE-BASELINES.md`

```
├── → HARDWARE.md, VARIANT-DIFFERENCES.md
└── (referenced by: ACCESSIBILITY.md, BRANDING.md, many dev-guide docs)
```

`GOVERNANCE.md`

```
├── → PHILOSOPHY.md, CONTRIBUTING.md
└── (referenced by: CODE_OF_CONDUCT.md, CONTRIBUTING.md, BRANDING.md)
```

`CODE_OF_CONDUCT.md`

```
├── → CONTRIBUTING.md, GOVERNANCE.md, SUPPORT.md, PRIVACY.md, SECURITY.md
└── (referenced by: CONTRIBUTING.md, SUPPORT.md, README.md)
```

`CONTRIBUTING.md`

```
├── → CODE_OF_CONDUCT.md, AGENTS.md, GOVERNANCE.md, ARCHITECTURE.md,
│      DESIGN-DECISIONS.md, SUPPORT.md, SECURITY.md, PRIVACY.md,
│      ROADMAP.md, PHILOSOPHY.md
└── (referenced by: README.md, SUPPORT.md, CODE_OF_CONDUCT.md)
```

`SUPPORT.md`

```
├── → CODE_OF_CONDUCT.md, CONTRIBUTING.md, FAQ.md, TROUBLESHOOTING.md,
│      PRIVACY.md, SECURITY.md, GOVERNANCE.md, END-OF-LIFE-POLICY.md,
│      ROADMAP.md, HARDWARE.md, MIGRATION-GUIDE.md
└── (referenced by: MIGRATION-GUIDE.md, README.md)
```

`MIGRATION-GUIDE.md`

```
├── → INSTALLER.md, HARDWARE.md, VARIANT-DIFFERENCES.md,
│      PERFORMANCE-BASELINES.md, TROUBLESHOOTING.md, FAQ.md,
│      SUPPORT.md, PRIVACY.md, DESKTOP.md, CONFIGURATION.md,
│      DISASTER-RECOVERY.md, ACCESSIBILITY.md, END-OF-LIFE-POLICY.md
└── (referenced by: README.md, SUPPORT.md)
```

`ROADMAP.md`

```
├── → (references most top-level docs by context)
└── (referenced by: README.md, SUPPORT.md, CONTRIBUTING.md)
```

`CHANGELOG.md`

```
├── → (references releases, which reference docs)
└── (referenced by: README.md)
```

`END-OF-LIFE-POLICY.md`

```
├── → ROADMAP.md, GOVERNANCE.md
└── (referenced by: SUPPORT.md, MIGRATION-GUIDE.md)
```

`FAQ.md`

```
├── → (references many docs as "see X.md for details")
└── (referenced by: SUPPORT.md, MIGRATION-GUIDE.md, TROUBLESHOOTING.md)
```

`TROUBLESHOOTING.md`

```
├── → (references many docs depending on symptom)
└── (referenced by: SUPPORT.md, MIGRATION-GUIDE.md, FAQ.md)
```

`README.md`

```
├── → (references INDEX.md, AGENTS.md, all top-level docs by name)
└── (referenced by: none — it's the entry point)
```

## Index Maintenance

### When To Update This Index

Update this index whenever:

```
├── A new document is added to the repository
│   └── Add it to the appropriate section with status and description
├── A document is renamed
│   └── Update the filename, and check all documents that reference it
├── A document's status changes
│   └── 🔲 → 📝 → ✅ (update the status icon)
├── A document is deprecated or removed
│   └── Mark as ⛔ Deprecated (do not silently delete entries)
├── A new topic emerges that should be in the Topic Index
│   └── Add the topic and list relevant documents
└── Cross-references change
    └── Update the Dependency Map
```

The index update should be part of the same merge request as the document change that triggered it. This keeps the index accurate at every commit.

### Index Audit

Periodic audit (monthly or before each release):

1. Verify every document in the repository appears in this index

```
ls *.md user-guide/*.md dev-guide/*.md
```

Compare output to this index.

2. Verify every document in this index exists in the repository

   - If a document is listed but doesn't exist: remove it or mark as "planned, not yet created" 

   - If a document exists but isn't listed: add it 

3. Verify status icons are accurate

   - 🔲 Not started → file doesn't exist or is empty/placeholder 

   - 📝 Drafted → file exists with content but not reviewed 

   - ✅ Complete → file exists, reviewed, and considered final 

   - ⛔ Deprecated → file exists but is superseded 

4. Verify cross-references in the Topic Index

   - Each "See X.md" reference points to a real document 

   - Broken references indicate renamed or removed documents 

5. Verify the Dependency Map

   - Check that key cross-references are still accurate 

   - Add new dependencies that have emerged 

Audit results:

```
├── If everything checks out: no action needed
├── If discrepancies found: create a merge request to fix the index
└── The BDFL is responsible for ensuring audits happen
```

## What This Document Does Not Cover

- **Document content** (this index lists documents, it doesn't summarize them): See individual documents 

- **House style and conventions**: See `AGENTS.md` 

- **Phase tracking and completion percentages**: See `AGENTS.md` (Phase 2 tracking) 

- **Contribution process**: See `CONTRIBUTING.md` 

- **Roadmap and timeline**: See `ROADMAP.md` 

- **Change history for individual documents**: See Git history (`git log -- [filename]`) 

- **Release history**: See `CHANGELOG.md` 

🐕 BigRangaTech


