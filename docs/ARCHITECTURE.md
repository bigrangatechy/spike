# Spike Architecture

## Overview

Spike is a Linux distribution built on Ubuntu Server LTS with a custom desktop environment called Spike Shell. It targets older hardware — primarily Celeron-class laptops with 4GB of RAM — and adapts its configuration to the hardware it detects at install time.

Spike ships as a single ISO that supports two variants: Spike Standard (optimized for 4GB RAM) and Spike Plus (optimized for 8GB+ RAM). The installer detects hardware capabilities and recommends the appropriate variant.

### Spike System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Spike Shell (Custom)                 │
│  ┌─────────┐ ┌─────────┐ ┌──────────┐ ┌─────────────┐ │
│  │ Panel   │ │ Launcher│ │ Notify   │ │ Settings    │ │
│  │ +Applets│ │ (Kickoff│ │ Daemon   │ │ (Custom+KCM)│ │
│  │         │ │  style) │ │          │ │             │ │
│  └────┬────┘ └────┬────┘ └────┬─────┘ └──────┬──────┘ │
│       │           │           │               │        │
│       └───────────┴───────────┴───────────────┘        │
│                         │                               │
├─────────────────────────┼───────────────────────────────┤
│                    KWin (Wayland)                        │
│                    + XWayland                            │
├─────────────────────────────────────────────────────────┤
│              KDE Standalone Applications                 │
│  Discover · Dolphin · Konsole · Kate · Ark · Spectacle  │
├─────────────────────────────────────────────────────────┤
│                  Flatpak Runtime                         │
│              (Pre-seeded KDE + GNOME)                    │
├─────────────────────────────────────────────────────────┤
│               Ubuntu Server LTS (x86_64)                 │
│  systemd · NetworkManager · PipeWire · ufw · AppArmor   │
├─────────────────────────────────────────────────────────┤
│                    Linux Kernel                          │
│            (Ubuntu LTS, module blacklisting)             │
└─────────────────────────────────────────────────────────┘
```

## Base System

### Ubuntu Server 26.04 LTS

Spike is built on **Ubuntu Server 26.04 LTS** (Resolute Raccoon), not Ubuntu Desktop. This provides:

- A clean minimal starting point with no desktop bloat 

- No Snap pre-installed 

- A current kernel and Mesa graphics stack (critical for Wayland on older GPUs) 

- 10-year extended support (ESM) per LTS release 

- Massive package ecosystem including PPAs for specialized needs 

- Automatic security patching through unattended-upgrades 

### What Is Removed

The following Ubuntu Server components are stripped during first-boot configuration:

- `cloud-init` (cloud provisioning, irrelevant for laptops) 

- `snapd` (if present) 

- `landscape-common` (Canonical telemetry/client) 

- `ubuntu-advantage-tools` (Pro nagging infrastructure) 

- Release upgrade prompts (locked to LTS) 

### What Is Kept

- `apt` package management 

- `systemd` init system 

- NetworkManager (network backend) 

- AppArmor (mandatory access control) 

- `unattended-upgrades` (automatic security patches) 

- `systemd-resolved` (DNS) 

- `ubuntu-drivers` (driver management tool for optional proprietary drivers) 

## Architecture

Spike supports **x86_64 only**. There are no plans for 32-bit support unless a separate conditional project demonstrates sufficient need and the maintenance burden of forking i386 libraries is justified.

## Display Stack

### Wayland With XWayland

Spike uses Wayland as its native display protocol with XWayland providing backward compatibility for X11 applications.

- Native Wayland applications get security and efficiency benefits 

- Legacy X11 applications run through XWayland translation layer 

- XWayland only spins up when an X11 app launches — zero idle overhead 

- Tested and validated on ThinkPad P50 (dual GPU) and AMD A4 Lenovo (weak hardware) 

### KWin As Compositor

KWin is used as the window manager and compositor, running in a standalone Wayland session without the full Plasma desktop shell.

- Maintained independently from Plasma 

- Excellent Wayland support 

- Supports theming for Spike visual identity 

- Can run without Akonadi, Baloo, or KIO slaves 

**What KWin runs without:**

- Akonadi (PIM/email indexing — never started) 

- Baloo (file indexer — never started) 

- KIO slaves (file access abstraction — not required for standalone apps) 

- `plasma-session` (Plasma shell — Spike Shell replaces it) 

- `plasma-workspace` (desktop workspace — not installed) 

## Spike Shell

Spike Shell is the custom desktop environment. It is the primary development effort of the project and the component that defines the user experience.

### Component Architecture

```
Spike Shell:
├── Panel
│   ├── Layout manager (left/center/right zones)
│   └── Applet host (loads and manages tray applets)
├── Launcher (Kickoff-style)
│   ├── Desktop file scanner (.desktop files)
│   ├── Category model
│   ├── Search/filter
│   ├── Recently installed tracker (new-app badges)
│   └── Favorites store
├── Tray Applets (14 total, 4 conditional)
│   ├── network/          → NetworkManager DBus
│   ├── volume/           → PipeWire DBus
│   ├── battery/          → UPower DBus
│   ├── brightness/       → /sys/class/backlight
│   ├── notifications/    → Internal notify daemon
│   ├── removable/       → udisks2 DBus
│   ├── bluetooth/       → BlueZ DBus (conditional)
│   ├── airplane/        → rfkill (conditional)
│   ├── keyboard-layout/ → libinput (conditional)
│   ├── night-light/     → KWin gamma
│   ├── update-notifier/ → apt + Flatpak polling
│   ├── clock/           → systemd-timesyncd
│   ├── session-menu/    → systemd loginctl
│   └── (4th conditional: determined during development)
├── Notification Daemon
│   ├── DBus listener (org.freedesktop.Notifications)
│   ├── Popup renderer (Qt, Spike-themed)
│   ├── History persistence (disk-before-display, atomic writes)
│   ├── History viewer (grouped by day, searchable)
│   └── Tray badge (unread count, never auto-clears)
├── Settings Panel
│   ├── Custom pages (appearance, notifications, network, memory, boot, …)
│   ├── KCM module loader (display, sound, power, bluetooth, printer — standalone pkgs only)
│   ├── Unified Spike-themed window (same process as spike-shell)
│   └── Context-aware help (links to user guide sections)
├── Session Manager
│   ├── Login/logout/shutdown/suspend
│   ├── Autostart management
│   └── Boot failure counter
└── Theme Engine
    ├── Purple/cyan color scheme
    ├── Qt stylesheet
    ├── KWin window decoration config
    └── Icon set
