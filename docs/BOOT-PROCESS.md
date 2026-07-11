# Spike Boot Process

## Purpose

This document specifies the complete boot sequence of a Spike system — from the moment the user presses the power button to the moment the desktop is ready. It covers firmware, bootloader, kernel, initramfs, systemd, session startup, and the boot failure recovery mechanism.

## Boot Sequence Overview

```
`Power button pressed`

`        │`

`        ▼`

`┌──────────────────────────┐`

`│  1. Firmware (UEFI/BIOS)  │`

`│     POST, hardware init   │`

`└──────────┬───────────────┘`

`           │`

`           ▼`

`┌──────────────────────────┐`

`│  2. GRUB2 Bootloader      │`

`│     Load kernel + initrd  │`

`└──────────┬───────────────┘`

`           │`

`           ▼`

`┌──────────────────────────┐`

`│  3. Linux Kernel          │`

`│     Hardware detection,   │`

`│     mount root filesystem │`

`└──────────┬───────────────┘`

`           │`

`           ▼`

`┌──────────────────────────┐`

`│  4. Initramfs             │`

`│     Decrypt (N/A), mount │`

`│     root, pivot\_root      │`

`└──────────┬───────────────┘`

`           │`

`           ▼`

`┌──────────────────────────┐`

`│  5. systemd (PID 1)       │`

`│     Mount filesystems,    │`

`│     start services        │`

`└──────────┬───────────────┘`

`           │`

`           ▼`

`┌──────────────────────────┐`

`│  6. Spike Session         │`

`│     KWin, Shell, Panel,   │`

`│     Applets, Desktop      │`

`└──────────┬───────────────┘`

`           │`

`           ▼`

`┌──────────────────────────┐`

`│  7. Desktop Ready          │`

`│     Boot failure counter   │`

`│     cleared, user active  │`

`└──────────────────────────┘`
```

### Estimated Timing (Celeron N4020, SSD)

| **Stage** | **Estimated Time** |
| :-: | :-: |
| 1. Firmware (POST) | 2-5 seconds (hardware-dependent) |
| 2. GRUB2 | 0.5-1 second (hidden, no timeout) |
| 3. Kernel | 3-5 seconds |
| 4. Initramfs | 1-2 seconds |
| 5. systemd | 5-8 seconds |
| 6. Spike Session | 3-5 seconds |
| **Total cold boot** | **15-26 seconds (target: \<40s)** |

Firmware is the most variable stage. Older laptops with slow UEFI implementations may take 5-8 seconds at POST. Spike cannot control this.

## 1. Firmware Stage

### UEFI (Default)

Spike installs in UEFI mode when the firmware supports it.

```
`UEFI boot path:`

`├── Firmware reads boot entry from NVRAM`

`├── Boot entry points to \\EFI\\spike\\grubx64.efi`

`├── GRUB2 EFI stub loaded by firmware`

`└── Control passes to GRUB2`
```

**Secure Boot:**

```
`├── Spike does NOT enroll custom keys`

`├── If Secure Boot is enabled in firmware:`

`│   ├── Ubuntu's signed GRUB2 shim is used (ubuntu-shim)`

`│   ├── Kernel is signed by Ubuntu's build key`

`│   ├── Boot proceeds normally`

`│   └── Third-party kernel modules (Broadcom bcmwl) may require MOK enrollment`

`└── If Secure Boot is disabled:`

`    └── Boot proceeds normally without shim`
```

**Fast Boot:**

```
`├── Some UEFI firmwares have "Fast Boot" or "Ultra Fast Boot"`

`├── These skip USB initialization at POST`

`├── Spike does not rely on USB at boot (no USB boot devices in normal operation)`

`└── If user needs to boot from USB (recovery), they must disable Fast Boot`

`    └── Instructions in TROUBLESHOOTING.md and user guide`
```

### BIOS (Legacy Fallback)

Spike installs in BIOS mode when UEFI is not available.

```
`BIOS boot path:`

`├── BIOS reads MBR (or boots from designated disk)`

`├── GRUB2 stage 1.5 loaded from MBR gap`

`├── GRUB2 core image loaded from /boot/grub/`

`└── Control passes to GRUB2`
```

Most Celeron-era laptops (2018+) support UEFI. BIOS mode exists primarily for older rescue targets. All features work identically in both modes.

### What Spike Does NOT Configure

Spike does not modify firmware settings. Specifically:

```
`├── Does not change boot order`

`├── Does not enable/disable Secure Boot`

`├── Does not enable/disable Fast Boot`

`├── Does not modify VT-d/IOMMU settings`

`└── Does not modify power management firmware settings`
```

If firmware-level changes are needed, the installer instructs the user in plain language (e.g., "Your computer's firmware needs to allow booting from USB. See your computer's manual for how to change this setting.")

## 2. GRUB2 Bootloader

### Configuration

GRUB2 is the sole bootloader. No alternative bootloaders are installed. Limine migration is planned post-1.0.

```
`Installed files:`

`├── /boot/grub/grub.cfg          → Generated, never hand-edited`

`├── /etc/default/grub             → Spike's GRUB defaults`

`├── /etc/grub.d/                   → GRUB configuration scripts`

`│   ├── 00\_header                  → Standard GRUB header`

`│   ├── 05\_debian\_theme            → Debian/Ubuntu theme hook`

`│   ├── 10\_linux                   → Main boot entry generation`

`│   ├── 20\_spike                   → Spike custom entries`

`│   └── 30\_os-prober               → DISABLED (no dual boot)`

`└── /boot/grub/themes/spike/       → Spike GRUB theme assets`
```

**`/etc/default/grub`:**

```
*`\# Spike GRUB2 configuration`*

*`\# This file is generated by spike-config at install time.`*

*`\# Users should NOT edit this file. Changes are made via`*

*`\# Settings → Advanced → Boot.`*


*`\# Boot menu behavior`*

`GRUB\_TIMEOUT=0`

`GRUB\_TIMEOUT\_STYLE=hidden`

`GRUB\_DEFAULT=0`

`GRUB\_DISABLE\_RECOVERY=false`


*`\# Display`*

