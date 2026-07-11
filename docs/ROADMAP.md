# Spike Roadmap

## Purpose

This document outlines the development roadmap for Spike Linux — from current state through production release and beyond. It defines milestones, deliverables, timelines, and the criteria for moving between phases.

Roadmaps are plans, not promises. Dates are targets, not deadlines. The project is developed by a solo developer using a docs-first methodology, which means scope and pace are constrained by reality. This document will be updated as the project evolves.

## Current Status

**Phase:** Pre-alpha (documentation)

**Started:** TBD by BDFL

**Status:** Active — writing specification documents before any code

**What is done:**

```
`├── Core philosophy defined (PHILOSOPHY.md)`

`├── Architecture specified (ARCHITECTURE.md, DESIGN-DECISIONS.md)`

`├── All major subsystems documented:`

`│   ├── Boot process (BOOT-PROCESS.md)`

`│   ├── Kernel configuration (KERNEL.md)`

`│   ├── Memory management (MEMORY.md)`

`│   ├── Desktop shell (DESKTOP.md)`

`│   ├── Installer (INSTALLER.md)`

`│   ├── Networking (NETWORKING.md)`

`│   ├── Multimedia (MULTIMEDIA.md)`

`│   ├── Power management (POWER-MANAGEMENT.md)`

`│   ├── Security (SECURITY.md)`

`│   ├── Privacy (PRIVACY.md)`

`│   ├── Configuration system (CONFIGURATION.md)`

`│   ├── Disaster recovery (DISASTER-RECOVERY.md)`

`│   ├── Variant differences (VARIANT-DIFFERENCES.md)`

`│   └── Performance baselines (PERFORMANCE-BASELINES.md)`

`├── Governance model defined (GOVERNANCE.md)`

`├── Project infrastructure decisions made`

`│   ├── GitLab CE self-hosted (primary)`

`│   ├── GitHub mirror (read-only)`

`│   ├── GPLv2+ license`

`│   ├── DCO (no CLA)`

`│   └── Monorepo structure`

`└── Branding identity established (BRANDING.md)`
```

**What is not done:**

```
`├── Remaining top-level docs (~15 documents)`

`├── User guide (17 documents)`

`├── Developer guide (19 documents)`

`├── Any code`

`├── Any ISO builds`

`├── Any hardware testing`

`└── Any translations`
```

## Phase Overview

```
`PRE-ALPHA          ALPHA              BETA              PRODUCTION`

`(docs-first)       (core build)       (polish + test)   (release)`


`Documentation │  Code + test    │  Polish + broaden │  Maintain`

`+ design       │  on hardware    │  + translate      │  + improve`


`Duration: ?    Duration: 8 mo    Duration: 10 mo    Duration: ONGOING`

`Status: Active  Status: Future   Status: Future     Status: Future`


`Total to production: ~18 months from start of alpha (target)`

`End of life: Minimum 5 years per release (see END-OF-LIFE-POLICY.md)`
```

## Phase 1: Pre-Alpha (Current)

### Objective

Complete all documentation before writing any code. Every architectural decision recorded with rationale. Every component specified. Every constraint documented. The codebase should be buildable by reading the documentation alone — no tribal knowledge required.

### Duration

Unknown — determined by documentation velocity. The docs-first methodology is intentional and will not be rushed. Writing documentation reveals design gaps and inconsistencies before they become expensive code rewrites.

### Deliverables

**Documentation (70+ files):**

```
`├── Top-level docs (~33 files)`

`│   ├── Completed: 16+ (philosophy, architecture, all major subsystems)`

`│   ├── Remaining: ~17 (governance, branding, hardware, accessibility,`

`│   │   troubleshooting, FAQ, migration, support, EOL, roadmap,`

`│   │   changelog, readme, index, code of conduct, contributing,`

`│   │   license)`

`│   └── Status: In progress`

`├── User guide (17 files)`

`│   ├── Not started`

`│   └── Prerequisite: Desktop shell spec must be finalized`

`└── Developer guide (19 files)`

`    ├── Not started`

`    └── Prerequisite: Architecture docs must be finalized`
```

**Project infrastructure:**

```
`├── GitLab CE instance provisioned (git.bigrangatech.com)`

`├── GitHub mirror configured (github.com/bigrangatech/spike)`

`├── CI/CD pipeline scaffolded`

`├── Website deployed (spike.bigrangatech.com)`

`├── Domain and DNS configured`

`├── Issue templates created`

`├── Pull request template created`

`└── Communication channels established (forum / Matrix)`
```

