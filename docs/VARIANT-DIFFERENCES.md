# Spike Variant Differences

## Purpose

This document is the definitive, exhaustive comparison between Spike Standard and Spike Plus. It is referenced by every other document in the Spike documentation suite. If a difference between Standard and Plus exists, it is listed here. If it is not listed here, it does not exist.

## Design Philosophy

Spike Standard and Spike Plus are **not** different editions, different products, or different target audiences.

They are the **same operating system** with different resource allocations.

```
Same ISO. Same codebase. Same installer. Same shell.
Same user experience. Same philosophy. Same golden rules.
```

The only difference is how aggressively Spike conserves resources. Standard tightens the belt. Plus loosens it.

A Standard user and a Plus user should feel like they're using the same operating system. Because they are.

The installer recommends a variant based on detected hardware. The user can override. If they choose Plus on inadequate hardware, a warning is shown but the choice is respected.

## Quick Reference Table

| **Category** | **Spike Standard** | **Spike Plus** |
| :-: | :-: | :-: |
| Target hardware | Celeron/Pentium, 4GB RAM | Modern i3/i5/i7/Ryzen, 8GB+ RAM |
| Idle RAM target | <400MB | <800MB |
| Animations | Off | On |
| Compositor effects | Minimal (no blur, no transparency) | Full (blur, transparency, fade) |
| Plymouth boot splash | Minimal (static logo + progress dots) | Full (animated logo + progress bar) |
| CPU governor | powersave | schedutil |
| ZRAM cap | 4GB max | Uncapped (up to RAM size) |
| Flatpak runtimes pre-seeded | KDE + GNOME | All common runtimes |
| Bluetooth codecs | SBC only | SBC, AAC, LDAC, aptX |
| Applet polling | Conservative intervals | Standard intervals |
| Qt rendering | Software rasterizer (if GPU weak) | OpenGL (GPU-accelerated) |
| Spike button hover | No glow effect | Subtle glow effect |
| Panel height | 32px (can increase to 48px) | 32px (can increase to 48px) |
| Background services | Minimal set | Standard set |

Everything **not** listed in this table is **identical** between variants.

## Detailed Differences

### 1. Memory Management

#### ZRAM

**Spike Standard:**

```
├── ZRAM enabled (if CPU capable: bogomips ≥ ~2200/core, dual-core+)
├── Compression: zstd
├── Disksize: min(RAM, 4096) — capped at 4GB
├── Priority: 100
└── If RAM is 4GB: ZRAM is 4GB (1:1 ratio)
```

**Spike Plus:**

```
├── ZRAM enabled (if CPU capable: bogomips ≥ ~2200/core, dual-core+)
├── Compression: zstd
├── Disksize: min(RAM, unlimited) — no cap
├── Priority: 100
└── If RAM is 8GB: ZRAM is 8GB (1:1 ratio)
    If RAM is 16GB: ZRAM is 16GB (1:1 ratio)
```

**Rationale:**

```
├── Standard caps at 4GB to prevent ZRAM consuming too much
│   of the 4GB total RAM (leaves room for actual processes)
├── Plus has no cap because 8GB+ systems have headroom
└── ZRAM compression ratio is typically 3:1 to 4:1 with zstd
    (4GB ZRAM uses ~1-1.3GB of actual RAM)
```

#### Swap File

**Spike Standard:**

```
├── Swap file size: 8GB (SSD/HDD), 4GB (SD/USB)
├── Priority: 10 (lower than ZRAM's 100)
├── Swappiness: SSD=15, HDD=5, SD/USB=10
├── Swappiness slider: SSD only, range 15-60, increase only
└── Earlyoom: 10% threshold, protects shell essentials
```

**Spike Plus:**

```
├── Swap file size: 8GB (SSD/HDD), 4GB (SD/USB)
├── Priority: 10 (lower than ZRAM's 100)
├── Swappiness: SSD=15, HDD=5, SD/USB=10
├── Swappiness slider: SSD only, range 15-60, increase only
└── Earlyoom: 10% threshold, protects shell essentials
```

**Wait — swap is IDENTICAL between variants?**

```
├── Yes. Swap configuration is based on storage type, not variant
├── ZRAM is the primary swap device (priority 100)
├── Swap file is secondary fallback (priority 10)
├── The only difference is ZRAM cap size (above)
└── Swap file size, swappiness, and earlyoom are the same
```

#### Memory Budget

**Spike Standard (<400MB idle target):**