`GRUB\_THEME=/boot/grub/themes/spike/theme.txt`

`GRUB\_GFXMODE=auto`

`GRUB\_GFXPAYLOAD\_LINUX=keep`

`GRUB\_BACKGROUND=/boot/grub/themes/spike/background.png`


*`\# Kernel parameters (applied to all boot entries)`*

`GRUB\_CMDLINE\_LINUX\_DEFAULT="quiet splash zswap.enabled=0 transparent\_hugepage=madvise"`

`GRUB\_CMDLINE\_LINUX=""`


*`\# Disable os-prober (no dual boot scanning)`*

`GRUB\_DISABLE\_OS\_PROBER=true`


*`\# Terminal`*

`GRUB\_TERMINAL\_OUTPUT=gfxterm`

`GRUB\_FONT=/boot/grub/themes/spike/font.pf2`


*`\# Disable submenu (flat list of entries)`*

`GRUB\_DISABLE\_SUBMENU=y`
```

### Boot Parameters Explained

| **Parameter** | **Purpose** |
| :-: | :-: |
| `quiet` | Suppress kernel messages on console (Plymouth handles display) |
| `splash` | Enable Plymouth boot splash |
| `zswap.enabled=0` | Disable zswap (conflicts with ZRAM — see MEMORY.md) |
| `transparent\_hugepage=madvise` | Use THP only for `madvise()` callers (prevents THP bloat) |

### Additional Parameters (Applied Per-Storage-Type)

At install time, spike-config adds storage-specific parameters:

| **Storage Type** | **Additional Parameters** |
| :-: | :-: |
| SSD | (none additional — noatime handled in fstab) |
| HDD | (none additional — commit=60 handled in fstab) |
| SD Card / USB | (none additional — handled in fstab) |

### Boot Entries

GRUB generates these entries (in order):

**1. "Spike" (default)**

```
`├── Boots the most recent kernel`

`├── Uses /boot/vmlinuz-x.x.x-xx-generic`

`├── Uses /boot/initrd.img-x.x.x-xx-generic`

`└── Hidden by default (no menu shown)`
```

**2. "Spike, with previous kernel" (linux-old)**

```
`├── Boots the previous kernel version (kept as rollback)`

`├── Only present if kernel update has occurred`

`└── Highlighted automatically on boot failure (see below)`
```

**3. "Spike Recovery" (recovery mode)**

```
`├── Boots into recovery environment`

`├── Drops to root shell with minimal services`

`├── Mounts root filesystem read-only by default`

`└── Provides: spike-rescue, fsck, remount rw, root shell`
```

### Hidden Menu Behavior

```
`Normal boot (no issues):`

`├── GRUB loads silently`

`├── No menu displayed`

`├── Default entry booted immediately`

`├── 3-second ESC window (user can interrupt)`

`└── If ESC not pressed: boot proceeds`


`ESC window:`

`├── During the first 3 seconds after GRUB loads:`

`│   ├── Pressing ESC shows the full menu`

`│   ├── Menu stays until user selects an entry`

`│   └── Default timeout for visible menu: 10 seconds`

`├── If ESC not pressed within 3 seconds:`

`│   └── Boot proceeds with default entry`

`└── ESC window length configurable in Settings → Boot (1-10 seconds)`


`3-second default rationale:`

`├── Long enough for a deliberate user to press ESC`

`├── Short enough not to annoy users who don't want a menu`

`└── Beginners won't know to press ESC — they just boot`
```

### Boot Failure Triggered Menu

If `/boot/.spike/boot-count \>= 3`:

```
`├── GRUB menu appears automatically`

`├── No ESC required`

`├── Timeout: 15 seconds (longer than normal — gives user time)`

`├── Previous kernel entry highlighted (not default)`

`├── Recovery entry visible and labeled clearly`

`└── Visual indication: red warning text at top of menu`
```

Menu text:

```
`┌────────────────────────────────────────────────┐`

`│                                                │`

`│  ⚠️  Spike failed to start 3 times.            │`

`│                                                │`

`│  Try using the previous kernel version,        │`

`│  or select Recovery to rescue your files.      │`

`│                                                │`

`│  \> Spike, with previous kernel                 │`

`│    Spike                                       │`

`│    Spike Recovery                              │`

`│                                                │`

`│  Booting selected entry in 15 seconds...       │`

`│                                                │`

`└────────────────────────────────────────────────┘`
```

This is the ONLY time a typical user will see the GRUB menu.

### GRUB Theme

**Theme:** Spike Circuit **Location:** `/boot/grub/themes/spike/`

```
`Assets:`

`├── background.png     → Dark background (\#1a1a2e) with circuit trace pattern`

`├── theme.txt           → Theme definition file`

`├── font.pf2            → PF2 font (regular and bold)`

`├── spike-logo.png     → Spike dog emblem (centered, top)`

`├── spinner/            → Animated loading indicator (simple)`

`└── colors:`

`    ├── Background:    \#1a1a2e (dark navy)`

`    ├── Text:          \#e0e0e0 (light gray)`

`    ├── Highlighted:   \#6d4aff (Spike purple)`

`    ├── Highlighted bg: \#2a2a4e`

`    └── Warning:       \#f44336 (red)`

`Theme appearance:`

`├── Spike logo centered at top`

`├── Boot entries listed below logo`

`├── Selected entry highlighted in purple`

`├── Countdown timer at bottom`

`└── No excessive decoration (keep rendering fast)`
```

### GRUB Update Behavior

```
`When kernel updates occur:`

`├── New kernel installed to /boot/`

`├── Previous kernel retained (one version back)`

`├── update-grub runs automatically (apt hook)`

`├── grub.cfg regenerated`

`├── Default entry points to new kernel`

`└── Previous kernel entry available as fallback`


`If kernel update breaks boot:`

`├── System fails to boot with new kernel`

`├── Boot failure counter increments`

`├── After 3 failures: GRUB menu shows with previous kernel highlighted`

`├── User selects previous kernel → system boots`

`├── User can then uninstall problematic kernel via Discover`

`└── Settings → Advanced → Boot shows boot failure count and reset option`
```

## 3. Linux Kernel Stage

### Kernel Loading

```
`GRUB loads:`

