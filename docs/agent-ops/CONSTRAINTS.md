# Spike — Hard Technical Constraints

## Purpose

This file lists every **non-negotiable technical constraint** in the Spike Linux project. These are derived from the specification documents and concentrated here so an agent can quickly verify "can I do X?" without hunting through 32 specs.

> **Rule:** If a constraint listed here conflicts with a spec document, the spec document is authoritative. Update this file to match. However, **never violate a constraint without explicit human approval** — even if a spec seems to allow it.


## 1. Filesystem

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Filesystem | ext4 only | `INSTALLER.md`, `ARCHITECTURE.md` |
| Forbidden filesystems | btrfs, zfs, xfs, f2fs | `INSTALLER.md` |
| Partition scheme | `/boot/efi` (512MB FAT32, UEFI) or `/boot` (1GB ext4, BIOS) + `/` (rest, ext4) | `INSTALLER.md` |
| Swap file | `/swapfile`, 8GB (SSD/HDD), 4GB (SD/USB) | `MEMORY.md` |
| Mount flags (SSD/NVMe) | `defaults,noatime` | `INSTALLER.md` |
| Mount flags (HDD) | `defaults,noatime,commit=60` | `INSTALLER.md` |
| Mount flags (SD/USB) | `defaults,noatime,commit=60,discard` | `INSTALLER.md` |


## 2. Memory Management

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| ZRAM compression | zstd | `MEMORY.md` |
| ZRAM priority | 100 | `MEMORY.md` |
| ZRAM disksize | Equal to RAM | `MEMORY.md` |
| ZRAM cap (Standard) | 4GB max | `VARIANT-DIFFERENCES.md` |
| ZRAM cap (Plus) | Uncapped (up to RAM size) | `VARIANT-DIFFERENCES.md` |
| ZRAM condition | Only on "capable" or "modern" CPUs (bogomips ≥ 2200/core, dual-core+) | `MEMORY.md`, `HARDWARE.md` |
| ZRAM skip condition | "low-end" CPUs (bogomips < 2000/core) — swap only | `MEMORY.md`, `HARDWARE.md` |
| Swap priority | 10 | `MEMORY.md` |
| Swap location | `/swapfile` | `MEMORY.md` |
| zswap | Disabled (kernel command line: `zswap.enabled=0`) | `KERNEL.md`, `BOOT-PROCESS.md` |
| THP | `madvise` (kernel command line: `transparent_hugepage=madvise`) | `KERNEL.md`, `BOOT-PROCESS.md` |
| Swappiness (SSD) | 15 | `MEMORY.md` |
| Swappiness (HDD) | 5 | `MEMORY.md` |
| Swappiness (SD/USB) | 10 | `MEMORY.md` |
| Swappiness user range | 15–60 (SSD only, increase only) | `MEMORY.md` |
| Earlyoom threshold | 10% of memory+swap remaining | `MEMORY.md` |
| Earlyoom protected processes | spike-shell, kwin_wayland, systemd, pipewire, wireplumber | `MEMORY.md` |
| Earlyoom preferred kill targets | firefox, chromium, libreoffice, gimp, blender | `MEMORY.md` |


## 3. Configuration (spike-config)

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| State store location | `/var/lib/spike/config/state.json` | `CONFIGURATION.md` |
| State store format | JSON (versioned) | `CONFIGURATION.md` |
| State store permissions | 644 (root:root) | `CONFIGURATION.md` |
| Defaults location | `/usr/lib/spike/config/defaults.json` | `CONFIGURATION.md` |
| Template location | `/usr/lib/spike/config/templates/*.tpl` | `CONFIGURATION.md` |
| Template variable syntax | `{{variable}}` substituted from state store | `CONFIGURATION.md` |
| Template validation | No leftover `{{}}` in output, no empty critical fields | `CONFIGURATION.md` |
| Atomic write pattern | Write to `.tmp` → `fsync` → `rename` → `fsync(dir)` | `CONFIGURATION.md` |
| Never write directly to target file | Enforced — always use temp+rename | `CONFIGURATION.md` |
| Changelog location | `/var/lib/spike/config/changelog.json` | `CONFIGURATION.md` |
| Changelog format | Append-only JSON array | `CONFIGURATION.md` |
| Changelog max entries | 5000 (oldest pruned) | `CONFIGURATION.md` |
| Changelog fields | timestamp, module, setting, old_value, new_value, source, files_regenerated, services_reloaded | `CONFIGURATION.md` |
| Idempotency | `spike-config --generate-all` produces identical output given same state | `CONFIGURATION.md` |
| State recovery | If `state.json` lost/corrupted → regenerate from `defaults.json` + hardware detect | `CONFIGURATION.md` |
| DBus interface | `org.spike.Config` | `CONFIGURATION.md` |
| Settings GUI | Never writes config files directly — always via spike-config | `CONFIGURATION.md` |

### Config Modules