```
Component                    Memory (approximate)
─────────────────────────────────────────────────────
Kernel + initramfs residual  ~80-120MB
systemd + core services      ~30-40MB
KWin (Wayland compositor)    ~100-150MB
Spike Shell (panel + applets)~40-55MB
Notification daemon           ~5-8MB
PipeWire + WirePlumber       ~8-12MB
NetworkManager               ~5-8MB
earlyoom                     ~2-3MB
udev + DBus                  ~5-8MB
Plymouth (during boot)       ~5-8MB (freed after boot)
─────────────────────────────────────────────────────
Total idle                   ~280-370MB
Headroom for user apps       ~3,630-3,720MB (of 4,096MB)
```

**Spike Plus (<800MB idle target):**

```
Component                    Memory (approximate)
─────────────────────────────────────────────────────
Kernel + initramfs residual  ~80-120MB
systemd + core services      ~40-50MB (more services enabled)
KWin (Wayland compositor)    ~120-180MB (effects enabled)
Spike Shell (panel + applets)~45-60MB
Notification daemon           ~5-8MB
PipeWire + WirePlumber       ~10-15MB (more codecs)
NetworkManager               ~5-8MB
earlyoom                     ~2-3MB
udev + DBus                  ~5-8MB
Plymouth (during boot)       ~8-12MB (freed after boot)
Additional caching           ~100-200MB (more aggressive)
─────────────────────────────────────────────────────
Total idle                   ~420-650MB
Headroom for user apps       ~7,350-7,580MB (of 8,192MB)
```

### 2. CPU and Performance

#### CPU Governor

**Spike Standard:**

```
├── Governor: powersave
├── Behavior: CPU stays at lowest stable frequency
├── Turbo boost: Accessed under load (hardware-managed)
├── Rationale: Battery life prioritized over burst performance
├── Celeron N4020 turbo: 2.80GHz (accessed under load even in powersave)
└── User can override in Settings → Power → CPU governor
```

**Spike Plus:**

```
├── Governor: schedutil
├── Behavior: Frequencies scale with scheduler load hints
├── Faster ramp-up than powersave
├── Rationale: Better balance of performance and efficiency
├── More capable CPUs benefit from faster frequency transitions
└── User can override in Settings → Power → CPU governor
```

**Both variants:**

```
├── Performance mode available as temporary override (AC only)
├── Performance mode forces highest frequency
├── Reverts to variant default when profile changes
└── Critical battery mode forces powersave (both variants)
```

#### Thermal Management

**Spike Standard:**

```
├── Conservative thermal envelope
├── Fans (if present) managed by firmware
├── CPU throttling: hardware-managed (Spike doesn't override)
├── Temperature monitored in Diagnostics
└── Warning at sustained 80°C+
```

**Spike Plus:**

```
├── Same thermal management as Standard
├── Slightly higher heat output (more services, GPU effects)
├── Fans (if present) managed by firmware
├── CPU throttling: hardware-managed
├── Temperature monitored in Diagnostics
└── Warning at sustained 80°C+
```

Thermal management is **identical** between variants. The hardware handles thermal limits; Spike respects them.

### 3. Desktop Shell

#### Animations

**Spike Standard:**

```
├── Window open/close: Instant (no animation)
├── Workspace switch: Instant (no slide)
├── Panel show/hide (auto-hide): Instant
├── Launcher open: Instant (no fade)
├── Notification popup: Instant (no slide-in)
├── Tooltip: Instant (no fade)
├── Spike button hover: No glow effect
└── Rationale: Every frame of animation costs GPU/CPU cycles
    that could be used for user applications
```

**Spike Plus:**

```
├── Window open/close: Fade + scale animation (150ms)
├── Workspace switch: Slide animation (200ms)
├── Panel show/hide (auto-hide): Slide animation (200ms)
├── Launcher open: Fade-in animation (150ms)
├── Notification popup: Slide-in animation (200ms)
├── Tooltip: Fade animation (100ms)
├── Spike button hover: Subtle purple glow effect
└── Rationale: Animations improve perceived smoothness
    and polish on capable hardware
```

#### Compositor Effects

**Spike Standard (KWin configuration):**

```
├── Blur: Disabled
├── Transparency: Disabled (panels and popups opaque)
├── Shadows: Disabled
├── Desktop cube/effects: Disabled
├── Color correction: Disabled
├── Fall back to software rasterizer if GPU weak
└── Rendering backend: QPainter (software) or OpenGL (if GPU supports)
```

**Spike Plus (KWin configuration):**

```
├── Blur: Enabled (behind panels, popups, translucent windows)
├── Transparency: Enabled (panel, notification popups)
├── Shadows: Enabled (windows, popups)
├── Desktop cube/effects: Disabled (unnecessary overhead)
├── Color correction: Enabled (for Night Light accuracy)
├── Rendering backend: OpenGL (GPU-accelerated)
└── If GPU doesn't support required GL features: fall back to Standard effects
```

