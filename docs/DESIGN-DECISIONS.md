# Spike Design Decisions

## Purpose

This document records the rationale behind every major architectural and project decision in Spike. It exists so that future maintainers, contributors, and curious users understand not just **what** was decided, but **why**.

Each decision includes the context that led to it, the alternatives considered, and the trade-offs accepted.

## Base System

### Ubuntu Server LTS

**Decision:** Build Spike on Ubuntu Server LTS (26.04 "Resolute Raccoon").

**Context:** The base distribution determines kernel freshness, package availability, security support lifespan, and maintenance burden. The target hardware (Celeron N4020, Intel UHD 600) needs recent Mesa and kernel for functional Wayland support.

**Alternatives considered:**

| **Distribution** | **Rejected because** |
| :-: | :-: |
| Debian Stable | Older kernel and Mesa stack. Gemini Lake GPUs need newer drivers for acceptable Wayland performance. Shorter support window than Ubuntu ESM. |
| Fedora | 13-month support cycle creates upgrade pressure on beginners. Rolling nature incompatible with "install and forget" philosophy. |
| Arch Linux | Rolling release demands constant attention. Package breakage unacceptable for target users. Maintenance burden too high. |
| Alpine Linux | musl libc breaks binary compatibility with many applications. Flatpak support is poor. Not suitable for a desktop distribution. |
| Buildroot | No package manager, no upgrade path. KDE applications need a real system. Security maintenance becomes the user's responsibility. Completely wrong fit for beginners. |
| Ubuntu Desktop | Ships with Snap, GNOME, and desktop-oriented bloat that would need to be stripped anyway. Starting from Server is cleaner. |

**Trade-offs accepted:**

```
`├── Ubuntu's Snap architecture is present in the ecosystem (we strip it, but it exists upstream)`

`├── Canonical's direction may shift in ways we can't control (mitigated by LTS pinning)`

`└── Some Ubuntu-specific patches may cause friction with upstream KDE (acceptable, KDE actively supports Ubuntu)`
```

**Why Server, not Desktop:** Starting from Ubuntu Server gives a clean minimal base. No GNOME shell to remove, no Snap to purge from a pre-configured desktop session, no Canonical desktop tooling to strip. The Spike team builds its own desktop from a blank slate.

### Ubuntu LTS Kernel 7.0 (No Custom Kernel)

**Decision:** Use the Ubuntu 26.04 LTS kernel (Linux 7.0) as-is.

**Context:** A custom kernel was considered for tighter optimization on Celeron hardware. The maintenance burden of rebasing against upstream LTS releases, tracking security patches, and testing hardware regressions is too high for a solo developer.

**Trade-offs accepted:**

```
`├── The kernel includes drivers and modules for hardware Spike will never encounter`

`├── Some kernel parameters need boot-time tuning rather than compile-time exclusion`

`└── Module blacklisting at install time handles the unused-driver problem adequately`
```

**Configuration approach:** Kernel boot parameters and module blacklisting are applied at install time. See **KERNEL.md** for details.

### x86\_64 Only

**Decision:** Support x86\_64 architecture only. No i386 build.

**Context:** 32-bit support was considered as a future conditional project. The maintenance burden of forking i386 libraries, finding compatible Flatpak runtimes, and testing on 32-bit hardware is significant. The target hardware (2013+ Celerons, AMD Jaguars) is overwhelmingly x86\_64.

**Condition for revisiting:** Only if significant user demand demonstrates that a meaningful population of 32-only hardware exists and cannot run a 64-bit kernel with 32-bit userspace (a common workaround).

## Display Stack

### Wayland Over X11

**Decision:** Use Wayland as the native display protocol with XWayland fallback.

**Context:** X11 is legacy and increasingly deprecated upstream. Modern KDE and GNOME development focuses on Wayland. The target hardware (Intel UHD 600 on Gemini Lake) has acceptable Wayland support with recent Mesa. Ubuntu 26.04 LTS drops X11 entirely from its default session, confirming the industry direction.

**Trade-offs accepted:**

```
`├── Some older applications may have rendering quirks under XWayland`

