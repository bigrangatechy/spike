Spike FAQ
Purpose

This document answers the most common questions users, potential users, and community members ask about Spike. It is written in plain language — no prior Linux knowledge assumed.

Questions are grouped by topic. If a question has a longer answer elsewhere, a cross-reference is provided.
Table Of Contents

1. What Is Spike?
2. Who Is Spike For?
3. Hardware Questions
4. Installation Questions
5. Using Spike (Everyday Questions)
6. Software And Apps
7. Comparisons With Other Operating Systems
8. Privacy And Security
9. Updates And Maintenance
10. Troubleshooting Quick Answers
11. Project And Community
12. Developer Questions

1. What Is Spike?
What Is Spike Linux?

Spike is a Linux-based operating system designed for laptops that are a few years old — specifically machines with dual-core Celeron processors and 4GB of RAM. It is built to be genuinely usable on that hardware, not just "technically bootable."

Spike runs a custom desktop environment called Spike Shell, built with Qt6 and Wayland. It is designed so that users never need to open a terminal or edit configuration files. Everything is controlled through a graphical settings panel.
Who Makes Spike?

Spike is developed by BigRangaTech, currently as a solo-developer open-source project. The project follows a "docs-first" methodology — all design decisions are documented before any code is written. Spike is licensed under GPLv2+.
Is Spike A Distro Or A Desktop Environment?

Both. Spike is a complete operating system (an Ubuntu Server LTS base with custom packages, installer, and configuration system) AND a desktop environment (Spike Shell — the panel, launcher, settings, and window management layer). They are designed together as one integrated product.
Why Is It Called Spike?

Spike's mascot is a dog named Spike. The name reflects the project's ethos: loyal, dependable, built to serve. See BRANDING.md for the full brand identity.
Is Spike Based On Ubuntu?

Yes. Spike uses Ubuntu Server LTS as its base. The Long-Term Support release provides a stable kernel, package repository, and security updates. Spike strips out Canonical's telemetry, snap, cloud tools, and unnecessary services, then adds its own desktop shell, installer, and configuration system.
What Is The Difference Between Spike Standard And Spike Plus?

Spike Standard is the default variant, tuned for minimum resource usage. It disables compositing effects (blur, transparency, shadows), uses software rendering, caps ZRAM at 4GB, and uses the powersave CPU governor. It is designed for 4GB RAM machines.

Spike Plus is the enhanced variant, tuned for machines with 8GB+ RAM and capable GPUs. It enables compositing effects, uses OpenGL rendering, uncaps ZRAM, uses the schedutil CPU governor, and pre-seeds all Flatpak runtimes. It is designed for machines that have more headroom.

The variant is auto-recommended at install time based on hardware detection, but the user makes the final choice. See VARIANT-DIFFERENCES.md.
2. Who Is Spike For?
Who Is The Target User?

Spike is for someone who has an older laptop (roughly 2019-era Celeron) that still works physically but has become slow or unsupported by its original operating system (Windows). That person wants the laptop to be useful again — for web browsing, email, documents, media — without needing to learn Linux or use a terminal.
Is Spike For Linux Enthusiasts?

Spike is built for non-technical users first. Linux enthusiasts are welcome to use it, contribute to it, and test it — but design decisions prioritize the beginner experience. If a feature confuses a non-technical user, it doesn't ship.
Can I Use Spike As My Daily Driver?

That is the goal. At alpha and beta stage, Spike is not yet production-ready. Once 1.0 is released, it is intended as a daily-driver operating system for the target hardware.
Is Spike Good For Gaming?

Spike is not designed for gaming. It targets Celeron-class hardware with integrated graphics. Lightweight games (2D, indie, browser-based) will work. Modern 3D games will not. Gaming is not a design goal and is not a factor in any performance or hardware decision.
Can Kids Or Elderly Users Use Spike?

Yes — Spike's no-terminal, no-config-file design is well-suited for users who just need a simple, reliable computer. The launcher is straightforward, settings are visual, and the disaster recovery system protects against mistakes.
Is Spike Suitable For Enterprise / Business Use?

