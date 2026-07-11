# Spike Hardware

## Purpose

This document defines the complete hardware specification for Spike Linux — what hardware is supported, what is not, how hardware is detected at install time, and how the system adapts to different configurations. It is the authoritative reference for all hardware-related decisions throughout the project.

Every other document references this one for hardware tier definitions, detection logic, and classification rules. If a question about hardware support arises, the answer starts here.

## Hardware Tiers

Spike categorizes all hardware into three tiers. These tiers drive every performance target, variant recommendation, and design decision in the project.

### Tier 1 — Primary Target

**Definition:** Hardware that Spike is designed to run on as a daily driver. All performance baselines must be met on this hardware. If Spike doesn't work well here, the project has failed.

**Reference machine:**

```
`├── CPU:     Intel Celeron N4020 (Gemini Lake Refresh, 2019)`

`│   ├── Cores: 2 (2 threads, no hyperthreading)`

`│   ├── Base clock: 1.10 GHz`

`│   ├── Burst: 2.80 GHz`

`│   ├── Cache: 4MB`

`│   ├── Architecture: x86-64`

`│   └── TDP: 6W`

`├── RAM:     4GB DDR4-2400 (soldered, non-upgradable on most machines)`

`├── GPU:     Intel UHD Graphics 600`

`│   ├── Execution units: 12 EU`

`│   ├── Frequency: 300-650 MHz`

`│   ├── Max dynamic frequency: 700 MHz`

`│   ├── Video memory: shared (no VRAM)`

`│   └── Driver: i915 (upstream kernel)`

`├── Storage: 240GB SATA SSD (SATA III, 6Gb/s)`

`├── Display: 1440×900 or 1366×768 (14" laptop, TN panel typical)`

`├── Wi-Fi:   Intel or Realtek (varies by laptop manufacturer)`

`├── Audio:   Intel HD Audio (Realtek ALC codec typical)`

`├── Bluetooth: Present on most models (Intel or Realtek combo)`

`├── Camera:  720p webcam (USB internal)`

`├── Battery: 3-cell Li-ion, ~37Wh typical (~4-6 hours)`

`└── Typical machine: Acer Aspire, Lenovo IdeaPad, HP Stream 14`
```

**Why this hardware:**

```
`├── 2019 Celeron N4020 was in millions of budget laptops sold 2019-2021`

`├── These laptops are now 5-7 years old — prime candidates for revival`

`├── 4GB RAM was standard for budget laptops in this era`

`├── Windows 11 does not support N4020 (not on approved CPU list)`

`├── Windows 10 EOL pushes these machines into obsolescence`

`└── This is the exact population Spike exists to serve`
```

### Tier 1 Hardware Variations

The reference machine is the Celeron N4020, but Tier 1 encompasses a range of similar hardware:

**Tier 1 CPU range (all acceptable as primary target):**

```
`├── Intel Celeron N4000 (1.10 GHz base, 2.60 GHz burst — slightly slower)`

`├── Intel Celeron N4020 (1.10 GHz base, 2.80 GHz burst — reference)`

`├── Intel Celeron N4100 (quad-core, 1.10 GHz base, 2.40 GHz burst — slightly better)`

`├── Intel Celeron N4120 (quad-core, 1.10 GHz base, 2.60 GHz burst — slightly better)`

`├── Intel Pentium Silver N5000 (quad-core, 1.10 GHz base, 2.70 GHz — Tier 1+)`

`└── Intel Pentium Silver N5030 (quad-core, 1.10 GHz base, 3.10 GHz — Tier 1+)`
```

**Common characteristics:**

```
`├── All are Gemini Lake / Gemini Lake Refresh architecture`

`├── All use Intel UHD Graphics 600 or 605`

`├── All use the i915 driver`

`├── All support VA-API (but NOT AV1 hardware decode)`

`├── All have 6W TDP (passive cooling on most laptops)`

`├── All are soldered (BGA package, not socketed)`

`└── All were sold in budget laptops (sub-$300 new)`
```

**Tier 1 RAM range:**

```
`├── 4GB (reference, minimum acceptable)`

`├── 6GB (better — some N4100 laptops shipped with this)`

`├── 8GB (comfortable — Spike Plus recommended)`

`└── Above 8GB: still Tier 1, but Spike Plus is strongly recommended`
```

**Tier 1 storage range:**

```
`├── 128GB SSD (minimum, tight but functional)`

`├── 240GB SSD (reference)`

`├── 256GB SSD (common)`

`├── 500GB HDD (acceptable but slow — HDD Tier 1 baselines apply)`

`├── 128GB eMMC (NOT ACCEPTABLE — see "Unsupported Storage")`

`└── NVMe (if laptop has NVMe slot — faster than SATA)`
```

### Tier 2 — Worst Case

**Definition:** Hardware below Tier 1 specifications. Spike must boot, install, and remain functional. Performance may be degraded — that is acceptable. Unusable is not.

**Reference machine:**

```
`├── CPU:     AMD A4 (Jaguar architecture, pre-2016)`

`│   ├── Cores: 2`

`│   ├── Base clock: ~1.5 GHz (varies by model)`

`│   ├── Architecture: x86-64`

`│   └── TDP: varies (15W typical)`

`├── RAM:     4GB DDR3`

`├── GPU:     AMD Radeon HD (integrated)`

`│   ├── Driver: amdgpu or radeon (depending on age)`

`│   └── VA-API: partial support (may not work on all models)`

`├── Storage: 500GB SATA HDD (5400 RPM typical)`

`├── Display: 1366×768 (15.6" laptop)`

`├── Wi-Fi:   varies (older adapters, may need firmware)`

`└── Typical machine: Lenovo ThinkPad with AMD A4`
```

**Why this hardware:**

```
`├── Represents the floor of what can reasonably run a modern Linux desktop`

`├── AMD A4 is significantly weaker than Celeron N4020`

`├── HDD (not SSD) represents the worst I/O scenario`

`├── If Spike works here, it works on anything in the target range`

`└── Some users will have laptops this old and want to try Spike`
```

**Tier 2 CPU range (all acceptable as worst case):**

```
`├── AMD A4 (Jaguar, 2013-2015)`

`├── AMD A6 (Jaguar/Richland, 2013-2016)`

`├── AMD A9 (Stoney Ridge, 2016-2018)`

`├── Intel Celeron N3060 (Braswell, 2016 — dual-core, very slow)`

`├── Intel Celeron N3350 (Apollo Lake, 2017 — dual-core)`

`├── Intel Atom x5/x7 (Cherry Trail, 2015-2016)`

`└── Any x86-64 CPU with ≥ 2 cores and ≥ 1.0 GHz base clock`
```

**What is BELOW Tier 2 (not supported):**

```
`├── Single-core CPUs`

`├── CPUs with base clock below 1.0 GHz`

`├── 32-bit (x86) processors`

`├── ARM processors`

`├── RAM below 4GB`

`├── Any CPU older than ~2013`

`└── Intel Atom Z series (pre-Cherry Trail, single-core or extremely slow)`
```

### Tier 3 — Development Machine

**Definition:** High-performance hardware used for development and regression testing. NOT representative of target users. Performance here must be excellent — if it's not, something is wrong with Spike, not the hardware.

**Reference machine:**

```
`├── CPU:     Intel Xeon E3-1505M v5 (ThinkPad P50)`

`│   ├── Cores: 4 (8 threads)`

`│   ├── Base clock: 2.80 GHz`

`│   └── Cache: 8MB`

`├── RAM:     32GB DDR4`

`├── GPU:     NVIDIA Quadro M2000M (2GB VRAM)`

`├── Storage: 512GB NVMe SSD`

`└── Display: 1920×1080 (15.6")`
```

**Purpose:**

```
`├── Building ISOs (fast compilation)`

`├── Running CI/CD pipelines (if self-hosted runner)`

`├── Testing in VMs (not for performance measurement)`

`├── Regression testing (does Spike break on fast hardware?)`

`├── Developing Spike Shell (fast iteration)`

`└── Not a target — just a dev tool`
```

**Tier 3 also includes:**

```
`├── Any modern Core i5/i7/Xeon machine`

`├── Any machine with 8GB+ RAM and SSD`

`├── Any machine with a dedicated GPU`

`└── These are NOT Spike's audience — Spike works here by accident, not design`
```

**Testing on Tier 3:**

```
`├── Verifies no regressions on fast hardware`

`├── Identifies hardware-specific bugs (NVIDIA, high-DPI, multi-monitor)`

`├── Does NOT validate performance baselines`

`└── A Spike build that is slow on Tier 3 has a real bug`
```

## Minimum Requirements

Absolute minimums (installer will refuse to proceed if not met):

| **Requirement** | **Minimum** | **Recommended** |
| :-: | :-: | :-: |
| CPU architecture | x86-64 | x86-64 |
| CPU cores | 2 | 2+ |
| CPU base clock | 1.0 GHz | 1.1 GHz+ |
| RAM | 4 GB | 4 GB (Standard) / 8 GB (Plus) |
| Storage capacity | 128 GB | 240 GB+ |
| Storage type | SATA SSD | SATA SSD or NVMe |
| Storage interface | SATA III | SATA III or NVMe |
| Display | 1366×768 | 1440×900+ |
| Boot mode | UEFI or BIOS | UEFI |
| USB port | USB 3.0 | USB 3.0+ |
| Internet (for install) | Optional | Recommended |

**Hard refusals (installation will not proceed):**

```
`├── RAM \< 4GB → "Spike requires at least 4GB of RAM"`

`├── Storage \< 128GB → "Spike requires at least 128GB of storage"`

`├── Storage type eMMC → "Spike does not support eMMC storage"`

`├── Storage type USB 2.0 → "Spike requires USB 3.0 or faster storage"`

`├── 32-bit CPU → "Spike requires a 64-bit processor"`

`├── Single-core CPU → "Spike requires at least a dual-core processor"`

`└── No storage device detected → "No suitable storage device found"`
```

## Storage Support

### Supported Storage Types

**SATA SSD (recommended):**

```
`├── Most common in Tier 1 target hardware`

`├── Speed: 500-550 MB/s read (SATA III)`

`├── SPIKE STATUS: Fully supported, primary target`

`├── Boot performance: 15-25s (Tier 1)`

`├── App launch: 1-2s average`

`└── Example: Kingston A400, Crucial BX500, Samsung 870 EVO`
```

**NVMe SSD (best):**

```
`├── Present in some newer budget laptops and Tier 3 machines`

`├── Speed: 1500-3500 MB/s read (PCIe 3.0 x4)`

`├── SPIKE STATUS: Fully supported`

`├── Boot performance: 12-20s (Tier 1)`

`├── App launch: \<1-2s average`

`├── Detected as /dev/nvme0n1 (namespace, not partition letter)`

`└── Example: WD Blue SN570, Samsung 970 EVO, Crucial P3`
```

**SATA HDD (supported, slow):**

```
`├── Present in Tier 2 hardware and some older Tier 1 machines`

`├── Speed: 80-150 MB/s read (5400 RPM), 100-200 MB/s (7200 RPM)`

`├── SPIKE STATUS: Supported with degraded performance`

`├── Boot performance: 30-50s (Tier 1), 45-65s (Tier 2)`

`├── App launch: 3-5s average`

`├── Swappiness: set to 5 (minimize swap, keep in RAM)`

`├── ZRAM: still enabled (CPU-based, not I/O dependent)`

`├── Upgrade recommendation shown to user`

`└── Example: Seagate Barracuda, WD Blue, Toshiba MK series`
```

**SD Card (USB 3.0+ class):**

```
`├── Some laptops have SD card slots or users use USB SD readers`

`├── Speed: varies widely (UHS-I: 50-104 MB/s, UHS-II: 156-312 MB/s)`

`├── SPIKE STATUS: Technically supported if UHS-I+ speed`

`├── Boot performance: 35-55s (Tier 1, UHS-I)`

`├── Not recommended for daily use (wear, reliability, speed)`

`├── Useful for testing/installing, not for production`

`└── Installation warning shown if SD card selected`
```

**USB 3.0+ external drive:**

```
`├── External SSD or fast flash drive via USB 3.0`

`├── Speed: 300-500 MB/s (USB 3.0 SSD enclosure)`

`├── SPIKE STATUS: Supported`

`├── Boot performance: 30-50s (depends on USB controller and drive)`

`├── Useful for portable Spike installations`

`└── Not ideal for daily use (USB connection reliability)`
```

### Unsupported Storage Types

**eMMC:**

```
`├── Why not supported:`

`│   ├── eMMC is slower than SATA SSD (50-400 MB/s, varies wildly)`

`│   ├── eMMC has limited write endurance (often lower than consumer SSDs)`

`│   ├── eMMC is typically small (32-128GB, often 64GB)`

`│   ├── eMMC degradation causes system slowdown over time`

`│   └── Laptops with eMMC often have no SATA/NVMe slot (dead end)`

`├── Installer behavior: refuses with explanation`

`├── Message: "eMMC storage is not supported by Spike. eMMC storage`

`│   has limited performance and lifespan. If your laptop has a`

`│   SATA or NVMe slot, install Spike on that instead."`

`└── If no other storage: Spike cannot be installed`
```

**USB 2.0 drive:**

```
`├── Why not supported:`

`│   ├── USB 2.0 maximum: 480 Mbps theoretical, ~40 MB/s real-world`

`│   ├── Boot time would exceed 90 seconds`

`│   ├── App launches would take 5-10+ seconds`

`│   ├── System would feel unusably slow`

`│   └── User experience would contradict Spike's mission`

`├── Installer behavior: refuses with explanation`

`├── Message: "This USB drive is USB 2.0, which is too slow for Spike.`

`│   Please use a USB 3.0 or faster drive."`

`└── Detection: USB device reports speed; \< 4800 Mbps = USB 2.0`
```

**Optical disc (CD/DVD/Blu-ray):**

```
`├── Why not supported:`

`│   ├── Read-only media (cannot install onto an optical disc)`

`│   ├── Boot-from-DVD would be extremely slow`

`│   ├── ISO exceeds CD capacity (3.1GB \> 700MB)`

`│   └── Modern laptops don't have optical drives`

`├── Installer behavior: N/A (not a target for installation)`

`└── Booting the installer from DVD: technically possible but unsupported`
```

### Storage Detection Logic

**Storage detection (at install time):**

```
`├── Scan /sys/block/ for block devices`

`├── Classify each device:`

`│   ├── Read /sys/block/sdX/queue/rotational`

`│   │   ├── 0 = SSD (non-rotational)`

`│   │   └── 1 = HDD (rotational)`

`│   ├── Read /sys/block/sdX/queue/scheduler (check if NVMe)`

`│   ├── Read /sys/block/mmcblkX/ (eMMC detection)`

`│   ├── Read USB device tree for USB-attached storage`

`│   └── Read device size from /sys/block/sdX/size`

`├── Classification:`

`│   ├── /dev/sdX with rotational=0 → SATA SSD`

`│   ├── /dev/sdX with rotational=1 → SATA HDD`

`│   ├── /dev/nvmeXnY → NVMe SSD`

`│   ├── /dev/mmcblkX → eMMC (reject)`

`│   └── USB devices: check USB version (reject if 2.0)`

`├── Size check: device\_size \>= 128GB (reject if below)`

`└── Multiple devices: present all qualifying devices, user selects`
```

**Storage classification drives:**

```
`├── Swappiness setting (HDD: 5, SSD: 10)`

`├── ZRAM priority (always enabled if CPU capable)`

`├── Boot time expectation (shown to user)`

`├── Upgrade recommendation (HDD → SSD)`

`└── Performance baseline target (HDD vs SSD targets)`
```

### Partitioning Scheme

**UEFI systems (primary):**

```
`├── /dev/sda1 (or /dev/nvme0n1p1)`

`│   ├── Mount: /boot/efi`

`│   ├── Type: EFI System Partition (FAT32)`

`│   ├── Size: 512MB`

`│   └── Flags: boot, esp`

`├── /dev/sda2 (or /dev/nvme0n1p2)`

`│   ├── Mount: /`

`│   ├── Type: Linux filesystem (ext4)`

`│   ├── Size: remainder of disk`

`│   └── Flags: none`

`└── Swap: swap file (not partition)`

`    ├── Location: /swapfile`

`    ├── Size: 2GB (Standard) / 4GB (Plus)`

`    └── Type: regular file (not btrfs swap subvolume)`
```

**BIOS systems (fallback):**

```
`├── /dev/sda1`

`│   ├── Mount: /boot`

`│   ├── Type: ext4`

`│   ├── Size: 512MB`

`│   └── Flags: boot`

`├── /dev/sda2`

`│   ├── Mount: /`

`│   ├── Type: ext4`

`│   ├── Size: remainder of disk`

`│   └── Flags: none`

`└── Swap: swap file (same as UEFI)`
```

**Why swap file instead of swap partition:**

```
`├── Easier to resize (no partition table editing)`

`├── No need for LVM`

`├── ZRAM handles most swap needs (swap file is fallback)`

`├── Hibernate requires swap file \>= RAM (checked at install)`

`├── If RAM \> 4GB: swap file = RAM size (for hibernate)`

`├── If RAM \<= 4GB: swap file = 2GB (hibernate may not be available)`

`└── Swap file priority: 10 (lower than ZRAM priority: 100)`
```

**Partition layout visualization (UEFI, 240GB SSD):**

```
`┌─────────┬──────────────────────────────────────────────────────┐`

`│ 512MB   │                    ~239GB                            │`

`│ FAT32   │                    ext4                               │`

`│ /boot/efi│                   /                                  │`

`│ ESP     │                    (root + swapfile inside)           │`

`└─────────┴──────────────────────────────────────────────────────┘`
```

**No separate /home partition:**

```
`├── Beginners don't need to understand partitions`

`├── Reinstallation uses Spike Rescue (data backup, not partition preservation)`

`├── LVM adds complexity without benefit for single-disk laptops`

`└── A separate /home is a power-user feature; Spike doesn't target power users`
```

## GPU Support

### Intel GPUs (Primary Target)

**Intel UHD Graphics 600 / 605 (Gemini Lake):**

```
`├── Driver: i915 (upstream kernel module)`

`├── VA-API: Supported (Intel media driver — iHD)`

`│   ├── H.264: ✓ (decode + encode)`

`│   ├── H.265/HEVC: ✓ (decode only)`

`│   ├── VP9: ✓ (decode only)`

`│   ├── AV1: ✗ (no hardware decode — disabled in Firefox)`

`│   └── VC-1: ✓ (decode only)`

`├── OpenGL: OpenGL 4.6 (Mesa, software vertex processing)`

`│   └── Performance: adequate for compositor, not for 3D gaming`

`├── Vulkan: not supported (12 EU is too limited)`

`├── Wayland: fully supported (KWin uses DRM/KMS)`

`├── Multi-monitor: supported (HDMI, DisplayPort, eDP)`

`├── Color depth: 8-bit (no 10-bit support)`

`└── Known issues:`

`    ├── Firefox AV1 video: disabled (forces VP9/H.264 fallback)`

`    └── WebGL: works but slow (acceptable for web compatibility)`
```

**Intel HD Graphics 500 (Apollo Lake):**

```
`├── Same driver stack as UHD 600`

`├── Slightly lower performance`

`├── Same VA-API capabilities (minus some HEVC profiles)`

`└── Treated as Tier 1 (or Tier 2 depending on CPU)`
```

**Intel HD Graphics (Bay Trail / Cherry Trail):**

```
`├── Driver: i915`

`├── VA-API: limited (older media driver — legacy path)`

`├── Often paired with Atom CPUs (Tier 2)`

`├── OpenGL: 4.5 (Mesa)`

`└── Performance: degraded but functional`
```

**Intel HD Graphics 510+ (Skylake and newer):**

```
`├── Better than target (these are in Tier 3 machines)`

`├── Full VA-API including some AV1 (on newer Intel)`

`├── Excellent OpenGL/Vulkan support`

`└── Spike runs perfectly — overkill for Tier 1`
```

### AMD GPUs

**AMD Radeon HD (integrated, pre-GCN):**

```
`├── Driver: radeon (legacy)`

`├── VA-API: partial or none (depends on specific model)`

`├── OpenGL: limited (older Mesa support)`

`├── Wayland: basic (modesetting driver)`

`├── Found in: Tier 2 hardware (AMD A4/A6)`

`├── Performance: degraded`

`└── Expected behavior: compositor works, video decode may fall to CPU`
```

**AMD Radeon (GCN and newer):**

```
`├── Driver: amdgpu`

`├── VA-API: supported (Mesa VDPAU/VA-API bridge)`

`├── OpenGL: fully supported`

`├── Wayland: fully supported`

`├── Found in: AMD A-series APUs (A9, A12), Ryzen integrated`

`├── Performance: generally good on GCN+, excellent on RDNA`

`└── Treated as: Tier 1 if CPU qualifies, Tier 2 otherwise`
```

**AMD discrete GPUs:**

```
`├── Driver: amdgpu`

`├── Works the same as integrated AMD`

`├── Not a target (budget laptops rarely have discrete AMD)`

`└── If present: treated like integrated, no special handling`
```

### NVIDIA GPUs

NVIDIA GPUs are categorized into three groups:

**Group 1: Maxwell and newer (GTX 900 series+, 2014+):**

```
`├── Driver options:`

`│   ├── nouveau (open source): works but no reclocking on most cards`

`│   └── nvidia (proprietary): full performance, but complex on Wayland`

`├── VA-API: only via proprietary driver (VDPAU wrapper)`

`├── Wayland: works with nvidia driver (GBM API, 525+)`

`├── Found in: Tier 3 machines (ThinkPad P50 with Quadro M2000M)`

`├── Spike behavior:`

`│   ├── Spike Standard: Integrated GPU mode (prefers Intel iGPU if hybrid)`

`│   ├── Spike Plus: Hybrid GPU mode (uses NVIDIA for demanding tasks)`

`│   └── Proprietary driver: installed if NVIDIA detected and user opts in`

`└── Not a target — but must not crash`
```

**Group 2: Kepler (GTX 600/700 series, 2012-2014):**

```
`├── Driver: nouveau only (nvidia proprietary dropped Kepler support)`

`├── No reclocking → performance is severely limited`

`├── VA-API: not supported`

`├── Wayland: basic (modesetting)`

`├── Rare in target hardware`

`└── Spike behavior: uses nouveau, warns about limited performance`
```

**Group 3: Fermi and older (GTX 400/500 series and older):**

```
`├── Driver: nouveau (may work for basic display)`

`├── No acceleration`

`├── VA-API: not supported`

`├── Found in: very old hardware (pre-2012)`

`├── Likely below Tier 2 threshold`

`└── Spike behavior: basic display only, no acceleration`
```

### NVIDIA Hybrid Graphics (Optimus)

Laptops with both Intel integrated and NVIDIA discrete GPU:

**Spike Standard:**

```
`├── GPU mode: Integrated`

`├── Intel iGPU handles all rendering`

`├── NVIDIA GPU powered off (if possible — runtime D3)`

`├── No nvidia driver loaded`

`├── nouveau may load for display init, then suspend`

`├── Battery life: maximized`

`└── Performance: Intel iGPU only`
```

**Spike Plus:**

```
`├── GPU mode: Hybrid`

`├── Intel iGPU handles compositing and light tasks`

`├── NVIDIA GPU activates for demanding applications`

`├── PRIME offloading: application requests NVIDIA render node`

`├── nvidia driver installed (if user opted in)`

`├── If nvidia driver not installed: falls back to Integrated mode`

`├── Battery life: reduced when NVIDIA GPU is active`

`└── Performance: best available`
```

**Driver selection at install time:**

```
`├── NVIDIA detected → installer asks:`

`│   "An NVIDIA GPU was detected. Spike can use the open-source`

`│    driver (nouveau) or the proprietary NVIDIA driver. The`

`│    proprietary driver offers better performance but is not`

`│    open source. Which would you prefer?"`

`│   ├── \[Proprietary (recommended for Plus)\]`

`│   ├── \[Open source (nouveau)\]`

`│   └── \[Decide later\]`

`├── If proprietary selected:`

`│   ├── nvidia-driver package installed`

`│   ├── Kernel module built (or prebuilt for LTS kernel)`

`│   ├── Prime sync configured`

`│   └── Reboot required`

`└── If open source:`

`    ├── nouveau loaded`

`    ├── No additional packages`

`    └── May show performance warning in Settings`
```

### GPU Driver Selection Logic

**GPU driver selection (at install time):**

```
`1. Detect GPU vendor(s):`

`   ├── lspci -nn | grep -E "VGA|3D|Display"`

`   ├── Read /sys/class/drm/card\*/device/vendor`

`   └── Identify: Intel only, AMD only, NVIDIA only, or hybrid (Intel+NVIDIA)`


`2. Select driver:`

`   ├── Intel only:`

`   │   ├── Driver: i915 (built into kernel, no action needed)`

`   │   ├── VA-API driver: iHD (intel-media-driver package)`

`   │   └── No user prompt needed`

`   ├── AMD only:`

`   │   ├── Driver: amdgpu (or radeon for pre-GCN)`

`   │   ├── VA-API driver: mesa-va-drivers`

`   │   └── No user prompt needed`

`   ├── NVIDIA only (rare in laptops):`

`   │   ├── Driver: nouveau (default, no prompt)`

`   │   ├── If GPU is Maxwell+: offer proprietary driver option`

`   │   └── VA-API: none (or VDPAU wrapper if proprietary)`

`   └── Hybrid (Intel + NVIDIA):`

`       ├── Default: Intel iGPU (Integrated mode)`

`       ├── If variant == Plus: offer NVIDIA driver option`

`       └── If variant == Standard: stay on Intel, NVIDIA off`


`3. Write to spike-config state store:`

`   ├── gpu\_vendor: intel/amd/nvidia/hybrid`

`   ├── gpu\_driver: i915/amdgpu/nouveau/nvidia`

`   ├── vaapi\_driver: iHD/radeonsi/vdpau/none`

`   ├── gpu\_mode: integrated/hybrid (hybrid only)`

`   └── vaapi\_working: true/false (tested at first boot)`


`4. Apply configuration:`

`   ├── Module blacklist: blacklist conflicting drivers`

`   ├── Environment variables: LIBVA\_DRIVER\_NAME set in session`

`   ├── Firefox prefs: media.ffmpeg.vaapi.enabled = true`

`   ├── Firefox prefs: media.av1.enabled = false (if no AV1 decode)`

`   └── Modprobe configuration: load correct driver at boot`
```

## CPU Detection And Classification

**CPU detection (at install time):**

```
`1. Read CPU information:`

`   ├── lscpu (architecture, cores, threads, frequency, cache)`

`   ├── /proc/cpuinfo (model name, bogomips, flags)`

`   ├── Read vendor: GenuineIntel or AuthenticAMD`

`   └── Read microarchitecture (from CPUID)`


`2. Classify CPU:`

`   ├── Read bogomips:`

`   │   ├── Approximate performance metric (not precise, but reliable enough)`

`   │   ├── Bogomips per core ≈ 2 × clock speed (MHz)`

`   │   └── Total bogomips = bogomips\_per\_core × core\_count`

`   ├── Read core count:`

`   │   ├── 1 core: REJECT (below minimum)`

`   │   ├── 2 cores: acceptable (Tier 1 or Tier 2)`

`   │   ├── 4 cores: good (Tier 1+ or Tier 3)`

`   │   └── 8+ cores: Tier 3`

`   ├── Read architecture:`

`   │   ├── x86-64: proceed`

`   │   ├── x86 (32-bit): REJECT`

`   │   ├── aarch64: REJECT (not supported)`

`   │   └── Other: REJECT`

`   ├── Estimate performance class:`

`   │   ├── Based on bogomips × cores, vendor, and microarchitecture`

`   │   ├── Classify as: Tier 1, Tier 1+, Tier 2, Tier 3, or REJECT`

`   │   └── This classification drives variant recommendation`

`   └── ZRAM capability check:`

`       ├── CPU benchmark: quick compression test`

`       ├── If compresses \> 2.5:1 in \<100ms: ZRAM enabled`

`       ├── If compresses \< 2.5:1 or takes \>200ms: ZRAM disabled`

`       │   (very weak CPU — ZRAM overhead not worth it)`

`       └── If ZRAM disabled: warn user about reduced memory capacity`


`3. CPU governor selection:`

`   ├── Spike Standard: powersave (max battery, minimal heat)`

`   ├── Spike Plus: schedutil (scales with load, better responsiveness)`

`   └── Governor set via spike-config (cpupower or sysfs)`
```

### CPU-Specific Optimizations

**Intel Gemini Lake (N4000/N4020/N4100/N4120):**

```
`├── Kernel: i915 GPU driver (built-in)`

`├── VA-API: intel-media-driver (iHD)`

`├── Boot parameters:`

`│   ├── transparent\_hugepage=madvise (prevents khugepaged overhead)`

`│   └── zswap.enabled=0 (conflicts with ZRAM)`

`├── Kernel modules:`

`│   ├── Load: i915, snd\_hda\_intel, intel\_lpss (for I2C/SPI peripherals)`

`│   └── Blacklist: none specific (Gemini Lake is well-supported)`

`├── Known issues:`

`│   ├── AV1 decode: not supported (Firefox AV1 disabled)`

`│   ├── Some N4000 laptops have Bay Trail audio (different codec)`

`│   └── HDMI audio: works but may need manual route selection`

`└── sysctl tunables:`

`    ├── vm.swappiness: 10 (SSD) or 5 (HDD)`

`    ├── vm.vfs\_cache\_pressure: 50`

`    └── kernel.nmi\_watchdog: 0 (saves ~1% CPU)`
```

**AMD Jaguar (A4/A6):**

```
`├── Kernel: amdgpu or radeon (depending on specific GPU)`

`├── VA-API: mesa-va-drivers (may not work on all models)`

`├── Boot parameters: same as Intel (THP=madvise, zswap=0)`

`├── Kernel modules:`

`│   ├── Load: amdgpu (or radeon), snd\_hda\_intel`

`│   └── Blacklist: radeon IF amdgpu is used (mutually exclusive)`

`├── Known issues:`

`│   ├── GPU driver: radeon vs amdgpu conflict (must pick one)`

`│   ├── VA-API: inconsistent across models (test at first boot)`

`│   └── Some models: video decode falls back to CPU (acceptable on Tier 2)`

`└── sysctl tunables:`

`    ├── vm.swappiness: 5 (HDD almost always on Tier 2)`

`    ├── vm.vfs\_cache\_pressure: 50`

`    └── kernel.nmi\_watchdog: 0`
```

**Intel Skylake+ (Tier 3):**

```
`├── Kernel: i915 (same as Gemini Lake, newer feature set)`

`├── VA-API: intel-media-driver (iHD, full support including some AV1)`

`├── Boot parameters: same`

`├── No special handling needed`

`└── Spike runs at full performance`
```

**NVIDIA (Tier 3, hybrid):**

```
`├── See GPU Support section above`

`├── Kernel modules:`

`│   ├── Load: i915 (Intel iGPU), nvidia (if proprietary, Plus only)`

`│   ├── Blacklist: nouveau (if proprietary selected)`

`│   └── Blacklist: nvidia (if nouveau selected)`

`├── Boot parameters:`

`│   └── nvidia-drm.modeset=1 (if proprietary, for Wayland)`

`└── PRIME sync: configured if hybrid mode`
```

## RAM Detection And Classification

**RAM detection (at install time):**

```
`1. Read RAM information:`

`   ├── free -h (total available RAM)`

`   ├── dmidecode -t memory (detailed: type, speed, slots, soldered/upgradable)`

`   └── /proc/meminfo (MemTotal)`


`2. Classify RAM:`

`   ├── \< 4GB: REJECT (below minimum)`

`   ├── 4GB: Tier 1 or Tier 2 (variant recommendation depends on CPU/storage)`

`   ├── 6GB: Tier 1 (Spike Plus if CPU and GPU qualify)`

`   ├── 8GB: Spike Plus recommended (if CPU/GPU qualify)`

`   ├── 16GB+: Spike Plus strongly recommended`

`   └── 32GB: Tier 3 (dev machine)`


`3. RAM classification drives:`

`   ├── Variant recommendation (Standard for ≤4GB, Plus for 8GB+)`

`   ├── ZRAM size calculation:`

`   │   ├── Standard: min(RAM, 4GB) — capped at 4GB`

`   │   ├── Plus: RAM (uncapped, but practically limited by CPU)`

`   │   └── If ZRAM capability test fails: 0GB (disabled)`

`   ├── Swap file size:`

`   │   ├── RAM ≤ 4GB: 2GB swap file (hibernate may not be available)`

`   │   ├── RAM \> 4GB: swap file = RAM size (for hibernate)`

`   │   └── If RAM ≥ 16GB: swap file = 8GB (hibernate resume time reasonable)`

`   ├── Earlyoom threshold:`

`   │   ├── 10% of (RAM + ZRAM + swap)`

`   │   ├── Standard (4GB RAM + 4GB ZRAM + 2GB swap): trigger at ~1GB free`

`   │   └── Plus (8GB RAM + 8GB ZRAM + 4GB swap): trigger at ~2GB free`

`   └── swappiness: 10 (SSD) or 5 (HDD)`


`4. RAM upgrade detection:`

`   ├── dmidecode shows if RAM is soldered or in SO-DIMM slots`

`   ├── If soldered (non-upgradable): no upgrade suggestion`

`   ├── If SO-DIMM (upgradable): suggest upgrade if RAM \< 8GB`

`   └── Suggestion: "Your laptop's RAM can be upgraded. Adding more`

`       RAM would improve performance with multiple applications."`
```

## Wi-Fi Support

### Supported Wi-Fi Adapters

All major Wi-Fi vendors are supported with firmware pre-installed on the ISO:

**Intel Wi-Fi:**

```
`├── Driver: iwlwifi (kernel module)`

`├── Firmware: linux-firmware package (iwlwifi-\*.ucode)`

`├── Common chips in target hardware:`

`│   ├── Intel Wireless-AC 9560 (common in Gemini Lake laptops)`

`│   ├── Intel Wireless-AC 9462 (common in Gemini Lake laptops)`

`│   ├── Intel Dual Band Wireless-AC 3165/3168`

`│   └── Intel Wireless 7265 (older, still common)`

`├── Secure Boot: works out of the box (Intel firmware is signed)`

`├── Performance: excellent on all Intel Wi-Fi`

`└── Known issues: none significant`
```

**Realtek Wi-Fi:**

```
`├── Driver: varies by chipset`

`│   ├── rtl8723de: rtw88\_8822be or rtl8723de (newer kernel has rtw88)`

`│   ├── rtl8821ce: rtl8821ce (out-of-tree, included on ISO)`

`│   ├── rtl8822bu: rtl8822bu (out-of-tree, included on ISO)`

`│   ├── rtl8188eu: rtl8188eu (older, but works)`

`│   └── rtl8192eu: rtl8192eu (older, but works)`

`├── Firmware: linux-firmware (most in-tree), DKMS for out-of-tree`

`├── Common in: budget laptops (Acer, Lenovo, HP with Realtek Wi-Fi)`

`├── Secure Boot: may require MOK enrollment for DKMS modules`

`├── Performance: adequate (Realtek is generally slower than Intel)`

`└── Known issues:`

`    ├── Some RTL8821CE chips have driver stability issues`

`    └── DKMS modules must be rebuilt on kernel update (automatic)`
```

**Qualcomm Atheros Wi-Fi:**

```
`├── Driver: ath9k (older) or ath10k (newer) or ath11k (newest)`

`├── Firmware: linux-firmware (ath9k\_hw, ath10k firmware)`

`├── Common chips:`

`│   ├── AR9485 (ath9k, single-band 2.4GHz)`

`│   ├── QCA9377 (ath10k, dual-band)`

`│   ├── QCA6174 (ath10k, dual-band, AC)`

`│   └── WCN3990 (ath10k, found in some SoCs)`

`├── Secure Boot: works (in-tree modules, signed firmware)`

`├── Performance: good (ath10k is reliable)`

`└── Known issues: ath10k firmware load occasionally slow (10-15s)`
```

**Broadcom Wi-Fi:**

```
`├── Driver: wl (proprietary, broadcom-sta-dkms)`

`├── Firmware: proprietary driver package (not in linux-firmware)`

`├── Common chips:`

`│   ├── BCM43428 (AirPort Extreme, older Macs — unlikely in target hardware)`

`│   ├── BCM4360 (some older laptops)`

`│   ├── BCM4356 (some Chromebook-style devices)`

`│   └── BCM43142 (older laptops, Bluetooth combo)`

`├── Secure Boot: requires MOK enrollment (DKMS, unsigned module)`

`│   ├── First boot: blue MOK enrollment screen`

`│   ├── User enrolls key (follows on-screen prompts)`

`│   └── After enrollment: Wi-Fi works`

`├── Performance: adequate (Broadcom Linux drivers are not great)`

`├── Known issues:`

`│   ├── MOK enrollment is confusing for beginners (first-boot wizard helps)`

`│   ├── bcmwl driver occasionally breaks on kernel update (DKMS rebuild)`

`│   └── No open-source driver for these chips (wl is the only option)`

`└── Rarity: uncommon in 2019-era Celeron laptops (usually Intel or Realtek)`
```

### Wi-Fi Detection Logic

**Wi-Fi detection (at install time):**

```
`1. Detect Wi-Fi adapter:`

`   ├── lspci -nn | grep -E "Network controller|Wireless"`

`   ├── lsusb | grep -i wireless (USB Wi-Fi adapters)`

`   ├── Read vendor ID and device ID`

`   └── Identify chipset family`


`2. Verify firmware availability:`

`   ├── Check if firmware file exists: /lib/firmware/`

`   ├── Check if driver module can load: modprobe \[driver\]`

`   ├── Check dmesg for firmware load success/failure`

`   └── If firmware missing: package additional firmware on ISO`


`3. Driver installation:`

`   ├── In-tree drivers (Intel, Atheros): no action needed (already in kernel)`

`   ├── DKMS drivers (Realtek out-of-tree): install dkms package, build module`

`   └── Proprietary drivers (Broadcom): install broadcom-sta-dkms, configure MOK`


`4. Module blacklist (conflicting drivers):`

`   ├── If using rtw88 (newer Realtek): blacklist older rtl8xxxu`

`   ├── If using amdgpu: blacklist radeon (GPU, but same principle)`

`   └── If using wl (Broadcom): blacklist b43, ssb, bcma, brcmsmac`


`5. Store in spike-config state:`

`   ├── wifi\_vendor: intel/realtek/atheros/broadcom/none`

`   ├── wifi\_driver: iwlwifi/rtw88\_xxx/ath10k/wl/none`

`   ├── wifi\_firmware\_loaded: true/false`

`   ├── wifi\_requires\_mok: true/false (Broadcom + Secure Boot)`

`   └── wifi\_secure\_boot\_compatible: true/false`


`6. Post-install verification (first boot):`

`   ├── Check iwconfig/ip link for wlan0 or wlpXsY`

`   ├── Check NetworkManager detects adapter`

`   ├── If adapter not detected: log error, show diagnostic in Settings`

`   └── If adapter detected but no networks: normal (user configures after install)`
```

### Wi-Fi 6 / Wi-Fi 6E / Wi-Fi 7

**Wi-Fi 6 (802.11ax):**

```
`├── Supported on newer Intel AX200/AX201/AX210 cards`

`├── Driver: iwlwifi (same as older Intel Wi-Fi)`

`├── Firmware: linux-firmware (updated versions)`

`├── Spike: works if present, but most target hardware doesn't have Wi-Fi 6`

`└── No special handling needed`
```

**Wi-Fi 6E (6 GHz band):**

```
`├── AX210 and newer`

`├── Driver: iwlwifi`

`├── Regulatory domain must support 6 GHz`

`├── Spike: works if present`

`└── Not expected in target hardware`
```

**Wi-Fi 7 (802.11be):**

```
`├── Very new (2024+)`

`├── Driver support: iwlwifi (Intel BE200)`

`├── Spike: should work if kernel and firmware are current`

`├── Not expected in target hardware`

`└── No special handling`
```

**Summary:** Spike supports all Wi-Fi standards that the upstream Linux kernel supports. No Spike-specific Wi-Fi limitations exist.

## Ethernet Support

Ethernet is less critical than Wi-Fi (most target laptops use Wi-Fi), but still fully supported:

**Intel Ethernet:**

```
`├── Driver: e1000e (common in Intel-based laptops)`

`├── Chips: I219-V, I219-LM, I218 (common in Celeron laptops)`

`├── Performance: excellent (gigabit where supported)`

`└── No known issues`
```

**Realtek Ethernet:**

```
`├── Driver: r8169 (in-tree, most Realtek chips)`

`├── Chips: RTL8111/8168/8411 (very common)`

`├── Performance: good (gigabit)`

`├── Known issues: occasional ASPM (power management) issues`

`└── Workaround: pcie\_aspm=off (if needed, added to boot params)`
```

**USB Ethernet:**

```
`├── Driver: varies by chipset (ax88179, rtl8152, etc.)`

`├── Common: USB gigabit adapters (ASIX, Realtek)`

`├── Performance: good (USB 3.0 gigabit adapters)`

`├── Detection: lsusb, module auto-load`

`└── No special handling`
```

**Ethernet detection:**

```
`├── lspci | grep Ethernet`

`├── /sys/class/net/en\* (or eth0 on older systems)`

`├── NetworkManager: managed automatically`

`├── No installer action needed (ethernet works on live USB)`

`└── If ethernet is primary: installer proceeds without Wi-Fi step`
```

## Bluetooth Support

**Bluetooth detection (at install time):**

```
`1. Detect Bluetooth adapter:`

`   ├── lsusb | grep -i bluetooth`

`   ├── rfkill list bluetooth`

`   ├── hciconfig -a`

`   └── Read /sys/class/bluetooth/`


`2. Verify driver:`

`   ├── btusb (most common — Intel, Realtek, Broadcom USB Bluetooth)`

`   ├── btintel (Intel-specific firmware loading)`

`   ├── btrtl (Realtek firmware loading)`

`   ├── btbcm (Broadcom firmware loading)`

`   └── ath3k (Atheros Bluetooth)`


`3. Firmware loading:`

`   ├── Intel: /lib/firmware/intel/ (ibt-\*.sfi, \*.ddc)`

`   ├── Realtek: /lib/firmware/rtl\_bt/ (rtlbt\_\*.bin)`

`   ├── Broadcom: /lib/firmware/brcm/ (\*.hcd)`

`   └── Atheros: /lib/firmware/ath3k-1.fw`


`4. Bluetooth service:`

`   ├── bluetooth.service: enabled if adapter detected`

`   ├── If no adapter: service disabled (saves ~5MB RAM)`

`   └── If adapter added later (USB dongle): udev triggers service start`


`5. Store in spike-config state:`

`   ├── bluetooth\_present: true/false`

`   ├── bluetooth\_vendor: intel/realtek/broadcom/atheros/none`

`   └── bluetooth\_service\_enabled: true/false`


`6. Panel applet:`

`   ├── If bluetooth\_present: Bluetooth applet loads in panel`

`   ├── If bluetooth\_present == false: applet does not load`

`   └── If USB dongle added later: applet loads dynamically`
```

### Bluetooth Profiles

**Supported Bluetooth profiles:**

```
`├── A2DP (Advanced Audio Distribution Profile):`

`│   ├── Stereo audio output to headphones/speakers`

`│   ├── Codec: SBC (always), AAC (Plus), LDAC (Plus, if hardware supports)`

`│   └── Spike Standard: SBC only (most compatible, lowest CPU)`

`├── HFP (Hands-Free Profile):`

`│   ├── Mono audio + microphone for calls`

`│   ├── Used when a headset is in call mode`

`│   └── Switches from A2DP to HFP when mic is needed`

`├── AVRCP (Audio/Video Remote Control Profile):`

`│   ├── Play/pause/skip controls on headphones`

`│   └── Media key forwarding to Spike`

`├── HID (Human Interface Device):`

`│   ├── Bluetooth mice, keyboards, game controllers`

`│   └── Standard input handling (no special Spike code)`

`├── PAN (Personal Area Network):`

`│   ├── Bluetooth tethering (internet via phone)`

`│   └── NetworkManager handles connection`

`└── OBEX (Object Exchange):`

`    ├── File transfer between devices`

`    ├── Bluetooth file receive in Spike`

`    └── Handled by obexd (part of BlueZ)`
```

## Audio Hardware Support

**Audio detection (at install time):**

```
`1. Detect audio adapter:`

`   ├── lspci | grep -i audio`

`   ├── aplay -l (list ALSA playback devices)`

`   ├── arecord -l (list ALSA capture devices)`

`   └── Read /sys/class/sound/`


`2. Common audio chips in target hardware:`

`   ├── Intel HD Audio (HDA):`

`   │   ├── Codec: Realtek ALC2xx/ALC2xx-VB (most common in Celeron laptops)`

`   │   ├── Codec: Conexant CX2xxxx (some Lenovo/HP models)`

`   │   ├── Driver: snd\_hda\_intel (in-tree)`

`   │   ├── Firmware: none needed (HDA is self-contained)`

`   │   └── Known issues: some laptops need model=alc255- HeadsetMic quirk`

`   ├── Intel SST (Smart Sound Technology):`

`   │   ├── Found on: some Bay Trail / Cherry Trail devices`

`   │   ├── Driver: snd\_sof (Sound Open Firmware) or legacy snd\_intel\_sst`

`   │   ├── More complex than HDA`

`   │   └── May require firmware from linux-firmware`

`   └── AMD HD Audio:`

`       ├── Codec: Realtek or Conexant (same as Intel HDA)`

`       ├── Driver: snd\_hda\_intel`

`       └── Usually works without issues`


`3. Audio configuration:`

`   ├── PipeWire: installed and configured (see MULTIMEDIA.md)`

`   ├── Sample rate: 44100Hz (locked, see MULTIMEDIA.md)`

`   ├── Channels: stereo (2.0) — no surround sound processing`

`   ├── Bluetooth audio: handled by PipeWire + WirePlumber`

`   └── HDMI audio: available but not default (user can switch in Settings)`


`4. Audio device naming:`

`   ├── Analog output: "Speakers — Built-In Audio" (user-friendly)`

`   ├── Headphones: "Headphones — Built-In Audio"`

`   ├── HDMI: "HDMI Audio — \[GPU name\]"`

`   └── Bluetooth: "\[Device Name\] — Bluetooth"`
```

## Modem Support

**Mobile broadband (cellular modem) detection:**

```
`1. Detect modem:`

`   ├── lsusb | grep -i -E "modem|3g|4g|lte"`

`   ├── mmcli -L (ModemManager device list)`

`   └── udev rules for USB modems`


`2. Modem types:`

`   ├── USB modems (most common for add-on cellular):`

`   │   ├── Huawei: huawei\_cdc\_ncm driver`

`   │   ├── Sierra Wireless: sierra\_net driver`

`   │   ├── ZTE: option driver + cdc\_ether`

`   │   └── Generic: cdc\_ether, qmi\_wwan, cdc\_ncm`

`   ├── Built-in modems (rare in target hardware):`

`   │   ├── Qualcomm: qmi\_wwan`

`   │   └── Fibocom: fibocom modules (may need firmware)`

`   └── M.2 modems (very rare in budget laptops)`


`3. ModemManager:`

`   ├── Installed: yes (needed for modem support)`

`   ├── Enabled: conditional (only if modem detected)`

`   ├── If no modem: ModemManager disabled (saves ~10MB RAM)`

`   └── If modem detected: enabled, panel applet for cellular`


`4. Panel applet:`

`   ├── If modem detected: cellular applet loads (shows signal, data)`

`   ├── If no modem: applet does not load`

`   └── If USB modem added later: applet loads dynamically`


`5. Cellular in installer:`

`   ├── If Wi-Fi available: installer uses Wi-Fi step (ignores modem)`

`   ├── If no Wi-Fi but modem present: installer offers cellular connection`

`   └── If no Wi-Fi and no modem: installer proceeds offline`


`6. Store in spike-config state:`

`   ├── modem\_present: true/false`

`   ├── modem\_vendor: huawei/sierra/zte/qualcomm/fibocom/none`

`   └── modem\_service\_enabled: true/false`
```

## Webcam Support

**Webcam detection (at install time):**

```
`1. Detect webcam:`

`   ├── lsusb | grep -i camera (USB webcams — most laptop cameras are USB internal)`

`   ├── ls /dev/video\*`

`   ├── Read /sys/class/video4linux/`

`   └── dmesg | grep -i uvcvideo`


`2. Driver:`

`   ├── uvcvideo (USB Video Class — covers ~95% of laptop webcams)`

`   ├── Some older webcams: gspca drivers (rare in target hardware)`

`   └── Intel IPS: integrated camera (actually USB internally, uses uvcvideo)`


`3. Common webcam chips:`

`   ├── Chicony Electronics (common in Lenovo, Acer)`

`   ├── Sunplus Innovation (common in HP)`

`   ├── Realtek (common in Asus)`

`   ├── Syntek (some Acer models)`

`   └── IMC Networks (some Asus models)`


`4. Webcam permissions:`

`   ├── Camera access gated by xdg-desktop-portal (see DESKTOP.md)`

`   ├── Applications must request camera permission`

`   ├── User sees permission dialog: "Firefox wants to use your camera"`

`   ├── Permission persisted per-application`

`   └── Camera indicator: panel applet shows when camera is in use`


`5. Webcam quality:`

`   ├── Most target laptops: 720p (1280×720)`

`   ├── Some newer budget laptops: 1080p`

`   ├── Frame rate: 15-30fps typical (USB 2.0 internal)`

`   └── Spike: no processing or enhancement (raw camera feed)`


`6. Privacy:`

`   ├── No camera access without user permission (portal)`

`   ├── Panel indicator when camera active`

`   ├── No facial recognition or camera-based features`

`   └── Camera can be disabled in Settings → Privacy → Camera`
```

## Peripherals

### USB Devices

**USB support:**

```
`├── USB 2.0: supported (keyboards, mice, flash drives, webcams)`

`├── USB 3.0/3.1/3.2: supported (external SSDs, fast flash drives)`

`├── USB-C: supported (data, display via DP alt mode if hardware supports)`

`├── USB hubs: supported (powered recommended for high-power devices)`

`└── USB device classes:`

`    ├── Mass storage (USB drives, SD readers): udisks2, auto-mount`

`    ├── Human interface (keyboard, mouse): evdev/libinput`

`    ├── Audio (USB headsets): snd\_usb\_audio`

`    ├── Video (webcams): uvcvideo`

`    ├── Printer (USB): cups (if installed)`

`    ├── Serial (Arduino, etc.): ttyUSB`

`    └── Network (USB Ethernet/Wi-Fi): respective drivers`
```

### External Displays

**External display support:**

```
`├── HDMI: supported (all target laptops have HDMI)`

`│   ├── Detection: /sys/class/drm/card0-HDMI-A-\*/status`

`│   ├── Audio: HDMI audio available (switchable in Settings → Sound)`

`│   └── Hot-plug: supported (Wayland KWin handles dynamically)`

`├── VGA: supported (some older Tier 2 laptops have VGA)`

`│   ├── Detection: /sys/class/drm/card0-VGA-1/status`

`│   ├── Audio: none (VGA is video only)`

`│   └── Hot-plug: supported`

`├── DisplayPort: supported (via USB-C or mini-DP if hardware has it)`

`│   ├── Detection: /sys/class/drm/card0-DP-\*/status`

`│   ├── Audio: DisplayPort audio available`

`│   └── Hot-plug: supported`

`├── Multi-monitor:`

`│   ├── Up to 2 external + 1 internal (3 total) — hardware permitting`

`│   ├── Extend mode: each monitor has its own resolution and layout`

`│   ├── Mirror mode: all monitors show same content`

`│   └── Single external: laptop screen can be turned off`

`└── Wayland multi-monitor: fully supported (KWin handles per-output config)`
```

### Printers

**Printer support:**

```
`├── CUPS: not installed by default (planned for future)`

`├── If printer support is needed:`

`│   ├── cups package installed via Discover`

`│   ├── Settings → Printers becomes available`

`│   └── USB and network printers supported`

`├── Printers detected via:`

`│   ├── USB: udev triggers CUPS backend discovery`

`│   └── Network: avahi/Bonjour discovery (if cups installed)`

`├── Driverless printing:`

`│   ├── IPP Everywhere: supported (most modern printers)`

`│   ├── AirPrint: supported (Apple protocol, works with CUPS)`

`│   └── No manufacturer drivers needed for IPP-compatible printers`

`├── Manufacturer-specific drivers:`

`│   ├── HP: hplip package (available in Discover)`

`│   ├── Epson: epson-inkjet-printer (may need manual install)`

`│   ├── Brother: brother-\* packages (may need manual install)`

`│   └── Canon: cnijfilter packages (may need manual install)`

`└── Status: printer support is functional but not a priority (see ROADMAP.md)`
```

### Input Devices

**Keyboard:**

```
`├── Internal laptop keyboard: evdev/libinput (auto-detected)`

`├── USB keyboard: evdev/libinput (auto-detected)`

`├── Bluetooth keyboard: BlueZ input profile (auto-paired)`

`├── Layout:`

`│   ├── Detected at install time (from installer locale selection)`

`│   ├── Default: US (QWERTY) if not specified`

`│   ├── Layout switchable in Settings → Keyboard`

`│   └── Multiple layouts supported (panel applet if configured)`

`├── Fn keys:`

`│   ├── Brightness, volume, media: handled by spike-shell keybindings`

`│   ├── Airplane mode: handled by rfkill`

`│   └── Vendor-specific: may or may not work (hardware/firmware dependent)`

`└── Input repeat: configurable in Settings → Keyboard (or accessibility settings)`
```

**Touchpad:**

```
`├── Internal laptop touchpad: libinput (synaptics driver deprecated)`

`├── Properties:`

`│   ├── Tap-to-click: configurable in Settings → Mouse/Touchpad`

`│   ├── Natural scrolling: configurable (default: on)`

`│   ├── Two-finger scroll: supported (default: on)`

`│   ├── Edge scroll: supported (default: off)`

`│   ├── Click method: button areas or clickfinger (hardware dependent)`

`│   └── Palm detection: enabled (libinput default)`

`├── External USB/Bluetooth mouse: libinput (separate config from touchpad)`

`└── Known issues:`

`    ├── Some older touchpads: recognized as generic mouse (no multitouch)`

`    └── Some Elan touchpads: may need kernel parameter i2c\_i801`
```

**Pointing stick (TrackPoint):**

```
`├── IBM/Lenovo TrackPoint: supported (libinput)`

`├── Sensitivity configurable in Settings → Mouse/Touchpad`

`├── Middle-click scroll: configurable`

`└── Rare in target hardware (mostly ThinkPads, which are Tier 2 or 3)`
```

## Hardware Detection Summary

### Detection Order (At Install Time)

The installer's hardware detection runs in this order:

```
`1. CPU detection`

`   ├── Architecture (x86-64 required)`

`   ├── Core count (≥ 2 required)`

`   ├── Clock speed`

`   ├── Bogomips (performance estimation)`

`   ├── Vendor/model`

`   └── ZRAM capability test`


`2. RAM detection`

`   ├── Total RAM (≥ 4GB required)`

`   ├── Speed/type`

`   └── Soldered vs. upgradable`


`3. Storage detection`

`   ├── All block devices enumerated`

`   ├── Type (SSD, HDD, NVMe, eMMC, USB)`

`   ├── Capacity (≥ 128GB required)`

`   ├── Speed test (quick read benchmark)`

`   └── Eligible devices presented to user`


`4. GPU detection`

`   ├── Vendor (Intel, AMD, NVIDIA, hybrid)`

`   ├── Model`

`   ├── Driver selection (i915, amdgpu, nouveau, nvidia)`

`   ├── VA-API capability test`

`   └── Multi-GPU handling (hybrid mode)`


`5. Network detection`

`   ├── Wi-Fi adapter (vendor, chipset, driver, firmware)`

`   ├── Ethernet adapter`

`   ├── Modem (if present)`

`   └── Connectivity test (online/offline)`


`6. Audio detection`

`   ├── Audio controller (HDA, SST)`

`   ├── Codec (Realtek, Conexant, etc.)`

`   ├── Speaker, headphone, mic, HDMI audio endpoints`

`   └── Audio test (play a short tone — verified at first boot)`


`7. Bluetooth detection`

`   ├── Adapter present?`

`   ├── Vendor/chipset`

`   ├── Firmware loading`

`   └── Service enable/disable decision`


`8. Webcam detection`

`   ├── Camera present?`

`   ├── Driver (uvcvideo)`

`   └── Device node (/dev/video\*)`


`9. Input device detection`

`   ├── Keyboard layout (from installer locale)`

`   ├── Touchpad (libinput)`

`   ├── External mouse (if connected)`

`   └── Pointing stick (if present)`


`10. Power/battery detection`

`    ├── Battery present?`

`    ├── Battery capacity and health`

`    ├── AC adapter present?`

`    ├── Lid switch present?`

`    └── Power profile capabilities`


`11. Sensor detection`

`    ├── Temperature sensors (CPU, GPU — for diagnostics)`

`    ├── Accelerometer (if present — rare in laptops)`

`    ├── Light sensor (if present — for auto-brightness, future)`

`    └── Proximity sensor (if present — very rare)`


`12. Compile results`

`    ├── All detections stored in spike-config state store`

`    ├── Module blacklist generated (conflicting drivers)`

`    ├── Boot parameters finalized (GPU, storage type)`

`    ├── sysctl tunables generated (swappiness, cache pressure)`

`    ├── Variant recommendation computed (CPU + RAM + GPU + storage)`

`    └── Configuration written to state.json`
```

### Detection Results Storage

All hardware detection results are stored in:

`/var/lib/spike/config/state.json` → hardware section:

```
`\{`

`  "hardware": \{`

`    "cpu": \{`

`      "vendor": "Intel",`

`      "model": "Intel Celeron N4020 CPU @ 1.10GHz",`

`      "cores": 2,`

`      "threads": 2,`

`      "base\_clock\_mhz": 1100,`

`      "burst\_clock\_mhz": 2800,`

`      "bogomips\_total": 4399,`

`      "architecture": "x86-64",`

`      "microarchitecture": "Goldmont Plus",`

`      "tier": 1,`

`      "performance\_class": "tier1",`

`      "zram\_capable": true,`

`      "zram\_benchmark\_ratio": 3.8,`

`      "zram\_benchmark\_ms": 45`

`    \},`

`    "ram": \{`

`      "total\_mb": 3789,`

`      "type": "DDR4",`

`      "speed\_mhz": 2400,`

`      "soldered": true,`

`      "upgradable": false,`

`      "slots\_used": 1,`

`      "slots\_total": 0`

`    \},`

`    "storage": \[`

`      \{`

`        "device": "/dev/sda",`

`        "type": "ssd",`

`        "interface": "sata",`

`        "size\_gb": 240,`

`        "model": "Kingston SA400S37240G",`

`        "serial": "50026B76830ABCDEF",`

`        "rotational": false,`

`        "is\_install\_target": true,`

`        "read\_speed\_mbps": 520,`

`        "smart\_healthy": true`

`      \}`

`    \],`

`    "gpu": \{`

`      "vendor": "Intel",`

`      "model": "Intel UHD Graphics 600",`

`      "driver": "i915",`

`      "vaapi\_driver": "iHD",`

`      "vaapi\_working": true,`

`      "av1\_decode": false,`

`      "mode": "integrated",`

`      "vulkan": false,`

`      "opengl\_version": "4.6"`

`    \},`

`    "wifi": \{`

`      "present": true,`

`      "vendor": "Intel",`

`      "model": "Intel Wireless-AC 9560",`

`      "driver": "iwlwifi",`

`      "firmware\_loaded": true,`

`      "requires\_mok": false,`

`      "secure\_boot\_compatible": true`

`    \},`

`    "ethernet": \{`

`      "present": false`

`    \},`

`    "bluetooth": \{`

`      "present": true,`

`      "vendor": "Intel",`

`      "model": "Intel Wireless-AC 9560 Bluetooth",`

`      "driver": "btusb",`

`      "firmware\_loaded": true,`

`      "service\_enabled": true`

`    \},`

`    "audio": \{`

`      "controller": "Intel HDA",`

`      "codec": "Realtek ALC256",`

`      "driver": "snd\_hda\_intel",`

`      "speakers\_detected": true,`

`      "headphones\_detected": true,`

`      "microphone\_detected": true,`

`      "hdmi\_audio": true`

`    \},`

`    "webcam": \{`

`      "present": true,`

`      "driver": "uvcvideo",`

`      "device": "/dev/video0",`

`      "resolution\_max": "1280x720",`

`      "framerate\_max": 30`

`    \},`

`    "power": \{`

`      "battery\_present": true,`

`      "battery\_capacity\_wh": 37.0,`

`      "battery\_health\_percent": 82,`

`      "ac\_adapter\_present": true,`

`      "lid\_switch\_present": true`

`    \},`

`    "modem": \{`

`      "present": false`

`    \},`

`    "tier": 1,`

`    "variant\_recommended": "standard"`

`  \}`

`\}`
```

### Hardware Re-Evaluation

Hardware can change after installation. Spike handles this:

**Triggers for hardware re-evaluation:**

```
`├── USB device hot-plug (udev event):`

`│   ├── New Wi-Fi adapter (USB dongle) → enable network applet`

`│   ├── New Bluetooth dongle → enable Bluetooth service and applet`

`│   ├── New audio device (USB headset) → add to PipeWire outputs`

`│   ├── New webcam → add to /dev/video\*`

`│   └── New modem → enable ModemManager and cellular applet`

`├── Boot (cold):`

`│   ├── Re-scan all hardware (in case components were swapped)`

`│   ├── Compare to state.json`

`│   ├── If hardware changed: update state.json`

`│   ├── If new GPU: trigger driver re-evaluation (rare)`

`│   ├── If RAM changed: recompute ZRAM/swap/earlyoom`

`│   └── If storage changed: update storage diagnostics`

`├── User action (Settings → Diagnostics → "Re-scan hardware"):`

`│   ├── Manual trigger for full hardware re-detection`

`│   ├── Updates state.json`

`│   ├── Applies any configuration changes`

`│   └── Useful after hardware upgrade (e.g., RAM added, SSD replaced)`

`└── spike-config --rebuild:`

`    ├── Developer command`

`    ├── Re-runs all detection modules`

`    ├── Regenerates all configuration files`

`    └── Useful for testing configuration changes`
```

**What happens when hardware changes:**

```
`├── RAM upgrade (e.g., 4GB → 8GB):`

`│   ├── state.json updated with new RAM`

`│   ├── ZRAM size recalculated (may increase)`

`│   ├── Swap file size recalculated (for hibernate)`

`│   ├── Earlyoom threshold recalculated`

`│   ├── If now 8GB+: Settings suggests switching to Spike Plus`

`│   └── Reboot required for ZRAM/swap changes`

`├── SSD upgrade (e.g., HDD → SSD):`

`│   ├── state.json updated with new storage type`

`│   ├── Swappiness changed (5 → 10)`

`│   ├── Performance baselines re-evaluated`

`│   ├── Upgrade recommendation removed`

`│   └── Notification: "SSD detected. Performance should be improved."`

`├── Wi-Fi adapter added (USB dongle):`

`│   ├── Driver loaded (udev)`

`│   ├── NetworkManager detects adapter`

`│   ├── Network applet shows adapter`

`│   └── No reboot required`

`└── GPU change (extremely rare on laptops):`

`    ├── If new GPU detected: driver re-evaluation`

`    ├── Module blacklist regenerated`

`    ├── Boot parameters may change`

`    └── Reboot required`
```

## Hardware Compatibility Registry

Spike maintains a hardware registry (opt-in, anonymous):

**Purpose:**

```
`├── Track which hardware configurations work with Spike`

`├── Identify hardware-specific bugs (patterns across same model)`

`├── Provide compatibility lookup for prospective users`

`├── Guide development priorities (fix what affects the most users)`

`└── NOT for telemetry — strictly opt-in, no personal data`
```

**What is submitted (if user opts in):**

```
`├── CPU model`

`├── RAM total`

`├── Storage type and size`

`├── GPU model`

`├── Wi-Fi adapter model`

`├── Bluetooth adapter model`

`├── Audio codec`

`├── Webcam model`

`├── Laptop model (if detectable via dmidecode)`

`├── Spike version and variant`

`├── Installation result (success/failure)`

`├── Known issues detected (VA-API not working, Wi-Fi issues, etc.)`

`└── NO personal data, NO browsing history, NO file contents`
```

**Registry flow:**

```
`├── At install completion:`

`│   ├── "Would you like to submit your hardware information to the`

`│   │   Spike Hardware Registry? This helps other users know if`

`│   │   their laptop is compatible. No personal data is collected."`

`│   ├── \[Yes, submit\]  \[No, thanks\]  \[Ask me later\]`

`│   └── If yes: hardware profile submitted anonymously`

`├── Post-install (ongoing):`

`│   ├── If user opted in: periodic update on hardware changes`

`│   ├── Triggered by: RAM upgrade, storage change, GPU change`

`│   ├── Frequency: only on hardware change (not periodic polling)`

`│   └── User can opt out anytime (Settings → Privacy → Hardware Registry)`

`├── Registry data is:`

`│   ├── Published on website (aggregate, anonymized)`

`│   ├── Searchable by laptop model`

`│   ├── Shows compatibility status (works / works with issues / doesn't work)`

`│   └── Updated as new submissions arrive`

`└── Privacy guarantee:`

`    ├── Submission is opt-in (default: not submitted)`

`    ├── No personally identifiable information`

`    ├── No IP addresses retained`

`    ├── No browsing or usage data`

`    ├── Data is aggregate only (individual submissions not published)`

`    └── User can withdraw and delete their submission`
```

## What This Document Does Not Cover

- **Memory management architecture** (ZRAM, swap, earlyoom): See **MEMORY.md** 

- **Kernel module blacklisting and sysctl tunables**: See **KERNEL.md** 

- **Boot process and hardware initialization sequence**: See **BOOT-PROCESS.md** 

- **Installer flow and step-by-step process**: See **INSTALLER.md** 

- **Performance baselines and targets per hardware tier**: See **PERFORMANCE-BASELINES.md** 

- **GPU driver selection rationale**: See **DESIGN-DECISIONS.md** 

- **Multimedia** (VA-API, codecs, audio pipeline): See **MULTIMEDIA.md** 

- **Networking** (Wi-Fi configuration, VPN, DNS): See **NETWORKING.md** 

- **Bluetooth audio codecs and pairing**: See **MULTIMEDIA.md** and **NETWORKING.md** 

- **Power management** (battery, suspend, hibernate): See **POWER-MANAGEMENT.md** 

- **Accessibility** (screen reader, on-screen keyboard, motor aids): See **ACCESSIBILITY.md** 

- **Disaster recovery** (hardware failure, data rescue): See **DISASTER-RECOVERY.md** 

- **Configuration system** (state store, spike-config): See **CONFIGURATION.md** 

- **Variant differences** (hardware-driven variant selection): See **VARIANT-DIFFERENCES.md** 

- **Privacy policy** (hardware registry data collection): See **PRIVACY.md** 

- **Security** (firewall, AppArmor, secure boot): See **SECURITY.md** 

🐕 BigRangaTech