`├── NVIDIA proprietary drivers have historically poor Wayland support (mitigated by nouveau fallback and improving upstream support)`

`└── Debugging Wayland issues is harder than X11 (mitigated by strong upstream support from KDE)`
```

**XWayland behavior:** Launches on demand. When no X11 applications are running, XWayland consumes zero resources.

### KWin Without Plasma

**Decision:** Use KWin as the window manager/compositor in a standalone Wayland session, without the full Plasma desktop shell.

**Context:** KWin is a mature, well-maintained compositor with excellent Wayland support. It supports theming, effects, and configuration. Running it standalone avoids pulling in plasma-workspace, plasma-session, and their dependencies.

**What KWin runs without:**

| **Component** | **Why excluded** |
| :-: | :-: |
| Akonadi | PIM/email indexing service. Unnecessary overhead. Never started. |
| Baloo | File indexer. Consumes CPU and I/O. Contradicts Spike's lightweight goals. Never started. |
| KIO slaves | File access abstraction layer. Not required for standalone KDE apps accessing local files. |
| plasma-session | Plasma shell session manager. Spike Shell's session manager replaces it. |
| plasma-workspace | Full Plasma desktop workspace. Not installed. |

**Alternatives considered:**

| **Compositor** | **Rejected because** |
| :-: | :-: |
| Weston | Reference compositor, not suitable for daily desktop use. No theming, no effects. |
| Mutter | Tied to GNOME ecosystem. Would pull GNOME dependencies. Different development priorities. |
| Sway / wlroots | Tiling window manager paradigm. Wrong UX for beginners. Missing features expected in a desktop OS (system tray, notifications, etc.). |
| XFWM4 / Marco | X11-centric. Limited Wayland support. |

### Custom Spike Shell (Not Full Plasma, Not XFCE, Not LXQt)

**Decision:** Build a custom desktop shell (Spike Shell) rather than using an existing desktop environment.

**Context:** The target user is a beginner who found an old laptop. Their experience needs to feel cohesive, designed, and intentional — not like a collection of parts from different projects held together with scripts.

**Alternatives considered:**

| **Desktop** | **Rejected because** |
| :-: | :-: |
| Full KDE Plasma | Too heavy for 4GB Celeron targets. Akonadi, Baloo, and plasma-workspace add overhead Spike can't afford. Feels like "someone else's desktop" — not Spike's identity. |
| XFCE | Functional but visually dated. Relies on X11 primarily. Feels like a compromise, not a design choice. Package ecosystem is smaller than KDE. |
| LXQt | Closer in spirit but underdeveloped. Inconsistent theming. Limited applet ecosystem. Not mature enough for a polished beginner experience. |
| GNOME Shell | Heavy memory footprint. Extension system is fragile. Diverges from Spike's design language. |
| Cinnamon | Built on GNOME internals. Carries their weight. Linux Mint-specific development focus. |

**Why custom, not repackaged:** Existing lightweight desktops feel like "not real OSes." They lack cohesive software centers, discoverable settings, and the polish that makes a user feel confident. Spike Shell solves this by being purpose-built for the target audience while leveraging KDE Frameworks and standalone KDE applications for proven, well-maintained components.

**The hybrid approach:** Spike Shell handles the shell layer (panel, launcher, applets, notifications, settings, session, theme). KDE standalone applications handle the application layer (file manager, software center, text editor, terminal, etc.). This gives Spike proven applications without the overhead of a full desktop environment.

## NVIDIA Driver Policy

**Decision:** Ship nouveau (open-source NVIDIA driver) by default. Offer proprietary NVIDIA driver as optional post-install installation. Do not include proprietary driver on ISO.

**Context:** NVIDIA hardware exists in the wild. While Celeron-class laptops rarely have discrete NVIDIA GPUs, users upgrading from Windows or using mixed-GPU laptops (Optimus configurations) will encounter NVIDIA hardware. Excluding NVIDIA entirely would prevent Spike from running on those systems.

**Alternatives considered:**

| **Option** | **Rejected because** |
| :-: | :-: |
| Include proprietary driver on ISO | NVIDIA EULA forbids redistribution without separate acceptance. ISO size would increase significantly. Driver versions age quickly and may not match the user's specific GPU generation. |
| Drop NVIDIA support entirely | Too restrictive. Users with NVIDIA hardware would be blocked from using Spike even when their integrated graphics are adequate. Contradicts the "rescue old hardware" mission. |
| Force proprietary driver on detection | Requires internet connection at install time. Increases installer complexity. Breaks offline installs. Violates beginner-first principle (driver version selection is not a beginner task). |

**Approach taken:**

```
`├── nouveau is included in the kernel and works out of the box for display and basic compositing`