`├── /boot/vmlinuz-x.x.x-xx-generic (kernel image)`

`└── /boot/initrd.img-x.x.x-xx-generic (initial ramdisk)`


`Kernel begins execution:`

`├── Decompresses itself into memory`

`├── Initializes CPU, memory management`

`├── Parses kernel command line (from GRUB)`

`├── Mounts initial ramfs (initramfs)`

`└── Hands control to initramfs init script`
```

### Kernel Messages

```
`With "quiet splash" parameters:`

`├── Kernel boot messages suppressed on console`

`├── Plymouth splash screen displayed instead`

`├── Messages still captured in kernel ring buffer`

`└── Accessible later via: journalctl -k or dmesg`


`Without splash (recovery mode):`

`├── Kernel messages displayed on console`

`├── Useful for diagnosing boot failures`

`└── Recovery mode always shows messages`
```

### Module Loading During Kernel Phase

```
`Built-in modules (compiled into kernel):`

`├── ext4 filesystem driver`

`├── Core USB subsystem`

`├── Core ACPI`

`└── Console framebuffer`


`Modules loaded from initramfs:`

`├── Storage drivers (ahci, nvme, sdhci, usb-storage)`

`├── LVM/crypto modules (if needed — typically not)`

`├── Filesystem modules`

`└── Input drivers (keyboard, basic input)`


`Modules loaded after root mount (from /lib/modules/):`

`├── Network drivers (iwlwifi, ath, rtl, etc.)`

`├── GPU drivers (i915, amdgpu, nouveau)`

`├── Audio drivers (snd-hda-intel, snd-sof)`

`├── Bluetooth (btusb, btintel)`

`└── Everything else via udev and modprobe`
```

### Module Blacklisting

At install time, spike-config generates `/etc/modprobe.d/spike-blacklist.conf`

**Rules:**

```
`├── Only ABSENT soldered hardware is blacklisted`

`├── Never blacklist hot-pluggable or USB devices`

`├── Never blacklist a module for hardware that might be connected later`

`├── If hardware is not detected at install time AND is not hot-pluggable: blacklist it`
```

**Blacklist generation process:**

```
`1. Scan PCI devices (lspci -nn)`

`2. Scan USB devices (lsusb)`

`3. For each kernel module:`

`   a. Determine what hardware it supports`

`   b. If that hardware is NOT present on this machine:`

`      - Is it soldered/PCI? → blacklist it`

`      - Is it USB/hot-pluggable? → DO NOT blacklist (might be plugged in later)`

`4. Write blacklist file`

`5. Update initramfs`
```

**Example blacklist entries:**

```
*`\# Spike module blacklist — generated at install time`*

*`\# Hardware: Intel N4020, Intel UHD 600, no Bluetooth`*

`blacklist nouveau        *\# No NVIDIA GPU present (PCI, soldered)`*

`blacklist amdgpu         *\# No AMD GPU present (PCI, soldered)`*

`blacklist radeon         *\# No legacy AMD GPU present`*

`blacklist btusb          *\# No Bluetooth controller (PCI/USB soldered)`*

`blacklist snd\_hda\_codec\_realtek  *\# No Realtek audio codec (using Intel SST)`*
```

**NOT blacklisted (even if absent):**

```
*`\# iwlwifi — might plug in a USB Wi-Fi adapter... wait, no`*

*`\# Actually: iwlwifi is for Intel Wi-Fi which IS present`*

*`\# Below are examples of what NOT to blacklist:`*

*`\# usb-storage  — user might plug in USB drive`*

*`\# usbhid       — user might plug in USB keyboard/mouse`*

*`\# uas          — user might use USB attached SCSI device`*
```

### GPU Driver Initialization

**Intel (i915):**

```
`├── Module: i915 (built into initramfs)`

`├── Loads during initramfs stage (early KMS)`

`├── Display switches to native resolution immediately`

`├── Plymouth can use DRM rendering`

`└── No firmware loading needed (Gemini Lake: none required)`
```

**AMD (amdgpu):**

```
`├── Module: amdgpu (built into initramfs)`

`├── Loads during initramfs stage (early KMS)`

`├── May require firmware from /lib/firmware/amdgpu/`

`└── Firmware must be in initramfs if early KMS is desired`
```

**NVIDIA (nouveau):**

```
`├── Module: nouveau (in initramfs)`

`├── Loads during initramfs stage`

`├── Basic display works (no 3D acceleration on newer cards)`

`├── Proprietary driver NOT loaded (see NVIDIA section in KERNEL.md)`

`└── If proprietary driver installed post-install:`

`    ├── nouveau blacklisted by DKMS`

`    ├── nvidia module loaded instead`

`    └── Initramfs rebuilt to include nvidia module`
```

## 4. Initramfs Stage

### Purpose

The initramfs (initial RAM filesystem) provides the minimal environment needed to mount the real root filesystem. Spike uses the standard Ubuntu initramfs with minor customization.

### Boot Sequence

```
`1. Kernel mounts initramfs (tmpfs in memory)`

`2. /init script executes:`

`   ├── Parse kernel command line`

`   ├── Load essential modules (storage, filesystem)`

`   ├── Initialize early DRM/KMS (GPU driver)`

`   ├── Mount /dev, /proc, /sys (virtual filesystems)`

`   ├── Find root filesystem:`

`   │   ├── Read root= parameter from kernel cmdline`

`   │   ├── Wait for device (up to 30 seconds)`

`   │   └── If not found: drop to recovery shell`

`   ├── Mount root filesystem read-only`

`   ├── INCREMENT BOOT FAILURE COUNTER`

`   │   (read /boot/.spike/boot-count, increment, write back)`

`   ├── Run fsck on root filesystem (if needed)`

`   ├── Mount root filesystem read-write (if fsck passed)`

`   ├── Pivot root: switch from initramfs to real root`

`   └── exec /sbin/init (systemd)`
```

### Boot Failure Counter (Initramfs Stage)

The boot failure counter is incremented VERY early — in the initramfs, before systemd starts. This ensures that even if systemd or spike-session fails, the counter has already been incremented for this boot attempt.

**Counter file:** `/boot/.spike/boot-count`

**Increment logic (in initramfs hook):**

```
`┌──────────────────────────────────────────────────────┐`