**Repository:**

```
`├── Read-only to public`

`├── Issues open (feedback welcome)`

`├── No code contributions accepted`

`└── Translations accepted (once user guide is written)`
```

### Exit Criteria

Pre-alpha ends when:

- All top-level documentation is complete and reviewed 

- User guide is complete (all 17 files, with screenshots planned) 

- Developer guide is complete (all 19 files) 

- Repository structure matches AGENTS.md codebase layout 

- Infrastructure is operational (GitLab, CI, website) 

- BDFL sign-off on documentation completeness 

**Pre-alpha exit checklist:**

- All 33 top-level docs written and reviewed 

- All 17 user guide docs written 

- All 19 dev guide docs written 

- Repository scaffolded (empty source trees with CMakeLists.txt) 

- CI/CD pipeline runs (even if just "hello world") 

- Website live with documentation 

- GitLab issues open for community feedback 

- Hardware registry page live (survey form, not results) 

- BDFL review and sign-off 

### What The Community Can Do During Pre-Alpha

| **Activity** | **Allowed?** |
| :-: | :-: |
| Read documentation | ✓ |
| Report documentation errors | ✓ |
| Suggest features (issues) | ✓ |
| Register hardware | ✓ (hardware survey) |
| Discuss in forum / Matrix | ✓ |
| Submit translations | ✓ (once user guide exists) |
| Submit code | ✗ (not yet) |
| Fork the repository | ✓ (but no merges accepted) |


## Phase 2: Alpha (Months 1–8)

### Objective

Build the core system. Spike Shell boots to a usable desktop. The installer works end-to-end. Core functionality is operational on Tier 1 hardware. Basic hardware testing confirms the architecture works on real machines.

### Duration

8 months from alpha start.

### Version Target

- 0.1.0-alpha → Initial alpha (end of month 4-5) 

- 0.2.0-alpha → Expanded alpha (end of month 8) 

### Alpha Milestone 1: First Boot (Months 1–4)

**Goal:** Spike boots to a desktop on Tier 1 hardware.

**Deliverables:**

```
`├── Ubuntu Server LTS base configured`

`│   ├── Snap stripped`

`│   ├── cloud-init removed`

`│   ├── Telemetry removed`

`│   └── Unnecessary services disabled`

`├── Kernel configured`

`│   ├── Boot parameters applied`

`│   ├── Module blacklist generated`

`│   └── GPU driver selected (Intel i915 + VA-API)`

`├── GRUB2 installed and themed`

`│   ├── Hidden menu with ESC window`

`│   ├── Boot failure counter (grubenv)`

`│   └── Recovery entry present`

`├── KWin (Wayland) running standalone`

`│   ├── No Akonadi, Baloo, KIO, Plasma`

`│   └── XWayland available on demand`

`├── Spike Shell (minimal):`

`│   ├── Panel (bottom, 32px)`

`│   ├── Launcher (applications list + search)`

`│   ├── Clock applet`

`│   ├── Session menu (logout, shutdown, reboot)`

`│   └── Theme engine (basic Qt stylesheet)`

`├── SDDM configured`

`│   ├── Spike-themed login screen`

`│   ├── Wayland session`

`│   └── Auto-login optional`

`├── Plymouth boot splash`

`│   ├── spike-minimal theme (static logo + dots)`

`│   └── Handoff to SDDM (may not be perfect)`

`├── PipeWire + WirePlumber`

`│   ├── Audio output works`

`│   └── 44100Hz locked`

`├── NetworkManager`

`│   ├── Wi-Fi connects`

`│   └── All firmware on ISO`

`├── Memory management`

`│   ├── ZRAM configured (if CPU capable)`

`│   ├── Swap file created`

`│   ├── Swappiness set per storage type`

`│   └── earlyoom running`

`├── Security baseline`

`│   ├── ufw enabled (deny incoming)`

`│   ├── AppArmor enforcing`

`│   ├── Root locked`

`│   └── No telemetry (verified)`

`└── Settings panel (minimal)`

`    ├── Appearance (basic)`

`    ├── Network (KCM)`

`    └── About`
```

**Exit criteria for Milestone 1:**

- Boots on Celeron N4020 (Tier 1, SSD) 

- Desktop is visible and interactive 