`├── Proprietary driver is available for users who need CUDA, gaming, or hardware video acceleration`

`├── ubuntu-drivers tool (included with Ubuntu Server LTS) handles driver listing and installation`

`├── Installation is initiated through Settings → Software Sources → Additional Drivers (GUI, no terminal)`

`├── Post-install notification informs NVIDIA users of the option without forcing action`

`└── Hybrid graphics (Optimus) supported via PRIME render offload with three modes: Integrated Only, Hybrid, Discrete Only`
```

**Trade-offs accepted:**

```
`├── nouveau has weaker Wayland support (mitigated by XWayland automatic fallback)`

`├── nouveau lacks VA-API hardware video decode on most NVIDIA GPUs (users use integrated GPU for video playback)`

`├── Proprietary driver must be installed manually (beginner UX: notification explains this clearly)`

`├── Fermi-era GPUs (pre-GTX 600) are unsupported by modern proprietary driver (use nouveau only)`

`├── Secure Boot requires MOK enrollment for proprietary driver (installer handles notification if needed)`

`└── Some users may blame Spike for NVIDIA issues that are upstream nouveau limitations (documented in troubleshooting)`
```

## Filesystem

### ext4 Everywhere

**Decision:** Use ext4 for all installations. No Btrfs, no LVM, no ZFS.

**Context:** Btrfs with snapshots was considered for SSD systems to enable rollback functionality. After designing the rescue/reinstall-with-data-restore loop, snapshots became redundant for the target user.

**Alternatives considered:**

| **Filesystem** | **Rejected because** |
| :-: | :-: |
| Btrfs | Snapshot management adds complexity. CoW overhead on slow HDDs. eMMC wear concerns (even though we don't support eMMC, consistency matters). Beginners don't understand snapshots. The rescue USB loop achieves data safety more simply. |
| LVM | Adds a layer of abstraction for zero user benefit when there's a single disk with a single partition. Complicates recovery. |
| ZFS | License incompatibility (CDDL vs GPL). Excessive memory overhead. Designed for servers, not laptops. |
| f2fs | Flash-optimized but less tested than ext4. Recovery tools are less mature. Benefit on SATA SSDs is marginal. |

**Trade-offs accepted:**

```
`├── No snapshot-based rollback (replaced by USB backup + reinstall-with-restore)`

`├── No transparent compression (ZRAM handles memory compression instead)`

`└── No built-in data integrity checking beyond ext4's journaling`
```

### No Manual Partitioning

**Decision:** The installer handles all partitioning automatically. No manual option exists.

**Context:** The target user doesn't know what a partition is. Offering manual partitioning introduces risk (data loss, unbootable systems) and complexity (explaining EFI vs BIOS, boot partitions, swap placement) for zero benefit to the target audience.

**Trade-offs accepted:**

```
`├── Power users cannot customize partition layouts (they can use another distribution)`

`├── Dual-boot configurations are impossible by design`