`│ 1. Mount /boot (or / if /boot is not                 │`

`│    a separate partition) read-write                  │`

`│ 2. Read current count from file                      │`

`│    (default to 0 if file doesn't exist)              │`

`│ 3. Increment by 1                                    │`

`│ 4. Write new count to file                           │`

`│ 5. Sync filesystem                                   │`

`│ 6. Unmount /boot (if separately mounted)             │`

`└──────────────────────────────────────────────────────┘`
```

This happens BEFORE any user-space services start. If the system hangs during systemd or spike-session, the counter has already been incremented.

The counter is only CLEARED when spike-session successfully starts (see Section 6).

### Recovery Shell

If root filesystem cannot be found or mounted:

```
`┌──────────────────────────────────────────────────────┐`

`│                                                      │`

`│  Spike could not start properly.                     │`

`│                                                      │`

`│  Your files are safe. You can recover them           │`

`│  using the Spike Rescue tool.                        │`

`│                                                      │`

`│  Options:                                            │`

`│  1. \[Drop to root shell\] — for advanced              │`

`│     recovery (requires terminal knowledge)           │`

`│  2. \[Reboot to live USB\] — insert Spike USB          │`

`│     and use "Rescue My Files"                        │`

`│                                                      │`

`└──────────────────────────────────────────────────────┘`
```

This screen appears instead of the standard Ubuntu initramfs rescue prompt. It provides clear guidance rather than a cryptic `(initramfs)` prompt.

Root shell (option 1) is available but discouraged. The user guide and documentation always recommend option 2 (live USB rescue) as the primary recovery path, in accordance with Golden Rule 2.

### Initramfs Customization

Spike modifies the Ubuntu initramfs in these ways:

1. **Boot failure counter hook** 

   - `/etc/initramfs-tools/scripts/init-premount/spike-boot-count` 

2. **Recovery shell replacement** 

   - `/etc/initramfs-tools/scripts/init-bottom/spike-recovery` 

3. **Module inclusion (storage + GPU for early KMS)** 

   - `/etc/initramfs-tools/modules` (additions) 

4. **Custom messaging** 

   - `/etc/initramfs-tools/conf.d/spike.conf` (quiet/splash tweaks) 

All customizations are applied by spike-config at install time. `update-initramfs` is run as the final installation step.

## 5. systemd Stage

### Boot Target

**Default target:** `graphical.target`

This is equivalent to runlevel 5 in SysV init. It means: bring up all services including the display manager.

Spike does NOT use `multi-user.target` (text mode) as default. The system always boots to graphical mode.

**Recovery mode uses:** `rescue.target`

### Service Startup Order

systemd starts services in dependency order:

**Phase 1 — Early boot:**

```
`├── systemd-journald          → Logging (starts first)`

`├── systemd-udevd             → Device manager (triggers module loading)`

`├── systemd-modules-load      → Load modules from /etc/modules-load.d/`

`├── keyboard-setup            → Keyboard layout`

`├── fake-hwclock              → Save/restore hardware clock (if no RTC battery)`

`└── lvm2-monitor              → (inactive — no LVM configured)`
```

**Phase 2 — Filesystems:**

```
`├── local-fs.target           → Mount local filesystems (/, /boot/efi)`

`├── swap.target               → Activate swap file`

`├── systemd-tmpfiles-setup    → Create temporary directories`

`└── zram-init                 → Configure ZRAM device (if applicable)`
```

**Phase 3 — Core services:**

```
`├── dbus.service              → DBus system bus`

`├── NetworkManager.service    → Network management`

`├── systemd-resolved.service  → DNS resolution`

`├── systemd-timesyncd.service → NTP time sync`

`├── ssh.service               → (DISABLED by default — not installed)`

`├── ufw.service               → Firewall`

`├── apparmor.service          → Security profiles`

`├── cron.service              → Scheduled tasks (unattended-upgrades)`

`├── unattended-upgrades       → Automatic security patches`

`├── earlyoom.service          → OOM prevention daemon`

`└── polkit.service            → Privilege escalation`
```

**Phase 4 — Audio:**

```
`├── pipewire.service          → Audio server (user-level, started by session)`

`├── wireplumber.service       → Audio session manager (user-level)`

`└── rtkit-daemon              → Real-time scheduling for audio`
```

**Phase 5 — Display:**

```
`├── sddm.service              → Display manager (login screen)`

`└── spike-session.target      → Spike desktop session (user-level)`
```

**Phase 6 — Post-desktop:**

```
`├── flatpak-system-helper     → Flatpak system services`

`├── udisks2                   → Disk management (for removable devices applet)`

`├── upower                    → Power management (for battery applet)`

`├── colord                    → Color management (for night light)`

`├── accounts-daemon           → User account management (for Settings)`

`├── ModemManager              → (conditional — only if modem detected)`

`└── switcheroo-control        → (conditional — for dual-GPU systems)`
```

### Disabled Services (Stripped From Ubuntu Base)

Services REMOVED or DISABLED by spike-config:

| **Service** | **Reason** |
| :-: | :-: |
| `snapd.service` | Snap is not installed |
| `snapd.socket` | Snap is not installed |
| `snapd.seeded.service` | Snap is not installed |
| `cloud-init.service` | Cloud init (not a cloud VM) |
| `cloud-init-local.service` | Cloud init |
| `cloud-config.service` | Cloud init |
| `cloud-final.service` | Cloud init |
| `landscape-client.service` | Ubuntu Advantage |
| `ubuntu-advantage.service` | Ubuntu Advantage |
| `apt-daily.timer` | Replaced by spike update schedule |
| `apt-daily-upgrade.timer` | Replaced by spike update schedule |
| `motd-news.timer` | News fetcher (unnecessary network) |
| `whoopsie.service` | Crash reporter (privacy) |
| `apport.service` | Crash reporter (privacy) |
| `kerneloops.service` | Kernel crash reporter (privacy) |

### systemd Timer Replacements

Spike replaces Ubuntu's default apt timers with its own:

**Original (disabled):**

```
`├── apt-daily.timer        → Ran apt update + download`