Spike is designed for personal laptops, not enterprise deployments. It lacks centralized management (MDM, group policy, domain join), network authentication (LDAP, Active Directory), and fleet deployment tools. Businesses with simple needs could use it, but it is not designed for managed environments.
3. Hardware Questions
What Hardware Does Spike Support?

Spike targets:

Tier 1 (Primary target — must be daily-driver usable):
├── Intel Celeron N4020 (dual-core, 2 threads)
├── 4GB RAM
├── Intel UHD Graphics 600
├── 128GB+ storage (SSD recommended, HDD supported)
└── Typical 2019 budget laptop

Tier 2 (Worst case — must boot and function):
├── AMD A4 (Jaguar architecture, pre-2016)
├── 4GB RAM
├── AMD Radeon HD (integrated)
├── 500GB SATA HDD
└── Lenovo ThinkPad with AMD A4

Tier 3 (Development machine — regression checking only):
├── Intel Xeon (ThinkPad P50)
├── 32GB RAM
├── NVIDIA Quadro M2000M
├── 512GB NVMe
└── Not representative of target users

See HARDWARE.md for full hardware specifications and detection logic.
Will Spike Work On My Laptop?

If your laptop has an Intel Celeron processor (N4000, N4020, N4100 series) with 4GB RAM and at least 128GB storage, Spike is designed for it. If your laptop is significantly more powerful (Core i3/i5/i7, 8GB+ RAM), Spike will run but you may want to choose Spike Plus at install time.

If your laptop is older than 2016 or has less than 4GB RAM, Spike may boot but performance is not guaranteed. The minimum is 4GB RAM — Spike will refuse to install with less.
Does Spike Support ARM?

No. Spike is x86-64 only. There are no current plans for ARM support. If demand exists in the future, it will be evaluated post-1.0.
Can I Install Spike On A Desktop PC?

Yes, if it meets the hardware requirements. Spike is designed for laptops (power management, lid switch, battery applet), but it works on desktops. The battery and lid-close settings simply won't appear if the hardware isn't present.
Does Spike Support Touchscreens?

Not currently. Touchscreen support is a potential post-1.0 feature. If your laptop has a touchscreen, it may work as a basic pointing device (mouse emulation) but multi-touch gestures are not implemented.
Can I Dual-Boot Spike With Windows?

No. Spike does not support dual-boot installations. The installer erases the entire target drive. If you need your Windows installation, use a separate laptop or drive. Data backup is offered during installation.
What About NVMe SSDs?

NVMe SSDs are fully supported. If your laptop has an NVMe slot, Spike will detect and use it. NVMe is faster than SATA SSD, so boot times and app launches will be quicker.
Does Spike Work With External Monitors?

Yes. HDMI, DisplayPort, and VGA outputs are supported. External monitors can be configured in Settings → Display (extend, mirror, or single display). See TROUBLESHOOTING.md if external monitors aren't detected.
What Wi-Fi Cards Are Supported?

All major Wi-Fi vendors are supported with firmware pre-installed on the ISO:

Intel: iwlwifi (most common in Celeron laptops)
Atheros: ath9k, ath10k, ath11k
Realtek: rtl8723de, rtl8821ce, rtl8822bu, and others
Broadcom: wl (proprietary, requires MOK enrollment if Secure Boot is on)

See NETWORKING.md for the full driver and firmware list.
Does Spike Support 32-bit (x86) Processors?

No. Spike is 64-bit (x86-64) only. 32-bit processors are not supported and will not be added.
4. Installation Questions
How Do I Install Spike?

    Download the Spike ISO from spike.bigrangatech.com
    Write it to a USB 3.0+ flash drive (minimum 8GB) using a tool like Balena Etcher or dd
    Plug the USB into the laptop
    Power on, enter the boot menu (usually F12), select the USB
    The live environment boots — click "Install Spike" on the desktop
    Follow the 10-step installer (welcome, timezone, Wi-Fi, account, variant, backup, storage, install)
    Reboot to your new Spike desktop