`└── Users with exotic storage setups may not be supported`
```

### No eMMC Support

**Decision:** eMMC storage is explicitly unsupported.

**Context:** eMMC flash has limited write endurance compared to SSDs. Spike's swap file and system logging would accelerate wear on eMMC modules, leading to premature failure. The project owner has personal experience with eMMC failure.

**Trade-offs accepted:**

```
`├── Some budget laptops with only eMMC storage are excluded`

`└── Users with eMMC-only devices are directed to replace the storage or use a different distribution`
```

## Memory Management

### Adaptive ZRAM

**Decision:** Enable ZRAM conditionally based on CPU capability, not universally.

**Context:** ZRAM compresses memory pages in RAM, effectively increasing usable memory. On capable CPUs (dual-core, adequate bogomips), this is a net win — compression/decompression is fast enough that the memory gain outweighs CPU cost. On weak single-core processors, ZRAM compression overhead can make the system slower, not faster.

**Detection method:**

```
`├── CPU cores: nproc`

`├── CPU speed: /proc/cpuinfo bogomips`

`├── Capable: dual-core+ with bogomips ≥ ~2200 per core`

`└── Low-end: single-core or bogomips below threshold`
```

**Trade-offs accepted:**

```
`├── Detection heuristics may misclassify edge-case CPUs (mitigated by conservative thresholds)`

`└── Users can't manually toggle ZRAM without the Settings GUI (acceptable — if CPU is borderline, the safer choice is no ZRAM)`
```

### Swap File, Not Swap Partition

**Decision:** Use a swap file on the root partition, not a dedicated swap partition.

**Context:** A swap file can be resized without repartitioning. It simplifies the installer (one fewer partition to create). On ext4, swap file performance is comparable to swap partition performance.

### Earlyoom

**Decision:** Run earlyoom as a safety valve.

**Context:** On 4GB RAM systems, memory exhaustion is a real scenario. The Linux kernel's OOM killer is notoriously slow and unpredictable. Earlyoom monitors memory and kills the largest process before the system becomes unresponsive, preserving the desktop session.

**Trade-offs accepted:**

```
`├── Applications may be killed unexpectedly under memory pressure (better than a frozen system)`

`└── Earlyoom adds a small constant background process (~2MB)`
```

### Swappiness Tuning By Storage Type

**Decision:** Set swappiness based on detected storage type (SSD: 15, HDD: 5).

**Context:** Swappiness controls how aggressively the kernel swaps pages to disk. On SSDs, swapping is relatively fast, so a moderate swappiness (15) is fine. On HDDs, swapping causes severe latency, so swappiness should be minimized (5) to keep applications in RAM as long as possible.

## Software Management

### Flatpak Over Snap

**Decision:** Use Flatpak for user-installed applications. Snap is stripped entirely.

**Context:** Snap is Canonical's container format, tightly coupled to Ubuntu's infrastructure and the Snap Store (proprietary backend). Flatpak is community-governed, supports multiple repositories (Flathub is the de facto standard), and integrates cleanly with Discover.

**Trade-offs accepted:**

```
`├── Some Ubuntu-specific applications are only available as Snaps (not relevant to target users)`

`└── Flatpak runtime sizes are significant (mitigated by pre-seeding on ISO)`
```

### Pre-Seeded Flatpak Runtimes

**Decision:** Include KDE and GNOME Flatpak runtimes on the ISO.

**Context:** First app installation in Flatpak requires downloading the entire runtime (~500MB+). For users on slow connections, this makes the first app install painfully slow. Pre-seeding runtimes eliminates this barrier.

**Standard vs Plus:** Standard includes only KDE and GNOME runtimes (selective). Plus includes all common runtimes (freedesktop, KDE, GNOME, and others).

### Discover As Software Center

**Decision:** Use KDE Discover as the software center.

**Context:** Discover handles both apt (system packages) and Flatpak (user applications) in a single, beginner-friendly interface. It supports ratings, screenshots, and search. Building a custom software center would be a massive undertaking with no clear benefit.

**Trade-offs accepted:**

```
`├── Discover occasionally has bugs related to Flatpak integration (mitigated by upstream KDE development)`

