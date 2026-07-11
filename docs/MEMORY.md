# Spike Memory Management

## Purpose

This document specifies how Spike manages memory on hardware with 4GB of RAM (or less). Memory management is the single most critical factor in making old hardware feel responsive. Every decision in this document exists to answer one question: how do we make 4GB feel like more?

## Overview

Spike uses a layered memory strategy that adapts at install time based on detected hardware. The system is configured once during installation and then operates autonomously — no user intervention required.

Memory Layers (in order of use):

```
`┌──────────────────────────────────────────┐`

`│  Layer 1: Physical RAM (4GB)              │`

`│  → Fastest. Everything runs here first.   │`

`├──────────────────────────────────────────┤`

`│  Layer 2: ZRAM Compressed (up to 4GB)    │`

`│  → Compresses inactive pages in RAM.     │`

`│  → Effective ~8-10GB depending on data.  │`

`│  → Only enabled on capable CPUs.         │`

`├──────────────────────────────────────────┤`

`│  Layer 3: Swap File on Disk (8GB)         │`

`│  → Last resort before intervention.      │`

`│  → Slow on HDD, acceptable on SSD.       │`

`├──────────────────────────────────────────┤`

`│  Layer 4: Earlyoom                        │`

`│  → Kills largest process before OOM.     │`

`│  → Prevents system freeze.               │`

`└──────────────────────────────────────────┘`
```

## Total Effective Memory

On a 4GB Celeron N4020 with ZRAM enabled:

```
`Physical RAM:              4GB`

`ZRAM compressed effective:  ~4-6GB (4GB compressed with zstd, varies by data type)`

`Swap file:                  8GB`

`─────────────────────────────────────`

`Total before intervention:  ~16-18GB`

`Total realistic usable:     ~12-14GB (before earlyoom triggers)`
```

> **Note on ZRAM effective capacity:** The theoretical maximum of a 4GB ZRAM device with zstd compression at a 3:1–4:1 ratio is ~12–16GB of uncompressed data. However, not all memory pages compress equally — executable code pages, already-compressed data (JPEGs, MP3s, encrypted content), and kernel pages resist compression. The "~4-6GB effective" figure reflects realistic mixed workloads, not best-case synthetic benchmarks. The "~16-18GB total" is similarly conservative, accounting for partial compression and the overhead of the ZRAM device itself.

Without ZRAM (low-end CPU):

```
`Physical RAM:              4GB`

`Swap file:                  8GB`

`─────────────────────────────────────`

`Total before intervention:  ~12GB`

`Total realistic usable:     ~8-10GB (before earlyoom triggers)`
```

## Hardware Detection

Memory configuration is determined at install time by detecting two hardware characteristics: CPU capability and storage type.

### CPU Classification

Detection method:

```
`├── CPU cores: nproc`

`├── CPU speed: /proc/cpuinfo bogomips`

`└── Classification:`

`    ├── Capable: dual-core+ AND bogomips ≥ ~2200 per core`

`    │   → ZRAM enabled`

`    │   → Standard swappiness`

`    │`

`    └── Low-end: single-core OR bogomips \< ~2200 per core`

`        → ZRAM disabled`

`        → Conservative swappiness`
```

### Why Bogomips

Bogomips is a simple kernel-reported metric that measures CPU loops per second. It is not a precise benchmark, but it correlates well enough with real-world compression throughput for classification purposes. The threshold of ~2200 bogomips per core was chosen because:

- Celeron N4020 (Gemini Lake Refresh): ~2200 bogomips/core → classified as **capable** 

- AMD A4 (Jaguar): ~1400-1800 bogomips/core → classified as **low-end** (varies by model) 

- Single-core atoms: below threshold → classified as **low-end** 

The threshold is conservative — when in doubt, the system chooses the safer option (no ZRAM) to avoid making a slow CPU slower.

### Edge Cases