See INSTALLER.md for the full installer specification.
How Big Is The ISO?

The ISO is approximately 3.1GB. It must be written to a USB 3.0+ drive (USB 2.0 is not supported). See PERFORMANCE-BASELINES.md for ISO size targets.
Can I Use A DVD?

No. The ISO is too large for a standard DVD (4.7GB capacity is sufficient, but Spike targets USB-only for simplicity and speed). DVDs are also impractically slow for live environments.
How Long Does Installation Take?

SSD: ~8-12 minutes (excluding data backup)
HDD: ~12-18 minutes (excluding data backup)
Data backup: Varies (depends on amount of data)

See PERFORMANCE-BASELINES.md for installation time estimates.
Will Spike Erase My Files?

Yes — the installer erases the entire target drive. That is why Spike offers a data backup step during installation. The installer scans for user data (Documents, Photos, etc.) on the existing operating system, copies it to a USB drive with SHA256 verification, and restores it after installation.

If you skip the backup step, your files will be lost. There is no way to recover them after a full installation.
Do I Need An Internet Connection To Install?

An internet connection is strongly recommended but not strictly required. Wi-Fi configuration is offered during installation. If you skip Wi-Fi, the installer can still proceed — packages are pre-loaded on the ISO. However, some post-install features (updates, Flatpak app installation) require internet.
Can I Install Without A USB Drive?

No. Spike installs from a USB drive only. Network installation (PXE) is not supported. SD card installation is technically possible if the SD card is USB 3.0+ speed, but it is not recommended (slow and unreliable).
What If The Installer Fails?

Common installer failures and solutions are covered in TROUBLESHOOTING.md, Section 2 (Installation Problems). If the installer crashes or freezes:

    Hard reset (hold power button 5 seconds)
    Boot from USB again
    Try reinstalling
    If it fails at the same point: check storage health (the drive may be failing)
    If data backup was completed: your files are safe on the USB drive

5. Using Spike (Everyday Questions)
Do I Ever Need To Use The Terminal?

No. Spike is designed so that every user-facing task has a graphical interface. The terminal (Konsole) is installed for developers and advanced users, but a regular user should never need to open it.

If you encounter a problem that requires terminal commands to fix, that is considered a bug. Report it.
How Do I Install Applications?

Open Discover (the software center) from the launcher. Browse or search for applications, click "Install." Applications are installed as Flatpaks from Flathub. See the user guide for detailed instructions.
How Do I Change My Wallpaper?

Settings → Appearance → Wallpaper. Choose from the included wallpapers or browse to your own image.
How Do I Connect To Wi-Fi?

Click the network icon in the bottom-right panel area. Select your Wi-Fi network from the list. Enter the password. You're connected. See NETWORKING.md for full network management.
How Do I Change Screen Brightness?

Use the brightness slider in the panel (bottom-right area) or the function keys on your keyboard (Fn + brightness up/down). See POWER-MANAGEMENT.md for brightness control details.
How Do I Take A Screenshot?

Press the Print Screen key. Spectacle (screenshot tool) opens with options for full screen, active window, or region. Screenshots are saved to ~/Pictures/Screenshots/.
How Do I Shut Down Or Restart?

Click the session menu icon in the panel (bottom-right). Select "Shut Down" or "Restart." You can also shut down from the launcher — search "Shut Down."
How Do I Lock The Screen?

Press Super + L (Windows key + L). The screen locks immediately. You can also lock from the session menu in the panel.
Can I Change The Language?

Yes. Settings → Region & Language. Spike supports 11 languages. Changing the language requires a logout/login to take effect. See the user guide for the full language list.
How Do I Adjust The Volume?

Click the speaker icon in the panel. Drag the volume slider. You can also use keyboard volume keys. Microphone volume is in Settings → Sound → Input.
How Do I Set Up A Printer?