`└── Discover pulls in some KDE Frameworks dependencies (acceptable, shared with other KDE apps)`
```

## Desktop Shell

### Custom Network Applet (Not plasma-nm, Not nm-tray)

**Decision:** Build a custom network applet as part of Spike Shell.

**Context:** plasma-nm may pull Plasma dependencies. nm-tray is visually inconsistent with Spike's design language. A custom applet talks directly to NetworkManager via DBus and renders in Spike's theme.

**This decision cascaded:** Once the custom network applet was decided, it became clear that all tray applets should be custom for visual consistency. This led to the full 14-applet Spike Shell tray architecture.

### All Firmware On ISO

**Decision:** Ship all major Wi-Fi firmware on the ISO, selected dynamically at install time.

**Context:** Target users are beginners reviving old laptops. If Wi-Fi doesn't work on first boot, they're stuck. Including all firmware ensures Wi-Fi works out of the box on the widest range of hardware.

**Firmware included:**

```
`├── Intel Wi-Fi (iwlwifi)`

`├── Atheros (ath)`

`├── Realtek (rtl)`

`└── Broadcom (bcmwl-kernel-source, proprietary)`
```

**Trade-offs accepted:**

```
`├── Larger ISO size (~300MB for firmware)`

`└── Proprietary Broadcom driver included (necessary for broad hardware support)`
```

### Notification Disk-Before-Display Invariant

**Decision:** Every notification is written to disk before it is displayed on screen.

**Context:** If a user hears a notification sound but can't find the notification, they lose trust in the system. Disk-first guarantees that even if the display renderer crashes, the notification is preserved in history.

**Implementation:**

```
`├── DBus notification received`

`├── Write to ~/.local/share/spike/notifications/history.json (atomic write)`

`├── Display popup`

`├── If display fails, notification is still in history`

`└── Tray badge shows unread count, persists until user opens history`
```

**Trade-offs accepted:**

```
`├── Slight latency increase (disk write before display, typically \<5ms)`

`└── Transient notifications are kept in history (only display behavior changes, not persistence)`
```

### 3-Day Default Notification Retention

**Decision:** Default notification history retention is 3 days, adjustable in Settings (1-31 days).

**Context:** Longer retention risks filling disk on small storage configurations. 3 days covers a weekend — if a user puts their laptop away Friday and picks it up Monday, they can still see Friday's notifications.

### KDE KCM Modules Inside Custom Settings

**Decision:** Load KDE System Settings modules (KCM) inside Spike's custom Settings window.

**Context:** KDE's KCM modules (display, sound, power, keyboard, mouse, network, etc.) are well-built and maintained. Rebuilding all of them from scratch would be wasteful. Loading them inside Spike's themed window gives users a unified settings experience while leveraging proven KDE components.

**Custom Spike settings pages handle Spike-specific configuration that KDE doesn't cover:**

```
`├── Memory management status`

`├── Boot behavior`

`├── Notification settings`

`├── Update preferences`

`├── Storage diagnostics`

`├── System diagnostics`

`└── NVIDIA driver management (Additional Drivers, accessed via Software Sources section)`
```

## Boot

### GRUB2 With Hidden Menu

**Decision:** Use GRUB2 with the boot menu hidden by default.

**Context:** Beginners don't need to see a boot menu on every startup. It's intimidating and confusing. But the menu must be accessible for recovery scenarios.

**Behavior:**

```
`├── Menu hidden, 3-5 second ESC window during boot`

`├── 3 consecutive boot failures → menu appears automatically, recovery entry highlighted`

`├── Recovery entry always present when menu is shown`

