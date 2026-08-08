# Spike Installer

## Purpose

This document specifies the Spike installer — the tool that transforms a blank or used laptop into a Spike system. The installer is the user's first experience with Spike. It must be approachable enough for someone who has never installed an operating system, while handling all the complex configuration decisions automatically.

## Design Philosophy

The installer follows one principle: minimize user decisions. Every question asked is a question a beginner might not know how to answer. Every automated step is a potential mistake the user doesn't have to make.

```
├── No manual partitioning
├── No dual boot
├── No encryption
├── No package selection
├── No desktop customization during install
├── No network configuration beyond Wi-Fi credentials
└── No technical terminology without plain-language explanation
```

### Related data tools (not the installer itself)

```
├── Move My Files (spike-migration) — guided pre-install copy to SpikeBackup/
│   or post-install import; spec: SPIKE-MIGRATION.md
├── Rescue My Files (spike-rescue) — disaster recovery from a broken system;
│   spec: DISASTER-RECOVERY.md Layer 3
├── Installer Step 7 — optional backup of the *target* drive during install
│   (same SpikeBackup/ layout)
└── Reinstall with restore — Layer 4; consumes SpikeBackup/ after a fresh install
```

Overview map: `SPIKE-RECOVERY-TOOL-GENERAL.md`.

## Installer Flow Overview

```
User boots from Spike ISO
        │
        ▼
Live environment starts (Spike Shell, read-only)
        │
        ▼
Desktop icon: "Install Spike" (double-click)
        │
        ▼
┌─────────────────────────────────────────────────┐
│ STEP 1:  Welcome + Language selection            │
│ STEP 2:  Timezone selection                      │
│ STEP 3:  Wi-Fi connection                        │
│ STEP 4:  Username + password                     │
│ STEP 5:  Computer name (hostname)                │
│ STEP 6:  Variant selection (auto-recommended)    │
│ STEP 7:  Data backup to USB (optional)           │
│ STEP 8:  Storage confirmation + wipe warning     │
│ STEP 9:  Installation (automated, progress bar)  │
│ STEP 10: Reboot to desktop                        │
└─────────────────────────────────────────────────┘
```

### Step Count

Ten steps, but only seven require user input. Steps 9 and 10 are passive (progress bar and reboot). The average install time on a Celeron N4020 with SSD is estimated at 8-12 minutes.

## Step 1: Welcome + Language Selection

```
┌──────────────────────────────────────────────────┐
│                                                  │
│              Welcome to Spike                    │
│                                                  │
│         Let's Make Tech Repairable Again          │
│                                                  │
│  Choose your language:                            │
│  ┌──────────────────────────────────────────┐   │
│  │ English (United States)                 ▼ │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  (Dropdown lists all supported languages)        │
│                                                  │
│  [Continue]                                      │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Language List

Spike ships with 11 supported languages at launch:

- English (US), English (UK) 

- Français 

- Deutsch 

- Español 

- Italiano 

- Português (BR) 

- Nederlands 

- Polski 

- Русский 

- 日本語 

The language selected here sets the system language, installer language, keyboard layout suggestion, and timezone suggestion. Translations for the installer are embedded in the ISO. System translations are downloaded post-install if needed.

### Accessibility

An accessibility icon is present on the welcome screen (bottom-left corner) that allows the user to enable screen magnification, high contrast, or on-screen keyboard before beginning the installation.

## Step 2: Timezone Selection

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Select your timezone                             │
│                                                  │
│  ┌──────────────────────────────────────────┐   │
│  │ Australia/New Zealand — Auckland        ▼ │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  Region: Australasia                             │
│  City: Auckland                                  │
│                                                  │
│  [Back]  [Continue]                              │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Detection

If Wi-Fi was already connected (or Ethernet present), the installer attempts geolocation via IP address lookup. If the lookup succeeds, the timezone is pre-selected and the user confirms. If the lookup fails or no network is available, the user selects manually from a dropdown organized by region.

### Keyboard Layout Suggestion

Based on timezone, the installer suggests a keyboard layout:

```
Timezone → suggested keyboard layout:
├── United States, Canada → US English
├── United Kingdom, Ireland → UK English
├── Germany, Austria, Switzerland → German
├── France, Belgium → French
├── Spain, Latin America → Spanish
├── Italy → Italian
├── Japan → Japanese
└── (user can change in next step or later)
```

The suggested layout is pre-selected but can be overridden. Keyboard layout is confirmed in the background — the installer accepts the suggestion by default. If the user needs a different layout, they can type in a test box to verify.

## Step 3: Wi-Fi Connection

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Connect to Wi-Fi                                 │
│                                                  │
│  Available networks:                             │
│                                                  │
│  📶 Home WiFi (secure)          ██████░░  [Connect]│
│  📶 Neighbor's WiFi (secure)    ████░░░░          │
│  📶 Coffee Shop (open)          ███████░          │
│  📶 Guest Network (secure)      ██░░░░░░          │
│                                                  │
│  [Skip Wi-Fi]  [Refresh]                         │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Behavior

- NetworkManager performs a Wi-Fi scan 

- Networks are listed by signal strength (strongest first) 

- Open and secure networks are visually distinguished 

- Clicking "Connect" on a secure network prompts for password 

- Password field has a "Show password" toggle (eye icon) 

- Incorrect password shows inline error and lets user retry 

- If no Wi-Fi adapter detected: step is skipped, Ethernet assumed 

- "Skip Wi-Fi" allows installation without network (limits post-install features like Flatpak) 

### Why Wi-Fi During Install

Connecting to Wi-Fi during installation serves three purposes:

1. Geolocation for timezone pre-selection (Step 2) 

2. Package updates — the installer pulls the latest security patches during installation 

3. Flatpak runtime validation — verifies pre-seeded runtimes are current 

If the user skips Wi-Fi, the system installs from the ISO only. Updates and runtime checks happen on first boot after the user connects to Wi-Fi in the installed system.

## Step 4: Username + Password

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Create your account                              │
│                                                  │
│  Your name:                                       │
│  ┌──────────────────────────────────────────┐   │
│  │ John Smith                                │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  Username:                                        │
│  ┌──────────────────────────────────────────┐   │
│  │ john                                      │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  Password:                                        │
│  ┌──────────────────────────────────────────┐   │
│  │ ••••••                                    │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  Confirm password:                               │
│  ┌──────────────────────────────────────────┐   │
│  │ ••••••                                    │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  Minimum 6 characters. Choose something          │
│  you'll remember.                                │
│                                                  │
│  [Back]  [Continue]                              │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Username Rules

- Lowercase letters, numbers, hyphens, underscores only 

- Must start with a letter 

- 3-32 characters 

- Auto-generated from "Your name" field (first name, lowercased) 

- User can edit the auto-generated username 

### Password Policy

- Minimum 6 characters 

- No complexity requirements (no forced special characters, uppercase, or numbers) 

- No password strength meter (doesn't judge the user) 

- Password confirmation must match 

- The password is used for both login and sudo 

### Why No Complexity Requirements

Beginners who are forced to use complex passwords tend to write them down or use predictable patterns (Password1!, etc.). A simple memorable password that the user doesn't need to write down is better security than a complex one taped to the laptop lid.

### Root Account

The root account is locked (disabled). The user account has sudo access. There is no separate root password. This is configured automatically — the user is never asked about root.

## Step 5: Computer Name (Hostname)

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Name your computer                               │
│                                                  │
│  This name identifies your computer on networks.  │
│  You can use the suggested name or type your own. │
│                                                  │
│  Computer name:                                   │
│  ┌──────────────────────────────────────────┐   │
│  │ spike-laptop                              │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  Suggestion: spike-laptop                        │
│  (Or: spike-[username], e.g., spike-john)        │
│                                                  │
│  [Back]  [Continue]                              │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Hostname Rules

- Lowercase letters, numbers, hyphens only 

- Must start with a letter 

- 3-63 characters 

- Cannot end with a hyphen 

- Default suggestion: `spike-laptop` 

- Alternative suggestion: `spike-[username]` (e.g., `spike-john`) 

### Where The Hostname Appears

- Network discovery (other devices see this name) 

- Terminal prompt (`john@spike-laptop:~$`) 

- Bluetooth device name 

- Settings → About page 

## Step 6: Variant Selection

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Spike detected your hardware:                    │
│                                                  │
│  CPU:    Intel Celeron N4020                      │
│  RAM:    4GB                                      │
│  Storage: 240GB SSD                               │
│  GPU:    Intel UHD Graphics 600                   │
│                                                  │
│  Based on your hardware, Spike recommends:        │
│                                                  │
│  ● Spike Standard                                 │
│    Optimized for 4GB RAM systems.                 │
│    Animations off, minimal background services.   │
│                                                  │
│  ○ Spike Plus                                     │
│    For systems with 8GB+ RAM.                    │
│    (Not recommended for your hardware)            │
│                                                  │
│  [Back]  [Continue with Standard]                 │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Detection Logic

```
If RAM >= 8GB AND CPU is modern dual-core+ (non-Celeron/Pentium class):
    → Recommend Spike Plus