#### Qt Rendering Backend

**Spike Standard:**

```
├── Primary: QPainter (software rasterizer)
├── Used when: GPU is Intel UHD 600 (Gemini Lake) or weaker
├── Fallback: If QPainter fails, try OpenGL
├── Advantage: Predictable rendering, no GPU dependency
├── Disadvantage: Slightly higher CPU usage for UI rendering
└── Selected when: hardware classification = "low-end" or "capable" with weak GPU
```

**Spike Plus:**

```
├── Primary: OpenGL
├── Used when: GPU supports OpenGL 2.1+ (most modern GPUs)
├── Fallback: QPainter (software) if OpenGL unavailable
├── Advantage: GPU handles UI rendering, frees CPU
├── Disadvantage: Uses GPU memory and cycles
└── Selected when: hardware classification = "modern" with capable GPU
```

Selection is automatic at install time based on GPU detection. Users can override in **Settings → Appearance → Advanced → Rendering backend**. Override is discouraged (defaults are tuned for the hardware).

#### Panel and Applets

**Both variants:**

```
├── Same panel layout (left, center, right zones)
├── Same applet set (14 applets, 4 conditional)
├── Same launcher (categories, search, favorites, recently used)
├── Same settings panel (same pages, same KCM modules)
├── Same notification daemon (disk-before-display invariant)
├── Same session manager (same login/logout flow)
├── Same theme engine (same colors, same fonts, same wallpapers)
├── Panel height: 32px default (adjustable 24-48px on both)
└── Panel position: Bottom (default) or Top (both variants)
```

**The only shell differences:**

```
├── Animation presence (Standard: none, Plus: yes)
├── Compositor effects (Standard: minimal, Plus: full)
├── Spike button hover glow (Standard: no, Plus: yes)
├── Rendering backend (Standard: software preferred, Plus: OpenGL preferred)
└── Applet polling intervals (see below)
```

#### Applet Polling Intervals

**Spike Standard (conservative — less frequent polling):**

```
├── Network: Signal check every 10 seconds
├── Battery: Update every 30 seconds (or on UPower event)
├── Brightness: On-demand (hardware event-driven)
├── Update notifier: Check every 6 hours
├── Removable devices: Event-driven (udev, no polling)
├── Bluetooth: Event-driven (BlueZ, no polling)
├── Notification badge: Real-time (internal event)
├── Clock: Real-time (timer-based, every 1 second)
└── Night Light: Every 60 seconds (when active)
```

**Spike Plus (standard — more frequent polling):**

```
├── Network: Signal check every 5 seconds
├── Battery: Update every 15 seconds (or on UPower event)
├── Brightness: On-demand (hardware event-driven)
├── Update notifier: Check every 4 hours
├── Removable devices: Event-driven (udev, no polling)
├── Bluetooth: Event-driven (BlueZ, no polling)
├── Notification badge: Real-time (internal event)
├── Clock: Real-time (timer-based, every 1 second)
└── Night Light: Every 30 seconds (when active)
```

Event-driven applets (removable devices, Bluetooth) are identical between variants — they respond to hardware events, not polling. Clock and notification badge are identical (real-time). Only periodic-polling applets differ, and only slightly.

### 4. Boot Process

#### Plymouth

**Spike Standard — "spike-minimal":**

```
├── Static Spike logo centered on dark background (#1a1a2e)
├── 5 progress dots at bottom (fill left to right)
├── Software rendering (no GPU dependency)
├── Memory footprint: ~5-8MB
├── Fade to black at end (SDDM fades in from black)
└── No animation beyond progress dots filling
```

**Spike Plus — "spike-full":**

```
├── Animated Spike logo (subtle pulsing glow)
├── Animated progress bar (smooth fill, left to right)
├── GPU-accelerated rendering (DRM)
├── Memory footprint: ~8-12MB
├── Smooth fade to SDDM login screen
└── Requires KMS and GPU with basic 2D acceleration
```

**Both variants:**

```
├── Same Spike logo (identical asset)
├── Same dark background color
├── Same duration (matches actual boot time)
├── If GPU/DRM fails: both fall back to "spike-text" (text-based)
└── Plymouth freed from memory after boot (not persistent)
```

#### GRUB and Boot Sequence

**Identical between variants:**

```
├── Same GRUB2 bootloader
├── Same boot parameters (quiet, splash, zswap.enabled=0, THP=madvise)
├── Same hidden menu behavior (ESC window, 3-failure trigger)
├── Same boot failure counter (/boot/.spike/boot-count)
├── Same GRUB theme (Spike Circuit)
├── Same recovery mode entry
├── Same initramfs customization (boot counter, recovery screen)
├── Same systemd service startup order
├── Same autostart filtering
├── Same session startup sequence
├── Same environment variables (except rendering backend)
├── Same SDDM theme
└── Same session manager flow
```