- Wi-Fi connects to a known network 

- Audio plays from speakers 

- System shuts down cleanly 

- Idle memory \< 500MB (relaxed target for initial boot) 

- No crashes during 30-minute idle session 

### Alpha Milestone 2: Installer (Months 3–6)

**Goal:** Spike can be installed from USB to a laptop.

**Deliverables:**

```
`├── Live ISO builds successfully`

`│   ├── squashfs with full system`

`│   ├── All firmware included`

`│   ├── Flatpak runtimes pre-seeded (KDE + GNOME)`

`│   └── USB 3.0 bootable`

`├── Spike Installer (10-step flow):`

`│   ├── Welcome + language`

`│   ├── Timezone (IP geolocation if online)`

`│   ├── Wi-Fi connection`

`│   ├── Username + password`

`│   ├── Computer name`

`│   ├── Variant selection (auto-recommended)`

`│   ├── Data backup to USB (scans, copies, verifies)`

`│   ├── Storage confirmation (type "ERASE")`

`│   ├── Installation (20 automated tasks)`

`│   └── Reboot to desktop`

`├── Hardware detection modules:`

`│   ├── CPU detection (bogomips, cores, classification)`

`│   ├── Storage detection (type, size, rejection logic)`

`│   ├── GPU detection (vendor, model, driver selection)`

`│   ├── Network detection (WiFi, Ethernet, firmware)`

`│   ├── Bluetooth detection`

`│   └── Modem detection`

`├── Partitioning (automatic):`

`│   ├── UEFI: /boot/efi + / + swapfile`

`│   └── BIOS: /boot + / + swapfile`

`├── spike-config generates at install time:`

`│   ├── GRUB configuration`

`│   ├── Kernel parameters`

`│   ├── Module blacklist`

`│   ├── ZRAM/swap/swappiness`

`│   ├── Plymouth theme`

`│   ├── SDDM configuration`

`│   ├── Sysctl tunables`

`│   └── Mount options`

`├── Data backup system:`

`│   ├── Scans Windows/Linux/macOS user directories`

`│   ├── Copies to USB with SHA256 verification`

`│   └── Preserves folder structure`

`└── Post-install first boot:`

`    ├── Welcome wizard`

`    ├── Post-install notifications`

`    └── First-boot configuration`
```

**Exit criteria for Milestone 2:**

- ISO boots on Tier 1 hardware from USB 

- Installer completes all 10 steps 

- Installed system boots from internal storage 

- All hardware detected correctly on Celeron N4020 

- Data backup works (files copied to USB, verified) 

- GRUB installs correctly (UEFI and BIOS) 

- Module blacklist generated correctly 

- Installed system meets Milestone 1 criteria 

### Alpha Milestone 3: Core Shell (Months 5–8)

**Goal:** Spike Shell is functional with core applets and settings.

**Deliverables:**

```
`├── All core tray applets:`

`│   ├── Network (NetworkManager DBus)`

`│   ├── Volume (PipeWire DBus)`

`│   ├── Battery (UPower DBus)`

`│   ├── Brightness (/sys/class/backlight)`

`│   ├── Notifications (internal daemon)`

`│   ├── Removable devices (udisks2 DBus)`

`│   ├── Update notifier (apt + Flatpak polling)`

`│   ├── Clock (systemd-timesyncd)`

`│   ├── Session menu (systemd loginctl)`

`│   └── Night light (KWin gamma)`

`├── Conditional applets (hardware-detected):`

`│   ├── Bluetooth (BlueZ DBus)`

`│   ├── Airplane mode (rfkill)`

`│   └── Keyboard layout (libinput)`

`├── Notification daemon (full spec):`

`│   ├── DBus listener (org.freedesktop.Notifications)`

`│   ├── Disk-before-display invariant (atomic writes)`

`│   ├── History persistence (history.json)`

`│   ├── History viewer (grouped by day, searchable)`

`│   ├── Do Not Disturb mode`

`│   ├── Tray badge (unread count, never auto-clears)`

`│   └── Crash recovery (systemd restart, history reload)`

`├── Settings panel (expanded):`

`│   ├── Appearance (theme, colors, fonts)`

`│   ├── Notifications (retention, DND, history)`

`│   ├── Memory (read-only overview + swappiness slider)`

`│   ├── Boot (splash, timeout, failure counter)`

`│   ├── Storage (disk usage, mount info)`

`│   ├── Diagnostics (system info, security status)`

`│   ├── Sound (KCM)`

`│   ├── Display (KCM)`

`│   ├── Power (KCM)`

`│   ├── Keyboard (KCM)`

`│   ├── Mouse/Touchpad (KCM)`

`│   ├── Network (KCM)`

`│   ├── Bluetooth (KCM, conditional)`

`│   ├── Date & Time (KCM)`

`│   └── About (with user guide reader)`

`├── KDE standalone apps installed:`

`│   ├── Discover (software center)`

`│   ├── Dolphin (file manager)`

`│   ├── Konsole (terminal)`

`│   ├── Kate (text editor)`

`│   ├── Ark (archive tool)`

`│   ├── Spectacle (screenshots)`

`│   └── KCalc (calculator)`

`├── Flatpak configured:`

`│   ├── Flathub repository added`

`│   ├── Pre-seeded runtimes available`

`│   └── Firefox installed as Flatpak with VA-API prefs`

`├── spike-update.timer:`

`│   ├── Security updates automatic (every 6 hours)`

`│   ├── Conditions checked (network, idle, memory)`

`│   └── Notifications for kernel updates`

`└── Lock screen:`

`    ├── Password required`

`    ├── Clock display`

`    ├── No notification content preview`

`    └── Ctrl+Alt+F2 disabled when locked`
```