```

### Design Principles

- All applets talk to system services via stable DBus interfaces 

- No dependency on Plasma internals or `plasma-workspace` 

- Visual identity is consistent across all custom components 

- Every user-adjustable setting has a GUI — no config file editing 

- The terminal is never required for normal system use 

### Performance Budget

```
Spike Shell memory targets (Spike Standard, idle):
├── Panel + applets:          ~60-80MB
├── Notification daemon:      ~5-8MB
├── KWin (Wayland):           ~100-150MB
├── System + kernel:          ~80-100MB
├── Display server overhead:  ~40-60MB
├── Total idle target:        <400MB
└── Available for apps:       ~3.5GB
```

## KDE Standalone Applications

Spike uses KDE applications that run independently of the Plasma desktop shell. These are Qt applications that share KDE Frameworks libraries but do not require Plasma services.

### Bundled Applications

| **Application** | **Purpose** | **Why This One** |
| :-: | :-: | :-: |
| Discover | Software center | Handles both apt and Flatpak, beginner-friendly |
| Dolphin | File manager | Most polished Qt file manager, themable |
| Konsole | Terminal | For power users who want it, never required |
| Kate | Text editor | Lightweight, capable, KDE-integrated |
| Ark | Archive tool | Handles zip, tar, etc. via file associations |
| Spectacle | Screenshots | Keyboard-driven, saves to clipboard or file |
| KCalc | Calculator | Simple, sufficient |
| KDE Settings KCMs (selected) | Configuration | Standalone packages only, hosted inside Spike Settings |

### What Is Not Installed

- Akonadi-dependent applications (KMail, KOrganizer) 

- Baloo-dependent file search tools 

- Plasma Workspace components 

- Any application that requires `plasma-session` 

## Software Management

### Flatpak via Discover

User-installed applications come through Flatpak, managed by Discover.

- Discover handles both system packages (apt) and user applications (Flatpak) 

- Users search, click install, and apps appear in the launcher 

- No terminal needed, no repository jargon 

### Pre-Seeded Runtimes

Common Flatpak runtimes are pre-installed on the ISO to avoid large downloads on first app install:

**Spike Standard (selective):**

```
├── KDE Flatpak runtime
└── GNOME Flatpak runtime
```

**Spike Plus (all common):**

```
├── KDE Flatpak runtime
├── GNOME Flatpak runtime
├── freedesktop runtime
└── Additional common runtimes
```

### System Packages

Core system packages come from Ubuntu LTS repositories:

- Security patches via `unattended-upgrades` (automatic) 

- Non-security updates via Discover notification 

- KDE standalone apps maintained by Ubuntu/Debian packagers 

- No custom package repository required 

## Filesystem

### Partitioning

All installations use ext4. No Btrfs, no LVM, no manual partitioning.

**UEFI systems:**

```
├── /boot/efi  (512MB, FAT32)
├── /          (rest of disk, ext4)
└── /swapfile  (8GB, on root)
```

**BIOS systems:**

```
├── /boot      (1GB, ext4)
├── /          (rest of disk, ext4)
└── /swapfile  (8GB, on root)
```

### Mount Options

Adaptive based on detected storage type:

| **Storage Type** | **Mount Options** | **Swap Size** | **Swappiness** |
| :-: | :-: | :-: | :-: |
| SATA SSD | `defaults,noatime` | 8GB | 15 |
| NVMe SSD | `defaults,noatime` | 8GB | 15 |
| SATA HDD | `defaults,noatime,commit=60` | 8GB | 5 |
| SD card | `defaults,noatime,commit=60,discard` | 4GB | 10 |
| USB 3.0 drive | `defaults,noatime,commit=60,discard` | 4GB | 10 |

### Storage Support

- Minimum 128GB 

- Supported: SATA SSD, NVMe, SATA HDD, SD card (UHS-I+), USB 3.0 drive 

- Not supported: eMMC (reliability concerns, wear-out risk) 

- No encryption, no dual boot, no manual partitioning 

## Memory Management

### Adaptive Configuration

Memory management is configured at install time based on detected hardware:

**Detection:**

```
├── CPU cores and bogomips → classify as capable or low-end
├── Storage type → SSD, HDD, SD, USB
└── RAM amount → swap sizing
```

**If capable CPU:**

```
├── Enable ZRAM (zstd, equal to RAM, max 4GB)
├── ZRAM priority: 100
├── Swap file: 8GB, priority: 10
└── Swappiness: per storage type
```

**If low-end CPU:**

```
├── Skip ZRAM (avoid CPU bottleneck)
├── Swap file: 8GB
└── Swappiness: per storage type
```

**Safety valve:**

```
└── Earlyoom kills largest process before OOM
```

### Memory Ladder

```
Application requests memory
        │
        ▼
   Physical RAM (4GB)
        │ full
        ▼
   ZRAM compressed (4GB → ~8-10GB effective)
        │ full (or skipped on weak CPU)
        ▼
   Swap file on disk (8GB)
        │ full
        ▼
   Earlyoom kills largest process