If RAM <= 4GB OR CPU is Celeron/Pentium class:
    → Recommend Spike Standard
```

**Edge cases:**

```
├── RAM between 5-7GB: Recommend Standard (conservative)
├── 8GB RAM with Celeron: Recommend Standard (CPU is the bottleneck)
├── 4GB RAM with modern i5: Recommend Standard (RAM is the bottleneck)
└── 8GB+ RAM with modern i3/i5/i7/Ryzen: Recommend Plus
```

### User Override

The user can select either variant regardless of the recommendation. If they choose Plus on inadequate hardware, the installer shows:

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  ⚠️  Spike Plus is not recommended for your       │
│     hardware.                                     │
│                                                  │
│  Your system has 4GB RAM. Spike Plus expects      │
│  8GB or more. Performance may be affected.        │
│                                                  │
│  [Use Spike Standard instead]                     │
│  [Continue with Spike Plus anyway]                │
│                                                  │
└──────────────────────────────────────────────────┘
```

If they choose Standard on adequate hardware, no warning is shown (choosing less is always safe).

### What The Variant Affects

All variant differences are applied as install-time configuration. The codebase is identical. See **VARIANT-DIFFERENCES.md** for the complete comparison.

## Step 7: Data Backup (Optional)

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Back up your files?                              │
│                                                  │
│  Spike found personal files on this computer:     │
│                                                  │
│  📁 Documents   — 124 files                       │
│  📁 Photos      — 1,203 files                     │
│  📁 Videos      — 47 files                        │
│  📁 Music       — 89 files                        │
│  📁 Downloads   — 56 files                        │
│  📁 Desktop     — 12 files                        │
│                                                  │
│  Total: 1,531 files (8.2 GB)                      │
│                                                  │
│  Installing Spike will erase everything on this   │
│  drive. Would you like to back up your files to   │
│  a USB drive first?                              │
│                                                  │
│  [Back up my files to USB]                        │
│  [Skip backup and continue]                       │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Scan Process

When the user reaches this step, the installer has already scanned the target drive for personal files. The scan happens silently in the background during earlier steps.

**Scan targets:**

```
├── Windows: C:\\Users[username]\\{Documents, Photos, Videos, Music, Downloads, Desktop, Pictures}
├── Linux:   /home/[username]/{Documents, Photos, Videos, Music, Downloads, Desktop, Pictures}
├── macOS:   /Users/[username]/{Documents, Photos, Videos, Music, Downloads, Desktop, Pictures}
└── Multiple user accounts: All user directories scanned, summed together
```

**Scan output:**

```
├── File counts per directory
├── Total size
├── No file listing shown (privacy — filenames not displayed)
└── Corrupt/unreadable files counted separately
```

### Backup Flow

If the user chooses to back up:

```
1. Prompt: "Connect a USB drive with at least [size] GB free space"
2. Wait for USB insertion (detect via udev)
3. Verify USB has sufficient free space
4. If insufficient space:
   ├── Show how much space is needed vs. available
   ├── Offer to exclude large directories (Videos, Photos)
   └── Or use a different USB drive
5. Begin copy:
   ├── Preserve folder structure (Documents/, Photos/, etc.)
   ├── Copy to USB at /run/media/usb/SpikeBackup/
   ├── Checksum verification on every file (SHA256)
   └── Progress bar: "Backing up files... 45% (689/1,531 files)"
6. On completion:
   ├── Report: "1,528 files copied successfully. 3 files could not be read (skipped)."
   ├── List skipped files (if any) in a expandable details section
   └── "You can now safely remove the USB drive or leave it connected."
7. Proceed to Step 8
```

