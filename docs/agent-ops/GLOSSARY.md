# Spike — Glossary of Terms

## Purpose

This file defines every Spike-specific term an AI agent (or human contributor) needs to understand when working on the project. Terms are cross-referenced to their specification documents.

> **Rule:** If you encounter an unfamiliar term while reading specs or code, check this glossary first. If it's not here and not obvious from context, flag it to the human for definition.

## Project & Organization

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Spike** | The Linux distribution itself — the product being developed. A lightweight Ubuntu-based rescue OS targeting old/disposable laptops. | `AGENTS.md`, `README.md` |
| **BigRangaTech** | The organization that owns Spike. An existing multi-project org; Spike is one project under it. Each project gets its own docs and eventually its own website. | `AGENTS.md`, `GOVERNANCE.md` |
| **BDFL** | Benevolent Dictator For Life. The project creator who holds final decision authority. Has a documented succession plan. | `GOVERNANCE.md` |
| **Golden Rules** | Three non-negotiable rules: (1) Users never edit config files, (2) CLI tools exist for developers only, (3) If it's not documented, it doesn't exist. | `PHILOSOPHY.md` |
| **Repairable** | Core value: Hardware should be serviceable, software should be understandable. Not disposable. | `PHILOSOPHY.md` |
| **Documentation-first** | Methodology: Discuss until convergent → write docs → then code. No undocumented features. | `AGENTS.md` |


## Desktop Environment

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Spike Shell** | The custom desktop shell. NOT full KDE Plasma — a standalone shell built on KWin with custom components (panel, launcher, notification daemon, session manager). | `DESKTOP.md`, `ARCHITECTURE.md` |
| **spike-session** | The session manager process. Starts SDDM → spike-shell → KWin → services. Handles login/logout/autostart filtering. | `DESKTOP.md` |
| **KWin** | The Wayland compositor (from KDE Frameworks). Used standalone, NOT as part of plasma-session. Acts as the display server. | `ARCHITECTURE.md`, `BOOT-PROCESS.md` |
| **SDDM** | Display Manager (login screen). Themed for Spike (dark background, purple/cyan accents, BigRangaTech branding). | `BOOT-PROCESS.md` |
| **Panel** | The bottom (default) or top taskbar. 32px height default (adjustable 24–48px). Three zones: left/center/right. Contains launcher button, pinned apps, clock, system tray applets. | `DESKTOP.md` |
| **Launcher** | Kickoff-style application launcher (3-pane: favorites, categories, search). Opens via Super key. | `DESKTOP.md` |
| **System tray applets** | 14 always-present applets: clock, network, battery, brightness, volume, updates, notifications, removable devices, keyboard layout, input method, night light, power profiles, camera/mic indicator, spike-menu (launcher button). + 4 conditional (BT, modem, etc.). | `DESKTOP.md` |
| **Notification daemon** | Notification system following **disk-before-display invariant** (write to disk BEFORE showing on screen). Never loses notifications due to crash or power loss. | `DESKTOP.md` |
| **Spikes menu** | The launcher button on the panel (left zone). Opens the application launcher. | `DESKTOP.md` |
| **KDE standalone apps** | KDE applications included WITHOUT full Plasma: Discover, Dolphin, Konsole, Kate, Ark, Spectacle, KCalc, plus selected Settings KCMs from standalone packages (`kscreen`, `plasma-pa`, `powerdevil`, `bluedevil`, `print-manager`). Not full System Settings / not `plasma-nm`. | `ARCHITECTURE.md`, `CONSTRAINTS.md` |
| **KCM** | KDE Control Module. Loaded in Spike Settings via KF6 `KCModuleLoader` when the provider package does not pull `plasma-desktop` / `plasma-workspace`. | `DESKTOP.md` |
| **Network applet** | Spike Shell tray applet + Settings → Network page talking to NetworkManager over D-Bus (custom; not plasma-nm / nm-tray). | `DESKTOP.md`, `NETWORKING.md` |