`└── GRUB2 themed with Spike branding`
```

**Alternatives considered:**

| **Bootloader** | **Rejected because** |
| :-: | :-: |
| systemd-boot | Less feature-rich than GRUB. Module loading and initramfs integration less mature. Theming support limited. |
| Limine | Promising, modern design, better theming. But less tested on old hardware. Migration planned for future with documented trigger criteria. |
| rEFInd | Primarily for UEFI. Inconsistent BIOS support. Overkill for single-OS systems. |

**Migration to Limine — trigger criteria:**

```
`├── Limine reaches sufficient stability on target hardware`

`├── GRUB2 maintenance becomes burdensome`

`└── Community pressure for faster boot times`
```

### Module Blacklisting At Install Time

**Decision:** Generate a module blacklist at install time based on detected hardware.

**Context:** The Ubuntu LTS kernel includes modules for hardware Spike will never encounter (enterprise RAID controllers, Infiniband, exotic sensors). These modules load on boot, consuming memory. Blacklisting them at install time reclaims that memory.

**Critical rule:** Only blacklist modules for absent soldered hardware. Never blacklist hot-pluggable or USB-connected devices — the user may plug one in later.

## Multimedia

### PipeWire Over PulseAudio

**Decision:** Use PipeWire + WirePlumber for audio.

**Context:** PulseAudio is legacy. PipeWire is the modern Linux audio stack with better performance, lower latency, and cleaner architecture. WirePlumber is the recommended session manager.

**Configuration choices:**

```
`├── 44100Hz sample rate (saves CPU on weak processors; 48000Hz provides no audible benefit for the target use case)`

`├── Stereo-only (target hardware is laptops with built-in stereo speakers)`

`├── Medium resampling quality (balanced CPU/quality)`

`├── Logging suppressed (unnecessary I/O on low-end storage)`

`└── No JACK support (pro audio is not the target use case)`
```

### Conditional Bluetooth

**Decision:** Start Bluetooth services only if Bluetooth hardware is detected.

**Context:** Many target laptops (especially budget Celeron models) don't have Bluetooth. Running BlueZ and the PipeWire Bluetooth module on hardware without Bluetooth wastes memory.

**Implementation:** udev rule triggers BlueZ startup when a Bluetooth adapter is detected. This includes USB Bluetooth dongles plugged in later.

### VA-API With AV1 Disabled In Firefox

**Decision:** Disable AV1 in Firefox to force VP9 fallback for hardware video decode.

**Context:** Gemini Lake (Celeron N4020) GPUs can hardware-decode VP9 but not AV1. Firefox prefers AV1 when available on websites (like YouTube). With AV1 enabled, video decode falls back to software, consuming significant CPU on a Celeron. Disabling AV1 forces VP9, enabling hardware decode.

**Trade-offs accepted:**

```
`├── Users can't watch AV1-only content with hardware acceleration (rare — most content has VP9 fallback)`

`└── Bandwidth may be slightly higher (VP9 files are generally larger than AV1)`
```

## Security

### No Encryption

**Decision:** Do not offer disk encryption.

**Context:** Full disk encryption (LUKS) adds a password prompt at every boot. For target users (beginners using revived laptops at home), this is an unnecessary barrier. The password complexity and recovery implications are disproportionate to the threat model.

**Trade-offs accepted:**

```
`├── Stolen laptops expose user data (acceptable risk for the target audience)`

`└── Users who need encryption should use a different distribution`
```

**Mitigation:** ufw firewall protects against network threats. AppArmor provides application sandboxing. User password protects local login.

### Standard User/Sudo Model

**Decision:** Single user account with sudo privileges. Root login disabled.

**Context:** Simplifies the model for beginners. One password, one account. sudo handles privilege escalation. Root account is locked (cannot log in directly).

**Password policy:** Minimum 6 characters. No complexity requirements (no forced special characters, no uppercase requirements). Beginners struggle with complex passwords and end up writing them down, which is worse security than a simple memorable password.

### No Telemetry

**Decision:** Zero telemetry, zero data collection.

**Context:** The target user is installing Linux for the first time. They already feel uncertain about privacy in the tech world. Collecting data — even anonymously — violates the trust relationship. Spike's network connections are limited to: NTP time sync, package mirror access, and Flatpak repository access. Nothing else.

## Installer

### Fully Automatic, No Choices Beyond User Identity

**Decision:** The installer asks for language, timezone, Wi-Fi, username, password, hostname, and variant. Everything else is automated.

**Context:** Each additional choice is a point where a beginner can make a mistake, get confused, or abandon the installation. By reducing choices to the absolute minimum, the installer becomes approachable for someone who has never installed an operating system.

### Data Backup Before Wipe

**Decision:** The installer scans the target drive for personal files and offers to back them up to USB before wiping.

**Context:** Beginners often don't understand that installing a new operating system erases everything. Rather than relying on a scary warning dialog, the installer actively helps preserve their data.

**Implementation:**

```
`├── Scans for Windows, Linux, and macOS user directories (Documents, Photos, Videos, Music, Downloads, Desktop)`