The **only** boot difference is the Plymouth theme. Everything else in the boot process is identical.

### 5. Multimedia

#### Bluetooth Audio Codecs

**Spike Standard:**

```
├── SBC: Enabled (universal, all BT audio devices)
├── AAC: Disabled (CPU cost of encoding on Celeron)
├── LDAC: Disabled (CPU cost, unnecessary for target hardware)
├── aptX: Disabled (CPU cost, unnecessary for target hardware)
├── Result: Universal compatibility, lowest CPU usage
└── Audio quality: Good (SBC at 328 kbps is acceptable for most use)
```

**Spike Plus:**

```
├── SBC: Enabled
├── AAC: Enabled (better quality for Apple devices)
├── LDAC: Enabled (hi-res audio for Sony and compatible devices)
├── aptX: Enabled (better quality for Qualcomm-compatible devices)
├── Result: Best available quality per device
└── Audio quality: Optimal (codec negotiated per device capability)
```

**Codec selection:**

```
├── PipeWire/WirePlumber negotiates best available codec
├── If device supports LDAC and LDAC is enabled: LDAC used
├── If device only supports SBC: SBC used (regardless of variant)
├── Codec visible in Settings → Sound → Bluetooth device properties
└── User can override codec selection per device
```

#### PipeWire Configuration

**Spike Standard:**

```
├── Sample rate: 44100Hz (fixed)
├── Channels: 2 (stereo only)
├── Resampler quality: 1 (low — saves CPU)
├── Logging: Suppressed (level 1, warnings only)
├── JACK: Not configured
└── Bluetooth codecs: SBC only
```

**Spike Plus:**

```
├── Sample rate: 44100Hz (fixed)
├── Channels: 2 (stereo only)
├── Resampler quality: 1 (low — same as Standard)
├── Logging: Suppressed (level 1, warnings only)
├── JACK: Not configured
└── Bluetooth codecs: SBC, AAC, LDAC, aptX
```

**Why resampler quality is the same:**

```
├── Quality 1 is sufficient for speech and music
├── Quality 4 (highest) adds CPU overhead even on capable CPUs
├── The difference is imperceptible on consumer speakers/headphones
├── Bluetooth codec is the quality differentiator, not resampler
└── Both variants lock to 44100Hz (prevents dynamic switching overhead)
```

#### VA-API and Video Acceleration

**Identical between variants:**

```
├── VA-API: Enabled (if GPU supports it)
├── Intel: intel-media-va-driver-non-free (LIBVA_DRIVER_NAME=iHD)
├── AMD: mesa-va-drivers (LIBVA_DRIVER_NAME=radeonsi)
├── NVIDIA: vdpau-va-driver (or proprietary via VDPAU)
├── AV1: Disabled in Firefox (Gemini Lake lacks hardware decode)
├── VP9: Hardware decoded (Gemini Lake supports VP9 decode)
├── Firefox preferences: Same on both variants
└── Screen capture: Same Portal-based system on both variants
```

Video acceleration is **not** a variant differentiator. Both variants get the same VA-API configuration. The difference is only in CPU overhead (Standard: less headroom for software decode of unsupported codecs).

### 6. Flatpak Runtimes

**Spike Standard (pre-seeded on ISO and installed system):**

```
├── org.kde.Platform (latest stable)
├── org.kde.Sdk (for development, if user installs)
├── org.gnome.Platform (latest stable)
└── org.gnome.Sdk (for development, if user installs)
```

**Spike Plus (pre-seeded on ISO and installed system):**

```
├── org.kde.Platform (latest stable)
├── org.kde.Sdk
├── org.gnome.Platform (latest stable)
├── org.gnome.Sdk
├── org.freedesktop.Platform (latest stable)
├── org.freedesktop.Sdk
├── org.gtk.Gtk3theme (common themes for GTK apps)
├── org.mozilla.Firefox.BaseApp (base runtime for Firefox)
├── org.libreoffice.LibreOffice.BaseApp (base runtime for LibreOffice)
└── io.github.spike.Platform (Spike-specific runtime, if developed)
```

**Rationale:**

```
├── Standard pre-seeds only the two most common runtimes
│   (KDE and GNOME cover ~90% of Flatpak apps)
├── Plus pre-seeds all commonly used runtimes
│   (reduces first-app-launch time for a wider range of apps)
├── Missing runtimes are downloaded on-demand when an app
│   requires them (via Discover or automatic)
└── Pre-seeding is a convenience, not a functional difference
    (both variants can install and run the same Flatpak apps)
```