### Backup Verification

Every file copied is verified with SHA256 checksum comparison. If a checksum doesn't match, the file is re-copied. If it still fails, the file is listed as "failed to copy" and the user is informed.

### Partial Corruption Handling

If the source drive has corrupt sectors or damaged files:

```
├── Installer attempts to read the file
├── If read fails: file is logged as "could not read"
├── Copy continues with remaining files (does not abort)
├── Summary screen shows:
│   ├── Files copied successfully: 1,528
│   ├── Files that could not be read: 3
│   └── "These files may be damaged on your old drive. You can try
│       recovering them with a data recovery tool."
└── User can view the list of failed files (filename + path)
```

### If No Personal Files Found

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  No personal files found                          │
│                                                  │
│  Spike scanned this drive and didn't find any      │
│  personal files to back up. The drive may be       │
│  empty or freshly formatted.                      │
│                                                  │
│  [Continue]                                       │
│                                                  │
└──────────────────────────────────────────────────┘
```

Step 7 is skipped entirely if the drive is empty or has no recognizable user data directories.

### If USB Drive Too Small

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Not enough space on USB drive                    │
│                                                  │
│  Your files: 8.2 GB                               │
│  USB free space: 4.1 GB                           │
│                                                  │
│  You can:                                         │
│  • Use a larger USB drive                         │
│  • Exclude some folders:                         │
│                                                  │
│    ☑ Documents (1.2 GB)                          │
│    ☑ Photos (3.1 GB)                             │
│    ☐ Videos (2.8 GB)                             │
│    ☑ Music (0.3 GB)                              │
│    ☑ Downloads (0.4 GB)                          │
│    ☑ Desktop (0.4 GB)                           │
│                                                  │
│  Selected: 5.4 GB                                 │
│  USB free: 4.1 GB                                │
│                                                  │
│  [Back]  [Retry with selected folders]            │
│                                                  │
└──────────────────────────────────────────────────┘
```

## Step 8: Storage Confirmation + Wipe Warning

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  ⚠️  Final confirmation                            │
│                                                  │
│  Spike will install on:                           │
│                                                  │
│  Drive: Samsung SSD 850 EVO 240GB                │
│  Type: SSD (SATA)                                 │
│  Size: 240 GB                                     │
│                                                  │
│  Everything on this drive will be erased.        │
│  This cannot be undone.                           │
│                                                  │
│  ✓ Files backed up to USB (if backup completed)  │
│  ✓ Ready to install                               │
│                                                  │
│  Type "ERASE" to confirm:                         │
│  ┌──────────────────────────────────────────┐   │
│  │                                           │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
│  [Back]  [Install Spike]                          │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Type-To-Confirm

The user must type "ERASE" (case-insensitive) to enable the "Install Spike" button. This prevents accidental clicks on the most destructive action in the installer.

### Storage Notices

At this step, any storage-related warnings from the detection phase are shown:

**If HDD detected:**

```
├── Warning shown (see MEMORY.md for HDD upgrade warning)
└── User acknowledged earlier, shown here as "✓ HDD notice acknowledged"
```

**If SD card or USB drive detected as install target:**

```
├── Warning: "Installing to an SD card/USB drive. Performance may
│   be lower than an internal drive."
└── User must acknowledge before continuing
```

**If eMMC detected:**

```
├── Installation blocked
└── Message: "eMMC storage is not supported by Spike. eMMC drives
    wear out quickly with regular system use. Please replace with
    an SSD or use a different computer."
```

**If USB 2.0 drive detected:**

```
├── Installation blocked
└── Message: "This USB drive is too slow for Spike. USB 3.0 or
    faster is required."
```

### Single Drive Requirement

Spike installs to a single drive. If multiple drives are present, the installer selects the largest one and shows it in the confirmation. The user cannot choose a different drive — if they need a specific drive, they should disconnect the others before installing.

**Exception:** If the largest drive is an HDD and a smaller SSD is present, the installer selects the SSD.

## Step 9: Installation (Automated)

