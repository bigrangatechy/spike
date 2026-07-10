Spike Performance Baselines
Purpose

This document defines the performance targets for Spike Linux across all supported hardware tiers. It establishes what "usable" means in measurable terms — boot time, memory usage, application launch speed, and system responsiveness.

Current status: All values are estimates based on architectural projections. Real measurements will replace estimates during alpha testing on physical hardware. Until then, these targets guide design decisions and resource budgets.
Design Philosophy

Performance baselines serve one purpose: ensuring the user experience is dignified on old hardware. A Celeron N4020 with 4GB RAM should feel responsive, not tolerated. These baselines define the minimum acceptable experience.

Three principles govern these targets:

    Targets are minimums, not aspirations — Meeting a target means "acceptable," not "great." Design decisions should aim below targets to leave headroom.

    Measurements reflect user perception — Boot time means "time from power button to usable desktop," not "time from GRUB to systemd target." User-perceived latency is what matters.

    Tier 1 is non-negotiable — If Spike doesn't meet Tier 1 baselines on Celeron N4020 hardware, the mission has failed. No amount of Plus-tier polish compensates for a broken primary target.

Hardware Tiers

These tiers are defined in AGENTS.md and used consistently across all documentation.
Tier 1 — Primary Target (Must Be Daily-Driver Usable)