**Exit criteria for Alpha (0.2.0-alpha):**

- All Tier 1 baselines met (or within 10% of target) 

- Installer tested on at least 5 different laptops 

- All core applets functional 

- Notification daemon never loses a notification 

- Settings panel covers all user-facing configuration 

- No terminal required for any normal task (Golden Rule 2 verified) 

- No config file editing required for any user setting (Golden Rule 1 verified) 

- Idle memory \< 400MB (Standard) on Tier 1 

- Boot time \< 40s (Standard) on Tier 1 SSD 

- No critical bugs 

### Alpha Phase Contribution Model

| **Activity** | **Allowed?** |
| :-: | :-: |
| Report bugs | ✓ (encouraged) |
| Test on hardware | ✓ (encouraged, hardware registry) |
| Submit translations | ✓ |
| Submit code patches | ✓ (case-by-case, DCO required) |
| Code review | BDFL only |
| Branch protection | Not yet (main is writable by BDFL) |
| CI/CD | Running, tests encouraged but not enforced |


## Phase 3: Beta (Months 9–18)

### Objective

Polish the system. Broaden hardware testing. Add translations. Finalize all documentation. Enforce code review. Prepare for production release.

### Duration

10 months (months 9–18 from alpha start).

### Version Target

- 0.3.0-beta → Public beta (month 10-12) 

- 0.4.0-beta → Feature-complete beta (month 14-16) 

- 0.5.0-beta → Release candidate (month 17-18) 

### Beta Milestone 1: Public Beta (Months 9–12)

**Goal:** Broad public testing with a polished experience.

**Deliverables:**

```
`├── Plymouth smooth handoff (no flicker)`

`├── All 14 tray applets functional (including 4th conditional)`

`├── Spike Rescue tool:`

`│   ├── Live ISO desktop icon`

`│   ├── Scans for installed operating systems`

`│   ├── Mounts read-only`

`│   ├── Copies files with SHA256 verification`

`│   └── Reports summary`

`├── Data restore in installer:`

`│   ├── Detects SpikeBackup/ on USB`

`│   ├── Restores to /home/\[user\]/`

`│   ├── Sets ownership correctly`

`│   └── Verifies with SHA256`

`├── Performance optimized to meet all baselines:`

`│   ├── Boot time \< 40s on Tier 1 SSD`

`│   ├── Idle memory \< 400MB on Standard`

`│   ├── App launch times within targets`

`│   └── Shutdown \< 5s on SSD`

`├── Accessibility features:`

`│   ├── On-screen keyboard`

`│   ├── Screen reader (initial)`

`│   ├── High contrast mode`

`│   ├── Large text option`

`│   └── Visual alerts (for hearing-impaired users)`

`├── Initial translations (target: top 5 languages by user base)`

`├── Hardware registry collecting data`

`├── User guide complete with screenshots`

`└── Branding finalized:`

`    ├── GRUB theme`

`    ├── Plymouth themes (both variants)`

`    ├── SDDM theme`

`    ├── Qt stylesheet`

`    ├── KWin decorations`

`    ├── Wallpapers`

`    ├── Icon set`

`    └── Boot/spinner animations`
```