| **Module** | **Generates** | **Source** |
| :-: | :-: | :-: |
| `memory` | `sysctl.d/99-spike-memory.conf`, `zram-generator.conf`, udev zram rules, earlyoom config | `CONFIGURATION.md` |
| `boot` | `/etc/default/grub`, boot-count, Plymouth theme | `CONFIGURATION.md` |
| `security` | ufw rules, sudoers, module blacklist | `CONFIGURATION.md` |
| `network` | `NetworkManager.conf`, `resolved.conf`, `/etc/hosts` | `CONFIGURATION.md` |
| `multimedia` | `pipewire.conf`, wireplumber, environment, Firefox prefs, GPU driver modprobe | `CONFIGURATION.md` |
| `power` | `logind.conf`, battery health tmpfiles, cron weekly | `CONFIGURATION.md` |
| `privacy` | `journald.conf`, audit rules | `CONFIGURATION.md` |
| `desktop` | `sddm.conf`, `spike-shell.conf`, Qt stylesheet, KWin decoration, dconf | `CONFIGURATION.md` |
| `updates` | `apt.conf.d/99-spike-updates`, `unattended-upgrades` config, `spike-update.timer` | `CONFIGURATION.md` |

### CLI Interface (Developers Only)

| **Command** | **Purpose** | **Source** |
| :-: | :-: | :-: |
| `spike-config --generate-all` | Regenerate all config files from state | `CONFIGURATION.md` |
| `spike-config --generate <module>` | Regenerate single module | `CONFIGURATION.md` |
| `spike-config --detect` | Run hardware detection | `CONFIGURATION.md` |
| `spike-config --state` | Dump state store | `CONFIGURATION.md` |
| `spike-config --state-set <module> <key> <value>` | Set a state value | `CONFIGURATION.md` |
| `spike-config --changelog` | View changelog | `CONFIGURATION.md` |
| `spike-config --rollback <entry-id>` | Rollback to previous config state | `CONFIGURATION.md` |
| `spike-config --validate` | Validate all config files against state | `CONFIGURATION.md` |


## 4. Kernel

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Kernel | Ubuntu LTS kernel | `ARCHITECTURE.md` |
| Architecture | x86_64 only | `ARCHITECTURE.md` |
| 32-bit support | None (deferred indefinitely) | `ARCHITECTURE.md` |

### Boot Parameters (Kernel Command Line)

| **Parameter** | **Value** | **Source** |
| :-: | :-: | :-: |
| `quiet` | Set | `BOOT-PROCESS.md` |
| `splash` | Set | `BOOT-PROCESS.md` |
| `zswap.enabled` | `0` | `KERNEL.md` |
| `transparent_hugepage` | `madvise` | `KERNEL.md` |

### Module Blacklisting Rules

| **Rule** | **Detail** | **Source** |
| :-: | :-: | :-: |
| Never blacklist | `usbcore`, `uhci_hcd`, `ehci_hcd`, `xhci_hcd` (USB) | `KERNEL.md` |
| Never blacklist | Network drivers (hot-pluggable) | `KERNEL.md` |
| Never blacklist | Audio drivers | `KERNEL.md` |
| Never blacklist | Bluetooth drivers | `KERNEL.md` |
| Never blacklist | Hot-pluggable bus drivers | `KERNEL.md` |
| Blacklist only | Unused, non-hotpluggable modules (e.g., floppy, parallel port) | `KERNEL.md` |

### sysctl Network Hardening

| **sysctl Key** | **Value** | **Source** |
| :-: | :-: | :-: |
| `net.ipv4.conf.all.rp_filter` | `1` | `SECURITY.md` |
| `net.ipv4.conf.default.rp_filter` | `1` | `SECURITY.md` |
| `net.ipv4.conf.all.accept_redirects` | `0` | `SECURITY.md` |
| `net.ipv4.conf.default.accept_redirects` | `0` | `SECURITY.md` |
| `net.ipv6.conf.all.accept_redirects` | `0` | `SECURITY.md` |
| `net.ipv6.conf.default.accept_redirects` | `0` | `SECURITY.md` |


## 5. Boot Process

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Bootloader | GRUB2 | `BOOT-PROCESS.md` |
| `GRUB_DISABLE_OS_PROBER` | `true` (no dual boot detection) | `BOOT-PROCESS.md` |
| `GRUB_DISABLE_SUBMENU` | `y` (flat kernel list) | `BOOT-PROCESS.md` |
| `GRUB_TERMINAL_OUTPUT` | `gfxterm` (themed graphical) | `BOOT-PROCESS.md` |
| Boot menu | Hidden by default, 3–5 second ESC window | `BOOT-PROCESS.md` |
| Recovery entry | Always present in GRUB | `BOOT-PROCESS.md` |
| Boot failure counter | After 3 consecutive failures → GRUB menu shows automatically, recovery highlighted, 10–15 second timeout | `BOOT-PROCESS.md`, `DISASTER-RECOVERY.md` |
| Counter reset | Successful boot resets counter to 0 | `DISASTER-RECOVERY.md` |
| Previous kernel fallback | Available in GRUB menu | `DISASTER-RECOVERY.md` |
| Display manager | SDDM (Spike-themed) | `BOOT-PROCESS.md` |
| Plymouth theme (Standard) | `spike-minimal` (static logo, progress dots) | `BOOT-PROCESS.md`, `VARIANT-DIFFERENCES.md` |
| Plymouth theme (Plus) | `spike-full` (animated logo, progress bar) | `BOOT-PROCESS.md`, `VARIANT-DIFFERENCES.md` |
| Plymouth theme source | Set by spike-config, not hardcoded | `CONFIGURATION.md` |