```
┌──────────────────────────────────────────────────┐
│                                                  │
│            Installing Spike...                    │
│                                                  │
│  ████████████████████░░░░░░░░░░░░░░  62%          │
│                                                  │
│  Formatting drive...                    ✓ Done   │
│  Copying system files...                ✓ Done    │
│  Installing kernel...                   ✓ Done    │
│  Configuring memory (ZRAM + swap)...    ✓ Done    │
│  Installing GPU drivers...              ✓ Done    │
│  Generating module blacklist...         ✓ Done    │
│  Pre-seeding Flatpak runtimes...        ⏳ Working  │
│  Applying Spike theme...                ○ Pending  │
│  Configuring GRUB bootloader...         ○ Pending  │
│  Setting up user account...            ○ Pending  │
│  Applying security settings...         ○ Pending  │
│  Final configuration...                ○ Pending  │
│                                                  │
│  [Show details ▼]                                 │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Installation Tasks (In Order)

**1. Partition the drive**

```
├── UEFI: /boot/efi (512MB FAT32) + / (rest, ext4)
└── BIOS: /boot (1GB ext4) + / (rest, ext4)
```

**2. Create swap file**

```
fallocate -l 8G /swapfile    *# (or 4G for SD/USB)
chmod 600 /swapfile
mkswap /swapfile
swapon /swapfile
# Set priority 10
```

**3. Configure ZRAM (if CPU capable)**

```
├── Create /etc/modules-load.d/spike-zram.conf
├── Create /etc/udev/rules.d/99-spike-zram.rules
└── Set zstd, 4G disksize, priority 100
```

**4. Configure swappiness**

```
└── Write to /etc/sysctl.d/99-spike-memory.conf based on storage type
```

**5. Install base system packages**

```
├── Debootstrap or unpack squashfs to /
├── Install KDE standalone apps (Dolphin, Discover, Konsole, etc.)
└── Strip cloud-init, snapd, landscape-common, ubuntu-advantage-tools
```

**6. Install GPU drivers**

```
├── Intel: intel-media-va-driver-non-free, LIBVA_DRIVER_NAME=iHD
├── AMD: mesa-va-drivers, LIBVA_DRIVER_NAME=radeonsi
└── NVIDIA: nouveau loaded, notification file created (no proprietary driver)
```

**7. Install and configure audio**

```
├── PipeWire + WirePlumber
├── Set 44100Hz, stereo, medium resampling
├── Suppress logging
└── Configure Bluetooth conditional startup (udev rule)
```

**8. Install and configure network**

```
├── NetworkManager (already present from base)
├── All Wi-Fi firmware packages installed (Intel, Atheros, Realtek, Broadcom)
├── Select correct firmware for detected hardware
├── Configure ufw (deny incoming, allow outgoing)
└── Configure systemd-resolved
```

**9. Generate module blacklist**

```
├── Scan PCI and USB devices
├── Cross-reference with module database
├── Write /etc/modprobe.d/spike-blacklist.conf
└── Update initramfs
```

**10. Pre-seed Flatpak runtimes**

```
├── Copy KDE runtime from ISO to system
├── Copy GNOME runtime from ISO to system
├── (Spike Plus: copy all common runtimes)
└── Configure Flathub repository
```

**11. Install Spike Shell**

```
├── Copy spike-shell binaries to /usr/bin/
├── Copy spike-config to /usr/lib/spike/
├── Copy branding to /usr/share/spike/
├── Copy user guide to /usr/share/spike/user-guide/
└── Install systemd units for spike-session
```

**12. Apply GRUB2 configuration**

```
├── Install GRUB2 to disk (UEFI or BIOS)
├── Apply Spike GRUB theme
├── Set boot parameters (quiet, splash, zswap.enabled=0, etc.)
├── Configure hidden menu (ESC window, 3-failure fallback)
└── Update GRUB config
```

**13. Apply Plymouth theme**

```
├── Install spike-minimal theme (Standard) or spike-full (Plus)
└── Set as default
```

**14. Apply Spike theme**

```
├── Qt stylesheet to /usr/share/spike/themes/
├── KWin window decoration config
├── Icon theme overrides
└── Wallpaper to /usr/share/spike/wallpapers/
```

**15. Configure security**

```
├── Enable AppArmor (Ubuntu default)
├── Enable ufw (deny incoming)
├── Configure unattended-upgrades (security only, automatic)
└── Disable root login
```

**16. Create user account**

```
├── useradd -m -s /bin/bash [username]
├── Set password
├── Add to sudo group
├── Set hostname
└── Configure auto-login (if selected)
```

**17. Configure CPU governor**

```
├── Standard: powersave
└── Plus: schedutil
```

**18. Write kernel tunables**

```
├── /etc/sysctl.d/99-spike-memory.conf
├── /etc/sysctl.d/99-spike-fs.conf
└── /etc/sysctl.d/99-spike-network.conf
```

**19. Final configuration**

```
├── Set system locale and language
├── Set timezone
├── Configure keyboard layout
├── Enable spike-session service
├── Enable earlyoom service
├── Enable NetworkManager service
├── Enable PipeWire services
└── Generate initramfs (final)
```

**20. Unmount and finalize**

```
├── Unmount target partitions
└── Mark installation complete
```

### Error Handling During Installation

**If a package fails to install:**

```
├── Attempt retry (up to 3 times)
├── If still failing:
│   ├── Log the error
│   ├── Show error dialog: "Spike could not install [package].
│      This may be due to a network issue. The installation will
│      continue, but this component may not work correctly."
│   └── Continue installation (non-fatal unless critical package)
└── Critical packages (kernel, GRUB, spike-shell): abort with error message
```

**If the disk runs out of space:**

```
├── Abort installation
├── Error: "Not enough space on the drive. Spike needs at least
│   128GB of storage. [X] GB available."
└── User can restart with a larger drive
```

**If power is lost during installation:**

```
├── System was not yet modified (pre-format): restart from live ISO
├── System was partially installed: restart from live ISO
│   └── Installer detects partial install and offers fresh start
└── Swap file and partitions may be in inconsistent state (re-format on retry)
```

## Step 10: Reboot

```
┌──────────────────────────────────────────────────┐
│                                                  │
│              ✅ Installation Complete!             │
│                                                  │
│         Spike is ready to use.                    │
│                                                  │
│  Your computer will restart in 10 seconds.       │
│  [Restart now]  [Continue using live USB]        │
│                                                  │
│  Don't forget to:                                 │
│  • Remove the USB drive after restart             │
│  • Keep your backup USB safe (if you made one)   │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Post-Install First Boot

On first boot after installation, the system performs:

```
1. Normal boot sequence (see BOOT-PROCESS.md)
2. SDDM login screen (or auto-login if configured)
3. Spike Shell starts
4. Post-install checks:
   ├── Verify Flatpak runtimes are valid
   ├── Check for pending security updates (if network available)
   ├── Apply any post-install firmware selection (Wi-Fi)
   └── Display post-install notifications:
       ├── NVIDIA hardware detected (if applicable)
       ├── HDD upgrade recommendation (if HDD)
       ├── Security updates available (if any)
       └── "Welcome to Spike" first-run message
5. Desktop ready
```

### Welcome Message

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  🐕 Welcome to Spike!                             │
│                                                  │
│  Your computer is set up and ready to use.        │
│                                                  │
│  Here are a few things to get started:            │
│                                                  │
│  📦 Install apps — Click the Spike button and      │
│     search, or open Discover for the app store    │
│                                                  │
│  🌐 Browse the web — Firefox is ready to go       │
│                                                  │
│  ⚙️  Change settings — Open Settings from the      │
│     Spike menu or click the gear icon             │
│                                                  │
│  ❓ Need help? — Press F1 anywhere in Settings     │
│     for the built-in user guide                  │
│                                                  │
│  [Get started]                                    │
│                                                  │
└──────────────────────────────────────────────────┘
```

## Firmware Selection

### Overview

At install time, the installer detects Wi-Fi, Bluetooth, and other peripheral hardware, then installs the appropriate firmware packages. All major firmware is included on the ISO so that hardware works out of the box without an internet connection.

### Firmware Packages On ISO

The following firmware packages are included on the Spike ISO:

| **Package** | **Covers** | **Notes** |
| :-: | :-: | :-: |
| `linux-firmware` | Intel Wi-Fi (iwlwifi), Atheros (ath), most Realtek (rtl), audio codecs, webcam sensors, misc | Ubuntu's standard firmware blob package — covers most consumer hardware |
| `bcmwl-kernel-source` | Broadcom BCM43xx Wi-Fi (proprietary) | Compiled as DKMS module against installed kernel; requires MOK enrollment on Secure Boot |
| `firmware-realtek` | Additional Realtek network chipset firmware | Supplement to linux-firmware for edge-case Realtek chips |
| `firmware-misc-nonfree` | Various peripheral firmware (sound, sensors) | Pulled in as dependency where needed |
| `intel-microcode` | Intel CPU microcode updates | Security and stability patches for Intel processors |
| `amd64-microcode` | AMD CPU microcode updates | Security and stability patches for AMD processors |

### Wi-Fi Firmware Selection

The installer detects the Wi-Fi adapter vendor and model, then ensures the correct firmware is available:

```
Wi-Fi firmware selection:
1. Detect Wi-Fi adapter:
   ├── lspci -nn | grep -E 'Network|Wireless'
   └── lsusb (for USB Wi-Fi dongles)