`└── apt-daily-upgrade.timer → Ran apt upgrade`
```

**Spike replacement:**

```
`└── spike-update.timer     → Custom update schedule`
```

**`spike-update.timer` behavior:**

```
`├── Triggers spike-update.service`

`├── Schedule: Every 6 hours (with jitter)`

`├── Conditions:`

`│   ├── Network must be connected`

`│   ├── System must be idle for 10+ minutes`

`│   └── Memory pressure must be below threshold (earlyoom check)`

`├── Actions:`

`│   ├── apt update (refresh package lists)`

`│   ├── Check for security updates`

`│   ├── If security updates available: install them`

`│   ├── If non-security updates available: notify via Discover`

`│   └── Send notification: "Security updates installed" or "Updates available"`

`└── Never forces reboot (gentle notification only)`
```

### ZRAM Initialization

ZRAM is configured during systemd Phase 2:

```
`1. zram-init service (or udev rule) creates /dev/zram0`

`2. Sets compression algorithm: zstd`

`3. Sets disksize: min(RAM, 4GB) for Standard, min(RAM, unlimited) for Plus`

`4. mkswap /dev/zram0`

`5. swapon -p 100 /dev/zram0  (priority 100, higher than swap file)`

`6. Verify with: zramctl`
```

**Condition:** Only runs if CPU is capable (bogomips ≥ ~2200/core, dual-core+). If CPU is not capable: ZRAM is skipped, swap file is the only swap device.

See **MEMORY.md** for full ZRAM specification.

### Filesystem Mounting

**`/etc/fstab`** (generated by installer):

```
*`\# Spike filesystem table — generated at install time`*

*`\# Device        Mount Point    Type   Options                              Dump  Pass`*

`UUID=xxxx-xxxx  /              ext4   noatime,discard                      0     1`

`UUID=yyyy-yyyy  /boot/efi      vfat   umask=0077                           0     1`

`/swapfile       none           swap   sw,pri=10                            0     0`

`/zram0          none           swap   sw,pri=100                           0     0  (if ZRAM enabled)`
```

Note: ZRAM is not actually in fstab — it's configured via zram-generator or udev rules. Shown here for clarity.

**Mount options per storage type:**

| **Storage Type** | **Mount Options** |
| :-: | :-: |
| SSD | `noatime,discard` |
| HDD | `noatime,commit=60` |
| SD | `noatime,commit=60,discard` |
| USB | `noatime,commit=60,discard` |
| NVMe | `noatime,discard` (or `noatime` only if discard not supported) |

The installer detects storage type and writes the correct fstab. Users never edit fstab. If a drive changes, spike-config regenerates it.

## 6. Spike Session Stage

### Display Manager

**SDDM (Simple Desktop Display Manager):**

| **Property** | **Value** |
| :-: | :-: |
| Service | `sddm.service` (systemd-managed) |
| Configuration | `/etc/sddm.conf` |
| Theme | `spike-sddm-theme` |
| Background | Spike branded wallpaper |
| Logo | Spike dog emblem |
| Session | `spike-session` (only option — no dropdown) |
| Auto-login | Optional (configured during install or in Settings → Users) |
| Remember last user | Yes (pre-fills username) |
| Num Lock | Off by default (can be toggled in settings) |
| Virtual keyboard | Available (accessibility) |

```
`SDDM startup:`

`├── Starts after graphical.target is reached`

`├── Starts KWin (or greeter's own compositor)`

`├── Displays login screen`

`├── Waits for user authentication`

`└── On successful login: starts spike-session`
```

### Session Startup Sequence

After user authenticates at SDDM:

**1. SDDM starts `spike-session.desktop`**

```
`└── Exec=/usr/bin/spike-session`
```

**2. spike-session starts:**

```
`├── Load session configuration`

`├── Set up environment variables:`

`│   ├── XDG\_SESSION\_TYPE=wayland`

`│   ├── XDG\_CURRENT\_DESKTOP=Spike`

`│   ├── QT\_QPA\_PLATFORM=wayland`

`│   ├── WAYLAND\_DISPLAY=wayland-0`

`│   ├── SDL\_VIDEODRIVER=wayland`

`│   ├── MOZ\_ENABLE\_WAYLAND=1`

`│   └── LIBVA\_DRIVER\_NAME=\[iHD|radeonsi|nvidia\]`

`├── Start PipeWire (user session):`

`│   ├── pipewire.service (user)`

`│   ├── pipewire-pulse.service (user, PulseAudio compat)`

`│   └── wireplumber.service (user, session manager)`

`├── Start KWin (Wayland compositor):`

`│   ├── kwin\_wayland --noxwayland (native Wayland, XWayland loaded on demand)`

`│   ├── Apply Spike KWin configuration (window decorations, effects)`

`│   └── Wait for compositor ready signal`

`├── Start Spike Shell:`

`│   ├── Theme engine loads (apply Qt stylesheet)`

`│   ├── Panel starts:`

`│   │   ├── Detect hardware for conditional applets`

`│   │   ├── Load applets (network, volume, battery, etc.)`

`│   │   ├── Position panel (layer-shell protocol)`

`│   │   └── Panel becomes visible`

`│   ├── Notification daemon starts:`

`│   │   ├── Load history.json from disk`

`│   │   ├── Restore tray badge from last\_read\_id`

`│   │   └── Register on DBus (org.freedesktop.Notifications)`

`│   ├── Launcher initializes (scans .desktop files, sets up inotify)`

`│   └── Settings remains unloaded (on-demand only)`

`├── Process autostart entries:`

`│   ├── /etc/xdg/autostart/\*.desktop (filtered — only Spike-approved)`

`│   └── ~/.config/autostart/\*.desktop (user-added)`

`├── CLEAR BOOT FAILURE COUNTER:`

`│   ├── Write 0 to /boot/.spike/boot-count`

`│   └── This is the marker of a successful boot`

`└── Emit "Desktop Ready" signal`
```

**3. Desktop is now usable**

```
`├── Background services continue initializing`