## 6. Storage

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Minimum storage | 128GB | `INSTALLER.md`, `HARDWARE.md` |
| Storage detection | `lsblk -d -o NAME,ROTA` (rotational flag) | `INSTALLER.md` |
| SATA SSD | Supported ✅ | `HARDWARE.md` |
| SATA HDD | Supported ✅ | `HARDWARE.md` |
| NVMe SSD | Supported ✅ | `HARDWARE.md` |
| SD card (UHS-I+, 128GB+) | Supported ✅ | `HARDWARE.md` |
| USB 3.0 (128GB+) | Supported ✅ | `HARDWARE.md` |
| eMMC | **NOT SUPPORTED** ❌ (wear-out risk) | `INSTALLER.md`, `HARDWARE.md` |
| Dual boot | Not supported | `INSTALLER.md` |
| Disk encryption (LUKS) | Not offered (deliberate — beginner recovery simplicity) | `INSTALLER.md`, `SECURITY.md` |
| Partitioning | Fully automatic, no manual option | `INSTALLER.md` |

### eMMC Detection Gotcha

```
eMMC devices may report as non-rotational (like SSDs) in lsblk.
Check for /dev/mmcblk* device names to identify eMMC.
Installer must REFUSE eMMC, not warn.
```


## 7. CPU Classification

| **Class** | **Condition** | **Behavior** | **Source** |
| :-: | :-: | :-: | :-: |
| "low-end" | bogomips < 2000/core (e.g., AMD A4) | ZRAM skipped, swap only | `HARDWARE.md`, `MEMORY.md` |
| "capable" | bogomips ≥ 2200/core, dual-core+ (e.g., Celeron N4020) | ZRAM enabled | `HARDWARE.md`, `MEMORY.md` |
| "modern" | Core i3/i5/i7/i9, Ryzen, Athlon (post-2018) | Full capabilities | `HARDWARE.md` |
| "Celeron/Pentium class" | Model name contains Celeron/Pentium/Atom (pre-2018) | Conservative defaults | `HARDWARE.md` |


## 8. Desktop & Display

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Display server | Wayland (KWin as compositor) | `ARCHITECTURE.md`, `DESKTOP.md` |
| XWayland | Available as fallback for X11-only apps | `ARCHITECTURE.md` |
| Desktop shell | Custom Spike Shell (NOT full KDE Plasma) | `ARCHITECTURE.md`, `DESKTOP.md` |
| Shell components | Panel, launcher, notification daemon, session manager, tray applet framework, settings panel, first-run wizard | `DESKTOP.md` |
| Panel position | Bottom (default) or top | `DESKTOP.md` |
| Panel height | 32px default (adjustable 24–48px) | `DESKTOP.md` |
| Panel zones | 3: left (launcher + pinned apps), center (clock), right (system tray) | `DESKTOP.md` |
| Launcher style | Kickoff-style (3-pane: favorites, categories, search) | `DESKTOP.md` |
| Super key | Opens launcher | `DESKTOP.md` |
| Notification invariant | **Disk before display** (write to disk, THEN show on screen) | `DESKTOP.md` |
| Notification history | `~/.local/share/spike/notifications/history.json` | `DESKTOP.md` |
| Notification retention | 3 days (configurable 1–31), max 500 entries (configurable) | `DESKTOP.md` |
| Notifications scope | Per-session (not visible to other users) | `DESKTOP.md` |
| Recently used (launcher) | Cleared on every logout | `DESKTOP.md` |
| Autostart filtering | Only approved autostart entries run | `DESKTOP.md` |
| Settings architecture | Hybrid: custom Spike pages + KDE KCMs from **standalone** packages only (never `plasma-desktop` / `plasma-workspace`) | `DESKTOP.md`, `DESIGN-DECISIONS.md` |
| Settings KCM allowlist | Display=`kscreen`, Sound=`plasma-pa`, Power=`powerdevil`, Bluetooth=`bluedevil`, Printer=`print-manager` | `DESKTOP.md` |
| Settings Network GUI | Custom Spike UI (tray + Settings); **not** `plasma-nm` (QtWebEngine) | `DESKTOP.md`, `NETWORKING.md` |
| Settings rule | Spike-config settings via `org.spike.Config`; live network connections via NetworkManager D-Bus | `DESKTOP.md`, `CONFIGURATION.md`, `NETWORKING.md` |
| Guest session | Not available (deliberate — complexity + data leakage risk) | `PRIVACY.md` |

### KDE Standalone Apps (Included)

```
Discover, Dolphin, Konsole, Kate, Ark, Spectacle, KCalc
+ Settings KCMs via standalone packages (kscreen, plasma-pa, powerdevil, bluedevil, print-manager)
```

Do **not** list full Plasma System Settings / `plasma-nm` as shipped apps — Spike hosts selected KCMs inside Spike Settings.

### Qt Rendering

| **Variant** | **Rendering Preference** | **Rationale** | **Source** |
| :-: | :-: | :-: | :-: |
| Standard | Software rasterizer preferred | Avoids GPU overhead on low-end | `VARIANT-DIFFERENCES.md` |
| Plus | OpenGL preferred | Better visual quality | `VARIANT-DIFFERENCES.md` |