2. Parse vendor and device ID
3. Match against firmware database:
   ┌──────────────────────────────────────────────────────────────┐
   │ Vendor    │ Device Family     │ Firmware Package             │
   ├───────────┼───────────────────┼──────────────────────────────┤
   │ Intel     │ Centrino, AC, AX  │ linux-firmware (iwlwifi-*)  │
   │ Atheros   │ ath5k, ath9k,     │ linux-firmware (ath*)        │
   │           │ ath10k, ath11k    │                              │
   │ Realtek  │ rtl8188, rtl8723, │ linux-firmware (rtl*)        │
   │           │ rtl8821, rtl8822  │                              │
   │ Broadcom │ BCM4313, BCM43224,│ bcmwl-kernel-source (DKMS)  │
   │           │ BCM4360, BCM4356  │                              │
   │ MediaTek │ MT7601, MT7610    │ linux-firmware (mt7*)        │
   └──────────────────────────────────────────────────────────────┘
4. Install matched firmware package (if not already installed)
5. Load correct kernel module for detected adapter
6. Verify interface appears (ip link shows wlan0 or wlp*)
```

### Vendor-Specific Details

**Intel Wi-Fi (Primary Target Hardware):**

Intel Wi-Fi adapters are the most common in Celeron-era laptops (Acer, HP, Lenovo, Asus). Firmware is included in `linux-firmware`:

```
├── Firmware files: /lib/firmware/iwlwifi-*.ucode
├── Kernel module:  iwlwifi (in-tree, loaded automatically)
├── Works on first boot — no additional installation needed
├── Common chipsets on target hardware:
│   ├── Intel Wireless-AC 9560 (Celeron N4020 laptops, common)
│   ├── Intel Wireless-N 2230 (older Ivy Bridge laptops)
│   ├── Intel Dual Band Wireless-AC 3165, 3168 (common in budget laptops)
│   └── Intel Wireless-AC 8260, 8265 (business-class laptops)
└── No MOK enrollment required (in-tree module)
```

**Atheros (Qualcomm) Wi-Fi:**

Atheros adapters are common in older laptops and some budget models:

```
├── Firmware files: /lib/firmware/ath*.bin
├── Kernel modules: ath5k, ath9k, ath9k_htc, ath10k, ath10k_pci
├── All modules in-tree, loaded automatically
├── Common chipsets on target hardware:
│   ├── AR9285 (Qualcomm Atheros AR9285, common in 2012-2015 laptops)
│   ├── AR9462 (older Bluetooth+Wi-Fi combo cards)
│   ├── QCA9377 (common in Acer and Lenovo budget laptops)
│   └── QCA6390 (newer Qualcomm Wi-Fi 6, rare on target hardware)
└── No MOK enrollment required (in-tree module)
```

**Realtek Wi-Fi:**

Realtek adapters are common in USB Wi-Fi dongles and some budget laptop internal cards:

```
├── Firmware files: /lib/firmware/rtlwifi/ (most chips)
├── Kernel modules: rtl8188ee, rtl8192ce, rtl8192de, rtl8192se,
│                  rtl8723ae, rtl8723be, rtl8821ae, rtl8822be
├── Most Realtek chips are covered by linux-firmware and in-tree drivers
├── Known edge cases (may require DKMS modules — flag for alpha testing):
│   ├── rtl8812au / rtl8821au → dkms-rtl8812au (USB 3.0 dongles, AC1200)
│   │   ├── Not in Ubuntu repos by default
│   │   ├── Available from PPA or manual build
│   │   └── Requires internet connection at install time (not on ISO)
│   ├── rtl8814au → dkms-rtl8814au (USB 3.0 dongles, AC1900+)
│   │   └── Same situation as rtl8812au
│   └── rtl8188eu → rtl8188eus (older USB N150 dongles)
│       └── Sometimes needs out-of-tree driver
├── Detection during alpha:
│   └── If Realtek USB adapter detected and not working:
│       ├── Notification: "Your Wi-Fi adapter may need a driver
│       │   that isn't included. Connect via Ethernet to install it
│       │   automatically."
│       └── Auto-install dkms module if PPA is available
└── No MOK enrollment for in-tree modules
```

**Broadcom Wi-Fi (Proprietary Driver):**

Broadcom Wi-Fi is the most problematic vendor for Linux. The open-source `brcmsmac` and `brcmfmac` drivers cover some chips, but many common Broadcom adapters require the proprietary `bcmwl-kernel-source` driver:

```
├── Chips requiring proprietary driver:
│   ├── BCM4313, BCM43142, BCM43224, BCM43225
│   ├── BCM4331, BCM4360, BCM4356
│   └── Common in: MacBook Pros (2010-2015), some HP and Dell laptops
├── Package: bcmwl-kernel-source (DKMS, builds against installed kernel)
├── Kernel module: wl (proprietary, out-of-tree)
├── Closed source — cannot fix bugs upstream
├── Secure Boot: REQUIRES MOK enrollment (unsigned kernel module)
└── Installer behavior:
    ├── If Broadcom Wi-Fi detected:
    │   ├── Install bcmwl-kernel-source
    │   ├── Blacklist conflicting open-source drivers:
    │   │   ├── b43
    │   │   ├── b43legacy
    │   │   ├── ssb
    │   │   ├── bcma
    │   │   └── brcmsmac
    │   ├── If Secure Boot is active:
    │   │   ├── Trigger MOK enrollment process
    │   │   ├── Display user-friendly explanation:
    │   │   │   "Your Wi-Fi adapter requires a special driver that
    │   │   │    needs to be trusted by your computer's security
    │   │   │    system. You'll be asked to create a temporary
    │   │   │    password and restart to complete setup."
    │   │   ├── User creates temporary MOK password
    │   │   ├── On next reboot: blue MOK Management screen appears
    │   │   ├── User enrolls key using their password
    │   │   ├── System reboots again
    │   │   └── wl module loads successfully
    │   └── If Secure Boot is disabled:
    │       └── wl module loads directly, no MOK needed
    └── Post-install verification: ip link shows interface