`├── NetworkManager connects to saved Wi-Fi (if any)`

`├── Update notifier begins polling (delayed first check)`

`└── Welcome message shown (first boot only)`
```

### Boot Failure Counter Clear

**Location:** `/boot/.spike/boot-count`

**Clear condition:** spike-session has successfully:

```
`├── Started KWin (compositor ready)`

`├── Started Panel (visible on screen)`

`├── Started Notification daemon (DBus registered)`

`├── Loaded theme (stylesheet applied)`
```

Once all four conditions are met:

```
`1. Mount /boot (if separate partition) read-write`

`2. Write "0" to /boot/.spike/boot-count`

`3. Sync filesystem`

`4. Unmount /boot (if separately mounted)`

`5. Log: "Boot successful — failure counter cleared"`
```

If any of the four conditions FAIL:

```
`├── Counter is NOT cleared`

`├── Session continues to attempt startup`

`├── If session fully crashes: counter remains incremented`

`└── After 3 failures: GRUB shows menu on next boot`
```

This ensures that a "partial" boot (where the system sort-of starts but the desktop doesn't come up) counts as a failure.

### Autostart Filtering

Spike filters `/etc/xdg/autostart/\*.desktop` entries:

**Approved autostart entries:**

```
`├── org.kde.discover.notifier    → Discover update notifications`

`├── org.kde.kded6                → KDE daemon (hardware events, settings)`

`├── polkit-kde-authentication    → Privilege escalation dialog`

`├── org.kde.kwalletd5            → (only if user has wallet)`

`├── geoclue-demo-agent           → Location for night light`

`├── pulseaudio-kde               → (replaced by PipeWire equivalent)`

`└── spike-\*                       → Spike-specific autostart entries`
```

**Blocked autostart entries:**

```
`├── org.kde.akonadi\_\*            → Akonadi (stripped, not installed)`

`├── org.kde.baloo\_\*              → Baloo (stripped, not installed)`

`├── org.kde.kioexec              → KIO executor (not used)`

`├── snap-userd                   → Snap user daemon (not installed)`

`├── ubuntu-one-\*                 → Ubuntu One (not installed)`

`├── update-notifier              → Replaced by spike-update-notifier`

`└── gnome-\*                      → GNOME autostart entries (not applicable)`
```

Filtering is done via spike-config at install time:

```
`├── Remove unwanted .desktop files from /etc/xdg/autostart/`

`├── Add Hidden=true to entries that can't be removed`

`└── User autostart (~/.config/autostart/) is never filtered`
```

### Environment Variables

Set by spike-session before any shell components start:

```
*`\# Display`*

`XDG\_SESSION\_TYPE=wayland`

`XDG\_CURRENT\_DESKTOP=Spike`

`XDG\_SESSION\_DESKTOP=spike`

`QT\_QPA\_PLATFORM=wayland`

`WAYLAND\_DISPLAY=wayland-0`

`SDL\_VIDEODRIVER=wayland`

`MOZ\_ENABLE\_WAYLAND=1`

`GDK\_BACKEND=wayland`


*`\# Audio`*

`PIPEWIRE\_LATENCY=256/48000`


*`\# GPU acceleration`*

`LIBVA\_DRIVER\_NAME=iHD          *\# (or radeonsi for AMD, nvidia for NVIDIA)`*

`LIBGL\_ALWAYS\_SOFTWARE=0         *\# Never force software rendering`*


*`\# Firefox tuning (applied to Flatpak too)`*

`MOZ\_DISABLE\_AV1=1              *\# Disable AV1 (force VP9 on Gemini Lake)`*


*`\# Qt`*

`QT\_QPA\_PLATFORMTHEME=qt5ct     *\# Use qt5ct for app styling`*

`QT\_AUTO\_SCREEN\_SCALE\_FACTOR=1  *\# Automatic HiDPI scaling`*

`QT\_SCALE\_FACTOR=1              *\# Default scale (adjustable in Settings)`*


*`\# Paths`*

`XDG\_CONFIG\_HOME=$HOME/.config`

`XDG\_DATA\_HOME=$HOME/.local/share`

`XDG\_CACHE\_HOME=$HOME/.cache`

`XDG\_RUNTIME\_DIR=/run/user/$(id -u)`


*`\# Locale`*

`LANG=en\_US.UTF-8               *\# (set during install)`*

`LC\_TIME=en\_US.UTF-8            *\# (follows locale)`*


*`\# Spike-specific`*

`SPIKE\_VARIANT=standard         *\# (or "plus")`*

`SPIKE\_VERSION=0.1.0-alpha`
```

## 7. Desktop Ready

### First Boot vs Subsequent Boots

**First boot (fresh installation):**

```
`├── spike-session detects /var/lib/spike/first-boot file`

`├── After desktop is ready:`

`│   ├── Display welcome message (see INSTALLER.md)`

`│   ├── Check for pending security updates (if network available)`

`│   ├── Check NVIDIA hardware (if applicable)`

`│   ├── Check HDD upgrade recommendation (if HDD detected)`

`│   └── Display relevant notifications`

`├── Remove first-boot marker file`

`└── Subsequent boots skip this`
```

**Subsequent boots:**

```
`├── Desktop ready`

`├── NetworkManager reconnects to saved Wi-Fi`

`├── Update notifier checks (delayed: waits 5 min after boot)`

`├── No welcome message`

`└── User sees their desktop as they left it`
```

### Post-Boot Background Tasks

Tasks that start AFTER desktop is ready (non-blocking):

**Network reconnection (0-5 seconds):**

```
`├── NetworkManager connects to last known Wi-Fi`

`├── If connection fails: tray applet shows "Disconnected"`

`└── User can select network from tray applet`
```

**Time synchronization (0-10 seconds):**

```
`├── systemd-timesyncd contacts NTP servers`

`├── If offline: clock stays at hardware clock time`

`└── If online: clock synchronized silently`
```

**Update check (5-minute delay):**

```
`├── spike-update.service triggered by spike-update.timer`

`├── First check delayed by 5 minutes after boot`

`├── Checks apt and Flatpak repositories`

`├── Security updates: install silently`

`├── Non-security: notification in tray`