Settings → Printers (if available). Click "Add Printer." Spike detects network and USB printers automatically. See TROUBLESHOOTING.md if printers aren't detected.
How Do I Connect Bluetooth Headphones?

Click the Bluetooth icon in the panel (if present). Make sure your headphones are in pairing mode. Select them from the list and click "Pair." See NETWORKING.md for Bluetooth details and MULTIMEDIA.md for audio codec information.
What Happens When I Close The Laptop Lid?

By default, the laptop suspends (sleeps). You can change this in Settings → Power → Lid Close Action. Options: Suspend, Hibernate (if available), Do Nothing, Lock Screen. See POWER-MANAGEMENT.md.
How Do I Find My Files?

Open Dolphin (file manager) from the launcher or panel. Your files are organized in the usual folders: Documents, Downloads, Pictures, Music, Videos. See the user guide for file management basics.
Can I Use Multiple Desktops / Workspaces?

Yes. Spike supports virtual desktops (workspaces). Use the workspace switcher in the panel or the keyboard shortcut (Super + Page Up/Down). KWin manages workspace behavior. See DESKTOP.md for workspace configuration.
6. Software And Apps
What Applications Come Pre-Installed?

Internet:
├── Firefox (Flatpak, with VA-API and privacy configuration)

Office:
├── LibreOffice Writer, Calc, Impress (Flatpak)

Media:
├── VLC (Flatpak)
├── Celluloid (Flatpak, optional lightweight player)

Utilities:
├── Dolphin (file manager)
├── Konsole (terminal)
├── Kate (text editor)
├── Ark (archive tool)
├── Spectacle (screenshot)
├── KCalc (calculator)

System:
├── Discover (software center)
├── Settings panel (Spike custom pages + KDE KCM modules)

Can I Install Other Applications?

Yes. Open Discover and search for what you need. Applications are installed as Flatpaks from Flathub. Most popular Linux applications are available: Spotify, Discord, Telegram, Steam, GIMP, OBS, and more.
Can I Install .deb Packages?

Technically yes (via dpkg or apt), but it is discouraged. Spike's application model is Flatpak-based. Installing .deb packages bypasses the sandbox and can create dependency conflicts. If an application is only available as .deb, it will still work — but updates and support are the user's responsibility.
Can I Install Snap Packages?

No. Snap is completely removed from Spike. The snapd daemon is not installed. Snap packages cannot be installed. This is intentional — see DESIGN-DECISIONS.md for the rationale.
Why Is Firefox Installed As A Flatpak Instead Of A System Package?