**Exit criteria:**

- All Milestone 3 criteria still met (no regressions) 

- Plymouth handoff smooth on Tier 1 

- Spike Rescue tested on corrupted install 

- Data restore tested end-to-end 

- At least 10 hardware configurations tested 

- At least 5 translations in progress 

- User guide reviewed by non-technical user (usability test) 

### Beta Milestone 2: Feature Complete (Months 13–16)

**Goal:** All features implemented. All translations. All hardware tested.

**Deliverables:**

```
`├── All translations complete (11 languages):`

`│   ├── English (source)`

`│   ├── Spanish`

`│   ├── French`

`│   ├── German`

`│   ├── Portuguese (Brazilian)`

`│   ├── Italian`

`│   ├── Russian`

`│   ├── Chinese (Simplified)`

`│   ├── Japanese`

`│   ├── Dutch`

`│   └── (1 TBD based on community demand)`

`├── Accessibility complete:`

`│   ├── Screen reader (full)`

`│   ├── Magnifier`

`│   ├── Sticky keys, slow keys, bounce keys`

`│   ├── Mouse keys`

`│   ├── Color filters`

`│   └── All accessibility settings in GUI`

`├── Broad hardware testing:`

`│   ├── 20+ hardware configurations tested`

`│   ├── Tier 2 (AMD A4) fully functional`

`│   ├── NVIDIA hardware tested (nouveau + proprietary)`

`│   ├── Hybrid graphics tested`

`│   └── Edge-case hardware documented`

`├── Developer guide complete (all 19 files)`

`├── CI/CD comprehensive:`

`│   ├── Builds on every push`

`│   ├── Automated tests for critical paths`

`│   ├── ISO build automated`

`│   └── Hardware test results tracked`

`├── Full settings panel:`

`│   ├── All custom pages complete`

`│   ├── All KCM modules integrated`

`│   ├── Search across all settings`

`│   ├── Context-aware help buttons`

`│   └── User guide reader integrated`

`└── Variant differences fully implemented:`

`    ├── Animations (Standard: off / Plus: on)`

`    ├── Compositor effects (Standard: minimal / Plus: full)`

`    ├── Plymouth (Standard: static / Plus: animated)`

`    ├── ZRAM cap (Standard: 4GB / Plus: uncapped)`

`    ├── CPU governor (Standard: powersave / Plus: schedutil)`

`    ├── Flatpak runtimes (Standard: selective / Plus: all)`

`    └── NVIDIA mode (Standard: Integrated / Plus: Hybrid)`
```

**Exit criteria:**

- All features from all docs implemented 

- All 11 translations complete 

- 20+ hardware configs tested 

- Tier 2 fully functional 

- No known critical or high-severity bugs 

- All performance baselines met on Tier 1 

- Golden Rules verified on every feature 

- Critical constraints verified (all sections of AGENTS.md Section 7) 

### Beta Milestone 3: Release Candidate (Months 17–18)

**Goal:** Freeze features. Fix bugs. Prepare for production.

**Deliverables:**

```
`├── Feature freeze — no new features`

`├── Bug fix only phase`

`├── Security review:`

`│   ├── No open inbound ports (verified)`

`│   ├── No telemetry (verified)`

`│   ├── No remote access (verified)`

`│   ├── AppArmor enforcing (verified)`

`│   ├── ufw enabled (verified)`

`│   └── Root locked (verified)`

`├── Documentation audit:`

`│   ├── All 70+ docs reviewed for accuracy`

`│   ├── Screenshots updated to match final UI`

`│   ├── Cross-references verified`

`│   └── No TODOs or placeholders remaining`

`├── Final performance validation:`

`│   ├── All baselines measured on Tier 1`

`│   ├── All baselines measured on Tier 2`

`│   ├── Regression test matrix complete`

`│   └── Results recorded in baselines.json`

`├── Installer validation:`

`│   ├── Tested on diverse hardware (20+ machines)`

`│   ├── UEFI and BIOS tested`

`│   ├── Data backup/restore tested`

`│   ├── Fresh install tested`

`│   └── Reinstall-with-restore tested`

`├── Disaster recovery validation:`

`│   ├── Boot failure counter tested`

`│   ├── Previous kernel fallback tested`

`│   ├── Recovery mode tested`

`│   └── Spike Rescue tested`

`├── Update mechanism tested:`

`│   ├── Security updates apply automatically`

`│   ├── Non-security updates notify correctly`

`│   ├── Flatpak updates work`

`│   └── Kernel update + reboot flow tested`

`└── Release preparation:`

`    ├── ISO signed`

`    ├── Checksums generated (SHA256)`

`    ├── Download page prepared`

`    ├── Release notes written (CHANGELOG.md)`

`    └── Announcement drafted`
```