`└── Subsequent checks: every 6 hours`
```

**Flatpak runtime validation (30-second delay):**

```
`├── Verify pre-seeded runtimes are intact`

`├── Check for runtime updates on Flathub`

`├── If runtime update available: notify via Discover`

`└── No silent runtime updates (user initiates)`
```

**File indexer (NOT RUNNING):**

```
`├── Baloo is NOT installed`

`├── No file indexing occurs at boot`

`├── File search uses locate (updated weekly via cron) or find`

`└── This is a conscious decision to save memory and I/O`
```

### Spike Plus Differences

Spike Plus boot differences:

1. **Plymouth:** Animated boot splash (vs static for Standard)

```
`├── Progress animation plays during boot`

`├── Smooth fade to SDDM login screen`

`└── Uses GPU-accelerated rendering`
```

2. **Compositor effects:**

```
`├── Blur enabled on translucent windows`

`├── Fade animations on window open/close`

`└── Slide animations on workspace switch`
```

These start during spike-session, not earlier.

3. **Additional Flatpak runtimes pre-seeded:**

```
`├── All common runtimes verified at boot`

`└── More runtimes = faster first app launch`
```

4. **CPU governor set to `schedutil`** (vs `powersave` for Standard)

```
`├── Applied by spike-config during systemd Phase 2`

`└── More responsive frequency scaling`
```

5. **Memory budget is higher** (\<800MB vs \<400MB)

```
`├── More services can run`

`├── Caching is more aggressive`

`└── No impact on boot sequence, only post-boot behavior`
```

Boot sequence itself is IDENTICAL between Standard and Plus. Only post-boot configuration differs.

## Boot Failure Handling

### Failure Scenarios

**Scenario 1: Kernel panic (bad kernel update)**

```
`├── Boot attempt 1: kernel panics → reboot (counter = 1)`

`├── Boot attempt 2: kernel panics → reboot (counter = 2)`

`├── Boot attempt 3: kernel panics → reboot (counter = 3)`

`├── Boot attempt 4: GRUB menu appears (counter ≥ 3)`

`│   ├── Previous kernel highlighted`

`│   ├── User selects previous kernel`

`│   └── System boots successfully`

`├── spike-session clears counter`

`└── User uninstalls bad kernel via Discover`
```

**Scenario 2: GPU driver crash**

```
`├── Boot attempt 1: KWin crashes → black screen (counter = 1)`

`├── User hard-powers off → reboot (counter already incremented)`

`├── Boot attempt 2: KWin crashes → black screen (counter = 2)`

`├── User hard-powers off → reboot`

`├── Boot attempt 3: KWin crashes → black screen (counter = 3)`

`├── Boot attempt 4: GRUB menu appears`

`│   ├── User selects "Spike Recovery"`

`│   ├── Recovery mode boots to root shell`

`│   └── User runs spike-rescue to back up files`

`└── Fresh install with restore`
```

**Scenario 3: Corrupted filesystem (power loss)**

```
`├── Boot attempt 1: initramfs fsck repairs minor errors → boot continues`

`├── If fsck cannot repair:`

`│   ├── initramfs drops to recovery shell`

`│   ├── Spike recovery screen shown`

`│   └── User directed to live USB rescue`

`└── Boot counter may not apply (fsck interrupts boot before counter)`
```

**Scenario 4: Full disk (no space)**

```
`├── Boot may succeed but spike-session fails to start`

`├── Notification daemon cannot write history (disk full)`

`├── Panel may start but updates fail`

`├── This is a runtime issue, not a boot failure`

`└── Diagnostics page shows disk full warning`
```

### Recovery Mode

GRUB Recovery entry boots into `rescue.target`:

**What starts in recovery mode:**

```
`├── systemd-journald (logging)`

`├── dbus (system bus)`

`├── Basic filesystem mounts (read-only)`

`├── NetworkManager (optional, if user enables)`

`├── Root shell on tty1`

`└── NOTHING ELSE (no GUI, no audio, no display manager)`
```

**Recovery mode provides:**

```
`├── Root shell (password required if root set — it's not, so sudo user)`

`│   Wait: root login disabled. Recovery mode uses sulogin.`

`│   Since root has no password, sulogin allows passwordless root shell.`

`│   This is standard Ubuntu behavior for recovery mode.`

`├── fsck (filesystem check)`

`├── spike-rescue (file recovery tool — if terminal-based available)`

`│   Note: GUI rescue tool is on the live ISO, not in recovery mode`

`├── apt (repair broken packages, if network available)`

`├── dpkg (repair interrupted installations)`

`└── Reboot option`
```

**Recovery mode limitations:**

```
`├── No graphical interface`

`├── No Spike Shell`

`├── No audio`

`├── Requires terminal usage (violates Golden Rule 2)`

`└── This is intentional — recovery mode is a last resort`
```

For non-technical users, the recommended recovery path is:

1. Boot from Spike USB (live ISO) 

2. Use "Rescue My Files" GUI tool 

3. Fresh install with restore 

Recovery mode is for:

- Developers debugging boot issues 

- Power users who prefer terminal recovery 

- Edge cases where live USB is unavailable 

## Plymouth Boot Splash

### Purpose

Plymouth provides a graphical boot splash that replaces kernel boot messages with branded visuals. It creates a smooth visual transition from firmware to desktop.

### Themes

**Spike Standard — "spike-minimal":**

```
`├── Static Spike logo centered on dark background (\#1a1a2e)`

`├── Simple progress dots at bottom (5 dots, fill left to right)`

`├── No animation beyond progress dots`

`├── Uses software rendering (no GPU dependency)`

`├── Minimal memory footprint (~5-8MB)`

`└── Fade to black at end (SDDM fades in from black)`
```

**Spike Plus — "spike-full":**

```
`├── Animated Spike logo (subtle pulsing glow)`

`├── Animated progress bar (smooth fill)`

`├── GPU-accelerated rendering (DRM)`

`├── Smooth fade to SDDM login screen`

`├── Slightly higher memory footprint (~8-12MB)`

`└── Only used on hardware with adequate GPU support`
```

**Fallback — "spike-text":**

```
`├── If GPU/DRM initialization fails during boot`