### 7. Background Services

**Spike Standard (minimal service set):**

**Enabled services:**

```
├── systemd-journald           → Logging
├── systemd-udevd              → Device management
├── dbus                        → IPC
├── NetworkManager              → Network
├── systemd-resolved            → DNS
├── systemd-timesyncd           → NTP
├── ufw                         → Firewall
├── apparmor                    → Security
├── earlyoom                    → OOM prevention
├── cron                        → Scheduled tasks
├── unattended-upgrades         → Security updates
├── polkit                      → Privilege escalation
├── rtkit-daemon                → Audio real-time scheduling
├── udisks2                     → Disk management
├── upower                      → Power management
├── colord                      → Color management
├── accounts-daemon             → User accounts
├── avahi-daemon                → mDNS (printer discovery)
├── sddm                        → Display manager
└── spike-session               → Desktop session
```

**Disabled/removed services:**

```
├── snapd, cloud-init, landscape, apport, whoopsie
├── ModemManager (if no modem detected)
├── switcheroo-control (if no dual GPU)
├── kerneloops
└── motd-news
```

**Spike Plus (standard service set):**

All Standard services **plus:**

```
├── colord-sensor (if color sensor present)
├── switcheroo-control (if dual-GPU detected)
├── ModemManager (if modem detected)
└── power-profiles-daemon (alternative to manual governor mgmt)
```

The additional services in Plus:

```
├── Only enabled if corresponding hardware is detected
├── Each adds ~5-10MB RAM
├── Provide hardware-specific features (color profiles, GPU switching)
└── Not user-facing differentiators (hardware determines availability)
```

### 8. Power Management

**Spike Standard:**

```
├── CPU governor: powersave (default)
├── Screen blank: 15 min (AC), 5 min (battery)
├── Dimming: 5 min idle (AC), 3 min (battery)
├── Disk spin-down: 15 min (AC, HDD), 10 min (battery, HDD)
├── WiFi power saving: Adaptive (off on AC, on on battery)
├── USB autosuspend: Adaptive (off on AC, on on battery)
├── Suspend: Default sleep action
├── Hibernate: Available if swap ≥ RAM
├── Hybrid sleep: ON (safety net for low RAM)
└── Critical battery: powersave + dim to 30%
```

**Spike Plus:**

```
├── CPU governor: schedutil (default)
├── Screen blank: 15 min (AC), 5 min (battery)
├── Dimming: 5 min idle (AC), 3 min (battery)
├── Disk spin-down: 15 min (AC, HDD), 10 min (battery, HDD)
├── WiFi power saving: Adaptive (off on AC, on on battery)
├── USB autosuspend: Adaptive (off on AC, on on battery)
├── Suspend: Default sleep action
├── Hibernate: Available if swap ≥ RAM
├── Hybrid sleep: OFF (sufficient RAM to survive power loss)
└── Critical battery: powersave + dim to 30%
```

**The only power management differences:**

```
├── CPU governor (powersave vs schedutil)
└── Hybrid sleep (ON for Standard, OFF for Plus)
```

Everything else in power management is **identical**.

### 9. Security

**Identical between variants:**

```
├── Firewall (ufw): Same rules, same defaults
├── AppArmor: Same profiles, same enforcement
├── Secure Boot: Supported, not required
├── Password policy: 6 chars minimum, no complexity
├── Root login: Disabled
├── sudo: Same configuration
├── Polkit: Same rules
├── Screen lock: Same behavior
├── File permissions: Same defaults
├── Kernel security features: Same (inherited from Ubuntu)
├── sysctl tunables: Same network hardening
├── Automatic security updates: Same schedule, same policy
├── Never-force-reboot: Same (both variants)
├── No SSH server: Same (both variants)
├── No telemetry: Same (both variants, absolute)
└── Malware approach: Same (defense in depth, no antivirus)
```

Security is **not** a variant differentiator. Both variants have identical security posture.

### 10. Privacy

**Identical between variants:**

```
├── Zero telemetry (absolute)
├── Zero data collection (absolute)
├── Application permission system (same portal-based controls)
├── Camera/microphone indicators (same, non-negotiable)
├── Camera/microphone kill switches (same)
├── Firefox privacy configuration (same ETP Strict, HTTPS-Only)
├── Location services (same, opt-in, approximate)
├── Data retention policies (same defaults, same configurability)
├── Multi-user isolation (same file permissions)
├── No cloud integration (same for both)
├── MAC randomization (same, off by default)
├── DNS privacy (same defaults, DoH/DoT available)
├── Connection audit trail (same, viewable in Settings)
└── All privacy settings in Settings → Privacy (same UI)
```