**Exit criteria (production readiness):**

- All Tier 1 baselines met (measured, not estimated) 

- Tier 2 boots and functions (measured) 

- Zero critical bugs 

- Zero high-severity bugs 

- All documentation complete and accurate 

- Security review passed 

- Golden Rules verified 

- Critical constraints verified 

- BDFL sign-off 

### Beta Phase Contribution Model

| **Activity** | **Allowed?** |
| :-: | :-: |
| Report bugs | ✓ |
| Test on hardware | ✓ |
| Submit translations | ✓ |
| Submit code | ✓ (DCO required, code review required) |
| Branch protection | ✓ (main protected, MR required) |
| CI/CD | Must pass before merge |
| Squash merge | Preferred (clean history) |


## Phase 4: Production (Month 19+)

### Objective

Ship Spike 1.0. Maintain, improve, and support.

### Version Target

- 1.0.0 → First production release 

- 1.X.Y → Ongoing maintenance 

### 1.0.0 Release

**Release 1.0.0 includes:**

```
`├── Everything from Beta Milestone 3`

`├── ISO published on spike.bigrangatech.com`

`├── SHA256 checksums published`

`├── GPG signature on ISO`

`├── Release announcement`

`├── Documentation published on website`

`├── User guide available in Settings (offline reader)`

`├── 11 language translations`

`├── Hardware registry results published (anonymized)`

`└── Support channels active`
```

**Post-release:**

```
`├── Monitor for critical bugs`

`├── Hotfix releases as needed (1.0.1, 1.0.2, etc.)`

`├── Security patches via automatic updates`

`├── Community support in forum / Matrix`

`├── Hardware registry continuously updated`

`└── Translation improvements ongoing`
```

### Post-1.0 Roadmap

Potential future work (not committed, evaluated based on adoption):

```
`├── Limine bootloader migration (trigger criteria in DESIGN-DECISIONS.md)`

`├── Disk encryption support (opt-in, must integrate with rescue tooling)`

`├── Additional language translations (community-driven)`

`├── ARM64 support (if demand exists, if hardware warrants)`

`├── Custom software center (if Discover limitations become problematic)`

`├── Tablet/touch mode (if touch hardware demand emerges)`

`├── Printing system (CUPS integration, if not already done)`

`├── Backup scheduling (automated data backup beyond installer)`

`├── System migration tool (import settings from Windows/macOS)`

`└── Community-driven themes`
```

None of these are promised. They are possibilities. The post-1.0 roadmap will be shaped by user feedback, community contributions, and the BDFL's assessment of what serves the mission best.

## Timeline Summary

| **Phase** | **Duration** | **Version** | **Status** |
| :-: | :-: | :-: | :-: |
| Pre-alpha | ? | — | In progress |
| Alpha | 8 months | 0.1–0.2-alpha | Future |
| Beta | 10 months | 0.3–0.5-beta | Future |
| Production | Ongoing | 1.0+ | Future |

**Total to 1.0:** ~18 months from alpha start (target)

> Note: The 18-month target assumes alpha starts promptly after documentation is complete. The pre-alpha duration is intentionally unbounded — rushing documentation compromises the docs-first methodology and the "built to outlast" principle.

## Risk Assessment

### Risks To Timeline

| **Risk** | **Likelihood** | **Impact** | **Mitigation** |
| :-: | :-: | :-: | :-: |
| Solo developer burnout | Medium | High | Realistic pace, community support, BDFL succession plan |
| Hardware diversity issues | Medium | Medium | Broad testing during alpha/beta, hardware registry |
| Ubuntu 26.04 LTS delayed | Low | High | Fall back to 24.04 LTS, adjust kernel expectations |
| Wayland regressions on old hardware | Medium | Medium | XWayland fallback, nouveau fallback, kernel pinning |
| NVIDIA driver compatibility issues | Medium | Low | nouveau default, proprietary optional, documented limitations |
| Flatpak runtime bloat | Low | Medium | Selective pre-seeding, runtime cleanup |
| Translation community not forming | Medium | Low | English-first release, translations incremental |
| Scope creep | High | High | Golden Rules as gatekeeper, BDFL veto, phased contribution model |
| Funding/resource constraints | Medium | Medium | Minimal infrastructure costs, self-hosted, volunteer community |