`├── Copies to USB with checksum verification`

`├── Preserves folder structure`

`└── Reports what was and wasn't copied`
```

### One ISO, Two Variants

**Decision:** Ship a single ISO. The installer detects hardware and recommends Standard or Plus.

**Context:** Two ISOs would force users to figure out which one they need before downloading — a barrier for beginners who don't know their RAM amount or CPU model. One ISO with installer-side detection removes that decision entirely.

**User override:** The recommendation can be overridden, but the installer clearly indicates which variant suits the detected hardware.

## Project

### Docs-First Methodology

**Decision:** Write all documentation before writing any code.

**Context:** Without documentation, architectural decisions live only in the developer's head. If the developer leaves, the project dies. Documentation-first ensures every decision is recorded with rationale, and the codebase can be understood by future maintainers.

**Side effect:** Writing documentation reveals design gaps, inconsistencies, and missing specifications before they become expensive code rewrites.

### GPLv2+ License

**Decision:** GPLv2+ for code, CC-BY-SA 4.0 for documentation and artwork.

**Context:** GPLv2+ is compatible with the kernel, KDE Frameworks, Flatpak, and Ubuntu packages. It allows future relicensing to GPLv3 if needed. It prevents proprietary forks while allowing integration with the broader free software ecosystem.

**Why not GPLv3:** Some components Spike depends on are GPLv2-only. GPLv3 compatibility is achieved through the "+" clause without locking in GPLv2 exclusively.

**Why not MIT/BSD:** Permissive licenses would allow proprietary forks that don't contribute back. Spike's mission includes community benefit. Copyleft ensures derivatives remain free.

### DCO, Not CLA

**Decision:** Require Developer Certificate of Origin (DCO) sign-off on all commits. No Contributor License Agreement (CLA).

**Context:** CLAs give the project owner power to relicense contributed code — a practice that discourages community participation and creates power imbalance. DCO simply affirms that the contributor has the right to submit their code. It's a legal safeguard, not a rights grab.

### BDFL Governance Initially

**Decision:** Benevolent Dictator For Life (BDFL) model during initial development.

**Context:** Early-stage projects need fast decision-making. Committee governance slows development when the community is small. The project owner (Ranga) makes final decisions on architecture, direction, and community standards.

**Governance structure:**

```
`├── All contributors have a voice — Ideas, critiques, and proposals are welcome from anyone. Discussion happens openly through issues, merge requests, and communication channels.`

`├── The BDFL has final say — When consensus cannot be reached, when arguments stall progress, or when a technical decision requires arbitration, the BDFL breaks the tie and the project moves forward.`

`├── Input is not ignored — The BDFL considers all feedback before ruling. Bad ideas that don't get adopted are explained respectfully. Good ideas get merged.`

`└── Prevents chaos — Without a clear authority figure, endless debates can paralyze a project. Contributors leave when nothing gets decided. BDFL ensures the project has direction.`
```

**Succession plan:** If the BDFL steps away, the position transfers to whoever is appointed as head developer. The successor inherits the same authority: final say on disputes, but expected to listen to the community first. BigRangaTech retains organizational ownership throughout.

**Transition triggers:**

```
`├── BDFL voluntarily appoints a successor`