Privacy is **not** a variant differentiator. Both variants have identical privacy posture.

### 11. Installer

**Identical between variants:**

```
├── Same 10-step installer flow
├── Same hardware detection modules
├── Same data backup process
├── Same data restore process
├── Same Spike Rescue tool
├── Same partitioning logic (ext4, automatic)
├── Same GRUB2 installation
├── Same user account creation
├── Same language/timezone/keyboard selection
```

**The only installer difference:**

```
├── Step 6: Variant selection
│   ├── Installer recommends variant based on detected hardware
│   ├── User can override
│   ├── Warning if Plus chosen on inadequate hardware
│   └── If Standard chosen on adequate hardware: no warning (safe)
└── Step 9: Installation task differences
    ├── Task 3 (ZRAM): ZRAM cap based on selected variant
    ├── Task 10 (Flatpak): Runtime set based on selected variant
    ├── Task 13 (Plymouth): Theme based on selected variant
    ├── Task 14 (Theme): Animation/compositor settings based on variant
    ├── Task 17 (CPU governor): Set based on variant
    └── Task 15 (Services): Additional services enabled for Plus (if hardware present)
```

The installer generates a **single ISO**. Variant selection happens at install time, not at download time. There is no "Spike Standard ISO" and "Spike Plus ISO" — there is one Spike ISO.

### 12. Networking

**Identical between variants:**

```
├── NetworkManager (same configuration)
├── All Wi-Fi firmware (Intel, Atheros, Realtek, Broadcom)
├── DNS via systemd-resolved (same configuration)
├── Firewall (ufw) with same rules
├── Bluetooth conditional activation (same udev rules)
├── VPN support (OpenVPN, WireGuard, IPSec — same)
├── Mobile broadband (conditional, same detection)
├── Captive portal detection (same mechanism)
├── Wi-Fi hotspot (same capability)
├── Airplane mode (same rfkill implementation)
├── Network diagnostics (same GUI, same tests)
└── Network tray applet (same UI, slightly different poll intervals)
```

Networking is effectively **identical** between variants. The only difference is network applet polling interval (10s vs 5s), which is a minor optimization and not user-perceptible.

### 13. Disaster Recovery

**Identical between variants:**

```
├── Boot failure counter (same mechanism, same threshold)
├── Recovery mode (same GRUB entry, same environment)
├── Spike Rescue tool (same GUI, same file scanning)
├── Reinstall with restore (same detection, same restore flow)
├── Data backup during install (same scan, same checksum verification)
├── All seven failure scenarios (same recovery paths)
└── USB creation tool (same GUI, same ISO)
```

Disaster recovery is **not** a variant differentiator. Both variants have identical recovery capabilities.

### 14. Configuration

**Identical between variants:**

```
├── spike-config architecture (state store, templates, changelog)
├── Atomic write guarantee
├── DBus interface
├── State store recovery
├── CLI interface
├── Hardware detection
```

**The only configuration difference:**

```
├── State store field: "variant": "standard" vs "variant": "plus"
├── This field controls:
│   ├── ZRAM cap size (memory module)
│   ├── Plymouth theme (boot module)
│   ├── Animation settings (desktop module)
│   ├── Compositor effects (desktop module)
│   ├── CPU governor (power module)
│   ├── Hybrid sleep (power module)
│   ├── Bluetooth codecs (multimedia module)
│   ├── Flatpak runtime set (installer module)
│   ├── Qt rendering backend (desktop module)
│   └── Applet polling intervals (desktop module)
└── All other config values are identical (security, privacy, network, etc.)
```

## Variant Selection Logic

### Installer Recommendation Algorithm

**Inputs:**

```
├── RAM total (from detect/cpu.cpp → /proc/meminfo)
├── CPU model and class (from detect/cpu.cpp)
├── CPU bogomips per core (from detect/cpu.cpp)
├── GPU vendor and model (from detect/gpu.cpp)
```

**Logic:**

```
if RAM >= 8192 AND CPU is not Celeron/Pentium class:
    recommend = "plus"
elif RAM >= 8192 AND CPU is Celeron/Pentium class:
    recommend = "standard"  *# CPU is the bottleneck
elif RAM >= 5120 AND RAM < 8192:
    recommend = "standard"  *# 5-7GB is borderline, be conservative
elif RAM < 5120:
    recommend = "standard"  *# Clearly needs Standard
else:
    recommend = "standard"  *# Default to conservative
```

**CPU classification:**

```
├── "Celeron/Pentium class": CPU model contains "Celeron", "Pentium",
│   "Atom" (pre-2018), or bogomips < 2000 per core
├── "Modern": CPU model contains "Core i3/i5/i7/i9", "Ryzen",
│   "Athlon" (post-2018), or bogomips > 2500 per core
└── "Capable": Anything else (old but not terrible)
```