## Configuration & State Management

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **spike-config** | The configuration engine for Spike. NOT a daemon — runs on-demand. Combines a state store (JSON) with a template engine to generate config files. Invoked by Settings GUI or systemd triggers. | `CONFIGURATION.md` |
| **State store** | `/var/lib/spike/config/state.json`. Single source of truth for all configuration. Contains hardware detection, memory settings, boot settings, security settings, updates, multimedia, power, network, privacy, installer, desktop settings. | `CONFIGURATION.md` |
| **Template engine** | Part of spike-config that generates config files. Takes templates (`/usr/lib/spike/config/templates/*.tpl`) with `\\{\\{variable\\}\\}` syntax, substitutes values from state store, validates output, writes atomically. | `CONFIGURATION.md` |
| **Atomic write** | Config file write pattern: write to `.tmp` → `fsync` → `rename` → `fsync(dir)`. Guarantees file is always OLD or NEW version, never corrupted. | `CONFIGURATION.md` |
| **Changelog** | `/var/lib/spike/config/changelog.json`. Append-only JSON array tracking every config change: timestamp, module, setting, old_value, new_value, source, files_regenerated, services_reloaded. Max 5000 entries (oldest pruned). | `CONFIGURATION.md` |
| **Idempotent** | Running `spike-config --generate-all` produces identical output every time given the same state store. No duplicates, no stale settings. | `CONFIGURATION.md` |
| **Config modules** | Nine config file generators: memory, boot, security, network, multimedia, power, privacy, desktop, updates. Each generates specific config files in `/etc/`. | `CONFIGURATION.md` |
| **DBus interface** | `org.spike.Config`. Methods: GetState, SetSetting, GenerateAll, DetectHardware, Rollback, etc. Settings GUI never writes config files directly — always via spike-config. | `CONFIGURATION.md` |
| **Defaults** | `/usr/lib/spike/config/defaults.json`. Per-variant defaults used if `state.json` is lost/corrupted. Regenerated from hardware detection + defaults. | `CONFIGURATION.md` |