```

Total effective memory on a 4GB Celeron with ZRAM: approximately 12-14GB before intervention.

## Multimedia

### Audio

- PipeWire (audio server) + WirePlumber (session manager) 

- Default sample rate: 44100Hz (saves CPU on weak processors) 

- Stereo-only default 

- Bluetooth audio: conditional on hardware detection (BlueZ started only if BT present) 

- No JACK, no pro audio features 

- Estimated overhead: 25-40MB RAM 

### Video

- VA-API hardware video decode via adaptive driver selection 

- Intel GPUs: `intel-media-va-driver-non-free`, `LIBVA_DRIVER_NAME=iHD` 

- AMD GPUs: `mesa-va-drivers` with `radeonsi` 

- NVIDIA GPUs: nouveau by default (no VA-API). Proprietary driver optional (see GPU Driver Configuration below) 

- No hardware decode: fallback to software, lower Firefox defaults, warn user 

- AV1 disabled in Firefox to force VP9 (hardware decode on Gemini Lake+) 

- Firefox Flatpak pre-configured with VA-API enabled 

- `gstreamer-vaapi` installed system-wide for KDE app video support 

## GPU Driver Configuration

### Intel (Primary Target)

The Celeron N4020 uses Intel UHD Graphics 600 (Gemini Lake Refresh). Driver configuration:

```
Kernel module:    i915 (in-tree, loaded via modeset=1)
Userspace:        mesa (iris driver stack)
VA-API driver:    intel-media-va-driver-non-free
LIBVA_DRIVER_NAME: iHD
```

The `iHD` driver covers Gen 8+ Intel graphics (Broadwell through Gemini Lake). The older `i965` driver is not used.

### AMD (Secondary Target)

AMD A4/A6/A9 (Jaguar architecture and newer):

```
Kernel module:    amdgpu (in-tree, loaded via modeset=1)
Userspace:        mesa (radeonsi driver stack)
VA-API driver:    mesa-va-drivers
LIBVA_DRIVER_NAME: radeonsi
```

### NVIDIA (Hybrid/Discrete GPU Support)

Spike ships the open-source nouveau driver by default. The proprietary NVIDIA driver is available as an optional installation for users who need it.

**Default: nouveau (included on ISO)**

- Works out of the box for basic display output and compositing 

- No licensing restrictions — safe to distribute 

- Lower memory footprint than proprietary driver 

- Limitation: No VA-API hardware video decode on most NVIDIA GPUs 

- Wayland support is functional but less mature than Intel/AMD 

**Optional: NVIDIA proprietary driver (user-installed)**

- Available for GeForce GTX 900+ and newer GPUs 

- Requires internet connection at install time 

- Includes VA-API video acceleration (on supported GPU generations) 

- Cannot be pre-seeded on ISO due to NVIDIA EULA redistribution restrictions 

- Increases ISO size if pre-installed (excluded for space and licensing reasons) 

**Driver selection at install time:**

```
If PCI device vendor == Intel:
    apt install intel-media-va-driver-non-free
    echo "LIBVA_DRIVER_NAME=iHD" >> /etc/environment