**Edge cases:**

```
├── 8GB RAM + Celeron N4020: Recommend Standard
│   (CPU is the bottleneck, not RAM)
├── 4GB RAM + Core i5: Recommend Standard
│   (RAM is the bottleneck, not CPU)
├── 4GB RAM + Celeron N4020: Recommend Standard
│   (both are bottlenecks)
├── 8GB RAM + Core i3 (8th gen+): Recommend Plus
│   (neither is a bottleneck)
└── 16GB RAM + Celeron N4020: Recommend Standard
    (CPU is still the bottleneck despite abundant RAM)
```

### User Override Warnings

If user selects Plus on hardware where Standard is recommended:

```
┌──────────────────────────────────────────────────┐
│  ⚠️  Spike Plus is not recommended for your       │
│     hardware.                                     │
│                                                  │
│  Your system has:                                │
│  • 4GB RAM (Spike Plus expects 8GB+)            │
│  • Intel Celeron N4020 (Spike Plus expects a    │
│    more capable CPU)                             │
│                                                  │
│  Performance may be noticeably slower with        │
│  Spike Plus on this hardware. Animations and      │
│  visual effects may stutter, and fewer            │
│  applications can run simultaneously.            │
│                                                  │
│  [Use Spike Standard instead]                     │
│  [Continue with Spike Plus anyway]                │
│                                                  │
└──────────────────────────────────────────────────┘
```

If user selects Standard on hardware where Plus is recommended:

```
├── No warning shown
├── Choosing less (fewer effects, lower memory) is always safe
└── Settings → About shows: "Spike Standard is running.
    Your hardware supports Spike Plus. [Switch to Plus]"
```

### Post-Install Variant Switching

Users can switch variants after installation:

**Settings → About → System variant:**

```
┌──────────────────────────────────────────────────┐
│  System Variant                                   │
│                                                  │
│  Current variant: Spike Standard                  │
│                                                  │
│  Your hardware:                                   │
│  • CPU: Intel Celeron N4020 (2 cores)             │
│  • RAM: 4GB                                       │
│  • GPU: Intel UHD Graphics 600                    │
│                                                  │
│  Recommended variant: Spike Standard              │
│                                                  │
│  [Switch to Spike Plus]                           │
│  (Not recommended for your hardware)              │
│                                                  │
└──────────────────────────────────────────────────┘
```

**Switching process:**

```
spike-config --state-set system variant plus
spike-config --generate-all
├── User notified: "Variant changed to Spike Plus.
│   Some changes take effect immediately. Please log out
│   and log back in for all changes to apply."
├── On next login: Plus settings active
├── No reinstall required (same packages, different config)
└── User can switch back at any time (same process, reversed)
```

**What changes immediately (before re-login):**

```
├── CPU governor (schedutil)
├── ZRAM cap (uncapped) — requires reboot for ZRAM resize
├── Swappiness (unchanged — same for both variants)
└── Flatpak runtime set (additional runtimes downloaded in background)
```

**What changes on next login:**

```
├── Animations enabled
├── Compositor effects (blur, transparency)
├── Qt rendering backend (OpenGL)
├── Applet polling intervals (standard)
├── Spike button hover glow
└── Bluetooth codecs (AAC, LDAC, aptX enabled)
```

**What changes on next reboot:**

```
├── Plymouth theme (spike-full)
├── ZRAM resized (if cap changed)
└── Background services (additional services enabled if hardware present)
```

## What Is Identical (Comprehensive List)

For absolute clarity, here is **everything** that is the same between Spike Standard and Spike Plus:

**Identity:**

```
├── Same name (Spike)
├── Same slogan
├── Same logo
├── Same organization (BigRangaTech)
├── Same license (GPLv2+ / CC-BY-SA 4.0)
```

**Philosophy:**

```
├── Same golden rules (both)
├── Same mission
├── Same target audience (beginners)
├── Same values
```

**Kernel:**

```
├── Same kernel (Ubuntu LTS)
├── Same boot parameters (except none differ)
├── Same module blacklisting logic
├── Same GPU driver policy (nouveau default, proprietary optional)
├── Same sysctl tunables (network hardening, memory)
├── Same transparent_hugepage=madvise
```

**Filesystem:**

```
├── Same ext4 everywhere
├── Same partitioning (automatic, no manual)
├── Same mount flags (per storage type)
├── Same swap file size and priority
├── Same swappiness (per storage type)
```

**Installer:**

```
├── Same 10-step flow
├── Same detection modules
├── Same backup/restore
├── Same partitioning
├── Same GRUB installation
```