## Variants

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Standard** | The lightweight variant. ZRAM capped at 4GB, animations off, minimal compositor effects, powersave governor, SBC Bluetooth codecs only, KDE+GNOME Flatpak runtimes pre-seeded, hybrid sleep ON, idle RAM target <400MB. ~280–370MB idle memory. | `VARIANT-DIFFERENCES.md` |
| **Plus** | The full-feature variant. ZRAM uncapped (up to RAM), animations on, full compositor effects (blur/transparency/shadows), schedutil governor, all Bluetooth codecs (AAC/LDAC/aptX), all common Flatpak runtimes pre-seeded, hybrid sleep OFF, idle RAM target <800MB. ~420–650MB idle memory. | `VARIANT-DIFFERENCES.md` |
| **Variant selection** | Determined at install based on hardware detection. User can override (warning shown if mismatched). Post-install switching available (no reinstall needed) via \`spike-config --state-set system variant <plus | standard>\`. |
| **The 14 differences** | Exactly 14 differences between Standard and Plus. Everything else is identical between variants. See `CONSTRAINTS.md` for complete list. | `VARIANT-DIFFERENCES.md` |


## Hardware & System

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Capable CPU** | Classification: bogomips ≥ 2200/core AND dual-core+. Example: Celeron N4020. Receives ZRAM enabled. | `HARDWARE.md`, `MEMORY.md` |
| **Low-end CPU** | Classification: bogomips < 2000/core. Example: AMD A4. ZRAM skipped (swap only). | `HARDWARE.md`, `MEMORY.md` |
| **Modern CPU** | Classification: Core i3/i5/i7/i9, Ryzen, Athlon (post-2018). Full capabilities enabled. | `HARDWARE.md` |
| **eMMC** | Embedded MultiMediaCard storage. **NOT SUPPORTED** in Spike due to wear-out risk. Detected via `/dev/mmcblk*` device names. | `HARDWARE.md`, `INSTALLER.md` |
| **Rotational flag** | From `lsblk -d -o NAME,ROTA`. SSD/NVMe report ROTA=0, HDD reports ROTA=1. Used for mount flags and swappiness. eMMC may falsely report as non-rotational. | `INSTALLER.md` |
| **ZRAM** | Compressed RAM block device. Uses zstd compression, priority 100. Effectively expands 4GB RAM to ~8–10GB. Only enabled on capable CPUs. | `MEMORY.md` |
| **zswap** | Kernel-level compressed swap cache. **DISABLED** in Spike (kernel command line: `zswap.enabled=0`). Conflicts with ZRAM. | `KERNEL.md` |
| **THP** | Transparent Huge Pages. Set to `madvise` (kernel command line: `transparent_hugepage=madvise`) to prevent khugepaged overhead. | `KERNEL.md` |
| **Earlyoom** | OOM killer alternative. Triggers at 10% memory+swap remaining. Protected: spike-shell, kwin_wayland, systemd, pipewire, wireplumber. Preferred kills: firefox, chromium, libreoffice. | `MEMORY.md` |
| **swappiness** | Kernel parameter controlling swap preference. 15 (SSD), 5 (HDD), 10 (SD/USB). User-adjustable slider for SSD only (15–60 range). | `MEMORY.md` |
| **VA-API** | Video Acceleration API. Hardware video decode driver interface. Intel N4020 uses `intel-media-va-driver-non-free` (LIBVA_DRIVER_NAME=iHD). Decodes H.264, H.265 (8-bit), VP8, VP9. NOT AV1. | `MULTIMEDIA.md`, `HARDWARE.md` |
| **AV1** | Next-gen video codec. N4020 lacks AV1 hardware decode. **Disabled** in Firefox (pref: `media.av1.enabled=false`) to force VP9 fallback. | `MULTIMEDIA.md` |
| **Wayland** | Display server protocol. Spike uses Wayland exclusively (KWin as compositor). XWayland available as fallback for X11-only apps. | `ARCHITECTURE.md` |


## Storage & Filesystem

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **ext4** | Journaling filesystem used everywhere. Btrfs considered and rejected for simplicity. | `INSTALLER.md` |
| **Mount flags (SSD)** | `defaults,noatime`. No access time updates (performance). | `INSTALLER.md` |
| **Mount flags (HDD)** | `defaults,noatime,commit=60`. Delayed commit reduces write amplification. | `INSTALLER.md` |
| **Mount flags (SD/USB)** | `defaults,noatime,commit=60,discard`. TRIM-like discard enabled for flash. | `INSTALLER.md` |
| **Dual boot** | Not supported. Installer refuses to coexist with other OS. No GRUB OS prober. | `INSTALLER.md` |
| **Disk encryption (LUKS)** | Not offered. Deliberate decision for beginner recovery simplicity (encrypted disks are unrecoverable without password). | `INSTALLER.md`, `SECURITY.md` |


## Boot & Recovery

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **GRUB2** | Bootloader. Theme: dark background, Spike emblem, BigRangaTech branding, purple/cyan accents. Hidden by default, 3–5 second ESC window. | `BOOT-PROCESS.md` |
| **Boot failure counter** | Initramfs counter incremented on each boot. After 3 consecutive failures, GRUB menu shows automatically with recovery entry highlighted (10–15 second timeout). Successful boot resets to 0. | `BOOT-PROCESS.md`, `DISASTER-RECOVERY.md` |
| **Recovery mode** | GRUB entry that boots minimal single-user root shell (no graphics, no network). `spike-config --rollback` available here. | `DISASTER-RECOVERY.md` |
| **Spike Rescue** | GUI tool on live ISO. "Rescue my data" option mounts broken system READ-ONLY, copies user data to USB with SHA256 verification, preserves folder structure. | `DISASTER-RECOVERY.md` |
| **spike-reset** | Factory reset tool (reinstall + restore workflow). | `DISASTER-RECOVERY.md` |


## Networking & Privacy

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **systemd-resolved** | DNS resolver. Caches locally. No application can bypass (prevents DNS leaks). Default: router-provided via DHCP. Fallback: 1.1.1.1, 8.8.8.8. | `NETWORKING.md` |
| **DoT** | DNS-over-TLS. Supported but disabled by default (user can enable). | `NETWORKING.md`, `PRIVACY.md` |
| **DoH** | DNS-over-HTTPS. Available in Firefox only. Disabled system-wide. | `PRIVACY.md` |
| **Portals** | `xdg-desktop-portal`. Wayland-native permission system for camera, microphone, files, screen capture, notifications, background. States: Allowed/Ask/Denied. | `PRIVACY.md` |
| **Kill switch** | Camera/microphone disable buttons in Settings → Privacy. Camera unloads `uvcvideo` module; microphone mutes all PipeWire capture devices. Persistent across reboots. | `PRIVACY.md` |
| **Telemetry** | **ZERO**. Absolute and non-negotiable. Removed: ubuntu-report, apport, whoopsie, popularity-contest, landscape, motd-news, cloud-init, snapd. | `PRIVACY.md`, `SECURITY.md` |
| **Network connection list** | Complete list of Spike's network connections (5 total): apt update, Flatpak remote check, NTP sync, connectivity check, captive portal detection. Nothing else connects. | `PRIVACY.md` |
| **Airplane mode** | Via rfkill. Disables Wi-Fi and Bluetooth. Separate from kill switches (radio-based vs. driver-based). | `NETWORKING.md` |


## Package Management

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **apt** | System package manager. Ubuntu repositories. Security updates auto-installed via `spike-update.timer` (every 6 hours). | `ARCHITECTURE.md` |
| **Flatpak** | User application delivery. Installed via Discover. Sandboxed. Runtimes pre-seeded on ISO (KDE+GNOME for Standard, all common for Plus). | `ARCHITECTURE.md` |
| **Discover** | KDE's software manager. Flatpak frontend. Used in standalone KWin environment (NOT full Plasma). May need environment variable adjustments. | `ARCHITECTURE.md` |
| **snap** | **REMOVED entirely**. snapd not installed. Includes Canonical telemetry. | `ARCHITECTURE.md` |
| **FFmpeg VPX** | Software VPX decoder (`media.ffvpx.enabled=false` disabled to force hardware decode). | `MULTIMEDIA.md` |
| **PipeWire** | Audio server. Replaces PulseAudio. 44100Hz sample rate locked, stereo only, low resampler quality (saves CPU). | `MULTIMEDIA.md` |
| **WirePlumber** | PipeWire session manager. Minimal configuration (not full WirePlumber). | `MULTIMEDIA.md` |


## Security & Authentication

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Root account** | Locked. sudo used instead. | `SECURITY.md` |
| **sudo** | Privilege escalation for standard user. One password for login/sudo/polkit/screen unlock. Minimum 6 characters, no complexity requirements, no expiration. | `SECURITY.md` |
| **AppArmor** | Mandatory Access Control. Enabled with Ubuntu defaults. No custom profiles (yet). | `SECURITY.md` |
| **ufw** | Firewall. Default incoming: DENY, outgoing: ALLOW. Pre-configured: 5353/udp (mDNS). | `SECURITY.md`, `NETWORKING.md` |
| **Secure Boot** | Supported but not required. Broadcom Wi-Fi drivers need MOK enrollment if Secure Boot enabled (prompt at first boot). | `SECURITY.md` |
| **Enhanced Tracking Protection** | Firefox setting. Strict mode (blocks trackers, fingerprinting, crypto miners). | `SECURITY.md` |
| **HTTPS-Only mode** | Firefox setting. All HTTP upgraded to HTTPS. Warns on no-HTTPS sites. | `SECURITY.md` |


## Power Management

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **CPU governor** | Power policy. Standard: `powersave`. Plus: `schedutil`. Intel pstate preferred for Intel CPUs. Performance mode available as temporary override (AC only). | `POWER-MANAGEMENT.md` |
| **Power profiles** | Three modes: Performance (AC, schedutil/performance, autosuspend OFF), Battery Saver (<50%, powersave, autosuspend ON), Critical (<20%, powersave, dim to 30%, auto-shutdown at 5%). | `POWER-MANAGEMENT.md` |
| **Hybrid sleep** | Suspend-to-RAM AND disk simultaneously. Standard: ON (safety net for 4GB RAM). Plus: OFF (sufficient RAM). | `POWER-MANAGEMENT.md`, `VARIANT-DIFFERENCES.md` |
| **Night Light** | Blue light filter. Manual schedule or sunset-to-sunrise (requires location service). | `POWER-MANAGEMENT.md` |


## Multimedia

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Bluetooth audio codecs** | Standard: SBC only. Plus: SBC, AAC, LDAC, aptX. Codec negotiated per device (best available). User can override per device. | `MULTIMEDIA.md` |
| **Audio routing priority** | Bluetooth → HDMI → headphone jack → speakers. Automatic. | `MULTIMEDIA.md` |
| **Spectacle** | KDE screenshot tool. Full screen, region, window, active screen. Print Screen key → region capture. | `MULTIMEDIA.md` |
| **WebRTC** | Video calls supported in Firefox. Camera: uvcvideo, Microphone: via PipeWire. Echo cancellation + noise suppression enabled. Camera/mic indicators appear when active. | `MULTIMEDIA.md` |


## Documentation & Contributing

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **INDEX.md** | Master document index. Lists all 70 documents with status, descriptions, topic index, dependency map. | `INDEX.md` |
| **AGENTS.md** | Master project reference. Phases, conventions, tracking, cross-references. Not counted in 70 docs but governs all of them. | `AGENTS.md` |
| **DESIGN-DECISIONS.md** | Rationale for every major technical and UX decision. The "why" document. | `DESIGN-DECISIONS.md` |
| **Top-level documents** | 34 specification documents at root directory (README.md, MEMORY.md, CONFIGURATION.md, etc.). | `INDEX.md` |
| **user-guide/** | 17 sub-documents for non-technical users. Plain language, no jargon, no terminal, no prerequisites. | `INDEX.md` |
| **dev-guide/** | 19 sub-documents for developers. Technical, with code examples, API references, internals. Written alongside implementation. | `INDEX.md` |
| **DCO** | Developer Certificate of Origin. Chosen over CLA (lighter barrier to entry). Signed on every commit. | `CONTRIBUTING.md` |
| **BDFL-level decision** | A decision requiring explicit approval from the project creator. Logged in `DECISIONS.md`. | `GOVERNANCE.md` |
| **Phase 1** | Architecture design (complete). All major decisions resolved. | `AGENTS.md` |
| **Phase 2** | Specification documents (in progress). 8 core tech specs drafted + 14 additional top-level docs formatted. | `AGENTS.md` |
| **Phase 3** | Prototyping (not started). Build SPIKE base ISO, implement spike-config, Spike Shell, installer, Spike Rescue, test on target hardware. | `AGENTS.md` |
| **Alpha release** | Target: 8 months from start. Functional system on target hardware. Public read-only repo. Bug reports/hardware submissions accepted. | `ROADMAP.md` |
| **Beta release** | Target: 18 months from start. Feature-complete. Full contributions accepted. Translation contributions accepted. | `ROADMAP.md` |
| **Living docs** | Specifications evolve with implementation. Discoveries made during coding are added to specs as [IMPLEMENTATION NOTE]. | `AGENTS.md` |


## Color & Visual Identity

| **Term** | **Definition** | **Spec Reference** |
| :-: | :-: | :-: |
| **Purple (#6d4aff)** | Primary accent color. Used for UI elements, logos, themes, branding. | `BRANDING.md` |
| **Cyan/teal** | Secondary accent color. Used as system accent complement. | `BRANDING.md` |
| **Noto Sans** | Default font, 10pt. Unicode-compatible. | `BRANDING.md` |
| **Noto Sans Mono** | Monospace font. Code, terminals. | `BRANDING.md` |
| **Breeze** | Icon theme (KDE default). Also cursor theme (24px). | `BRANDING.md` |
| **Spike dog** | Logo mascot: herding dog (creator's real dog, Spike) with circuit board flanks, purple/cyan glow, circular emblem. | `BRANDING.md` |
| **Let's Make Tech Repairable Again** | Slogan. Core mission statement. | `PHILOSOPHY.md` |


## Filesystem Paths (Common Locations)

| **Path** | **Purpose** | **Spec Reference** |
| :-: | :-: | :-: |
| `/var/lib/spike/config/state.json` | State store (spike-config) | `CONFIGURATION.md` |
| `/var/lib/spike/config/changelog.json` | Change log (spike-config) | `CONFIGURATION.md` |
| `/var/lib/spike/power/` | Battery health history | `POWER-MANAGEMENT.md` |
| `/usr/lib/spike/config/templates/` | Config templates (spike-config) | `CONFIGURATION.md` |
| `/usr/lib/spike/config/defaults.json` | Defaults (spike-config) | `CONFIGURATION.md` |
| `/usr/share/spike/firefox/spike-prefs.js` | Firefox prefs (Spike-tuned) | `MULTIMEDIA.md` |
| `/usr/share/spike/wallpapers/` | Wallpaper files | `BRANDING.md` |
| `~/.config/spike/` | User settings (per-user) | `PRIVACY.md` |
| `~/.local/share/spike/notifications/history.json` | Notification history | `DESKTOP.md` |
| `/var/log/spike/connections.log` | Network connection log (7-day retention) | `PRIVACY.md` |


## Quick Lookup (By Category)

### What does spike-X mean?

| **Term** | **What it is** |
| :-: | :-: |
| spike-config | Configuration engine (state store + template engine) |
| spike-shell | Desktop shell binary (panel, launcher, notifications) |
| spike-session | Session manager process |
| spike-update.timer | Systemd timer for automatic updates (every 6 hours) |
| spike-menu | Launcher button on the panel |


### What's the difference between Standard and Plus?

| **Aspect** | **Standard** | **Plus** |
| :-: | :-: | :-: |
| ZRAM cap | 4GB max | Uncapped |
| Animations | Off | On |
| Idle RAM target | <400MB | <800MB |
| Memory budget | ~280–370MB | ~420–650MB |
| See `VARIANT-DIFFERENCES.md` for all 14 differences |  |  |


### What are the Golden Rules?

```
\`1. Users never edit config files\`  

\`2. CLI tools exist for developers only\`  

\`3. If it's not documented, it doesn't exist\`

 4. The user never has to touch the terminal

 5. If a gui doesnt exist either make one or it doesnt exist
```

### What network connections does Spike make?

```
\`1. apt update → archive.ubuntu.com (every 6 hours)\`  

\`2. Flatpak remote check → flathub.org (every 6 hours)\`  

\`3. NTP sync → pool.ntp.org (boot + periodic)\`  

\`4. Connectivity check → connectivity-check.ubuntu.com (boot + periodic)\`  

\`5. Captive portal detection → redirect target (new network)\`  

\`Nothing else.\`
```

🐕 BigRangaTech