### Scope Creep Management

Scope creep is the biggest risk to Spike. The BDFL is responsible for guarding against it.

**Anti-scope-creep measures:**

```
`├── Golden Rules as gatekeeper (every feature must comply)`

`├── Critical constraints in AGENTS.md Section 7 (non-negotiable)`

`├── "Is this for a beginner?" test (if not, it doesn't belong)`

`├── "Does this work on Celeron N4020?" test (if not, defer)`

`├── "Does this increase idle memory?" test (if yes, justify or reject)`

`├── BDFL has final say on all feature additions`

`├── Post-1.0 features are explicitly "not committed"`

`└── Every feature requires documentation before code`
```

### What Could Delay Production

| **Delay Cause** | **Contingency** |
| :-: | :-: |
| Documentation takes longer than expected | Accept it — docs-first is foundational |
| Alpha reveals architectural flaws | Redesign affected component, update docs, re-implement |
| Hardware testing reveals incompatibility | Document limitation, prioritize Tier 1, defer Tier 2 edge cases |
| Translation community doesn't materialize | Ship English-only 1.0, add translations later |
| Security vulnerability discovered pre-1.0 | Fix before release, no compromise on security baselines |
| BDFL unavailable | Succession plan (see GOVERNANCE.md) |

## Dependencies

### External Dependencies

| **Dependency** | **Status** | **Impact if delayed** |
| :-: | :-: | :-: |
| Ubuntu 26.04 LTS release | Scheduled Apr 2026 | Spike targets this kernel; fallback: 24.04 LTS |
| KDE Frameworks 6 (Qt6) | Released | Stable, no risk |
| PipeWire | Stable | No risk |
| WirePlumber | Stable | No risk |
| NetworkManager | Stable | No risk |
| Flatpak | Stable | No risk |
| KWin (standalone Wayland) | Stable | No risk |
| Discover | Stable | Occasional bugs, upstream maintains |
| systemd | Stable | No risk |
| GRUB2 | Stable | No risk |

### Internal Dependencies

| **Component** | **Depends on** | **Status** |
| :-: | :-: | :-: |
| Spike Shell | KWin, Qt6, systemd | Not started |
| Spike Installer | spike-config, hardware detect | Not started |
| Spike Rescue | Live ISO, Qt6 | Not started |
| spike-config | Templates, sysctl, modprobe | Not started |
| spike-branding | Artist assets, GRUB, Plymouth | Not started |
| ISO build system | All above | Not started |
| CI/CD | GitLab CE, Docker | Scaffolded |

## Community Milestones

| **Milestone** | **Target Phase** | **What Happens** |
| :-: | :-: | :-: |
| Issue tracker opens | Pre-alpha | Community can report doc errors, suggest features |
| Hardware registry opens | Pre-alpha | Users register target hardware |
| Forum / Matrix active | Pre-alpha | Community discussion begins |
| Translations open | Late pre-alpha | Community translates user guide |
| Alpha testing opens | Alpha milestone 1 | Community tests early builds |
| Code contributions open | Alpha (case-by-case) | Community submits patches |
| Full contributions open | Beta | Community submits code with review |
| Beta testing opens | Beta milestone 1 | Broader public testing |
| 1.0 release | Production | Public production release |

## What This Document Does Not Cover

- Detailed feature specifications: See individual subsystem documents (BOOT-PROCESS.md, KERNEL.md, MEMORY.md, etc.) 

- Architectural rationale: See DESIGN-DECISIONS.md 

- Governance and decision-making authority: See GOVERNANCE.md 

- End-of-life policy for releases: See END-OF-LIFE-POLICY.md 

- Contribution guidelines and workflow: See CONTRIBUTING.md 

- Code of conduct: See CODE\_OF\_CONDUCT.md 

- Release process (signing, publishing): See dev-guide/17-release-process.md 

- Performance targets: See PERFORMANCE-BASELINES.md 

- Hardware tier definitions: See HARDWARE.md 

🐕 BigRangaTech