If PCI device vendor == AMD:
    apt install mesa-va-drivers
    echo "LIBVA_DRIVER_NAME=radeonsi" >> /etc/environment
If PCI device vendor == NVIDIA:
    Load nouveau by default
    Do NOT install proprietary driver automatically
    Create post-install notification:
        "NVIDIA hardware detected. The open-source driver is active.
         If you need hardware video encoding, CUDA, or gaming
         performance, install the NVIDIA driver from
         Settings → Software Sources → Additional Drivers."
```

**Installation flow for proprietary driver:**

```
1. NVIDIA hardware detected at boot via udev
2. nouveau loads by default — display works immediately
3. Post-install notification informs user of optional proprietary driver
4. User navigates to Settings → Software Sources → Additional Drivers
5. ubuntu-drivers tool lists compatible NVIDIA driver versions
6. User selects and installs
7. nouveau is blacklisted, nvidia-drm is configured
8. Reboot required (gentle notification, never forced)
```

### Hybrid Graphics (NVIDIA Optimus)

Laptops with both Intel/AMD integrated graphics and NVIDIA discrete GPU are supported:

```
Settings → Power Management → Graphics
├── Integrated Only (default) — Uses only Intel/AMD iGPU, saves battery
├── Hybrid Mode — Discrete GPU activates on-demand for intensive tasks
└── Discrete Only — Uses NVIDIA GPU only (higher power consumption)
```

Hybrid mode uses PRIME render offload. The discrete GPU stays powered down unless an application explicitly requests it. This is the recommended mode for battery-powered laptops.

**Spike Standard recommendation:** Use "Integrated Only" for best battery life and lowest memory usage. Switch to Hybrid only if specific applications require the discrete GPU.

**Spike Plus recommendation:** Hybrid mode is the default on Plus, as the additional RAM and CPU headroom make on-demand GPU switching seamless.

### Known NVIDIA Limitations

| **Issue** | **Mitigation** |
| :-: | :-: |
| nouveau lacks hardware video acceleration | Use integrated GPU for video playback; disable discrete NVIDIA GPU |
| Wayland support is experimental on nouveau | Falls back to XWayland automatically |
| Secure Boot blocks unsigned modules | MOK enrollment required for proprietary driver (installer handles if needed) |
| Fermi-era GPUs (pre-GTX 600) unsupported by modern proprietary driver | Use nouveau only; legacy proprietary branch is unmaintained |

Users with NVIDIA hardware are encouraged to report their experience through the hardware registry at `spike.bigrangatech.com/hardware/` so the Spike team can improve support over time.

## Networking

- Backend: NetworkManager (included with Ubuntu Server) 

- GUI: Custom Spike Shell network tray applet + **Settings → Network** (NetworkManager D-Bus; not plasma-nm)

- All major Wi-Fi firmware vendors shipped on ISO (Intel, Atheros, Realtek, Broadcom) 

- Firmware dynamically selected at install based on detected hardware 

- VPN: OpenVPN + WireGuard via NetworkManager 

- Hotspot: supported, exposed in network applet 

- Mobile broadband: conditional on modem detection 

- Bluetooth: conditional on hardware detection (BlueZ started only if present) 

- DNS: `systemd-resolved` (Ubuntu default) 

- Firewall: `ufw` enabled, deny incoming by default 

## Boot Process

### Bootloader

- GRUB2 (migration to Limine planned for future) 

- GRUB2 themed with Spike branding (purple/cyan, Spike logo) 

- Boot menu hidden by default 

- ESC during 3-5 second window shows menu 

- Recovery entry always present when menu appears 

- 3 consecutive boot failures → menu appears automatically with recovery highlighted 

- Module blacklisting generated at install time based on detected hardware 

### Boot Sequence

```
Power on
    │
    ▼
