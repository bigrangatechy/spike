Spike Kernel
Base Kernel

Spike uses the Ubuntu 26.04 LTS (Resolute Raccoon) default generic kernel — Linux 7.0 — with no custom compilation, no patched sources, and no out-of-tree module building (except Broadcom's bcmwl-kernel-source proprietary driver).
Why No Custom Kernel

A custom kernel was evaluated and rejected:

    Maintenance burden: Rebasing against upstream LTS security patches, tracking regressions, and testing across the target hardware range is unrealistic for a solo developer.
    Minimal gain: The Ubuntu LTS kernel is already well-optimized. The gains from compiling a stripped kernel (slightly smaller image, slightly faster boot) are marginal compared to the risk and ongoing cost.
    Module blacklisting covers the gap: Unused modules can be prevented from loading at boot without recompiling. This recovers most of the memory a custom kernel would save.

What Spike Uses From The Ubuntu LTS Kernel

    Generic kernel package (linux-generic)
    initramfs-tools (Ubuntu default)
    Module loading via udev and modprobe
    Kernel Livepatch (available if Ubuntu Pro/ESM is active — not enabled by default, no user action required)

Support Lifecycle

Ubuntu 26.04 LTS receives:

    5 years standard security support (to 2031)
    Up to 10 years with ESM (extended to 2036)

Spike locks to the LTS kernel series for the lifetime of the release. Point releases (26.04.1, 26.04.2, etc.) bring kernel updates within the same major version. Spike does not chase HWE (Hardware Enablement) kernels — the LTS kernel is sufficient for the target hardware range.
Boot Parameters

The following kernel boot parameters are applied in GRUB2's configuration at install time:

GRUB_CMDLINE_LINUX_DEFAULT="quiet splash zswap.enabled=0 transparent_hugepage=madvise i915.modeset=1 amdgpu.modeset=1 nouveau.modeset=1 loglevel=3"

Parameter Breakdown
Parameter	Purpose
quiet	Suppress kernel boot messages (beginner-friendly)
splash	Enable Plymouth boot splash (Spike-themed)
zswap.enabled=0	Disable kernel zswap (Spike uses ZRAM instead — having both wastes memory)
transparent_hugepage=madvise	Allocate huge pages on demand rather than always (prevents memory fragmentation on low-RAM systems)
i915.modeset=1	Enable kernel mode-setting for Intel GPUs (required for Wayland)
amdgpu.modeset=1	Enable kernel mode-setting for AMD GPUs (required for Wayland)
nouveau.modeset=1	Enable kernel mode-setting for NVIDIA GPUs (required for Wayland on nouveau)
loglevel=3	Suppress non-critical kernel messages (err, warn, and above shown only)
Why zswap Is Disabled

Spike uses ZRAM for compressed swap in memory, managed by userspace. If zswap (kernel-level compressed swap cache) were also enabled, the two would compete — compressing already-compressed pages, wasting CPU cycles, and creating unpredictable memory behavior. Disabling zswap ensures ZRAM is the sole compressed memory layer.
Why transparent_hugepage=madvise

The default setting (always) aggressively allocates 2MB huge pages, which can cause memory fragmentation on 4GB systems. The madvise setting allows applications that explicitly request huge pages (via madvise()) to get them, while leaving the rest of the system on 4KB pages. This balances performance and memory efficiency on low-RAM hardware.
GPU-Specific Notes

i915.modeset=1, amdgpu.modeset=1, and nouveau.modeset=1 are all included because the installer runs on whatever hardware is present. On Intel systems, the AMD and NVIDIA parameters have no effect (modules not loaded). On AMD systems, the Intel and NVIDIA parameters have no effect. On NVIDIA systems, the Intel and AMD parameters have no effect. There is no harm in including all three — the kernel only applies the relevant one based on detected hardware.

When the proprietary NVIDIA driver is installed (replacing nouveau), nouveau.modeset=1 is replaced with nvidia-drm.modeset=1 in the GRUB configuration automatically by the ubuntu-drivers tool.
Module Blacklisting

At install time, Spike generates a module blacklist based on detected hardware. This prevents unnecessary kernel modules from loading at boot, reclaiming memory.
Blacklist Rules

CRITICAL RULE:
├── Only blacklist modules for ABSENT SOLDERED hardware
├── NEVER blacklist hot-pluggable devices
├── NEVER blacklist USB-connected devices
└── User may plug in a device later — the system must support it

What Gets Blacklisted

Modules for hardware categories that are absent and not hot-pluggable:
Category	Examples	Why
Enterprise storage	megaraid, mptsas, hpsa, aacraid	Target hardware has no RAID controllers
Infiniband	ib_core, ib_uverbs, mlx5_ib	Target hardware has no Infiniband
Exotic sensors	wmi_bmof, acpi_power_meter	Rarely present on consumer laptops
TV tuners	cx88, saa7134, em28xx	Not applicable to target hardware
Amateur radio	hamradio	Not applicable
Industrial I/O	industrialio, adc1x8s102	Not applicable
Fiber Channel	qla2xxx, lpfc, bfa	Not applicable
Old parallel/serial	parport, 8250_pci variants	Usually absent on target-era laptops
What Does NOT Get Blacklisted
Category	Why
USB subsystem (usbcore, xhci, ehci)	Hot-pluggable — user may connect USB devices
Bluetooth (btusb, btintel, btrtl)	May be internal or USB dongle
Wi-Fi drivers (iwlwifi, ath, rtl, brcm)	Hot-pluggable via USB adapters
Sound drivers (snd_hda_intel, snd_soc)	May be needed for HDMI audio
Webcams (uvcvideo)	Hot-pluggable
SD card readers (rtsx_pci, rtsx_usb)	Hot-pluggable
Input devices (hid, usbhid)	Hot-pluggable keyboards/mice
GPU drivers (i915, amdgpu, nouveau)	Always needed
Blacklist Generation

The blacklist is generated at install time by scanning detected hardware:

1. Run lspci -nn to enumerate present PCI devices
2. Run lsusb to enumerate present USB devices
3. Cross-reference with module database
4. For each known module category:
   ├── If matching PCI device is present → do NOT blacklist
   ├── If matching USB device is present → do NOT blacklist
   └── If no matching device → blacklist
5. Write blacklist to /etc/modprobe.d/spike-blacklist.conf
6. Update initramfs

The generated file looks like:

# /etc/modprobe.d/spike-blacklist.conf
# Generated by Spike Installer
# DO NOT EDIT — managed by Spike Settings
# To modify, use Settings → Advanced → Kernel Modules

# Enterprise RAID (not present)
blacklist megaraid_sas
blacklist mptsas
blacklist hpsa
blacklist aacraid

# Infiniband (not present)
blacklist ib_core
blacklist ib_uverbs
blacklist mlx5_ib

# [additional entries based on detected hardware]

NVIDIA And The Blacklist

The nouveau driver is never blacklisted at install time, even if no NVIDIA GPU is detected. This is because:

    A user may plug in an NVIDIA GPU later (external GPU enclosure, though rare)
    nouveau is in-tree and has negligible impact when no NVIDIA hardware is present (the module loads, finds no device, and idles)

When a user installs the proprietary NVIDIA driver through Settings → Software Sources → Additional Drivers, the ubuntu-drivers tool automatically blacklists nouveau and replaces it with nvidia-drm. This is handled by Ubuntu's standard driver management infrastructure — Spike does not interfere with this process.
User Management Of Blacklists

Per Spike's golden rule: users never edit config files. The blacklist file header explicitly states this. Module blacklisting can be viewed and managed through:

Settings → Advanced → Kernel Modules

This page shows:

    Currently blacklisted modules and why
    Detected hardware that caused the classification
    Warning if a previously-unknown device was hot-plugged (notification: "New device detected, system ready to use")

GPU Driver Configuration
Intel (Primary Target)

The Celeron N4020 uses Intel UHD Graphics 600 (Gemini Lake Refresh). Driver configuration:

Kernel module:    i915 (in-tree, loaded via modeset=1)
Userspace:        mesa (iris driver stack)
VA-API driver:    intel-media-va-driver-non-free
LIBVA_DRIVER_NAME: iHD

The iHD driver covers Gen 8+ Intel graphics (Broadwell through Gemini Lake). The older i965 driver is not used.
AMD (Secondary Target)

AMD A4/A6/A9 (Jaguar architecture and newer):

Kernel module:    amdgpu (in-tree, loaded via modeset=1)
Userspace:        mesa (radeonsi driver stack)
VA-API driver:    mesa-va-drivers
LIBVA_DRIVER_NAME: radeonsi

NVIDIA (Hybrid/Discrete GPU Support)

Spike ships the open-source nouveau driver by default. The proprietary NVIDIA driver is available as an optional post-install installation for users who need it.

Default: nouveau (included on ISO)

    Works out of the box for basic display output and compositing
    No licensing restrictions — safe to distribute on the ISO
    Lower memory footprint than the proprietary driver
    Wayland support is functional but less mature than Intel/AMD (XWayland fallback handles most cases)
    Limitation: No VA-API hardware video decode on most NVIDIA GPU generations

Optional: NVIDIA proprietary driver (user-installed)

    Available for GeForce GTX 900+ and newer GPUs
    Requires internet connection at install time (not on ISO due to NVIDIA EULA redistribution restrictions)
    Includes VA-API video acceleration (on supported GPU generations, via libva-nvidia-driver or VDPAU bridge)
    Cannot be pre-seeded on ISO due to licensing and size constraints
    Versions age quickly — downloading at install time ensures the latest compatible driver

Driver Selection At Install Time

The installer detects the GPU vendor and configures the appropriate driver:

If PCI device vendor == Intel:
    apt install intel-media-va-driver-non-free
    echo "LIBVA_DRIVER_NAME=iHD" >> /etc/environment

If PCI device vendor == AMD:
    apt install mesa-va-drivers
    echo "LIBVA_DRIVER_NAME=radeonsi" >> /etc/environment

If PCI device vendor == NVIDIA:
    Load nouveau by default
    Do NOT install proprietary driver automatically
    Create post-install notification file:
        "NVIDIA hardware detected. The open-source driver is active.
         If you need hardware video encoding, CUDA, or gaming
         performance, install the NVIDIA driver from
         Settings → Software Sources → Additional Drivers."

Proprietary NVIDIA Driver Installation Flow

1. NVIDIA hardware detected at boot via udev
2. nouveau loads by default — display works immediately
3. Post-install notification informs user of optional proprietary driver
4. User navigates to Settings → Software Sources → Additional Drivers
5. ubuntu-drivers tool lists compatible NVIDIA driver versions
6. User selects and installs
7. nouveau is automatically blacklisted by ubuntu-drivers
8. nvidia-drm is configured
9. GRUB2 updated: nouveau.modeset=1 → nvidia-drm.modeset=1
10. MOK enrollment prompted if Secure Boot is active
11. Reboot required (gentle notification, never forced)

Hybrid Graphics (NVIDIA Optimus)

Laptops with both Intel/AMD integrated graphics and NVIDIA discrete GPU are supported via PRIME render offload:

Settings → Power Management → Graphics
├── Integrated Only — Uses only Intel/AMD iGPU, discrete GPU powered down
├── Hybrid Mode — Discrete GPU activates on-demand for intensive tasks
└── Discrete Only — Uses NVIDIA GPU only (higher power consumption)

Spike Standard recommendation: Use "Integrated Only" for best battery life and lowest memory usage. Switch to Hybrid only if specific applications require the discrete GPU.

Spike Plus recommendation: Hybrid mode is the default on Plus, as the additional RAM and CPU headroom make on-demand GPU switching seamless.
NVIDIA Detection Script

At install time, the following logic runs to set up the post-install notification:

#!/bin/bash
# Spike Installer: GPU Detection and Notification

# Detect NVIDIA hardware
if lspci | grep -q "NVIDIA"; then
    # Check if proprietary driver is available in repos
    if apt-cache search "^nvidia-driver-[0-9]" >/dev/null 2>&1; then
        # Create notification file for Spike Shell to read on first login
        mkdir -p /run/spike/installer-notifications
        cat > /run/spike/installer-notifications/nvidia-hardware.txt <<EOF
NVIDIA GPU detected on your system.

The open-source nouveau driver is active and working for basic display.
If you need hardware video encoding, CUDA applications, or gaming
performance, you can install the NVIDIA proprietary driver.

To install:
  Settings → Software Sources → Additional Drivers → Choose NVIDIA driver

Note: Installing the proprietary driver requires an internet connection
and a system restart. Most users do not need to take action.
EOF
    fi
fi

The Spike Shell's post-install hook reads this notification file and displays it to the user after first login.
Known NVIDIA Limitations
Issue	Mitigation
nouveau lacks hardware video acceleration	Use integrated GPU for video playback; disable discrete NVIDIA GPU in hybrid mode
Wayland support is experimental on nouveau	Falls back to XWayland automatically
Secure Boot blocks unsigned kernel modules	MOK enrollment required for proprietary driver (ubuntu-drivers handles the prompt)
Fermi-era GPUs (pre-GTX 600) unsupported by modern proprietary driver	Use nouveau only; legacy proprietary branch is unmaintained
Proprietary driver not available offline	Must be installed after first boot with internet connection

Users with NVIDIA hardware are encouraged to report their experience through the hardware registry at spike.bigrangatech.com/hardware/ so the Spike team can improve support over time.
Kernel Tunables

The following sysctl values are set at install time:
Memory

# /etc/sysctl.d/99-spike-memory.conf

# Reduce swappiness (actual value set per storage type by spike-config)
vm.swappiness = 15          # Default (SSD). HDD systems override to 5.

# Overcommit memory — allow allocation but kill if needed
# (earlyoom handles this more gracefully than kernel OOM)
vm.overcommit_memory = 1

# Reduce vfs_cache_pressure to keep inode/dentry cache in memory
# (saves disk I/O on slow storage)
vm.vfs_cache_pressure = 50

Filesystem

# /etc/sysctl.d/99-spike-fs.conf

# Reduce dirty page cache writeback aggressiveness
# (prevents I/O spikes on slow HDD storage)
vm.dirty_background_ratio = 5
vm.dirty_ratio = 15

Networking

# /etc/sysctl.d/99-spike-network.conf

# IPv4 forwarding disabled (not a router)
net.ipv4.ip_forward = 0

# SYN flood protection
net.ipv4.tcp_syncookies = 1

# Disable IPv6 autoconfiguration (reduces network noise)
# Only if no IPv6 network detected at install
net.ipv6.conf.all.autoconf = 0
net.ipv6.conf.default.autoconf = 0

These are set by spike-config at install time and are adjustable through Settings → Advanced → System Diagnostics (read-only view) or Settings → Network for network-related values.
CPU Frequency Scaling
Default Governor

Spike Standard:
├── CPU governor: powersave (default for target hardware)
└── Rationale: Celeron laptops are primarily battery-powered.
   Powersave governor reduces heat and extends battery life.
   Burst frequencies still available when needed.

Spike Plus:
├── CPU governor: schedutil
└── Rationale: Modern CPUs with more headroom benefit from
   scheduler-driven frequency scaling.

The governor is set at install time and can be changed in Settings → Power.
Turbo Boost / Precision Boost

Left at kernel defaults (enabled). The Celeron N4020's burst frequency (up to 2.8GHz) is essential for responsiveness under load. Disabling turbo would make the system feel sluggish during app launches.
Kernel Security Features
AppArmor

AppArmor is enabled with Ubuntu's default profiles. No custom profiles are created. See SECURITY.md for details.
Kernel Module Signing

Ubuntu LTS kernels are signed by Canonical. Secure Boot is supported out of the box.

    The bcmwl-kernel-source (Broadcom proprietary Wi-Fi) driver requires MOK (Machine Owner Key) enrollment on Secure Boot systems — the installer handles this automatically if Broadcom hardware is detected.
    The NVIDIA proprietary driver also requires MOK enrollment on Secure Boot systems — this is handled by the ubuntu-drivers tool when the user installs the driver post-install.

Lockdown Mode

Ubuntu 26.04 LTS enables kernel lockdown in integrity mode when Secure Boot is active. This prevents unsigned kernel modules from loading. Spike does not modify this behavior.
Kernel Updates
How Kernel Updates Work

Kernel updates arrive through Ubuntu's standard apt channel:

sudo apt update && sudo apt upgrade

Or through Discover (GUI — the intended path for Spike users).

When a new kernel is installed:

    GRUB2 is automatically updated
    initramfs is regenerated (including Spike's module blacklist)
    Old kernel is retained (fallback if new kernel fails to boot)
    User is notified that a reboot is needed (gentle notification, never forced)

Boot Failure Handling

If a new kernel fails to boot:

1. System hangs or crashes during boot
2. User power-cycles (hard reset)
3. Boot failure counter increments (/boot/.spike/boot-count)
4. After 3 failed boots:
   ├── GRUB2 menu appears automatically
   ├── Previous (known-good) kernel is highlighted
   └── Recovery entry is available
5. User boots previous kernel
6. Notification: "Spike detected a boot failure. The previous kernel
   is running. The problematic update has been marked for removal."
7. Problematic kernel is held back from future updates
   until explicitly upgraded or the user retries

This is transparent to the user — they see a menu that says "Previous version" and "Recovery mode," not kernel version numbers or technical details.
NVIDIA Driver And Kernel Updates

When the kernel is updated and the NVIDIA proprietary driver is installed, the driver module must be rebuilt against the new kernel headers. Ubuntu's DKMS infrastructure handles this automatically:

Kernel update arrives via apt/Discover
    │
    ├── DKMS detects kernel version change
    ├── DKMS rebuilds nvidia-drm module against new kernel headers
    ├── If rebuild succeeds:
    │   ├── Module ready for next boot
    │   └── User notified of reboot needed
    └── If rebuild fails:
        ├── Old kernel retained as fallback
        ├── Notification: "NVIDIA driver could not be built for the
        │   new kernel. Booting with the previous kernel."
        └── User directed to check for driver updates

This is a known pain point with proprietary NVIDIA drivers across all Linux distributions. Spike mitigates it by retaining the previous kernel and notifying the user clearly if something goes wrong. The open-source nouveau driver does not have this problem — it is part of the kernel tree and updates automatically.
Kernel Removal Policy

Spike retains the previous kernel as a fallback. Kernels older than the previous one are automatically removed by the package manager. This prevents /boot from filling up on small partitions.
What This Document Does Not Cover

    ZRAM configuration: See MEMORY.md for ZRAM sizing, compression algorithm, priority, and detection logic
    Boot process sequence: See BOOT-PROCESS.md for the full boot chain from power-on to desktop
    GPU/video decode: See MULTIMEDIA.md for VA-API, Firefox configuration, AV1 disabling, and NVIDIA video limitations
    Module blacklist management GUI: See DESKTOP.md (Settings panel) for the user-facing module management interface
    Security hardening: See SECURITY.md for AppArmor, firewall, and update policies
    NVIDIA driver management GUI: See DESKTOP.md (Settings panel) for the Additional Drivers interface
    Variant differences: See VARIANT-DIFFERENCES.md for the complete comparison of Spike Standard and Spike Plus

This document reflects the kernel configuration for Spike Standard and Spike Plus. Differences between variants (CPU governor, module blacklisting aggressiveness, NVIDIA default mode) are noted inline. For the complete variant comparison, see VARIANT-DIFFERENCES.md.

🐕 BigRangaTech