**Desktop:**

```
├── Same panel (layout, applets, functionality)
├── Same launcher (categories, search, favorites)
├── Same notification daemon (disk-before-display invariant)
├── Same settings panel (same pages, same KCM modules)
├── Same session manager (login, logout, autostart)
├── Same theme engine (colors, fonts, wallpapers)
├── Same Spike button (logo, behavior)
```

**Networking:**

```
├── Same NetworkManager
├── Same firmware (all included)
├── Same DNS (systemd-resolved)
├── Same firewall (ufw)
├── Same Bluetooth conditional activation
├── Same VPN support
├── Same captive portal detection
├── Same airplane mode
```

**Security:**

```
├── Same firewall rules
├── Same AppArmor
├── Same password policy
├── Same root disabled
├── Same sudo config
├── Same Polkit rules
├── Same screen lock
├── Same file permissions
├── Same kernel security features
├── Same automatic updates (security auto-install)
├── Same never-force-reboot
├── Same no SSH server
├── Same no telemetry
```

**Privacy:**

```
├── Same zero data collection
├── Same application permissions (portal-based)
├── Same camera/mic indicators
├── Same camera/mic kill switches
├── Same Firefox privacy config
├── Same location services (opt-in)
├── Same data retention
├── Same multi-user isolation
├── Same no cloud integration
```

**Disaster Recovery:**

```
├── Same boot failure counter
├── Same recovery mode
├── Same Spike Rescue tool
├── Same reinstall with restore
├── Same all failure scenarios
```

**Configuration:**

```
├── Same spike-config architecture
├── Same state store format
├── Same template engine
├── Same atomic writes
├── Same changelog
├── Same DBus interface
├── Same CLI interface
├── Same hardware detection
├── Same state store recovery
```

**Updates:**

```
├── Same update schedule (6 hours)
├── Same update conditions (idle, network, memory)
├── Same security auto-install policy
├── Same non-security notify-only policy
├── Same Flatpak notify-only policy
├── Same gentle reboot notification
```

**ISO:**

```
├── Same ISO (single download)
├── Same packages (identical — variant is config-only)
├── Same firmware (all included)
├── Same Flatpak runtimes (Standard set on ISO, Plus set installed post-variant-selection)
└── Same branding assets
```

**Community:**

```
├── Same code of conduct
├── Same contribution process
├── Same DCO requirement
├── Same governance (BDFL)
└── Same support resources
```

## Summary: The Complete Difference List

Everything that differs between Spike Standard and Spike Plus:

1. **ZRAM cap:** 4GB max (Standard) vs uncapped (Plus) 

2. **Animations:** Off (Standard) vs On (Plus) 

3. **Compositor effects:** Minimal (Standard) vs Full (Plus) 

4. **Plymouth theme:** spike-minimal (Standard) vs spike-full (Plus) 

5. **CPU governor:** powersave (Standard) vs schedutil (Plus) 

6. **Bluetooth codecs:** SBC only (Standard) vs SBC+AAC+LDAC+aptX (Plus) 

7. **Flatpak runtimes:** KDE+GNOME (Standard) vs all common (Plus) 

8. **Applet polling:** Conservative intervals (Standard) vs Standard intervals (Plus) 

9. **Qt rendering:** Software preferred (Standard) vs OpenGL preferred (Plus) 

10. **Spike button hover:** No glow (Standard) vs Glow (Plus) 

11. **Hybrid sleep:** ON (Standard) vs OFF (Plus) 

12. **Idle RAM target:** <400MB (Standard) vs <800MB (Plus) 

13. **Background services:** Minimal set (Standard) vs Standard set (Plus) 

14. **Memory budget breakdown:** Tighter allocation (Standard) vs Relaxed allocation (Plus) 

That's it. **Fourteen differences.** Everything else is identical.

If anyone asks "what's the difference between Standard and Plus?", point them to this list. If something isn't on this list, it's the same in both variants.

## What This Document Does Not Cover

- Full specifications for each feature: See the respective document (`MEMORY.md`, `DESKTOP.md`, `BOOT-PROCESS.md`, etc.) 

- Installer flow and variant selection UI: See `INSTALLER.md` (Step 6) 

- Hardware detection logic: See `INSTALLER.md` (Detection Modules) and `CONFIGURATION.md` (Hardware Detection) 

- Configuration system architecture: See `CONFIGURATION.md` (State Store, Template Engine) 

- Performance benchmarks: See `PERFORMANCE-BASELINES.md` (to be measured during alpha) 

- Design rationale for variant decisions: See `DESIGN-DECISIONS.md` 

🐕 BigRangaTech