GRUB2 (hidden, 3-5 sec ESC window)
    │
    ▼
Kernel + initramfs
    │  (module blacklist applied)
    ▼
systemd init
    │
    ├── Mount filesystems (ext4, adaptive mount flags)
    ├── Start core services (NetworkManager, PipeWire, ufw)
    ├── Start Spike Shell session
    │   ├── KWin (Wayland)
    │   ├── Panel + applets
    │   ├── Notification daemon
    │   └── Settings (on-demand)
    ├── Clear boot failure counter (successful boot)
    └── Desktop ready
```

## Security

### AppArmor

- Enabled with Ubuntu default profiles 

- No custom profiles (maintenance burden not justified) 

- Flatpak sandboxing handles application isolation independently 

### Firewall

- `ufw` enabled by default 

- Deny all incoming connections 

- Allow all outgoing connections 

- Invisible to user, no configuration needed 

- Protects beginners on public Wi-Fi (coffee shops, libraries, schools) 

### Updates

- Security updates: automatic, background, when system is idle 10+ minutes 

- Non-security updates: notify via Discover, user initiated 

- Flatpak updates: notify via Discover, user initiated 

- No forced reboots — gentle notification when restart needed 

- Update checks delayed if system under memory pressure 

### Privacy

- No telemetry 

- No data collection 

- Network connections: NTP, package mirrors, Flatpak repositories — nothing else 

- User has control over all network behavior through Settings 

## Installer

### User Interaction

```
1. Language selection
2. Timezone selection
3. Wi-Fi connection
4. Username and password
5. Computer name (hostname, auto-generated suggestion: spike-laptop)
6. Variant selection (installer recommends based on detected hardware)
7. Data backup to USB (optional, scans for personal files)
8. Disk wipe confirmation
9. Install (automated, progress bar)
10. Reboot to desktop
```

### Automated Steps

- Hardware detection (CPU, storage, RAM, GPU, Bluetooth, modem) 

- Filesystem creation and mount option configuration 

- Memory subsystem configuration (ZRAM, swap, swappiness) 

- GRUB2 installation and theme application 

- Module blacklist generation 

- Flatpak runtime pre-seeding 

- Security configuration (AppArmor, ufw, unattended-upgrades) 

- GPU driver selection (Intel/AMD: automatic. NVIDIA: nouveau default, proprietary optional) 

- User account creation and system settings 

### Variant Selection

The installer detects hardware and recommends a variant:

```
If RAM >= 8GB and CPU is modern dual-core+:
    → Recommend Spike Plus