`├── BDFL passes away or becomes permanently unavailable`

`└── BDFL loses trust of the community (extremely rare, requires documented reasons)`
```

When succession occurs, the incoming head developer maintains the same model: collaborative input with a tiebreaker role.

**Rationale:** A distributed council model works for large projects with established norms. For Spike's early phase, rapid iteration matters more than democratic process. Once the project matures and the contributor base grows, governance can evolve to a council or meritocracy if the BDFL deems it beneficial.

**What this means in practice:**

```
`Contributor proposes a feature → Discussion ensues → Consensus attempts`

`    │`

`    ├── If consensus reached → Feature merged`

`    │`

`    └── If debate stalls or splits evenly → BDFL weighs in`

`        ├── Considers all arguments`

`        ├── Makes a ruling`

`        └── Project proceeds according to that ruling`
```

Even when the BDFL makes the final call, contributors retain the ability to fork the code under the GPLv2+ license if they fundamentally disagree with the direction.

### Self-Hosted GitLab CE

**Decision:** Use self-hosted GitLab CE as the primary code hosting platform with GitHub as a read-only mirror.

**Context:** Aligns with Spike's philosophy of sovereignty and self-reliance. GitHub is convenient but controlled by Microsoft. Self-hosting ensures the project's code, issues, and history remain on infrastructure the project controls.

GitHub mirror exists for discovery — most open source contributors discover projects on GitHub. The mirror README directs contributors to the primary GitLab instance.

### Monorepo

**Decision:** Keep all Spike components in a single repository.

**Context:** Spike Shell, the installer, the rescue tool, configuration tooling, and branding are tightly coupled. Splitting them across repos creates synchronization problems, complex CI pipelines, and version mismatches. A monorepo keeps everything in sync.

Spike Plus is not a separate repo. It is a build configuration (`build/iso-build/spike-plus.conf`) within the same monorepo. Same codebase, different defaults.

### Phased Contribution Model

**Decision:** Open contributions gradually as the project matures.

| **Phase** | **Contributions** |
| :-: | :-: |
| Pre-alpha | Public repo read-only. Issues open for bug reports and hardware registry. |
| Alpha | Bug reports, hardware testing, translations, code patches case-by-case with DCO. |
| Beta | Full open contributions. DCO required. Code review required. Branch protection enforced. |

**Rationale:** Opening code contributions too early leads to architectural churn, inconsistent code quality, and burnout from review burden. Phasing in contributions lets the project establish conventions before accepting external code.

### 8-Month Alpha, 18-Month Beta Timeline

**Decision:** Target 8 months to alpha, 18 months to beta.

**Context:** Solo development with a comprehensive scope. The timeline is aggressive but achievable with docs-first methodology reducing rework. Alpha includes core functionality (boot, desktop, installer, basic applets). Beta includes polish, broad hardware testing, translations, and the full settings panel.

## What This Document Is Not

This document does not prescribe implementation details. It captures decisions and rationale. For implementation specifics, see:

- **KERNEL.md** — Kernel configuration, boot parameters, module blacklisting, GPU driver configuration 

- **MEMORY.md** — ZRAM, swap, earlyoom, detection algorithms 

- **DESKTOP.md** — Spike Shell component specifications 

- **INSTALLER.md** — Installer flow, partitioning, data backup/restore 

- **BOOT-PROCESS.md** — Boot sequence, GRUB configuration, failure handling 

- **MULTIMEDIA.md** — PipeWire configuration, VA-API, Firefox tuning, NVIDIA video limitations 

- **NETWORKING.md** — NetworkManager, firmware, VPN, firewall 

- **VARIANT-DIFFERENCES.md** — Complete comparison of Spike Standard and Spike Plus 

This document will be updated as new decisions are made. Every architectural choice that affects users, contributors, or maintainers should be recorded here with its rationale.

🐕 BigRangaTech