| **Scenario** | **Behavior** |
| :-: | :-: |
| Bogomips exactly at threshold | Treat as low-end (conservative) |
| Dual-core with very low bogomips | Treat as low-end (cores alone don't guarantee capability) |
| Single-core with high bogomips | Treat as low-end (compression would monopolize the only core) |
| Virtual CPU (VM testing) | Follows same logic; may classify differently than bare metal |

### Storage Classification

Detection method:

```
`├── lsblk -d -o NAME,ROTA  (rotational flag: 1=HDD, 0=SSD/NVMe)`

`├── /sys/block/\*/queue/rotational  (per-device confirmation)`

`├── eMMC detection via /sys/block/\*/queue/rotational + device path`

`│   → If eMMC detected: REJECT INSTALL (unsupported storage type)`

`├── USB detection via lsblk + udev`

`│   → If USB 2.0: REJECT INSTALL (too slow for swap)`

`│   └── If USB 3.0+: accept with reduced swap and conservative settings`

`└── SD card detection via /sys/block/\*/device/type`

`    → If SD card: accept with reduced swap and conservative settings`
```

Classification:

```
`├── SATA SSD    → ROTA=0, not USB, not SD    → SSD profile`

`├── NVMe SSD    → ROTA=0, not USB, not SD    → SSD profile`

`├── SATA HDD    → ROTA=1, not USB, not SD    → HDD profile`

`├── SD card     → detected via device path   → SD profile`

`└── USB 3.0+    → detected via udev          → USB profile`
```

### Why ROTA Flag

The ROTA (rotational) flag from `lsblk` is the simplest reliable indicator of whether a device is solid-state or mechanical. It is populated by the kernel based on the device's reported characteristics.

- SSDs and NVMe: ROTA=0 (no rotating platters) 

- HDDs: ROTA=1 (rotating platters) 

- eMMC: ROTA=0 (solid-state, but rejected for reliability reasons — see below) 

- SD cards: ROTA=0 (solid-state, but conservative settings applied for wear and speed) 

The ROTA flag alone is insufficient — Spike cross-references device paths and `udev` to distinguish SD cards and USB drives from internal SSDs.

### Why eMMC Is Rejected

eMMC storage is common on exactly the kind of budget Celeron laptops Spike targets, which makes this decision worth explaining. eMMC is rejected for **reliability reasons, not performance**:

```
`├── eMMC has limited write endurance (typically 1,000-3,000 write cycles`

`│   per cell, compared to 3,000-100,000+ for consumer SSDs)`

`├── Spike uses an 8GB swap file, which generates sustained write cycles`

`│   on the storage device — eMMC wear would be accelerated significantly`

`├── eMMC controllers rarely implement wear leveling as robustly as SSDs`

`├── When eMMC fails, it tends to fail completely (no graceful degradation)`

`├── Replacement is typically not possible (eMMC is soldered to the`

`│   motherboard on most devices)`

`└── Risk: a user could install Spike, use it for 6-12 months with swap`

`    activity, and experience total storage failure with no recovery path`
```

Users with eMMC-only devices are directed to the user guide, which explains that an SSD upgrade (via SATA or M.2 if the laptop supports it) is required to install Spike.

## ZRAM Configuration

### What ZRAM Does

ZRAM creates a compressed block device in RAM that acts as a swap partition. Instead of writing inactive memory pages to disk (slow), the kernel compresses them and keeps them in RAM (fast). This effectively increases the amount of data the system can hold in RAM without touching disk.

Without ZRAM:

```
`├── 4GB RAM fills up`

`├── Inactive pages written to disk (slow I/O)`

`├── HDD: severe latency, system feels frozen`

`└── SSD: acceptable but wastes I/O bandwidth`
```

With ZRAM:

```
`├── 4GB RAM fills up`

`├── Inactive pages compressed in RAM (fast, CPU-bound)`

`├── zstd compression ratio: typically 2.5:1 to 4:1`

`├── 4GB ZRAM device holds ~10-16GB of uncompressed data`

`├── Disk swap only used when ZRAM is full`

`└── System stays responsive longer`
```

### ZRAM Settings

```
`Device:          /dev/zram0`

`Algorithm:       zstd`

`Size:            Equal to physical RAM (max 4GB on Spike Standard)`

`                 Equal to physical RAM (no cap on Spike Plus)`

`Priority:        100 (higher than swap file)`

`Compression:     zstd (best ratio, good speed on capable CPUs)`

`Disksize:        Match RAM (e.g., 4GB on 4GB system)`
```

### Why zstd

Algorithm comparison (qualitative):

```
`├── lzo:     Fast, but poor compression ratio (~2:1). Leaves memory on the table.`

`├── lz4:     Very fast, decent ratio (~2.5:1). Good for weak CPUs.`

`├── zstd:    Best ratio (~3:1 to 4:1), good speed on modern dual-core+.`

`│            Slightly more CPU than lz4 but the extra memory recovery is worth it.`

`└── deflate: Good ratio but slow. Not suitable for real-time compression.`
```

Spike Standard uses zstd on capable CPUs. If zstd proves too heavy on edge-case hardware during alpha testing, lz4 is the fallback (documented as a conditional switch).

### ZRAM Size Cap — Spike Standard

Spike Standard caps ZRAM at 4GB even on systems with more physical RAM. This is a deliberate variant-level limitation:

```
`├── Spike Standard targets 4GB systems — ZRAM equal to RAM is 4GB`

`├── On an 8GB system running Standard, ZRAM is capped at 4GB`

`├── Rationale:`

`│   ├── Keeps memory overhead predictable on low-spec targets`

`│   ├── Caps the CPU cost of compression (4GB of compressed pages`

`│   │   is manageable even on a Celeron N4020)`

`│   ├── Larger ZRAM devices on weak CPUs can cause compression`

`│   │   latency spikes during heavy page-out bursts`

`│   └── Users with \>4GB RAM are likely running Spike Plus anyway`

`│       (Plus removes the cap — see Spike Plus Differences below)`

`└── This cap is documented in VARIANT-DIFFERENCES.md`
```

### ZRAM and zswap Interaction

Spike explicitly disables zswap (the kernel's in-RAM compressed swap cache) via the boot parameter `zswap.enabled=0`. This is critical because ZRAM and zswap would otherwise both attempt to compress memory pages in RAM, causing double-compression overhead and wasted CPU cycles. With zswap disabled, ZRAM is the sole in-RAM compression mechanism and the swap file is the sole on-disk fallback. See `KERNEL.md` for the full kernel boot parameter specification.

### ZRAM Activation

ZRAM is configured at install time by `spike-config/memory/`:

```
*`\# /etc/modules-load.d/spike-zram.conf`*

*`\# Load zram module at boot`*

`zram`

*`\# /etc/udev/rules.d/99-spike-zram.rules`*

*`\# Create zram0 device with zstd compression`*

`KERNEL=="zram0", ACTION=="add", ATTR\{comp\_algorithm\}="zstd", ATTR\{disksize\}="4G", RUN+="/sbin/swapon --priority 100 /dev/zram0"`
```

If the CPU is classified as low-end, these files are not created and ZRAM is never activated. The system relies solely on the swap file.

### ZRAM Monitoring

Users can view ZRAM status through **Settings → Memory**:

```
`Settings → Memory:`

`├── Physical RAM:     3.8 GB / 4.0 GB (in use)`

`├── ZRAM:             1.2 GB compressed / 4.0 GB allocated`

`│   └── Compression:   3.1:1 (saving 2.7 GB of uncompressed data)`

`├── Swap file:        0.2 GB / 8.0 GB (in use)`

`├── ZRAM status:      Active (zstd)`

`└── Earlyoom status:  Active (monitoring)`
```

This page is read-only. ZRAM cannot be toggled by the user — it is an install-time decision based on hardware. A note explains why:

> Note: ZRAM was enabled because your CPU can handle compression efficiently. If your system feels slow, check the Diagnostics page for performance insights.

## Swap File Configuration

### Swap File Sizing

| **Storage Type** | **Swap Size** |
| :-: | :-: |
| SATA SSD | 8GB |
| NVMe SSD | 8GB |
| SATA HDD | 8GB |
| SD card | 4GB |
| USB 3.0+ | 4GB |

### Why A Swap File, Not A Partition

- Resizable without repartitioning (Settings can adjust if needed) 

- Simplifies the installer (one fewer partition to create) 

- Performance on ext4 is comparable to a swap partition 

- Easier to remove if a user upgrades RAM (though Spike doesn't support RAM upgrades through the GUI — this is a physical action) 

### Swap File Creation

Created at install time on the root partition:

```
*`\# Create swap file`*

`fallocate -l 8G /swapfile`

*`\# Or on filesystems where fallocate is unreliable for swap:`*

`dd if=/dev/zero of=/swapfile bs=1M count=8192`


`chmod 600 /swapfile`

`mkswap /swapfile`

`swapon /swapfile`
```

### Swap File TRIM/Discard

On SSD and NVMe systems, the swap file is mounted with the `discard` option to enable TRIM support:

```
`├── Discard/TRIM allows the SSD controller to reclaim freed swap pages`

`├── Maintains SSD performance over time (prevents write amplification)`

`├── Configured via /etc/fstab entry: /swapfile none swap defaults,discard 0 0`

`├── HDD systems: discard omitted (TRIM is meaningless for mechanical drives)`

`├── SD card / USB systems: discard omitted (wear leveling concern —`

`│   continuous TRIM can cause unnecessary erase cycles on flash media)`

`└── Verification: lsblk -D shows discard support for the underlying device`
```

### Swap Priority

```
`ZRAM:     priority 100  (used first — compressed, in RAM)`

`Swap file: priority 10  (used second — on disk, slower)`
```

The kernel uses highest-priority swap first. By setting ZRAM to 100 and the swap file to 10, the system always tries to compress pages in RAM before writing to disk.

### Swappiness Values

Swappiness controls how aggressively the kernel swaps pages out of RAM. Range: 0–200 (kernel default: 60).

| **Storage Type** | **Swappiness** | **Rationale** |
| :-: | :-: | :-: |
| SSD/NVMe | 15 | Swapping is relatively fast on SSDs. Moderate swappiness keeps inactive pages out of RAM without excessive I/O. |
| SATA HDD | 5 | Swapping on HDD causes severe latency. Minimize swapping to keep applications in RAM as long as possible. |
| SD card | 10 | Slower than SSD, faster than HDD. Middle ground. |
| USB 3.0+ | 10 | Similar to SD card. |

Set via:

```
*`\# /etc/sysctl.d/99-spike-memory.conf`*

`vm.swappiness = 15    *\# Default (SSD). HDD overrides to 5.`*
```

The installer writes the correct value based on detected storage. Users can view this in **Settings → Memory** and, on SSD/NVMe systems, adjust it upward via the swappiness slider (see below).

## Earlyoom

### What Earlyoom Does

Earlyoom is a userspace daemon that monitors memory usage and kills the largest process before the system runs out of memory entirely. The kernel's built-in OOM killer is slow, unpredictable, and often freezes the system before it triggers. Earlyoom intervenes earlier and more gracefully.

### Why Earlyoom Instead Of Kernel OOM

Kernel OOM killer:

```
`├── Triggers when system is completely out of memory`

`├── System may be frozen for 30+ seconds before killing a process`

`├── Selection algorithm is complex and sometimes kills the wrong process`

`├── On 4GB systems, by the time it triggers, the desktop is already unresponsive`

`└── User has to hard-reset — potential data loss`
```

Earlyoom:

```
`├── Triggers when available memory drops below threshold (10%)`

`├── Acts in milliseconds — system stays responsive`

`├── Kills the largest process (predictable, understandable)`

`├── User sees a notification: "Spike closed \[app name\] to free memory"`

`└── Desktop stays usable`
```

### Earlyoom Configuration

```
*`\# Installed: earlyoom package from Ubuntu repositories`*

*`\# Service: systemd unit (enabled at boot)`*


*`\# Configuration:`*

*`\# - Poll memory every 1 second`*

*`\# - Kill when available memory \< 10% AND available swap \< 10%`*

*`\# - Send SIGTERM first (graceful), SIGKILL after 1 second if still running`*

*`\# - Avoid killing: spike-shell, kwin\_wayland, systemd, PipeWire`*

*`\# /etc/default/earlyoom`*

`EARLYOOM\_ARGS="-r 3600 -m 10 -s 10 --prefer '(firefox|chromium|libreoffice|gimp|blender)' --avoid '(spike-shell|kwin\_wayland|systemd|pipewire|wireplumber|earlyoom)'"`
```

### Protected Processes

The following processes are protected from earlyoom (listed in `--avoid`):

| **Process** | **Why Protected** |
| :-: | :-: |
| `spike-shell` | The desktop shell — killing it would crash the desktop |
| `kwin\_wayland` | The compositor — killing it would crash the display |
| `systemd` | The init system — killing it would crash the OS |
| `pipewire` | Audio server — killing it would break audio |
| `wireplumber` | Audio session manager — killing it would break audio |
| `earlyoom` | Self-protection — cannot kill itself |

### Preferred Kill Targets

The following processes are preferred for killing (listed in `--prefer`):

| **Process** | **Why Preferred** |
| :-: | :-: |
| `firefox` | Typically the largest memory consumer; user can reopen |
| `chromium` | Same as Firefox |
| `libreoffice` | Large office suite; unsaved work may be lost (notification warns) |
| `gimp` | Large image editor; less common, high memory |
| `blender` | 3D editor; rare on target hardware but very memory-hungry |

### Data Loss Acknowledgment

Earlyoom sends SIGTERM first (graceful shutdown), followed by SIGKILL after 1 second if the process hasn't exited. This 1-second window is deliberate — it gives well-behaved applications a chance to save state, close files cleanly, and release resources. However, it is not long enough for interactive save dialogs:

```
`├── SIGTERM gives the application a chance to shut down gracefully`

`├── Applications with autosave (Firefox, LibreOffice with autosave enabled)`

`│   may recover some unsaved work on next launch`

`├── Applications without autosave: unsaved work WILL be lost if`

`│   the application cannot complete its shutdown within 1 second`

`├── The earlyoom notification explicitly warns the user:`

`│   "If you had unsaved work in \[app\], it may not have been saved."`

`├── This is a deliberate tradeoff:`

`│   ├── Longer grace period (e.g., 10 seconds) risks the system`

`│   │   freezing before earlyoom can act — defeating its purpose`

`│   ├── The 1-second window is long enough for graceful exit but`

`│   │   short enough to prevent total system lockup`

`│   └── Preserving system responsiveness for other apps is prioritized`

`│       over saving work in the single largest memory consumer`

`└── User guide recommends enabling autosave in applications that`

`    support it, especially LibreOffice`
```

### User Experience When Earlyoom Kills A Process

1. Memory pressure builds (many apps open, heavy workload) 

2. Available RAM + ZRAM + swap drops below 10% 

3. Earlyoom identifies largest unprotected process (e.g., Firefox) 

4. SIGTERM sent to Firefox (graceful shutdown attempt) 

5. If Firefox doesn't exit within 1 second → SIGKILL (forced) 

6. Notification appears: 

```
`┌─────────────────────────────────────────────┐`

`│  ⚠️  Spike closed Firefox to free memory       │`

`│                                               │`

`│  Your system was running low on memory.        │`

`│  Firefox was using the most memory and was      │`

`│  closed to keep your system responsive.         │`

`│                                               │`

`│  If you had unsaved work in Firefox, it may     │`

`│  not have been saved.                          │`

`│                                               │`

`│  Your other apps are still running.             │`

`│  You can reopen Firefox when ready.             │`

`└─────────────────────────────────────────────┘`
```

7. Notification is saved to history (disk-before-display invariant) 

8. Desktop remains responsive 

This notification is critical. Without it, the user would think Firefox crashed randomly and lose trust in the system. The notification explains what happened, why, and reassures them that their other work is safe.

## Per-Application Memory Limits

Spike does **not** configure per-application memory cgroups. All applications are free to consume available RAM, with earlyoom as the universal backstop:

```
`├── No cgroup memory limits are set on individual applications`

`├── Rationale:`

`│   ├── Celeron N4020 with 4GB RAM has very little headroom — setting`

`│   │   arbitrary per-app limits would either be too restrictive`

`│   │   (starving legitimate workloads) or too permissive (not helping)`

`│   ├── The target user runs a small number of apps at once`

`│   ├── Earlyoom already handles the runaway-process case effectively`

`│   ├── cgroup memory limits add configuration complexity for minimal`

`│   │   benefit on single-user, low-memory systems`

`│   └── Firefox's own multi-process architecture already sandboxes`

`│       individual tabs — a single runaway tab doesn't consume all RAM`

`├── Flatpak applications run in bubblewrap containers, which provide`

`│   filesystem isolation but do not impose memory limits by default`

`├── Future consideration: if Spike adds a "focus mode" or app-limiting`

`│   feature, cgroup memory controls could be introduced at that point`

`└── This decision is revisited in PERFORMANCE-BASELINES.md`
```

## Adaptive Memory Profiles

The installer combines CPU classification and storage detection to select one of four memory profiles:

### Profile Matrix

| **CPU Class** | **Storage Type** | **ZRAM** | **Swap** | **Swappiness** | **HDD Warning** |
| :-: | :-: | :-: | :-: | :-: | :-: |
| Capable | SSD/NVMe | Enabled (4GB, zstd) | 8GB | 15 | No |
| Capable | HDD | Enabled (4GB, zstd) | 8GB | 5 | Yes |
| Low-end | SSD/NVMe | Disabled | 8GB | 15 | No |
| Low-end | HDD | Disabled | 8GB | 5 | Yes |
| Capable | SD card | Enabled (4GB, zstd) | 4GB | 10 | SD warning |
| Low-end | SD card | Disabled | 4GB | 10 | SD warning |
| Capable | USB 3.0+ | Enabled (4GB, zstd) | 4GB | 10 | USB warning |
| Low-end | USB 3.0+ | Disabled | 4GB | 10 | USB warning |

### HDD Upgrade Warning

When the installer detects an HDD, the user sees this during the confirmation step:

```
`┌──────────────────────────────────────────────────┐`

`│                                                  │`

`│  💡  Storage Notice                                │`

`│                                                  │`

`│  Your laptop has a hard drive (HDD). Spike will   │`

`│  work, but a solid-state drive (SSD) would make   │`

`│  your system significantly faster.                │`

`│                                                  │`

`│  Consider upgrading to an SSD after installing     │`

`│  Spike. Even an inexpensive SSD will make a       │`

`│  noticeable difference.                           │`

`│                                                  │`

`│  \[Got it, continue\]                               │`

`│                                                  │`

`└──────────────────────────────────────────────────┘`
```

This is informational, not a blocker. The user can proceed with the HDD. The warning is also saved as a notification in the system history so the user can refer back to it later.

### SD Card / USB Warning

Similar warning for SD card and USB installations, noting that these are slower than internal SSDs and may affect performance.

## Settings Integration

### Memory Page

**Settings → Memory** provides a read-only overview of the memory system:

```
`Settings → Memory`

`├── Physical Memory`

`│   ├── Total: 4.0 GB`

`│   ├── In use: 2.1 GB`

`│   └── Available: 1.9 GB`

`├── ZRAM`

`│   ├── Status: Active (or Disabled)`

`│   ├── Algorithm: zstd`

`│   ├── Allocated: 4.0 GB`

`│   ├── Compressed: 1.2 GB`

`│   ├── Compression ratio: 3.1:1`

`│   └── Memory saved: 2.7 GB`

`├── Swap File`

`│   ├── Location: /swapfile`

`│   ├── Size: 8.0 GB`

`│   ├── In use: 0.2 GB`

`│   └── Priority: 10`

`├── Earlyoom`

`│   ├── Status: Active`

`│   ├── Memory threshold: 10%`

`│   ├── Swap threshold: 10%`

`│   └── Last action: (date/time or "No actions yet")`

`├── Swappiness`

`│   ├── Current value: 15 (tuned for SSD)`

`│   └── Slider (SSD only — see below)`

`└── CPU Classification`

`    ├── Cores: 2`

`    ├── Bogomips per core: 2200`

`    └── Classification: Capable (ZRAM enabled)`
```

### What Users Can Adjust

| **Setting** | **Adjustable?** | **Where** |
| :-: | :-: | :-: |
| ZRAM enable/disable | No | Install-time decision |
| ZRAM size | No | Install-time decision |
| ZRAM algorithm | No | Install-time decision |
| Swap file size | No | Install-time decision |
| Swappiness | Yes (SSD only, increase only) | **Settings → Memory** |
| Earlyoom threshold | Yes (Advanced) | **Settings → Advanced → Memory** |
| Earlyoom protected processes | No | Hardcoded (safety critical) |

### Swappiness Slider

Available only on SSD and NVMe systems. HDD, SD card, and USB installations have swappiness locked — no slider shown.

**Settings → Memory → Swappiness**

```
`┌──────────────────────────────────────────────┐`

`│  Swappiness                                   │`

`│                                              │`

`│  Controls how aggressively Spike moves        │`

`│  inactive data to the swap file.             │`

`│                                              │`

`│  Current: 15  (Default for SSD)              │`

`│                                              │`

`│  ──●───────────────────────────────────      │`

`│  15                                    60    │`

`│  (Default)                        (Maximum)  │`

`│                                              │`

`│  \[Reset to default\]                           │`

`└──────────────────────────────────────────────┘`
```

SSD/NVMe systems:

```
`├── Slider range: 15 (default) to 60 (maximum)`

`├── Cannot go below 15 (prevents overly conservative swapping)`

`├── Cannot exceed 60 (prevents excessive wear on SSD from constant swapping)`

`└── Change applies immediately (sysctl) and persists across reboots`
```

HDD systems:

```
`├── Swappiness locked at 5`

`├── No slider shown in Settings`

`├── Info note: "Swappiness is locked because your system uses a`

`│   hard drive. Higher values would cause performance issues."`

`└── User directed to HDD upgrade warning instead`
```

SD card / USB systems:

```
`├── Swappiness locked at 10`

`├── No slider shown`

`└── Same info note as HDD (adjusted wording)`
```

### Why This Design

- **Lower bound (15):** The tuned default. Going lower would keep too much data in RAM and starve active applications of memory on 4GB systems. 

- **Upper bound (60):** The kernel default. Going higher than 60 is unnecessary on 4GB systems with ZRAM already handling compression — the swap file is a secondary layer, not the primary one. 

- **SSD only:** HDD swapping causes severe latency. Letting a user increase swappiness on an HDD would make their system worse, not better. The slider simply doesn't appear. 

- **Increase only:** Preventing downward adjustment keeps the system safe. If someone sets swappiness too low, the kernel holds pages in RAM until ZRAM fills, then dumps everything to swap at once — causing stuttering. The tuned minimum prevents this. 

### When A User Might Adjust This

- **Heavy multitasker on SSD:** More aggressive swapping keeps the active app responsive by moving background app data to swap sooner. Slider set to 30–40. 

- **Single-app user:** Default of 15 is fine. No adjustment needed. 

- **Gaming on older hardware:** More aggressive swapping can help keep the game in RAM by pushing the desktop environment's inactive pages out. Slider set to 40–50. 

The slider includes a tooltip: "Higher values move inactive data to the swap file sooner, which can help when running many apps at once. Leave at default if unsure."

### Persistence

The swappiness value is written to a spike-specific config that takes precedence:

```
*`\# /etc/sysctl.d/99-spike-swappiness.conf`*

*`\# Managed by Spike Settings — DO NOT EDIT MANUALLY`*

*`\# Last modified: 2026-07-09 14:32:00`*

`vm.swappiness = 30`
```

If this file exists, it overrides the default from `99-spike-memory.conf`. If the user clicks "Reset to default," the file is removed and the system reverts to the install-time tuned value.

### Advanced Settings

Hidden behind "Show Advanced" toggle in Settings:

```
`Settings → Advanced → Memory:`

`├── Earlyoom memory threshold: 10% (slider: 5-20%)`

`├── Earlyoom swap threshold: 10% (slider: 5-20%)`

`└── Reset to defaults button`
```

Most users will never touch this. It exists for edge cases where a user's workload consistently triggers earlyoom and they want to adjust the threshold.

## Spike Plus Differences

Spike Standard:

```
`├── ZRAM: adaptive (capable CPUs only)`

`├── ZRAM size: equal to RAM, max 4GB`

`├── Swap: 8GB fixed`

`├── Swappiness: per storage type (adjustable on SSD, 15-60)`

`└── CPU governor: powersave`
```

Spike Plus:

```
`├── ZRAM: always enabled (modern CPUs can handle it)`

`├── ZRAM size: equal to RAM (no cap — 8GB on 8GB system, 16GB on 16GB system)`

`├── Swap: equal to RAM (8GB on 8GB system, 16GB on 16GB system)`

`├── Swappiness: 20 (SSD default — more headroom means slightly more aggressive swap is fine)`

`└── CPU governor: schedutil`
```

On Spike Plus, the memory page in Settings shows the same information but with different values reflecting the larger ZRAM and swap allocations. The swappiness slider is also available on SSD systems, with a default of 20 and a range of 20–60.

The Plus swappiness lower bound is 20 (rather than 15 on Standard) because Plus systems typically have more physical RAM (8GB+), meaning more pages can comfortably remain in RAM before swap is needed. A lower bound of 15 would be unnecessarily conservative on systems with adequate headroom — the slightly higher floor encourages the kernel to use the (also larger) swap file proactively, keeping physical RAM available for active workloads. This mirrors the Standard's design philosophy (prevent the "dump everything to swap at once" stutter) but tuned for the Plus memory profile.

## Failure Modes And Recovery

### ZRAM Device Fails To Initialize

**Scenario:** `zram` module fails to load at boot (corrupted module, kernel regression)

**Behavior:**

```
`├── udev rule fails silently (zram0 device not created)`

`├── System boots with swap file only`

`├── Notification on first login:`

`│   "ZRAM could not be started. Your system is using disk swap only.`

`│    Performance may be reduced. Restart your computer to try again."`

`└── System remains usable, just slower`
```

**Recovery:**

```
`├── Reboot usually resolves (transient module load failure)`

`├── If persistent: kernel update may fix (module regression)`

`└── If ZRAM is permanently broken: system falls back to swap-only profile`
```

### Swap File Corruption

**Scenario:** `/swapfile` becomes corrupted (filesystem error, unexpected power loss)

**Behavior:**

```
`├── Kernel logs swap errors`

`├── System may slow down significantly (no swap available)`

`├── earlyoom triggers more frequently`

`└── Notification: "Spike detected a problem with the swap file.`

`    System performance may be reduced. Restart to attempt repair."`
```

**Recovery:**

```
`├── fsck runs on next boot (ext4 journal recovery)`

`├── If swap file is still corrupt after fsck:`

`│   ├── spike-config recreates swap file`

`│   └── Notification: "Swap file repaired. Performance restored."`

`└── If filesystem is severely damaged: Spike Rescue tool from live ISO`
```

### Earlyoom Crashes

**Scenario:** earlyoom process crashes (unlikely but possible)

**Behavior:**

```
`├── systemd restarts earlyoom automatically (Restart=always in unit file)`

`├── Gap of \<2 seconds where no monitoring occurs`

`├── If earlyoom fails to restart repeatedly:`

`│   ├── systemd gives up after 5 attempts`

`│   ├── Notification: "Memory monitoring service stopped.`

`│    Your system will work but may freeze under heavy memory use.`

`│    Restart to restart the monitoring service."`

`└── Kernel OOM killer remains as fallback (less graceful but functional)`
```

## What This Document Does Not Cover

- **Kernel boot parameters affecting memory:** See `KERNEL.md` (zswap.enabled=0, transparent\_hugepage=madvise) 

- **zswap disabling rationale:** See `KERNEL.md` — zswap is disabled to prevent double-compression with ZRAM 

- **Install-time detection implementation:** See `INSTALLER.md` (hardware detection flow) 

- **Settings panel GUI implementation:** See `DESKTOP.md` (Settings architecture) 

- **General boot process:** See `BOOT-PROCESS.md` (sequence from power-on to desktop) 

- **Variant comparison:** See `VARIANT-DIFFERENCES.md` (full Standard vs Plus comparison) 

- **Performance benchmarks:** See `PERFORMANCE-BASELINES.md` (memory benchmarks under load) 

🐕 BigRangaTech