If RAM <= 4GB or CPU is Celeron-class:
    → Recommend Spike Standard
```

User can override recommendation.

### Data Rescue

- Installer scans target drive for personal files (Windows/Linux/Mac user directories) 

- Offers backup to USB before wiping 

- Verification-backed file copy (checksum comparison) 

- Preserves folder structure 

- Handles partial corruption gracefully 

### Recovery Loop

- Spike Rescue tool runs from live ISO 

- Mounts broken Spike installation read-only 

- Scans for user data 

- Copies to USB with verification 

- Installer detects existing Spike installation 

- Offers "Fresh install" or "Fresh install and restore my data" 

- Restores user files after reinstall with correct ownership 

## Build System

### Engine: live-build (one ISO)

Spike ISOs are built with **live-build**, wrapped by `scripts/build-iso.sh`. The recipe lives under `build/iso-build/`.

```
build/iso-build/
├── auto/                 → live-build auto/* helpers
├── config/               → package lists, hooks, includes
└── README.md             → Recipe overview
```

**Build (single artifact):**

```
./scripts/build-iso.sh
```

There is **no** `--variant standard|plus` build flag. One hybrid live ISO ships both variants’ *capability*; the installer applies Standard or Plus at install time via `spike-config` (see `VARIANT-DIFFERENCES.md`, `INSTALLER.md`).

**Live session:** Boot ISO → Spike Shell (read-only live) → “Install Spike” or Spike Rescue.

**Installer:** Custom Qt Widgets (`spike-installer`) — not Calamares. Live-build only produces the live environment and squashfs; install UX is Spike-owned.

See: `docs/dev-guide/03-build-environment.md`, `docs/dev-guide/04-building-spike.md`, `docs/dev-guide/07-installer-internals.md`.

### Configuration Differences (install-time, not build-time)

| **Setting** | **Spike Standard** | **Spike Plus** |
| :-: | :-: | :-: |
| Target RAM | 4GB | 8GB+ |
| ZRAM | Adaptive (CPU-dependent) | Always enabled |
| Swap | 8GB fixed | Match RAM |
| Animations | Off by default | On by default |
| Compositor effects | Minimal | Blur, transparency, fade |
| Background services | Aggressively stripped | Standard level |
| Flatpak runtimes | Selective pre-seed | All pre-seeded |
| Idle RAM target | <400MB | <800MB |
| Boot time target | <40 seconds | <30 seconds |
| Module blacklisting | Aggressive | Conservative |
| Plymouth theme | Minimal (static) | Full (animated) |
| NVIDIA default mode | Integrated Only | Hybrid Mode |
| CPU governor | `powersave` | `schedutil` |

Authoritative list: `VARIANT-DIFFERENCES.md` (14 differences). Flatpak pre-seed on the **ISO** includes what both variants need; Plus may enable additional runtimes/config at install without a second ISO.
### ISO Contents

Approximate ISO size: ~3.1GB

```
├── Ubuntu Server base:          ~800MB
├── KDE standalone apps:          ~400MB
├── Spike shell + config:         ~50MB
├── Flatpak runtimes (pre-seeded): ~1.2GB
├── Firmware and drivers:         ~300MB
├── VA-API drivers:               ~50MB
├── Bluetooth/modem/VPN:          ~80MB
├── Branding and Plymouth:       ~20MB
└── Live environment:             ~200MB
```

Note: NVIDIA proprietary driver is NOT included on the ISO. It is available as an optional post-install download. nouveau (open-source) is included in the kernel.

## Infrastructure

### Repository

- Primary: `git.bigrangatech.com:Ranga/spike.git` (GitLab CE, self-hosted) 

- Mirror: `github.com/bigrangatech/spike` (read-only) 

- CI/CD: GitLab CI (`.gitlab-ci.yml`) 

- Website: `spike.bigrangatech.com` (GitLab Pages) 

- Issue tracking: GitLab CE only 

- Merge requests: GitLab CE only, DCO required 

### License

- Code: GPLv2+ 

- Documentation: CC-BY-SA 4.0 

- Artwork (logo, splash, themes): CC-BY-SA 4.0 