`├── Simple text-based progress display`

`├── Shows "Starting Spike..." with boot stages listed`

`├── No graphics, no logo`

`└── Plymouth falls back automatically (no user intervention)`
```

### Plymouth Configuration

**Location:** `/usr/share/plymouth/themes/spike-\{minimal,full\}/`

```
`Configuration:`

`├── Theme selected by spike-config at install time`

`├── Standard: plymouth-set-default-theme spike-minimal`

`├── Plus: plymouth-set-default-theme spike-full`

`├── Initramfs includes Plymouth + theme files`

`└── Kernel parameter "splash" activates Plymouth`
```

**Plymouth and KMS:**

```
`├── Plymouth requires KMS (Kernel Mode Setting) to display graphics`

`├── GPU driver must load during initramfs for Plymouth to work`

`├── If GPU driver is a module (not in initramfs): Plymouth falls back to text`

`├── Spike puts GPU drivers in initramfs (i915, amdgpu, nouveau)`

`└── This ensures graphical Plymouth on all supported hardware`
```

**Plymouth and Disk Encryption:**

```
`├── Spike does not use disk encryption`

`├── No password prompt during Plymouth`

`├── Plymouth runs uninterrupted from kernel start to SDDM handoff`

`└── Simpler than encrypted systems (no prompt interruption)`
```

### Plymouth Transition

Boot visual timeline:

```
`Firmware POST`

`    │ (screen: firmware logo or black)`

`    ▼`

`GRUB loads (hidden)`

`    │ (screen: brief black, then Plymouth starts)`

`    ▼`

`Plymouth boot splash`

`    │ (screen: Spike logo + progress)`

`    ▼`

`Plymouth fade out`

`    │ (screen: fades to black)`

`    ▼`

`SDDM login screen`

`    │ (screen: SDDM with Spike theme)`

`    ▼`

`User logs in`

`    │ (screen: brief black, then desktop)`

`    ▼`

`Spike Shell desktop ready`
```

The transition from Plymouth to SDDM:

```
`├── Plymouth stops when SDDM is ready`

`├── SDDM starts its own compositor (or reuses KWin)`

`├── Brief black screen (\< 1 second) during handoff`

`├── No flickering or text visible`

`└── Feels like a continuous visual experience`
```

## Kernel Updates and Boot

### Kernel Update Process

When a kernel update is installed (via apt or Discover):

**1. New kernel package downloaded and installed**

```
`├── /boot/vmlinuz-x.x.x-new-generic`

`├── /boot/initrd.img-x.x.x-new-generic (rebuilt with spike hooks)`

`└── /lib/modules/x.x.x-new-generic/`
```

**2. Old kernel retained (one version back)**

```
`├── /boot/vmlinuz-x.x.x-old-generic`

`├── /boot/initrd.img-x.x.x-old-generic`

`└── /lib/modules/x.x.x-old-generic/`
```

**3. GRUB configuration updated**

```
`├── update-grub runs automatically (apt hook)`

`├── New kernel becomes default entry`

`├── Old kernel available as "Spike, with previous kernel"`

`└── Recovery entry updated for new kernel`
```

**4. Initramfs rebuilt**

```
`├── update-initramfs -u (all kernels)`

`├── Spike customizations applied (boot counter, recovery shell)`

`└── Module list verified`
```

**5. User notified**

```
`├── "Kernel update installed. Restart to apply."`

`├── Gentle notification (no forced reboot)`

`└── User can continue using old kernel until next reboot`
```

### Kernel Rollback

If new kernel causes boot failure:

```
`1. Counter increments on each failed boot`

`2. After 3 failures: GRUB menu appears`

`3. "Spike, with previous kernel" highlighted`

`4. User selects it → boots old kernel`

`5. spike-session starts → counter cleared`

`6. User opens Discover → removes new kernel package`

`7. update-grub runs → old kernel becomes default again`
```

**Settings → Advanced → Boot** shows:

```
`├── Current boot failure count`

`├── Reset button (clears counter)`

`├── Current kernel version`

`├── Previous kernel version (if available)`

`└── "Remove previous kernel" button (frees disk space)`
```

## Settings Integration

### Settings → Advanced → Boot

Boot settings page:

**Boot Behavior:**

```
`├── Boot menu: \[Hidden (show on failure)\] ▼`

`│   ├── Hidden (show on failure) \[default\]`

`│   └── Always show`

`├── Boot menu timeout: \[3 seconds\] (slider: 1-10)`

`├── ESC key window: \[3 seconds\] (slider: 1-10)`

`└── Boot splash: \[Spike Minimal\] ▼ (Standard) / \[Spike Full\] (Plus)`
```

**Boot Status:**

```
`├── Boot failure count: 0`

`├── \[Reset counter\]`

`├── Current kernel: 7.0.0-42-generic`

`├── Previous kernel: (none) / 7.0.0-41-generic`

`└── \[Remove previous kernel\] (if available)`
```

**Diagnostics:**

```
`├── \[View last boot log\] → opens journalctl -b in Konsole (preset command)`

`├── \[View boot configuration\] → shows /etc/default/grub (read-only)`

`└── \[View kernel parameters\] → shows /proc/cmdline (read-only)`
```

All changes to boot settings are applied by regenerating GRUB configuration. The user never edits `/etc/default/grub` directly. spike-config writes the file and runs `update-grub`.

## What This Document Does Not Cover

- **Memory configuration** (ZRAM, swap, earlyoom): See **MEMORY.md** 

- **Kernel parameters and module blacklisting details**: See **KERNEL.md** 

- **Desktop shell startup components**: See **DESKTOP.md** (Session Manager section) 

- **Installer partitioning and GRUB installation**: See **INSTALLER.md** 

- **Recovery tool GUI and file rescue**: See **INSTALLER.md** (Spike Rescue section) 

- **Disaster recovery procedures**: See **DISASTER-RECOVERY.md** 

- **Troubleshooting boot issues**: See **TROUBLESHOOTING.md** 

- **Kernel update and security patch flow**: See **SECURITY.md** (Updates section) 

🐕 BigRangaTech