```

**MediaTek Wi-Fi (Less Common):**

```
├── Firmware files: /lib/firmware/mt7*.bin
├── Kernel modules: mt7601u, mt76x0u, mt76x2u, mt76x2e (in-tree)
├── Mostly USB dongles — rare in built-in laptop Wi-Fi
└── Covered by linux-firmware, no special handling needed
```

### MOK Enrollment Flow (Detailed)

When a proprietary kernel module (Broadcom `wl` or NVIDIA proprietary driver) needs to be loaded on a Secure Boot system, the Machine Owner Key (MOK) enrollment process is triggered:

```
1. Installer detects that a proprietary module is needed
   (Broadcom Wi-Fi or NVIDIA GPU — or both)
2. Installer installs the DKMS package:
   ├── DKMS compiles module against installed kernel headers
   ├── Module is built and placed in /lib/modules/[kernel]/updates/dkms/
   └── Module is unsigned (not signed by Canonical's key)
3. MOK enrollment begins:
   ├── mokutil imports the public key for the module
   ├── mokutil requests a password from the user
   │   (installer handles this — user sees GUI prompt, not terminal)
   └── Password is stored temporarily for next-boot verification
4. Installer displays user-friendly message:
   ┌──────────────────────────────────────────────────┐
   │                                                  │
   │  Your computer needs to trust a driver           │
   │                                                  │
   │  Some hardware (Wi-Fi or graphics) requires a   │
   │  driver that isn't signed by your computer's     │
   │  manufacturer. To use it, your computer needs    │
   │  to be told to trust this driver.                │
   │                                                  │
   │  When your computer restarts, you'll see a blue  │
   │  screen asking you to:                            │
   │    1. Choose "Enroll MOK"                         │
   │    2. Choose "Continue"                           │
   │    3. Choose "Yes"                               │
   │    4. Enter the password you created here:        │
   │                                                  │
   │  Password: ┌──────────────┐                      │
   │            │ ••••••••       │                      │
   │            └──────────────┘                      │
   │  Confirm:  ┌──────────────┐                      │
   │            │ •••••••••       │                      │
   │            └──────────────┘                      │
   │                                                  │
   │  [I understand — restart now]                     │
   │                                                  │
   └──────────────────────────────────────────────────┘
5. System reboots
6. Blue MOK Management screen appears (firmware-level, pre-OS):
   ├── "Perform MOK management"
   ├── User selects: Enroll MOK → Continue → Yes → Enter password
   └── Key is enrolled in firmware's trusted key store
7. System reboots again
8. Kernel loads, trusts the enrolled key, loads the proprietary module
9. Wi-Fi (Broadcom) or display (NVIDIA) works
10. Post-enrollment notification:
    "Your driver is now trusted and working. You won't need
     to do this again unless you reinstall Spike."
```

**If MOK enrollment is skipped or fails:**

```
├── The proprietary module will NOT load on Secure Boot systems
├── Broadcom Wi-Fi: No Wi-Fi (user must use Ethernet or disable Secure Boot)
├── NVIDIA: nouveau driver loads instead (basic display, no 3D acceleration)
├── Installer does NOT force Secure Boot off (respects firmware settings)
└── Troubleshooting entry in TROUBLESHOOTING.md guides user through
    either completing MOK enrollment or disabling Secure Boot
```

### Bluetooth Firmware

Bluetooth firmware is handled separately from Wi-Fi firmware, even on combo cards:

```
Bluetooth firmware selection:
├── Intel Bluetooth (most common on target hardware):
│   ├── Firmware: /lib/firmware/intel/ibt-*.sfi / .ddc
│   ├── Module: btusb + btintel (in-tree)
│   ├── Included in linux-firmware
│   └── Works on first boot if Bluetooth hardware present
│
├── Realtek Bluetooth:
│   ├── Firmware: /lib/firmware/rtlbt/ (various .bin files)
│   ├── Module: btusb + btrtl (in-tree)
│   ├── Included in linux-firmware
│   └── Works on first boot
│
├── Broadcom Bluetooth (combo with Broadcom Wi-Fi):
│   ├── Firmware: Requires broadcom-sta-dkms or firmware-b43-installer
│   ├── Module: btusb + btbcm (in-tree)
│   ├── Included in linux-firmware for most chips
│   ├── Some older BCM2046 chips need firmware-b43-installer
│   └── Installer installs firmware-b43-installer if BCM2046 detected
│
├── Qualcomm Atheros Bluetooth:
│   ├── Firmware: /lib/firmware/qca/ (various .bin files)
│   ├── Module: btusb + btqca (in-tree)
│   └── Included in linux-firmware
│
└── USB Bluetooth dongles:
    ├── Most use generic btusb driver
    ├── Firmware included in linux-firmware for common brands
    └── Plug-and-play — no install-time configuration needed
```

**Conditional startup:** BlueZ (Bluetooth service stack) is only started if a Bluetooth adapter is detected at boot. This is handled by a udev rule, not by the installer. See **MULTIMEDIA.md** for Bluetooth audio configuration.

### Other Peripheral Firmware

Firmware for other peripherals is included in `linux-firmware` or installed as dependencies:

```
├── Audio codecs:
│   ├── Intel SST (Sound Solution Technology) — common on Celeron laptops
│   ├── Firmware: /lib/firmware/intel/ (various .bin files)
│   ├── Module: snd-soc-sst-byt-cr-rt5640, snd-hda-intel (in-tree)
│   └── Included in linux-firmware
│
├── Webcams:
│   ├── Most use uvcvideo (generic USB video class driver, in-tree)
│   ├── No firmware needed for standard UVC webcams
│   ├── Rare edge cases: some older webcams need ov519 firmware
│   └── Included in linux-firmware
│
├── SD card readers:
│   ├── Most use rtsx_pci or rtsx_usb (in-tree)
│   ├── No firmware needed
│   └── Some older Ricoh readers need firmware-loading — covered by linux-firmware
│
├── Touchpads:
│   ├── Synaptics: psmouse (in-tree, no firmware)
│   ├── Elan: elan_i2c (in-tree, no firmware)
│   └── Alps: alps (in-tree, no firmware)
│
├── Fingerprint readers:
│   ├── Most require libfprint and fprintd (installed post-install if detected)
│   ├── Some require proprietary firmware (not pre-installed)
│   └── Planned for alpha/beta — not blocking initial release
│
└── CPU microcode:
    ├── intel-microcode: Intel CPU security/stability patches
    ├── amd64-microcode: AMD CPU security/stability patches
    ├── Applied at boot (loaded by initramfs)
    └── Updated automatically via unattended-upgrades (security)
```

### Firmware Validation Post-Install

On first boot, the system verifies that all detected hardware has loaded its firmware:

```
Post-install firmware check (runs during spike-session startup):
1. Enumerate PCI and USB devices
2. For each network adapter:
   ├── Check that interface exists (ip link)
   ├── If Wi-Fi adapter present but no wlan interface:
   │   ├── Check dmesg for firmware loading errors
   │   ├── If firmware missing: log error, create notification
   │   │   "Your Wi-Fi adapter couldn't start. This may be a firmware
   │   │    issue. Connect via Ethernet and check for updates."
   │   └── If module missing: log error, create notification
   └── If interface exists: mark as OK
3. For each Bluetooth adapter (if present):
   ├── Check that hci0 exists (hciconfig)
   ├── If not present: check dmesg for firmware errors
   └── If firmware missing: log, notify user
4. For audio:
   ├── Check that PipeWire sees audio devices
   ├── If no audio devices: check dmesg for codec errors
   └── If codec firmware missing: log, notify user
5. Summary written to /var/log/spike/firmware-check.log
6. Any failures generate user-friendly notifications
7. No automatic remediation (user must connect via alternate method
   and update firmware or report the issue)
```

This validation ensures that if firmware failed to load silently during boot, the user is informed rather than discovering the problem later when trying to use Wi-Fi or audio.

### Firmware Updates

Firmware packages receive updates through Ubuntu's standard security channel:

```
├── linux-firmware updates: arrive via apt (security and bugfix updates)
├── Applied automatically by unattended-upgrades (if security-related)
├── Non-security firmware updates: notified via Discover
├── bcmwl-kernel-source: rebuilt by DKMS on kernel updates
├── Microcode updates: applied at boot, updated via apt
└── No separate firmware update tool (fwupd available but not emphasized)
    └── fwupd is installed (Ubuntu default) for LVFS-supported hardware
        (Dell, Lenovo, HP system firmware updates) but most target
        hardware predates LVFS adoption
```

## Spike Rescue Tool

### Purpose

When a Spike installation breaks (failed update, corrupted filesystem, misconfiguration), the user needs a way to recover their personal files before reinstalling. Spike Rescue is a tool that runs from the live ISO to extract user data from a broken system.

### How It Works

```
User boots from Spike ISO (same ISO used for installation)
    │
    ▼
Live environment starts
    │
    ▼
Desktop icon: "Install Spike" (normal install)
Desktop icon: "Rescue My Files" (rescue tool)
    │
    ▼
User clicks "Rescue My Files"
    │
    ▼
Spike Rescue tool starts
```

### Rescue Flow

```
1. Scan for installed operating systems
   ├── Check all drives for ext4/Linux partitions
   ├── Check for NTFS/Windows partitions
   └── Check for HFS+/macOS partitions
2. If Spike installation detected:
   ├── Show: "Spike installation detected on /dev/sda2"
   ├── Offer: "Recover files from this installation"
   └── Also offer: "Fresh install and restore my data"
3. If non-Spike Linux detected:
   ├── Show: "Linux system detected on /dev/sda2"
   └── Offer: "Recover files from this system"
4. If Windows detected:
   ├── Show: "Windows detected on /dev/sda2"
   └── Offer: "Recover files from Windows"
5. If multiple systems detected:
   ├── List all detected systems
   └── User selects which to recover from
```

### Recovery Process

```
Spike Rescue Recovery:
1. Mount source partition READ-ONLY
   ├── mount -o ro /dev/sda2 /mnt/source
   └── Read-only prevents further damage to the filesystem
2. Scan for user data
   ├── /mnt/source/home/[username]/{Documents, Photos, Videos, Music, Downloads, Desktop, Pictures}
   └── Report file counts and total size
3. Prompt for USB drive
   ├── "Connect a USB drive to save your recovered files"
   ├── Wait for USB insertion
   └── Verify sufficient free space
4. Copy files to USB
   ├── Destination: /run/media/usb/SpikeBackup/
   ├── Preserve folder structure
   ├── Checksum verification (SHA256)
   └── Progress bar with file count
5. Handle partial corruption
   ├── If a file cannot be read: log it, continue
   ├── If a directory cannot be traversed: log it, continue
   └── Summary: "1,528 files recovered. 3 files could not be read."
6. Unmount source partition
   └── umount /mnt/source
7. Completion
   ├── "Your files have been saved to USB."
   ├── "You can now reinstall Spike and restore your data."
   └── "Safe to remove USB drive"
```

### Reinstall With Restore

If the installer detects an existing Spike installation on the target drive:

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Spike detected on this computer                  │
│                                                  │
│  An existing Spike installation was found on       │
│  this drive. What would you like to do?           │
│                                                  │
│  ● Fresh install                                  │
│    Erase everything and install Spike from        │
│    scratch.                                       │
│                                                  │
│  ○ Fresh install and restore my data              │
│    Erase everything, install Spike, then          │
│    restore your files from USB.                   │
│                                                  │
│  [Back]  [Continue]                               │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Restore Process

If the user selects "Fresh install and restore my data":

```
1. Check for SpikeBackup folder on connected USB
   ├── If found: proceed with installation + scheduled restore
   └── If not found:
       ├── "No backup was found on the USB drive.
       │   You can still install Spike without restoring files."
       └── Offer: "Fresh install" or "Try a different USB"
2. Proceed with normal installation (Steps 1-9)
   └── (Data backup step is skipped — already have backup)
3. After installation completes, before reboot:
   ├── Mount USB drive
   ├── Copy SpikeBackup/ to /home/[new_username]/
   │   ├── Preserve folder structure
   │   ├── chown -R [new_user]:[new_user] /home/[new_username]/
   │   └── Checksum verification
   └── Unmount USB
4. Reboot to restored system
   └── User finds their files in their home directory
```

### What Gets Restored

**Restored from SpikeBackup/:**

```
├── Documents/
├── Photos/ (Pictures/ on Linux)
├── Videos/
├── Music/
├── Downloads/
├── Desktop/
└── (any other folders present in the backup)
```

**NOT restored (system-specific, cannot transfer):**

```
├── System settings (theme, network configs, etc.)
├── Application configurations (.config/, .local/)
├── Browser history/cache
├── Passwords/keys
└── Installed applications (must be reinstalled via Discover)
```

This is intentional — restoring system configurations from a broken installation could reintroduce the problem that caused the breakage. Only personal files (documents, photos, media) are restored.

## Installer Technical Architecture

### Implementation

```
spike-installer/
├── ui/              → Qt Widgets frontend (the screens the user sees)
├── detect/          → Hardware detection modules
│   ├── cpu.cpp      → nproc + bogomips
│   ├── storage.cpp  → lsblk + ROTA + eMMC/SD/USB detection
│   ├── gpu.cpp      → lspci vendor detection
│   ├── network.cpp  → Wi-Fi adapter detection
│   ├── bluetooth.cpp → BT adapter detection
│   └── modem.cpp    → Mobile broadband detection
├── partition/       → Partitioning and formatting logic
├── backup/          → Data backup and verification
└── restore/         → Data restore after reinstall
```

### Detection Modules

**`detect/cpu.cpp`:**

```
├── Read /proc/cpuinfo
├── Extract: model name, cores, bogomips
├── Classify: capable or low-end
└── Return struct: { model, cores, bogomips, classification }
```

**`detect/storage.cpp`:**

```
├── Run lsblk -d -o NAME,ROTA,SIZE,MODEL,VENDOR
├── For each block device:
│   ├── Check /sys/block/[dev]/queue/rotational
│   ├── Check device path for eMMC (/dev/mmcblk*)
│   ├── Check device path for SD card
│   ├── Check device path for USB (via udev ID_BUS)
│   └── Check USB version (via lsusb -v or /sys)
├── Classify: SSD, HDD, NVMe, SD, USB3, USB2, eMMC
├── Reject eMMC and USB 2.0 (block installation)
└── Return struct: { device, type, size_gb, model, rotational, acceptable }
```

**`detect/gpu.cpp`:**

```
├── Run lspci -nn | grep -E 'VGA|3D|Display'
├── Parse vendor ID:
│   ├── 0x8086 → Intel
│   ├── 0x1002 → AMD
│   └── 0x10DE → NVIDIA
├── Parse device name
└── Return struct: { vendor, model, driver_to_install, va_api_driver }
```

**`detect/network.cpp`:**

```
├── Check for Wi-Fi adapter via iw dev
├── Check for Ethernet via ip link
├── Return struct: { has_wifi, wifi_interface, has_ethernet, ethernet_interface }
```

**`detect/bluetooth.cpp`:**

```
├── Check for Bluetooth adapter via hciconfig or rfkill
├── Return struct: { has_bluetooth }
```

**`detect/modem.cpp`:**

```
├── Check for mobile broadband via mmcli or lsusb
├── Return struct: { has_modem }
```

### Partitioning Logic

**UEFI systems:**

```
parted /dev/sda mklabel gpt
parted /dev/sda mkpart esp fat32 1MiB 513MiB
parted /dev/sda set 1 esp on
parted /dev/sda mkpart primary ext4 513MiB 100%
mkfs.fat -F32 /dev/sda1
mkfs.ext4 /dev/sda2
mount /dev/sda2 /mnt
mkdir -p /mnt/boot/efi
mount /dev/sda1 /mnt/boot/efi
```

**BIOS systems:**

```
parted /dev/sda mklabel msdos
parted /dev/sda mkpart primary ext4 1MiB 1025MiB
parted /dev/sda set 1 boot on
parted /dev/sda mkpart primary ext4 1025MiB 100%
mkfs.ext4 /dev/sda1
mkfs.ext4 /dev/sda2
mount /dev/sda2 /mnt
mkdir -p /mnt/boot
mount /dev/sda1 /mnt/boot
```

### Firmware Selection

```
# After detecting Wi-Fi hardware, select appropriate firmware:
# All firmware packages are installed, but only the correct module loads
# Intel Wi-Fi:
# Package: linux-firmware (already includes iwlwifi)
# No additional action needed
# Atheros:
# Package: linux-firmware (already includes ath)
# No additional action needed
# Realtek:
# Package: linux-firmware (includes rtl drivers)
# Some Realtek chips may need dkms-rtl8812au or similar — check during alpha
# Broadcom (proprietary):
# Package: bcmwl-kernel-source
# Requires MOK enrollment on Secure Boot systems
if lspci | grep -q "Broadcom.*Network"; then
    apt install -y bcmwl-kernel-source
    *# If Secure Boot active: trigger MOK enrollment
fi
```

## What This Document Does Not Cover

- **Memory configuration details:** See **MEMORY.md** (ZRAM, swap, swappiness, earlyoom) 

- **Kernel parameters and module blacklisting:** See **KERNEL.md** 

- **Boot sequence and GRUB configuration:** See **BOOT-PROCESS.md** 

- **Desktop shell components:** See **DESKTOP.md** (panel, applets, settings) 

- **Network configuration post-install:** See **NETWORKING.md** 

- **Audio and video configuration:** See **MULTIMEDIA.md** 

- **Security policies:** See **SECURITY.md** 

- **Variant differences:** See **VARIANT-DIFFERENCES.md** 

🐕 BigRangaTech