CPU:       Intel Celeron N4020 (Gemini Lake Refresh, 2019)
Cores:     2 (2 threads)
Base:      1.10 GHz
Turbo:     2.80 GHz
Cache:     4MB
RAM:       4GB DDR4-2400 (soldered)
GPU:       Intel UHD Graphics 600 (12 EU, 300-650 MHz)
Storage:   240GB SATA SSD
Display:   1440×900 or 1366×768 (typical 14" laptop)

This is the reference hardware. Every performance decision is evaluated against this machine. If it works here, it works everywhere Spike targets.
Tier 2 — Worst Case (Must Boot and Function)

CPU:       AMD A4 (Jaguar architecture, pre-2016)
RAM:       4GB
GPU:       AMD Radeon HD (integrated)
Storage:   500GB SATA HDD
Machine:   Lenovo ThinkPad with AMD A4

This is the floor. Spike must boot, install, and remain functional. Performance may be degraded — that is acceptable. Unusable is not.
Tier 3 — Development Machine (Regression Check Only)

CPU:       Intel Xeon (ThinkPad P50)
RAM:       32GB
GPU:       NVIDIA Quadro M2000M
Storage:   512GB NVMe

This machine is for development and regression testing only. It is NOT representative of target users. Performance on Tier 3 should be excellent — if it's not, something is wrong.
Measurement Methodology
How Measurements Will Be Taken (During Alpha)

All measurements must be taken on physical hardware, not virtual machines. VMs do not accurately represent disk I/O, GPU performance, or firmware behavior on target hardware.

Boot time:

Measurement: Time from power button press to desktop ready
├── Start: Power button pressed (firmware POST begins)
├── End: Spike Shell panel visible, input responsive
├── Method: Stopwatch (manual) or timestamped boot log analysis
├── Runs: 5 consecutive boots, report median + range
└── Conditions: Cold boot (not reboot), no USB devices, no network

Idle memory:

Measurement: RAM usage after boot, no user applications running
├── Start measuring: 60 seconds after desktop appears
├── Tool: free -h (report "used" value, not "available")
├── Exclude: Page cache (report MemAvailable context separately)
├── Runs: 3 measurements, report median
└── Conditions: No applications launched, no network connection
    (network adds NetworkManager overhead, measured separately)

Application launch time:

Measurement: Time from click to window visible and responsive
├── Start: Mouse click on launcher entry
├── End: Application window fully rendered, accepts input
├── Method: Manual stopwatch (human reaction ~200ms variance) or
│   scripted with xdotool/wlr-randr timestamp logging
├── Runs: 5 launches per app, report median + range
└── Conditions: App not already cached (cold launch), no other
    apps running beyond desktop shell

Shutdown time:

Measurement: Time from "Shut Down" click to power off
├── Start: User clicks Shut Down in session menu
├── End: Machine powers off (fan stops, lights off)
├── Method: Manual stopwatch
├── Runs: 3 shutdowns, report median
└── Conditions: No applications running, clean shutdown

Hibernate resume:

Measurement: Time from power button press (during hibernate) to desktop ready
├── Start: Power button pressed (resume from hibernate)
├── End: Spike Shell panel visible, input responsive
├── Method: Manual stopwatch
├── Runs: 3 resumes, report median
└── Conditions: Hibernated with no apps open (baseline), then
    with Firefox + LibreOffice open (loaded resume)

Measurement Environment

Standard measurement conditions:
├── No USB devices connected (except measurement tools if needed)
├── No network connection (Wi-Fi off, no Ethernet)
├── No background tasks running (no updates, no indexing)
├── Display at native resolution
├── Firmware at default settings (no Fast Boot, no overclocking)
└── Room temperature (~22°C — thermal throttling affects performance)

For each measurement, record:
├── Hardware: exact model, CPU, RAM, storage
├── Spike version: x.x.x-alpha/beta/production
├── Variant: Standard or Plus
├── Date: when measured
├── Median value
├── Range (min-max)
└── Notes: anything anomalous (thermal throttle, background activity)

Boot Performance
Cold Boot Time

Target: < 40 seconds (Tier 1, SSD)

This is the headline metric. The user presses power and waits. 40 seconds is the maximum acceptable time from power button to usable desktop on the primary target hardware.

                        Tier 1 (SSD)    Tier 1 (HDD)    Tier 2 (HDD)
                        ────────────    ────────────    ────────────
Target                  < 40s           < 60s           < 90s
Estimate (current)     ~15-25s         ~30-50s         ~45-65s
Stretch goal            < 25s           < 45s           < 70s

Stage breakdown (estimated, Tier 1 SSD, Spike Standard):

Stage                          Estimated Time    Notes
──────────────────────────────────────────────────────────
1. Firmware (POST)             2-5s             Hardware-dependent, uncontrollable
2. GRUB2                        0.5-1s           Hidden, no timeout delay
3. Kernel load                  3-5s             Decompression + module init
4. Initramfs                    1-2s             Mount root, increment counter
5. systemd startup              5-8s             Services start in parallel
6. Plymouth splash              (overlaps 3-5)   Visual only, doesn't add time
7. SDDM → login                 1-3s             Auto-login: ~0s, manual: user-dependent
8. spike-session startup        3-5s             KWin + Shell + applets + counter clear
──────────────────────────────────────────────────────────
Total (auto-login)              ~15-25s
Total (manual login)            + user time     Add user's login duration

Boot time by storage type (Tier 1, Spike Standard, estimated):

Storage Type          Estimated Boot    Target Met?
─────────────────────────────────────────────────────
SATA SSD              ~15-25s           ✓ (< 40s target)
NVMe SSD              ~12-20s           ✓ (< 40s target)
SATA HDD              ~30-50s           ✓ (< 60s HDD target)
SD card (UHS-I+)     ~40-60s           ✓ (< 70s SD target)
USB 3.0+              ~35-55s           ✓ (< 60s USB target)

Variant comparison (estimated, Tier 1 SSD):

Metric                    Spike Standard    Spike Plus
────────────────────────────────────────────────────────
Boot to SDDM              ~10-18s           ~10-18s       (identical — same kernel, same GRUB)
Plymouth splash           spike-minimal     spike-full    (slight GPU init overhead for Plus)
Session startup           ~7-11s            ~9-14s       (Plus: effects + GL init)
Total (auto-login)        ~17-29s           ~19-32s
Target                    < 40s             < 30s

Warm Boot (Reboot)

                        Tier 1 (SSD)    Tier 2 (HDD)
Target                  < 45s           < 70s
Estimate                ~18-30s         ~35-55s

Warm boot (reboot) is slightly slower than cold boot in some cases because firmware may perform additional checks on reboot. Generally within ±5 seconds of cold boot.
Session Restart (Logout/Login, No Reboot)

                        Tier 1 (SSD)    Tier 2 (HDD)
Target                  < 10s           < 20s
Estimate                ~5-8s           ~10-15s

This measures logout → SDDM → login → desktop ready, without a full reboot. Faster than cold boot because kernel and firmware are already initialized.
Memory Performance
Idle Memory (Spike Standard)

Target: < 400MB (Tier 1, 4GB RAM)

This is the most critical memory metric. 4GB total RAM minus 400MB idle leaves ~3.6GB for user applications. Every megabyte saved in idle is a megabyte available for Firefox tabs, LibreOffice documents, and user workflow.

                        Tier 1 (4GB)    Tier 2 (4GB)
Target                  < 400MB         < 450MB
Estimate (current)      ~280-370MB      ~320-400MB
Stretch goal            < 350MB         < 400MB

Tier 2 has a slightly higher target because AMD A4 hardware may have slightly higher driver overhead.

Memory breakdown (Spike Standard, Tier 1, estimated):

Component                          Estimated Memory
──────────────────────────────────────────────────────
Kernel + initramfs residual        ~80-120MB
systemd + core services            ~30-40MB
KWin (Wayland compositor)          ~100-150MB
Spike Shell (panel + applets)      ~40-55MB
Notification daemon                 ~5-8MB
PipeWire + WirePlumber             ~8-12MB
NetworkManager                      ~5-8MB
earlyoom                            ~2-3MB
udev + DBus                         ~5-8MB
──────────────────────────────────────────────────────
Total idle                          ~280-370MB
Headroom for user apps              ~3,630-3,720MB (of 4,096MB)

Notes on memory measurement:

    Page cache (disk cache) is NOT counted as "used" memory. The kernel reclaims page cache on demand. Measurements report actual process memory.
    ZRAM is NOT counted as "used" memory. ZRAM is compressed swap, not process memory. ZRAM usage is tracked separately.
    "Available" memory (as reported by free -h) is the metric users see in Settings, but baseline tracking uses "used" for consistency.

Idle Memory (Spike Plus)

Target: < 800MB (8GB+ RAM)

                        Tier 1 (8GB)    Tier 3 (32GB)
Target                  < 800MB         < 800MB
Estimate (current)      ~420-650MB      ~450-680MB
Stretch goal            < 700MB         < 700MB

Memory breakdown (Spike Plus, estimated):

Component                          Estimated Memory
──────────────────────────────────────────────────────
Kernel + initramfs residual        ~80-120MB
systemd + core services            ~40-50MB         (more services enabled)
KWin (Wayland compositor)          ~120-180MB        (effects enabled)
Spike Shell (panel + applets)      ~45-60MB
Notification daemon                 ~5-8MB
PipeWire + WirePlumber             ~10-15MB          (more codecs)
NetworkManager                      ~5-8MB
earlyoom                            ~2-3MB
udev + DBus                         ~5-8MB
Additional caching                  ~100-200MB        (more aggressive)
──────────────────────────────────────────────────────
Total idle                          ~420-650MB
Headroom for user apps              ~7,350-7,580MB (of 8,192MB)

Memory Under Load

These targets define acceptable memory behavior with typical user workloads:

Scenario                                   Spike Standard (4GB)    Spike Plus (8GB)
──────────────────────────────────────────────────────────────────────────────────
Desktop + Firefox (5 tabs)                 ~500-700MB              ~600-900MB
Desktop + Firefox (10 tabs)                ~700-900MB              ~800-1,100MB
Desktop + LibreOffice Writer              ~400-550MB              ~500-650MB
Desktop + Firefox + LibreOffice           ~600-850MB              ~750-1,000MB
Desktop + VLC playing 1080p               ~450-600MB              ~500-700MB
Desktop + Firefox + LO + VLC              ~800-1,100MB            ~1,000-1,300MB

Earlyoom threshold:                       10% of RAM+swap         10% of RAM+swap
                                           (~1.6GB free)           (~3.2GB free)

When memory approaches the earlyoom threshold, the largest non-protected process is killed. This is by design — it prevents system freeze.
ZRAM Performance

Metric                                    Target                  Estimate
──────────────────────────────────────────────────────────────────────────
ZRAM compression ratio (typical)          > 2.5:1                 ~3:1 to 4:1
ZRAM compression ratio (worst case)       > 1.5:1                 ~2:1
ZRAM CPU overhead (idle)                  < 2%                    ~0-1%
ZRAM CPU overhead (under swap pressure)   < 15%                   ~5-10%
ZRAM activation time (at boot)            < 2s                    ~1s
Effective memory (4GB + 4GB ZRAM)         ~10-14GB                ~12-14GB

Compression ratio depends on data type:

    Text/HTML: ~4:1 (highly compressible)
    Compiled binaries: ~2.5:1 (moderate)
    Already-compressed data (JPEG, MP4): ~1:1 (incompressible)
    Mixed workload: ~3:1 average

Swap Usage

Metric                                    Target
──────────────────────────────────────────────────────────────────
Swap file usage at idle                   0MB
Swap file usage (normal workload)         < 1GB
Swap file usage (heavy workload)          < 4GB (Standard) / < 6GB (Plus)
ZRAM fills before swap file              Always (priority 100 > priority 10)

Application Launch Performance
Application Launch Times

Target: < 2 seconds (average application, Tier 1, SSD)

This is the second most critical metric after boot time. If clicking an app takes 5 seconds to open, the system feels slow regardless of how fast it boots.

Application                  Tier 1 (SSD)    Tier 1 (HDD)    Tier 2 (HDD)
──────────────────────────────────────────────────────────────────────────
Discover (software center)   < 2s            < 4s            < 5s
Firefox (Flatpak)             < 3s            < 5s            < 7s
Dolphin (file manager)        < 1.5s          < 3s            < 4s
Konsole (terminal)            < 1s            < 2s            < 3s
Kate (text editor)            < 1.5s          < 3s            < 4s
Settings                      < 1.5s          < 2.5s          < 3.5s
Spectacle (screenshot)        < 1s            < 2s            < 3s
KCalc (calculator)            < 1s            < 2s            < 3s
LibreOffice Writer (Flatpak) < 4s            < 7s            < 10s
VLC (Flatpak)                 < 2s            < 4s            < 5s
──────────────────────────────────────────────────────────────────────────
Average                      < 2s            < 3.5s          < 5s

Notes:

    Firefox has a higher target because it's a heavyweight application with a large Flatpak runtime. 3 seconds is acceptable; <2s is the stretch goal.
    LibreOffice Writer is similarly heavy. 4 seconds on SSD is acceptable for a full office suite.
    Applications launched from the launcher and from the panel favorites should have identical launch times.
    Second launch of the same application (warm cache) should be ~50% faster than cold launch.

Application Launch (Flatpak vs System)

Package type                 Tier 1 (SSD)    Notes
──────────────────────────────────────────────────────────────────
Flatpak (pre-seeded runtime) < 3s            Runtime already on disk
Flatpak (runtime download)   N/A             First install only, not measured
apt package                  < 2s            No container overhead
System app (KDE)             < 1.5s          Already in memory (shared libs)

Pre-seeded Flatpak runtimes exist specifically to make first-launch of Flatpak apps competitive with system packages. Without pre-seeding, first Flatpak launch requires runtime extraction (5-15 seconds on SSD).
Window Operation Responsiveness

Operation                    Spike Standard              Spike Plus
                             (no animations)             (with animations)
──────────────────────────────────────────────────────────────────────────
Window open                  < 100ms (instant)           < 250ms (150ms animation)
Window close                 < 100ms (instant)           < 250ms (150ms animation)
Workspace switch             < 100ms (instant)           < 300ms (200ms animation)
Panel show/hide               < 50ms (instant)            < 250ms (200ms animation)
Launcher open                < 100ms (instant)            < 250ms (150ms animation)
Notification popup            < 100ms (instant)            < 250ms (200ms animation)
Drag window                   < 16ms frame time            < 16ms frame time
Resize window                 < 16ms frame time            < 16ms frame time
──────────────────────────────────────────────────────────────────────────
Frame budget                  60fps (16.67ms)             60fps (16.67ms)

On Spike Standard, window operations are instantaneous (no animation overhead). On Spike Plus, animations add visual polish but must complete within their animation duration. The frame budget target is 60fps for drag/resize on both variants — if the compositor can't maintain 60fps during window manipulation, the user perceives jankiness.

Frame time degradation is acceptable on Tier 2 (AMD A4, HDD):

    Drag/resize may drop to 30fps (33ms frame time) under load
    Animations (Plus only) may stutter — acceptable on Tier 2
    Window open/close still completes within 500ms

Shutdown Performance
Shutdown Time

Target: < 5 seconds (Tier 1, SSD)

                        Tier 1 (SSD)    Tier 1 (HDD)    Tier 2 (HDD)
Target                  < 5s             < 10s           < 15s
Estimate (current)      ~3-5s            ~5-10s          ~8-15s

Shutdown stage breakdown (estimated, Tier 1 SSD):

Stage                                    Estimated Time
──────────────────────────────────────────────────────────
1. Send SIGTERM to user applications     ~1-2s
2. Flush Spike Shell state to disk       ~0.5s
3. Stop Spike Shell + KWin               ~0.5s
4. Stop user-level services              ~0.5s
5. spike-session exits                   ~0.1s
6. systemd system teardown               ~1-2s
7. Unmount filesystems                   ~0.5s
8. ACPI power off                        ~0.1s
──────────────────────────────────────────────────────────
Total                                    ~3-5s

The longest shutdown stage is application termination. If an application hangs during SIGTERM, the session manager waits 5 seconds before SIGKILL. A hung application can extend shutdown to ~10 seconds worst case.
Suspend Time

Target: < 3 seconds (Tier 1)

                        Tier 1          Tier 2
Suspend (enter)          < 3s            < 5s
Resume from suspend      < 2s            < 5s

Suspend should be fast — the user closes the lid and walks away. If suspend takes 10 seconds, the user may close the laptop bag before suspend completes, risking data loss.
Hibernate Resume Time

Target: < 10 seconds (Tier 1, SSD)

                        Tier 1 (SSD)    Tier 1 (HDD)    Tier 2 (HDD)
Hibernate (enter)        < 10s           < 20s           < 30s
Resume from hibernate    < 10s           < 25s           < 40s

Hibernate is slower than suspend because it writes RAM contents to disk. Resume reads the data back. On HDD, hibernate resume is significantly slower due to disk I/O.

Hibernate resume by RAM size (SSD, estimated):
├── 4GB RAM: ~5-8s
├── 8GB RAM: ~8-12s
├── 16GB RAM: ~15-25s
└── 32GB RAM: ~30-50s (Tier 3 only, not a target)

Disk I/O Performance
Disk I/O Targets

Disk I/O is the primary bottleneck on Tier 2 (HDD) hardware. These targets ensure the system remains usable on spinning rust.

Operation                         Tier 1 (SSD)    Tier 1 (HDD)    Tier 2 (HDD)
──────────────────────────────────────────────────────────────────────────
Read 100MB file                   < 1s            < 3s            < 5s
Write 100MB file                 < 1s            < 5s            < 8s
List directory (1000 files)       < 0.5s          < 2s            < 3s
Copy 500MB file                  < 5s            < 20s           < 30s
Delete 1000 files                < 1s            < 5s            < 8s
fsck on clean filesystem          < 5s            < 30s           < 60s

These are not user-facing metrics — they inform design decisions about caching, batching, and I/O patterns.
I/O Behavior Under Memory Pressure

When the system is under memory pressure (ZRAM filling, swap active), disk I/O becomes critical:

Scenario                                    Tier 1 (SSD)    Tier 1 (HDD)    Tier 2 (HDD)
──────────────────────────────────────────────────────────────────────────────────────
System responsive under swap pressure       ✓               Degraded         Degraded
App launch under swap pressure              < 4s            < 8s             < 12s
Window switch under swap pressure           < 1s            < 3s             < 5s
Desktop remains usable under pressure       ✓               Marginal         Marginal

On HDD, swap pressure causes visible latency. This is why swappiness is set to 5 on HDD systems — minimize swap usage, keep everything in RAM as long as possible.
Network Performance
Network Connection Targets

Operation                         Tier 1          Tier 2
──────────────────────────────────────────────────────────
Wi-Fi scan (available networks)   < 3s            < 5s
Connect to known Wi-Fi            < 5s            < 8s
Connect to new Wi-Fi              < 8s            < 12s
DHCP lease acquisition             < 3s            < 5s
DNS resolution (first query)       < 1s            < 2s
DNS resolution (cached)            < 10ms          < 10ms
Network icon updates (state change) < 1s          < 2s

Network performance is largely hardware-dependent (Wi-Fi adapter quality, router proximity, signal strength). Spike's role is to connect promptly when the hardware cooperates.
Network Applet Responsiveness

Operation                         Spike Standard          Spike Plus
──────────────────────────────────────────────────────────────────────
Signal strength update             Every 10s               Every 5s
Connection state change            < 1s (event-driven)     < 1s (event-driven)
Icon update on connect             < 1s                    < 1s
Applet open (click)                < 200ms                 < 200ms

Event-driven updates (connection state changes) are identical between variants. Only the periodic signal strength polling interval differs. See VARIANT-DIFFERENCES.md.
Notification Performance
Notification Latency

The notification daemon's disk-before-display invariant requires a disk write before showing a notification. This adds latency, but it must be imperceptible.

Metric                                    Target              Estimate
──────────────────────────────────────────────────────────────────────────
Notification receive to display           < 50ms              ~5-15ms
Atomic write to history.json              < 20ms              ~2-5ms (SSD)
                                                           ~10-30ms (HDD)
Popup animation (Plus only)              < 200ms             ~200ms
Tray badge update                         < 50ms              ~5-10ms
History viewer open                       < 500ms             ~100-300ms
History search (500 entries)              < 500ms             ~50-200ms

On HDD, the disk write for atomic notification persistence may take 10-30ms. This is within the < 50ms target. If disk I/O is under heavy load, the write may take longer — but the notification is still displayed after the write completes. The invariant is maintained.
Notification History Performance

Metric                                    Target              Notes
──────────────────────────────────────────────────────────────────────────
Load history.json at startup              < 100ms             Single JSON parse
Write notification to history             < 20ms              Atomic write + fsync
Search 500 entries                       < 500ms             Simple substring search
Search 1000 entries                       < 1s                Max count is 1000
History viewer scroll                      60fps              Smooth scrolling
Badge count update                        < 50ms             In-memory, no disk I/O

Audio Performance
Audio Latency

Metric                                    Target              Notes
──────────────────────────────────────────────────────────────────────────
Round-trip latency (mic → speaker)        < 50ms              Acceptable for video calls
Buffer underruns at idle                  0                   No audio glitches at idle
Buffer underruns under load               < 5/min             Occasional under load is acceptable
PipeWire daemon CPU at idle               < 1%                ~0.1-0.5%
PipeWire daemon CPU (audio playback)      < 3%                ~1-2%
PipeWire daemon CPU (BT audio)            < 5% (SBC)          ~2-3% SBC
                                          < 8% (AAC, Plus)    ~5-7% AAC encode

Bluetooth Audio Pairing

Operation                         Target              Notes
──────────────────────────────────────────────────────────
Pair new Bluetooth device          < 10s               Discovery + pairing
Reconnect paired device            < 5s                Auto-reconnect
Audio routing switch (A2DP → HFP)  < 1s                When mic needed for call
Audio routing switch (HFP → A2DP)  < 1s                When call ends

GPU Performance
VA-API Video Decode

Metric                                    Tier 1 (N4020)       Tier 2 (AMD A4)
──────────────────────────────────────────────────────────────────────────────
1080p H.264 decode CPU usage              < 15%                < 25%
1080p VP9 decode CPU usage                < 15%                < 25%
1080p H.265 decode CPU usage              < 15%                < 25%
1080p AV1 decode                          N/A (disabled)      Varies
CPU usage with VA-API (1080p60)            ~5-15%               ~10-20%
CPU usage without VA-API (1080p60)         ~60-90%              ~80-100%
Frame drops per minute (with VA-API)       0                   0-2
Frame drops per minute (without VA-API)    10-30+              30-60+

VA-API is the difference between smooth 1080p video and an unwatchable slideshow on Celeron hardware. This is why AV1 is disabled in Firefox — it forces VP9 fallback which has hardware decode support.
Compositor Performance

Metric                                    Spike Standard       Spike Plus
──────────────────────────────────────────────────────────────────────────────
Frame rate (idle desktop)                 60fps                60fps
Frame rate (window drag)                  60fps                60fps
Frame rate (animation)                    N/A (no anim)        60fps target
KWin CPU at idle                          < 2%                 < 3%
KWin CPU (window drag)                    < 5%                 < 8%
KWin CPU (animation active)               N/A                  < 10%
KWin memory                               ~100-150MB            ~120-180MB
Rendering backend                         QPainter (software)  OpenGL (GPU)

On Spike Standard, KWin uses software rendering (QPainter) when the GPU is weak (Intel UHD 600). This trades GPU cycles for CPU cycles, which is appropriate when the GPU is the bottleneck.

On Spike Plus, KWin uses OpenGL rendering. The GPU handles compositing, freeing the CPU for user applications.
Temperature Performance
Thermal Targets

Metric                                    Tier 1 (N4020)       Tier 2 (AMD A4)
──────────────────────────────────────────────────────────────────────────────
Idle CPU temperature                      < 50°C               < 55°C
Normal workload temperature               < 65°C               < 75°C
Heavy workload temperature                < 80°C               < 85°C
Sustained warning threshold               80°C                 80°C
Emergency shutdown                        105°C (hardware)     105°C (hardware)
Fan spin-up (if present)                  Firmware-managed     Firmware-managed

Spike does not manage fans or override thermal limits. The hardware's firmware/EC handles thermal protection. Spike monitors temperatures for diagnostic purposes and warns the user if sustained high temperatures are detected.

Settings → Advanced → Diagnostics → Temperatures:
├── Current CPU temperature
├── Max recorded temperature (since boot)
├── Average temperature (last 5 minutes)
├── Thermal headroom (distance to throttle point)
└── Warning indicator if sustained > 80°C

ISO Performance
ISO Size

Metric                                    Target              Estimate
──────────────────────────────────────────────────────────────────────────
ISO file size                             < 3.5GB             ~3.1GB
USB write time (USB 3.0, 32GB)            < 5 min             ~3-4 min
USB write time (USB 2.0)                  N/A                 N/A (not supported)
Live USB boot time (Tier 1 SSD)           < 60s               ~30-45s
Live USB boot time (Tier 1 HDD)           < 90s               ~60-80s

The ISO contains:

Component                                Size
──────────────────────────────────────────────────
Ubuntu Server base                       ~800MB
KDE standalone applications              ~400MB
Spike Shell + config tools               ~50MB
Flatpak runtimes (pre-seeded)            ~1.2GB
Firmware (all Wi-Fi vendors)             ~300MB
VA-API drivers                           ~50MB
Bluetooth/modem/VPN packages             ~80MB
Branding and Plymouth themes             ~20MB
Live environment (squashfs)              ~200MB
──────────────────────────────────────────────────
Total ISO                                ~3.1GB

Installation Time

Stage                                    Tier 1 (SSD)        Tier 1 (HDD)
──────────────────────────────────────────────────────────────────────────
1. Welcome + language                     ~30s                ~30s
2. Timezone                               ~10s                ~10s
3. Wi-Fi connection                       ~5-10s              ~5-10s
4. Username + password                    ~30s                ~30s
5. Computer name                          ~10s                ~10s
6. Variant selection                      ~10s                ~10s
7. Data backup (optional, varies)         User-dependent      User-dependent
8. Storage confirmation                   ~10s                ~10s
9. Installation (automated)              ~5-8 min            ~10-15 min
10. Reboot to desktop                     ~20s                ~40s
──────────────────────────────────────────────────────────────────────────
Total (excluding backup)                  ~8-12 min           ~12-18 min

Installation time is dominated by package installation (step 9). On SSD, apt unpacks and configures packages quickly. On HDD, disk I/O during package installation is the bottleneck.
Regression Testing
What Triggers A Regression Test

A regression test against these baselines must be run when:

Trigger                                  What to test
──────────────────────────────────────────────────────────────────────────
New component added to Spike Shell        Idle memory, session startup time
New background service added              Idle memory, boot time
Kernel parameter changed                  Boot time, memory, VA-API
New autostart entry added                 Session startup time, idle memory
New Flatpak runtime pre-seeded            ISO size, disk usage
Theme or rendering change                 Compositor performance, frame rate
spike-config module added/changed         Boot time (config generation)
Installer step changed                    Installation time
 Plymouth theme changed                   Boot time, visual transition

Regression Test Matrix

Test                                    Tier 1 (SSD)    Tier 2 (HDD)    Tier 3 (NVMe)
──────────────────────────────────────────────────────────────────────────────────
Cold boot time                          ✓               ✓               ✓
Idle memory (Standard)                  ✓               ✓               ✓
Idle memory (Plus)                      ✓               —               ✓
App launch (Firefox, Dolphin, Kate)     ✓               ✓               ✓
Shutdown time                           ✓               ✓               ✓
Suspend/resume                          ✓               ✓               ✓
VA-API video decode                     ✓               ✓               ✓
Compositor frame rate                   ✓               ✓               ✓
Notification latency                     ✓               ✓               ✓
Network connect time                     ✓               ✓               ✓
ISO write + live boot                   ✓               —               ✓
Installation time                       ✓               ✓               —

Tier 3 regression checks ensure nothing breaks on higher-end hardware. If a regression appears on Tier 3 but not Tier 1/2, it may indicate a hardware-specific issue worth investigating.
Baseline Tracking
Where Baselines Are Tracked

Location: /var/lib/spike/performance/baselines.json
Format: JSON
Permissions: 644 (root:root)

Structure:
{
  "version": 1,
  "last_updated": "2026-XX-XX",
  "hardware": {
    "tier": 1,
    "cpu_model": "Intel Celeron N4020",
    "ram_total_mb": 4096,
    "storage_type": "ssd",
    "storage_size_gb": 240,
    "gpu_vendor": "Intel",
    "gpu_model": "UHD Graphics 600"
  },
  "variant": "standard",
  "spike_version": "0.1.0-alpha",
  "metrics": {
    "boot_time_seconds": {
      "median": null,
      "min": null,
      "max": null,
      "runs": 0,
      "measured_at": null
    },
    "idle_memory_mb": {
      "median": null,
      "min": null,
      "max": null,
      "runs": 0,
      "measured_at": null
    },
    "app_launch_firefox_seconds": { ... },
    "app_launch_dolphin_seconds": { ... },
    ...
  },
  "notes": "All values null until alpha hardware testing begins"
}

Until real measurements are taken, all values are null. Estimates in this document are architectural projections, not measurements.
Performance Reporting

Settings → Advanced → Diagnostics → Performance:
├── Current baselines (from baselines.json)
├── Comparison to targets (green/yellow/red indicators)
├── Historical trend (if multiple measurements recorded)
├── [Run performance test] button:
│   ├── Measures boot time (requires reboot, measures on next boot)
│   ├── Measures idle memory (60-second observation)
│   ├── Measures app launch times (5 apps, 5 runs each)
│   ├── Generates report
│   └── Optionally submits to hardware registry (anonymous, opt-in)
└── [Export performance report] (for support tickets)

The hardware registry submission is strictly opt-in. If the user declines, no data leaves the machine. If they opt in, only performance metrics (no personal data) are submitted to help improve Spike across hardware.
Targets Summary

All targets in one place, for quick reference:

Metric                                    Target              Variant
──────────────────────────────────────────────────────────────────────────
Cold boot (Tier 1 SSD)                    < 40s               Both
Cold boot (Tier 1 HDD)                    < 60s               Both
Cold boot (Tier 2 HDD)                    < 90s               Both
Idle memory                               < 400MB             Standard
Idle memory                               < 800MB             Plus
Discover launch                           < 2s                Both
Firefox launch                            < 3s                Both
Average app launch                        < 2s                Both
Shutdown                                  < 5s                Both (SSD)
Suspend enter                             < 3s                Both
Suspend resume                            < 2s                Both
Hibernate resume                          < 10s               Both (SSD)
Compositor frame rate (idle)             60fps               Both
Compositor frame rate (drag)              60fps               Both
VA-API CPU usage (1080p)                  < 15%               Standard
Notification latency                      < 50ms              Both
ISO size                                  < 3.5GB             Both
Installation time (SSD)                   < 12 min            Both
Installation time (HDD)                   < 18 min            Both
ZRAM compression ratio                    > 2.5:1             Both
Swap file usage at idle                   0MB                 Both
Wi-Fi connect (known network)             < 5s                Both

What This Document Does Not Cover

    Boot process sequence and GRUB configuration: See BOOT-PROCESS.md
    Memory management architecture (ZRAM, swap, earlyoom): See MEMORY.md
    Kernel boot parameters and module blacklisting: See KERNEL.md
    GPU driver selection and VA-API configuration: See KERNEL.md and MULTIMEDIA.md
    Installer flow and timing: See INSTALLER.md
    Hardware tier definitions and detection: See HARDWARE.md (to be written)
    Variant differences (animation, effects, polling): See VARIANT-DIFFERENCES.md
    Testing procedures and CI/CD: See dev-guide/11-testing.md (to be written)
    Hardware registry submission: See SUPPORT.md (to be written)

🐕 BigRangaTech