### Cursor/Font/Icons

| **Property** | **Value** | **Source** |
| :-: | :-: | :-: |
| Font | Noto Sans 10pt | `BRANDING.md` |
| Monospace font | Noto Sans Mono | `BRANDING.md` |
| Icon theme | Breeze | `BRANDING.md` |
| Cursor | Breeze, 24px | `BRANDING.md` |
| Accent color | Purple (#6d4aff) | `BRANDING.md` |
| Secondary color | Cyan/teal | `BRANDING.md` |
| Wallpapers | `/usr/share/spike/wallpapers/` | `BRANDING.md` |


## 9. Package Management

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| System packages | apt | `ARCHITECTURE.md` |
| User applications | Flatpak **and** `.deb` via Discover (no Snap; AppImage not first-class) | `ARCHITECTURE.md`, `DESIGN-DECISIONS.md` |
| Snap | **REMOVED entirely** (snapd not installed) | `ARCHITECTURE.md` |
| Flatpak runtimes (Standard) | KDE + GNOME pre-seeded on ISO | `VARIANT-DIFFERENCES.md` |
| Flatpak runtimes (Plus) | All common runtimes pre-seeded on ISO | `VARIANT-DIFFERENCES.md` |
| Firefox delivery | Flatpak, Spike-tuned prefs | `MULTIMEDIA.md` |
| LibreOffice delivery | Flatpak | `ARCHITECTURE.md` |
| Firefox prefs location | `/usr/share/spike/firefox/spike-prefs.js` | `MULTIMEDIA.md` |

### Firefox Tuning Constraints

| **Pref** | **Value** | **Source** |
| :-: | :-: | :-: |
| `media.ffmpeg.vaapi.enabled` | `true` | `MULTIMEDIA.md` |
| `media.ffmpeg.vaapi.force-enabled` | `true` | `MULTIMEDIA.md` |
| `media.ffvpx.enabled` | `false` (disable FFmpeg VPX software decoder) | `MULTIMEDIA.md` |
| `media.av1.enabled` | `false` (force VP9 fallback → hardware decode) | `MULTIMEDIA.md` |
| `gfx.webrender.all` | `true` | `MULTIMEDIA.md` |

> **Do NOT enable AV1** even if Firefox suggests it. N4020 lacks AV1 hardware decode.


## 10. GPU & Video Acceleration

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Intel VA-API driver | `intel-media-va-driver-non-free` (LIBVA_DRIVER_NAME=iHD) | `MULTIMEDIA.md`, `HARDWARE.md` |
| AMD VA-API driver | `mesa-va-drivers` (LIBVA_DRIVER_NAME=radeonsi) | `MULTIMEDIA.md` |
| NVIDIA VA-API driver | `vdpau-va-driver` (or proprietary via VDPAU) | `MULTIMEDIA.md` |
| N4020 hardware decode | H.264, H.265 (8-bit), VP8, VP9 | `HARDWARE.md` |
| N4020 AV1 | **NOT supported** — disabled in Firefox | `HARDWARE.md`, `MULTIMEDIA.md` |
| VA-API config | Identical between variants | `MULTIMEDIA.md` |

> **Gotcha:** Intel Gemini Lake (N4020) uses iHD driver, NOT i965. Capital D, lowercase hd.


## 11. Audio (PipeWire)

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Audio daemon | PipeWire | `MULTIMEDIA.md` |
| Session manager | WirePlumber (minimal config) | `MULTIMEDIA.md` |
| Sample rate | 44100Hz (locked) | `MULTIMEDIA.md` |
| Channels | 2 (stereo only) | `MULTIMEDIA.md` |
| Resampler quality | 1 (low — saves CPU, same both variants) | `MULTIMEDIA.md` |
| Logging level | 1 (warnings only, suppressed) | `MULTIMEDIA.md` |
| JACK / pro audio | Not included | `MULTIMEDIA.md` |
| Volume range | 0–150% (boost above 100% = software amplification) | `MULTIMEDIA.md` |

### Bluetooth Audio Codecs

| **Variant** | **Codecs** | **Source** |
| :-: | :-: | :-: |
| Standard | SBC only | `MULTIMEDIA.md`, `VARIANT-DIFFERENCES.md` |
| Plus | SBC, AAC, LDAC, aptX | `MULTIMEDIA.md`, `VARIANT-DIFFERENCES.md` |

### Audio Routing Priority

```
Bluetooth → HDMI → headphone jack → speakers
```


## 12. Networking

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Network manager | NetworkManager (sole manager) | `NETWORKING.md` |
| DHCP client | dhclient | `NETWORKING.md` |
| DNS resolver | systemd-resolved (caches locally, no app can bypass) | `NETWORKING.md` |
| Firewall | ufw | `SECURITY.md`, `NETWORKING.md` |
| Firewall default incoming | DENY | `SECURITY.md` |
| Firewall default outgoing | ALLOW | `SECURITY.md` |
| Pre-configured firewall rule | 5353/udp (mDNS for printer discovery) | `NETWORKING.md` |
| VPN support | OpenVPN, WireGuard, IPSec/L2TP (via NetworkManager) | `NETWORKING.md` |
| Mobile broadband | Conditional on modem detection (ModemManager) | `NETWORKING.md` |
| Captive portal | Automatic detection, opens Firefox to login page | `NETWORKING.md` |
| Airplane mode | Via rfkill (disables Wi-Fi + Bluetooth) | `NETWORKING.md` |
| MAC randomization | Off by default (per-network optional for public Wi-Fi) | `PRIVACY.md` |

### DNS Configuration

| **Property** | **Value** | **Source** |
| :-: | :-: | :-: |
| Default DNS | Router-provided via DHCP | `NETWORKING.md` |
| Fallback DNS | 1.1.1.1, 8.8.8.8 | `NETWORKING.md` |
| DNS-over-TLS | Supported, disabled by default (user can enable) | `NETWORKING.md`, `PRIVACY.md` |
| DNS-over-HTTPS | Available in Firefox (disabled system-wide) | `PRIVACY.md` |
| App DNS bypass | Not permitted (prevents DNS leaks) | `NETWORKING.md` |

### Wi-Fi Firmware (ALL Pre-Installed on ISO)

| **Vendor** | **Driver/Firmware** | **Notes** | **Source** |
| :-: | :-: | :-: | :-: |
| Intel | `iwlwifi` | — | `NETWORKING.md` |
| Atheros | `ath9k`, `ath10k` | — | `NETWORKING.md` |
| Realtek | `rtl8723de` (`ant_sel=2` for improved signal), `rtl88xx` | — | `NETWORKING.md` |
| Broadcom | `bcmwl` | Requires MOK enrollment if Secure Boot enabled. `b43/ssb/bcma/brcmsmac` blacklisted. | `NETWORKING.md`, `SECURITY.md` |

> **Gotcha:** Broadcom + Secure Boot = MOK enrollment prompt at first boot, NOT during install.


## 13. Privacy (Absolute)

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Telemetry | **ZERO. Absolute and non-negotiable.** | `PRIVACY.md`, `SECURITY.md` |
| Analytics | Zero | `PRIVACY.md` |
| Crash reports | Zero | `PRIVACY.md` |
| Usage statistics | Zero | `PRIVACY.md` |
| "Anonymous" statistics | Zero — not even opt-out | `PRIVACY.md` |
| Cloud integration | NONE (no cloud sync, backup, auth, or settings sync) | `PRIVACY.md` |
| Cloud accounts | NONE (no "sign in with..." anywhere) | `PRIVACY.md` |
| Guest session | Not available (complexity + data leakage risk) | `PRIVACY.md` |

### Removed Ubuntu Components (Stripped at ISO Build)

```
ubuntu-report, apport, whoopsie, popularity-contest, landscape-client,
ubuntu-advantage-tools, motd-news, cloud-init, snapd
```

> None of these can be accidentally enabled — they are not installed.

### Complete Network Connection List

This is the **exhaustive** list of network connections Spike makes. Nothing else connects anywhere.

| **#** | **Connection** | **Destination** | **Data Sent** | **Frequency** | **Source** |
| :-: | :-: | :-: | :-: | :-: | :-: |
| 1 | apt update | archive.ubuntu.com | Package lists (no user data) | Every 6 hours | `PRIVACY.md` |
| 2 | Flatpak remote check | flathub.org | App metadata (no user data) | Every 6 hours | `PRIVACY.md` |
| 3 | NTP sync | pool.ntp.org | Time request (no user data) | On boot + periodic | `PRIVACY.md` |
| 4 | Connectivity check | connectivity-check.ubuntu.com | HTTP GET (no body) | On boot + periodic | `PRIVACY.md` |
| 5 | Captive portal detection | Redirect target | HTTP GET (no body) | When connecting to new network | `PRIVACY.md` |

> **If you add a network call not in this list, you have violated a non-negotiable constraint. STOP and escalate.**

### Camera/Microphone Indicators

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Panel indicator when camera/mic active | **NON-NEGOTIABLE** — cannot be hidden or disabled | `PRIVACY.md` |
| Indicator shows | Which app is using the device | `PRIVACY.md` |
| Click to disable/mute | Immediate kill available from panel | `PRIVACY.md` |
| Camera kill switch | Unloads `uvcvideo` module (apps get "not available") | `PRIVACY.md` |
| Microphone kill switch | Mutes all capture devices in PipeWire (apps get silence) | `PRIVACY.md` |
| Kill switch persistence | State persists across reboots | `PRIVACY.md` |
| Kill switch scope | Separate from Airplane Mode (not radio-based) | `PRIVACY.md` |

### App Permissions (Portal-Based)

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Permission system | `xdg-desktop-portal` (Wayland-native) | `PRIVACY.md` |
| Categories | files, camera, microphone, screen, notifications, background | `PRIVACY.md` |
| Permission states | Allowed, Ask, Denied, N/A | `PRIVACY.md` |
| Permission dialog buttons | Allow, Always Allow, Deny, Always Deny | `PRIVACY.md` |
| Storage | Per-application, revocable in Settings → Privacy | `PRIVACY.md` |
| New app default | Ask | `PRIVACY.md` |
| Pre-configured defaults | Firefox, VLC, LibreOffice, Spectacle | `PRIVACY.md` |

### Data Retention

| **Data Type** | **Retention** | **Configurable?** | **Source** |
| :-: | :-: | :-: | :-: |
| System logs (journald) | 7 days | Yes (1–30) | `PRIVACY.md` |
| Spike connection log | 7 days | Yes (1–30) | `PRIVACY.md` |
| Notification history | 3 days | Yes (1–31, max 500) | `PRIVACY.md` |
| Launcher recently used | Cleared on every logout | No | `PRIVACY.md` |
| Battery health history | Unlimited (until user resets) | No | `PRIVACY.md` |
| Clipboard | NOT stored (in-memory only, cleared on logout) | No | `PRIVACY.md` |
| All data location | Local only, never transmitted off-device | — | `PRIVACY.md` |

### Multi-User Isolation

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Home directory permissions | 750 (other users cannot access) | `PRIVACY.md` |
| Notifications | Per-session | `PRIVACY.md` |
| Launcher favorites/recent | Per-user | `PRIVACY.md` |
| Settings (user prefs) | `~/.config/spike/` | `PRIVACY.md` |
| Settings (system-wide) | Admin only | `PRIVACY.md` |
| Browser data | Per-user, isolated by Flatpak | `PRIVACY.md` |
| Clipboard | Cleared on user switch | `PRIVACY.md` |

### Location Services

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Default state | Disabled (opt-in) | `PRIVACY.md` |
| Accuracy | Approximate (city-level, IP-based — no GPS) | `PRIVACY.md` |
| Daemon | geoclue2 (does not start if disabled) | `PRIVACY.md` |
| Apps requesting location | Blocked if disabled at portal level | `PRIVACY.md` |


## 14. Security

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Threat model | Beginner user's laptop on untrusted networks (NOT enterprise, NOT nation-state, NOT physical tamper resistance) | `SECURITY.md` |
| Root account | Locked | `SECURITY.md` |
| SSH server | Not installed (client only — `openssh-client`) | `SECURITY.md` |
| Antivirus | None (defense in depth via sandboxing, AppArmor, firewall, no auto-execution) | `SECURITY.md` |
| AppArmor | Enabled with Ubuntu defaults, no custom profiles | `SECURITY.md` |
| Secure Boot | Supported but not required | `SECURITY.md` |
| Screen lock on blank | Auto-lock | `SECURITY.md` |
| Password on wake | Required (default) | `SECURITY.md` |
| Lock on user switch | Yes | `SECURITY.md` |
| Lock minimum timeout | 1 minute (cannot be set lower — security floor) | `SECURITY.md` |

### Authentication

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Model | Standard user/sudo (Ubuntu default) | `SECURITY.md` |
| One password for | Login, sudo, polkit, screen unlock | `SECURITY.md` |
| Min password length | 6 characters | `SECURITY.md` |
| Password complexity | No requirements | `SECURITY.md` |
| Password expiration | None | `SECURITY.md` |
| Auto-login | Off by default (optional in settings) | `SECURITY.md` |
| sudo lecture | First-use message explaining what sudo does | `SECURITY.md` |

### Automatic Updates

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Security updates | Auto-install (background, idle 10+ min, low CPU) | `SECURITY.md` |
| Update timer | `spike-update.timer` every 6 hours | `SECURITY.md` |
| Non-security updates | Notify via Discover (user installs) | `SECURITY.md` |
| Flatpak updates | Notify only (user installs via Discover) | `SECURITY.md` |
| Forced reboot | **NEVER** — gentle notification only | `SECURITY.md` |

### Firefox Security (Spike-Managed)

| **Pref** | **Value** | **Source** |
| :-: | :-: | :-: |
| Enhanced Tracking Protection | Strict | `SECURITY.md` |
| HTTPS-Only mode | Enabled | `SECURITY.md` |
| Popup blocking | Enabled | `SECURITY.md` |
| Flash/Java | Not available | `SECURITY.md` |
| DRM | Enabled (user choice for streaming) | `SECURITY.md` |

### Contributor Security Checklist

```
- No secrets in code
- No hardcoded credentials
- Dependencies checked for known vulnerabilities
- Config files validated before write
- No network calls from system services except the 5 documented in PRIVACY.md
```


## 15. Power Management

### CPU Governor

| **Variant** | **Default Governor** | **Source** |
| :-: | :-: | :-: |
| Standard | `powersave` | `POWER-MANAGEMENT.md`, `VARIANT-DIFFERENCES.md` |
| Plus | `schedutil` | `POWER-MANAGEMENT.md`, `VARIANT-DIFFERENCES.md` |
| Intel pstate | Preferred for Intel CPUs | `POWER-MANAGEMENT.md` |
| Performance mode | Available as temporary override (AC only) | `POWER-MANAGEMENT.md` |
| Revert behavior | Returns to variant default when profile changes | `POWER-MANAGEMENT.md` |

### Power Profiles

| **Profile** | **Trigger** | **Governor** | **Wi-Fi/BT/USB Autosuspend** | **Dimming** | **Source** |
| :-: | :-: | :-: | :-: | :-: | :-: |
| Performance | AC power | schedutil/performance | OFF | None | `POWER-MANAGEMENT.md` |
| Battery Saver | Battery < 50% | powersave | ON | Aggressive | `POWER-MANAGEMENT.md` |
| Critical | Battery < 20% | powersave | ON | Dim to 30%, urgent notifications, auto-shutdown countdown at 5% | `POWER-MANAGEMENT.md` |

### Screen Management

| **Property** | **AC** | **Battery** | **Source** |
| :-: | :-: | :-: | :-: |
| Screen blank | 15 min | 5 min | `POWER-MANAGEMENT.md` |
| Dimming (idle) | 5 min, target 50% | 3 min, target 50% | `POWER-MANAGEMENT.md` |
| Lid close | Suspend (configurable) | Suspend (configurable) | `POWER-MANAGEMENT.md` |
| Power button | Suspend (configurable) | Suspend (configurable) | `POWER-MANAGEMENT.md` |
| Night Light | Manual schedule or sunset-to-sunrise (requires location) | Same | `POWER-MANAGEMENT.md` |

### Sleep States

| **State** | **Default** | **Variant Difference** | **Source** |
| :-: | :-: | :-: | :-: |
| Suspend | Default sleep action | Same both variants | `POWER-MANAGEMENT.md` |
| Hibernate | Available if swap ≥ RAM | Same both variants | `POWER-MANAGEMENT.md` |
| Hybrid sleep | ON | Standard: ON (safety net), Plus: OFF | `POWER-MANAGEMENT.md`, `VARIANT-DIFFERENCES.md` |
| Require password on wake | YES | Same both variants | `POWER-MANAGEMENT.md` |

### Battery Health

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Monitoring | Percentage, capacity, cycle count, charge limiting (if supported) | `POWER-MANAGEMENT.md` |
| Warnings | 20% (notification), 10% (urgent), 5% (auto-shutdown countdown) | `POWER-MANAGEMENT.md` |
| Health history location | `/var/lib/spike/power/` | `POWER-MANAGEMENT.md` |
| Health history retention | Unlimited (until user resets) | `POWER-MANAGEMENT.md` |
| Battery data transmission | Never transmitted off-device | `POWER-MANAGEMENT.md` |

### Peripheral Power Saving

| **Device** | **Performance** | **Battery Saver** | **Source** |
| :-: | :-: | :-: | :-: |
| Wi-Fi power saving | OFF | ON (adaptive) | `POWER-MANAGEMENT.md` |
| Bluetooth power saving | OFF | ON (adaptive) | `POWER-MANAGEMENT.md` |
| USB autosuspend | OFF | ON (adaptive) | `POWER-MANAGEMENT.md` |
| HDD spindown | 15 min | 10 min (HDD only) | `POWER-MANAGEMENT.md` |


## 16. Variant System

### Variant Selection

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| ISO | Single ISO for both variants | `VARIANT-DIFFERENCES.md` |
| Selection time | At install, based on hardware detection | `INSTALLER.md` |
| User override | Allowed (warning shown if mismatched) | `INSTALLER.md` |
| Post-install switching | Available (no reinstall) | `VARIANT-DIFFERENCES.md` |
| Switch command | \`spike-config --state-set system variant <plus | standard>`then`spike-config --generate-all\` |
| Switch effects | Some immediate, some require re-login, some require reboot | `VARIANT-DIFFERENCES.md` |

### The 14 Differences

Everything NOT listed here is identical between variants.

| **#** | **Feature** | **Standard** | **Plus** |
| :-: | :-: | :-: | :-: |
| 1 | ZRAM cap | 4GB max | Uncapped (up to RAM) |
| 2 | Animations | Off | On (150–200ms transitions) |
| 3 | Compositor effects | Minimal (no blur/transparency/shadows) | Full (blur, transparency, shadows) |
| 4 | Plymouth theme | spike-minimal | spike-full |
| 5 | CPU governor | powersave | schedutil |
| 6 | Bluetooth codecs | SBC only | SBC, AAC, LDAC, aptX |
| 7 | Flatpak runtimes | KDE + GNOME | All common |
| 8 | Applet polling | Conservative (10s/30s) | Standard (5s/15s) |
| 9 | Qt rendering | Software rasterizer preferred | OpenGL preferred |
| 10 | Spike button hover | No glow | Subtle purple glow |
| 11 | Hybrid sleep | ON | OFF |
| 12 | Idle RAM target | <400MB | <800MB |
| 13 | Background services | Minimal set | Standard set |
| 14 | Memory budget | ~280–370MB | ~420–650MB |

### Identical Between Variants

```
kernel, filesystem, partitioning, installer, desktop shell, networking,
security, privacy, disaster recovery, configuration system, updates,
community model, ISO, packages
```


## 17. Disaster Recovery

### 4-Layer Recovery Model

| **Layer** | **Name** | **Trigger** | **Environment** | **Source** |
| :-: | :-: | :-: | :-: | :-: |
| 1 | Boot failure counter | 3 consecutive failed boots | Normal boot, GRUB menu auto-shows | `DISASTER-RECOVERY.md` |
| 2 | Recovery mode | Manual selection from GRUB | Single-user root shell, no graphics/network | `DISASTER-RECOVERY.md` |
| 3 | Spike Rescue (GUI) | Boot from USB installer | Live ISO, mounts broken system read-only, copies data to USB with SHA256 verification | `DISASTER-RECOVERY.md` |
| 4 | Reinstall + restore | Boot from USB installer | Detects existing Spike, offers fresh install + restore from SpikeBackup folder | `DISASTER-RECOVERY.md` |

### Recovery Promise

> "Your personal files are always recoverable."

### Data Restore Constraints

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Restore scope | Personal files only: Documents, Pictures, Videos, Music, Downloads, Desktop | `DISASTER-RECOVERY.md` |
| Does NOT restore | System configs, app configs, browser data | `DISASTER-RECOVERY.md` |
| File ownership | Fixed during restore (chown to new user) | `DISASTER-RECOVERY.md` |
| Backup verification | SHA256 checksums | `DISASTER-RECOVERY.md` |
| Filesystem error handling | Graceful — copy what's salvageable | `DISASTER-RECOVERY.md` |


## 18. Installer

### 10-Step Flow

| **Step** | **Name** | **Source** |
| :-: | :-: | :-: |
| 1 | Language | `INSTALLER.md` |
| 2 | Timezone (IP-based, user confirms) | `INSTALLER.md` |
| 3 | Wi-Fi (connect for firmware/updates if needed) | `INSTALLER.md` |
| 4 | Username | `INSTALLER.md` |
| 5 | Password (with reminder hint field) | `INSTALLER.md` |
| 6 | Hostname (suggests spike-laptop, user can change) | `INSTALLER.md` |
| 7 | Storage confirmation (data backup option, wipe warning) | `INSTALLER.md` |
| 8 | Variant selection (recommended based on hardware, user can override) | `INSTALLER.md` |
| 9 | Install (automated, progress bar, no user interaction) | `INSTALLER.md` |
| 10 | Reboot to desktop | `INSTALLER.md` |

### Data Backup (Step 7)

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Scan target | Windows/Linux/macOS personal files | `INSTALLER.md` |
| Backup destination | USB before wiping | `INSTALLER.md` |
| Verification | SHA256 checksums | `INSTALLER.md` |
| Existing Spike detection | Offers "Fresh install" or "Fresh install and restore my data" | `INSTALLER.md` |

### Hardware Detection (Runs During Step 9)

| **Detector** | **Output** | **Source** |
| :-: | :-: | :-: |
| `detect/cpu.cpp` | CPU model, cores, bogomips, classification | `INSTALLER.md` |
| `detect/storage.cpp` | Storage type, size, rotational, eMMC/USB check | `INSTALLER.md` |
| `detect/gpu.cpp` | GPU vendor, model, driver, VA-API driver | `INSTALLER.md` |
| `detect/network.cpp` | Wi-Fi/Ethernet, driver module | `INSTALLER.md` |
| `detect/bluetooth.cpp` | Bluetooth adapter presence | `INSTALLER.md` |
| `detect/modem.cpp` | Mobile broadband modem presence | `INSTALLER.md` |

### Installation Tasks (Automated During Step 9)

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


## 19. Branding

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Organization | BigRangaTech | `BRANDING.md` |
| Logo | Herding dog (Spike) with circuit board flanks, purple/cyan glow, circular emblem | `BRANDING.md` |
| Splash screen | BigRangaTech branded, Spike dog, dark background with glowing circuit traces | `BRANDING.md` |
| Slogan | "Let's Make Tech Repairable Again" | `BRANDING.md` |
| GRUB theme | Dark background, Spike emblem, BigRangaTech branding, purple/cyan accents | `BRANDING.md` |


## 20. Licensing

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Code license | GPLv2+ | `GOVERNANCE.md`, `AGENTS.md` |
| Documentation license | CC-BY-SA 4.0 | `GOVERNANCE.md`, `AGENTS.md` |
| Artwork license | CC-BY-SA 4.0 | `GOVERNANCE.md`, `AGENTS.md` |
| Dependency requirement | GPL-compatible only — do not import proprietary or GPL-incompatible dependencies | `GOVERNANCE.md` |

### License Compatibility

GPLv2+ is compatible with: KWin, KDE Frameworks (GPLv2+/LGPLv2.1+), Flatpak (LGPLv2.1+), Ubuntu packages (various GPL-compatible licenses).


## 21. Governance

| **Constraint** | **Value** | **Source** |
| :-: | :-: | :-: |
| Model | BDFL (Benevolent Dictator For Life) | `GOVERNANCE.md` |
| Decision authority | Creator (BDFL) holds final decision authority | `GOVERNANCE.md` |
| Succession | Documented plan exists | `GOVERNANCE.md` |
| Canonical trademark | Compliance documented (using Ubuntu base) | `GOVERNANCE.md` |
| DCO over CLA | DCO chosen (lighter barrier to entry) | `CONTRIBUTING.md` |


## Constraint Violation Response

If you detect that code (yours or existing) violates any constraint in this file:

1. **STOP implementing immediately** 

2. **Do not attempt to "fix" it without authorization** 

3. **Flag to human:** 

> "CONSTRAINT VIOLATION DETECTED: [describe violation]. Constraint: [cite constraint from this file]. Affected file: [path]."

4. **Wait for human decision** 

5. **Log in `DECISIONS.md`** as a constraint violation with resolution 

6. **Update `SESSION_LOG.md`** with details 

**Never silently fix a constraint violation.** The human needs to understand what happened and why before any fix is applied.


🐕 BigRangaTech