Flatpak sandboxing isolates Firefox from the rest of the system. This improves security (Firefox can't access system files without permission) and simplifies updates (Flatpak updates are self-contained). The downside is slightly longer first-launch time, mitigated by pre-seeded runtimes on the ISO.
Does Spike Come With An Email Client?

Not pre-installed. Thunderbird is available from Discover (Flathub) if you need a desktop email client. Firefox can also handle webmail (Gmail, Outlook, etc.).
Can I Use Microsoft Office?

Microsoft Office is not available for Linux. LibreOffice is installed as the office suite. It can open and save Microsoft Office formats (.docx, .xlsx, .pptx). For full compatibility, you can use Microsoft Office Online (web version) in Firefox.
Can I Use Steam?

Yes. Steam is available from Discover (Flathub). However, Spike targets Celeron-class hardware with integrated graphics — gaming performance will be limited. Lightweight and 2D games will work. Modern 3D games will not.
How Do Updates Work?

Security updates install automatically (every 6 hours via spike-update.timer). Non-security updates show a notification with an option to install. Flatpak applications update through Discover. Kernel updates are held back until the next reboot — the previous kernel is retained for fallback. See SECURITY.md for update details.
7. Comparisons With Other Operating Systems
How Is Spike Different From Windows?

Windows                              Spike
──────────────────────────────────────────────────────
Telemetry and data collection        No data collection at all
Forced updates                       Security updates automatic, user controls timing
Requires modern hardware             Designed for 2019 Celeron hardware
License costs money                  Free and open source (GPLv2+)
Terminal needed for some fixes       Terminal never needed for user tasks
Dual-boot supported                  No dual-boot
Growing system requirements          Minimal, stable requirements
Background services from vendor      Only essential services running

How Is Spike Different From ChromeOS Flex?

ChromeOS Flex is Google's solution for reviving old laptops. Key differences:

ChromeOS Flex                        Spike
──────────────────────────────────────────────────────
Cloud-centric (requires Google account)    Fully local (no account required)
Web apps (PWA) primary               Native + web apps
Limited offline use                  Full offline capability
Google telemetry                      Zero telemetry
Closed source                         Open source (GPLv2+)
Limited customization                 Full customization via Settings
Android apps (some models)            No Android app support
Minimal local storage management      Full local file management

How Is Spike Different From Ubuntu / Linux Mint / Fedora?

Spike shares a foundation with Ubuntu (LTS base, apt packages). Key differences:

Ubuntu / Mint / Fedora               Spike
──────────────────────────────────────────────────────
GNOME/Cinnamon/KDE desktop           Spike Shell (custom Qt6 desktop)
General-purpose Linux                Purpose-built for old Celeron hardware
Terminal skills expected             Terminal never needed
Config file editing common           Config files managed by spike-config
Some telemetry (Ubuntu)              Zero telemetry
Snap (Ubuntu) or Flatpak             Flatpak only
Manual memory tuning                 Automatic memory management at install

How Is Spike Different From Raspberry Pi OS?

Raspberry Pi OS is designed for ARM-based Raspberry Pi hardware. Spike is x86-64 only. They target completely different hardware categories. See HARDWARE.md for Spike's hardware scope.
Is Spike A Replacement For macOS?

No. Spike does not run on Apple hardware (it is x86-64, not ARM Apple Silicon). If you have a Mac, Spike is not for you. If you have a former Windows laptop, Spike may be for you.
Can Spike Run Windows Applications?

Not natively. WINE is not pre-installed. Some Windows applications can run under WINE (available from Discover or as a Flatpak), but compatibility is not guaranteed. The recommended approach is to find a native Linux alternative in Discover.
8. Privacy And Security
Does Spike Collect Any Data?

No. Zero. Nothing.

No telemetry, no analytics, no crash reports, no usage statistics, no hardware surveys. This is absolute and non-negotiable. See PRIVACY.md for the full privacy policy.
Does Spike Phone Home?

Spike makes exactly five types of outbound network connections: apt package list refresh, Flatpak metadata check, NTP time sync, connectivity check, and captive portal detection. None of these send user data. See PRIVACY.md for the complete list.
Is Spike Secure?

Spike includes a baseline security configuration: firewall (ufw) enabled by default, AppArmor enforcing, root account locked, no SSH server, no network services exposed, Flatpak application sandboxing, and xdg-desktop-portal permission system. See SECURITY.md for the full security model.
Can Spike Get Viruses?

Linux is not immune to malware, but the attack surface is significantly smaller than Windows. Flatpak sandboxing limits what applications can access. The firewall blocks incoming connections. User accounts are non-root by default. No automatic code execution from email or web pages (beyond what Firefox allows).

That said, Spike is not marketed as "immune to viruses." Users should still exercise caution: don't install untrusted software, don't run commands from unknown sources, keep the system updated.
Does Spike Use Disk Encryption?

Not currently. Disk encryption (LUKS) is a potential post-1.0 feature. It must integrate with the disaster recovery system before it can be added — an encrypted disk that can't be rescued is unacceptable. See DESIGN-DECISIONS.md.
Can Someone Access My Files If They Steal My Laptop?

Without disk encryption, yes. Physical access to an unencrypted Linux laptop means someone with technical skills can access files on the drive. This is true of any unencrypted operating system. If this is a concern, wait for disk encryption support or use a third-party full-disk encryption tool (not officially supported).
Does Spike Have Antivirus?

No antivirus software is pre-installed. Linux antivirus software is generally unnecessary for desktop use with proper sandboxing, firewall, and user hygiene. If you want to scan files (e.g., before sharing with Windows users), ClamAV is available from Discover.
9. Updates And Maintenance
How Does Spike Update?

Security updates (apt + Flatpak):
├── Checked every 6 hours (spike-update.timer)
├── Conditions: network available, system idle, enough memory
├── Installed automatically (no user interaction needed)
├── Notification: "Security updates installed"
└── Kernel updates: held until reboot, notification to restart

Non-security updates:
├── Notification: "Updates available"
├── User clicks "Install" in Discover or Settings → Software Updates
└── User controls timing

Flatpak app updates:
├── Managed by Discover
├── User-initiated or "Update all" in Discover
└── Notification when updates are available

See SECURITY.md for the full update model.
Do I Need To Reboot After Updates?

Security updates (non-kernel):     No reboot required
Kernel updates:                    Yes — reboot to use new kernel
                                  (previous kernel retained for fallback)
Flatpak updates:                   No reboot required
                                  (restart the updated app)
Spike Shell updates:               Logout/login recommended

How Long Are Updates Provided?

Spike follows Ubuntu LTS support timelines. The base system receives security updates for the LTS lifetime (typically 5 years). Flatpak applications are updated by their respective developers via Flathub — update availability depends on the app maintainer.

See END-OF-LIFE-POLICY.md for the full support timeline.
What Happens When A New Spike Version Is Released?

Major version releases (e.g., 2.0.0) may require a fresh installation — they involve breaking changes. Point releases (1.x.0) and patch releases (1.x.y) are delivered as normal updates and do not require reinstallation.

Migration guides are provided for major version changes. See MIGRATION-GUIDE.md (to be written).
Can Updates Break My System?

Kernel updates are the most common cause of boot issues. Spike mitigates this by retaining the previous kernel — if the new kernel fails, the boot failure counter triggers the GRUB menu and the user can select the previous kernel. See BOOT-PROCESS.md and DISASTER-RECOVERY.md.

Package updates can rarely cause dependency issues. If apt encounters a problem, it does not apply the broken update. The update is retried on the next cycle.
What Should I Do If An Update Breaks Something?

    Reboot — many issues resolve on restart
    If the system won't boot: press ESC at GRUB, select previous kernel
    If that works: report the kernel regression
    If neither kernel boots: boot from USB, use Spike Rescue to recover files
    See TROUBLESHOOTING.md for specific problem resolution

How Do I Clean Up Disk Space?

Settings → Advanced → Storage shows what's using space. You can:

    Empty trash
    Clear caches (thumbnail, package, Flatpak)
    Remove unused Flatpak runtimes
    Uninstall applications you don't use

See TROUBLESHOOTING.md, Section 10 (Disk Is Full) for details.
10. Troubleshooting Quick Answers
My Laptop Won't Boot

Press ESC during boot to see the GRUB menu. Select "previous kernel." If that boots, the recent kernel update is the problem. If nothing boots, use the Spike USB to rescue your files. See TROUBLESHOOTING.md, Section 1.
My Wi-Fi Doesn't Work

Check the panel network applet — is Wi-Fi on? Check for a physical Wi-Fi switch on the laptop. Try toggling airplane mode on and off. Restart the system. See TROUBLESHOOTING.md, Section 5.
No Sound

Check the volume applet — is it muted? Is the correct output device selected in Settings → Sound? Try restarting the system. See TROUBLESHOOTING.md, Section 4.
The Screen Is Black After Boot

Press ESC to see boot messages. If Plymouth (boot splash) is the problem, change boot splash to "Minimal" in Settings → Advanced → Boot. See TROUBLESHOOTING.md, Section 1.
I Forgot My Password

Reboot, press ESC at GRUB, select "recovery mode." Type passwd [your-username] to set a new password. Reboot. See TROUBLESHOOTING.md, Section 11.
The System Is Slow

Close applications you're not using (especially Firefox tabs). Check memory in Settings → Memory. If on an HDD, consider upgrading to an SSD. See TROUBLESHOOTING.md, Section 6.
An App Crashed

Reopen it from the launcher. If it crashes repeatedly, try reinstalling from Discover. Check for system updates. See TROUBLESHOOTING.md, Section 7.
My Battery Drains Fast

Check power profile in Settings → Power. Lower screen brightness. Turn off Bluetooth if not using it. Check battery health — old batteries drain faster. See TROUBLESHOOTING.md, Section 9.
I Can't Find My Files

Check the Trash in Dolphin. If you backed up before installation, your files are on the USB drive (SpikeBackup/ folder). If you didn't back up and the drive was erased, files cannot be recovered. See TROUBLESHOOTING.md, Section 10.
Where Do I Report Bugs?

Settings → Advanced → "Report a Problem" — this opens the GitLab issue tracker with system info pre-filled. You can also visit git.bigrangatech.com/spike/spike/-/issues directly.
11. Project And Community
Is Spike Free?

Yes. Spike is free and open source software, licensed under GPLv2+. There is no paid tier, no premium version, no in-app purchases. Spike Standard and Spike Plus are both free — the variant is a hardware optimization choice, not a payment tier.
How Can I Contribute?

During pre-alpha (now):
├── Read the documentation
├── Report documentation errors
├── Suggest features (GitLab issues)
├── Register your hardware (hardware survey)
└── Discuss in the forum / Matrix

During alpha:
├── Test on your hardware
├── Report bugs
├── Submit translations
└── Submit code patches (case-by-case)

During beta and beyond:
├── Submit code (DCO required, code review required)
├── Translate (all 11 languages)
├── Test and report
└── Help other users in the forum

See CONTRIBUTING.md for the full contribution guide.
Where Is The Source Code?

Primary repository: git.bigrangatech.com/spike/spike (GitLab CE, self-hosted) GitHub mirror: github.com/bigrangatech/spike (read-only)

The GitHub mirror is for visibility only. All issues, merge requests, and CI happen on GitLab.
Is There A Forum Or Chat?

Yes. The Spike community uses a self-hosted forum (forum.bigrangatech.com) and a Matrix room (#spike:matrix.org). Links are available on the website (spike.bigrangatech.com/community).
Can I Fork Spike?

Yes — it's GPLv2+ open source. You can fork, modify, and redistribute. However, the "Spike" name and branding are trademarks of BigRangaTech. A fork must use a different name and branding. See GOVERNANCE.md for trademark policy.
Who Is The BDFL?

The project founder (BigRangaTech) serves as Benevolent Dictator For Life (BDFL). The BDFL has final authority on all design decisions, feature acceptance, and project direction. See GOVERNANCE.md for the governance model.
How Can I Donate?

Donations are not currently solicited. The project is self-funded by the BDFL. If this changes, donation information will be posted on the website.
When Will Spike 1.0 Be Released?

The target is approximately 18 months from the start of alpha development. Pre-alpha (documentation) duration is intentionally unbounded. See ROADMAP.md for the full timeline.
What Happens If The BDFL Stops Working On Spike?

The project is designed to outlast any single contributor. All decisions are documented with rationale. All code is open source. A succession plan is defined in GOVERNANCE.md. The documentation-first methodology ensures that anyone can understand the system and continue development.
12. Developer Questions
Why Did You Build A Custom Desktop Instead Of Using GNOME/KDE Plasma?

Spike Shell exists because the target hardware (Celeron N4020, 4GB RAM) cannot run GNOME or full KDE Plasma at acceptable performance levels. A custom Qt6 desktop shell using KWin standalone provides the lightest possible desktop with full Wayland support. See DESIGN-DECISIONS.md for the full rationale.
Why Qt6 Instead Of GTK4?

Qt6 was chosen because KDE standalone applications (Dolphin, Konsole, Kate, etc.) are Qt-based. Using Qt for Spike Shell means shared libraries, shared rendering pipeline, and consistent theming. Building a GTK desktop would mean loading both Qt and GTK libraries simultaneously, increasing memory usage. See DESIGN-DECISIONS.md.
Why Ubuntu Server Instead Of Debian?

Ubuntu LTS provides predictable release cadence, broad hardware support (especially firmware), and long-term security updates. Debian Stable's firmware support is improving but historically lagged. Ubuntu Server (not Desktop) is used because the desktop layer is Spike's own — there's no point installing Ubuntu Desktop just to strip it. See DESIGN-DECISIONS.md.
Why Not Use systemd-boot Or Limine Instead Of GRUB2?

GRUB2 is used because it is the Ubuntu default, well-tested, and supports the boot failure counter mechanism via grubenv. Limine is being evaluated for the future (migration criteria documented in DESIGN-DECISIONS.md). systemd-boot lacks the scripting capability needed for the failure counter check.
Why Flatpak Instead Of Snap Or .deb?

Flatpak was chosen because it provides application sandboxing (bubblewrap isolation), centralized updates via Flathub, and doesn't require a system daemon like snapd. The sandboxing integrates with xdg-desktop-portal for permission management. See DESIGN-DECISIONS.md.
Why KWin Standalone Instead Of wlroots Or weston?

KWin is used because it integrates natively with Qt6 applications, supports the KDE platform themes that Spike's bundled applications (Dolphin, Konsole, etc.) expect, and provides both Wayland and XWayland in a single compositor. Building on wlroots would mean reimplementing Qt integration and KDE application support. See DESIGN-DECISIONS.md.
Why Not Use NetworkManager's Internal DHCP?

Spike uses dhclient instead of NetworkManager's internal DHCP client because dhclient is more compatible with older consumer routers — the exact routers found in homes with old laptops. See NETWORKING.md.
Why Is ZRAM Capped At 4GB For Spike Standard?

The ZRAM cap prevents excessive CPU usage on the Celeron N4020 (dual-core). ZRAM compression/decompression uses CPU cycles. Uncapped ZRAM on a dual-core processor under memory pressure can cause the system to spend more time compressing memory than executing user applications. The 4GB cap provides sufficient virtual memory expansion without overwhelming the CPU. See MEMORY.md.
Why Not Use Btrfs Instead Of ext4?

ext4 is used because it is rock-solid, well-understood, has excellent fsck recovery, and doesn't add complexity. Btrfs offers features (subvolumes, snapshots, compression) that are appealing but add overhead and complexity that doesn't serve the target user. The disaster recovery system doesn't rely on filesystem-level snapshots. See DESIGN-DECISIONS.md.
How Do I Set Up A Development Environment?

See the developer guide (dev-guide/) for build instructions, dependency setup, and coding conventions. The repository structure is defined in AGENTS.md.
Can I Run Spike In A Virtual Machine?

Yes, for development and testing. However, performance measurements taken in a VM are not valid — they don't represent real hardware I/O, GPU performance, or firmware behavior. VMs are useful for code testing, not for performance validation. See PERFORMANCE-BASELINES.md.
What This Document Does Not Cover

    Detailed feature specifications: See individual subsystem documents
    Installation step-by-step walkthrough: See INSTALLER.md and the user guide
    Troubleshooting deep dives: See TROUBLESHOOTING.md
    Privacy policy details: See PRIVACY.md
    Security model: See SECURITY.md
    Hardware detection logic: See HARDWARE.md (to be written)
    Performance targets: See PERFORMANCE-BASELINES.md
    Project governance and decision-making: See GOVERNANCE.md (to be written)
    Contribution workflow: See CONTRIBUTING.md (to be written)
    Code of conduct: See CODE_OF_CONDUCT.md (to be written)
    Release timeline and milestones: See ROADMAP.md
    Changelog: See CHANGELOG.md
    Design rationale for architectural choices: See DESIGN-DECISIONS.md

🐕 BigRangaTech
