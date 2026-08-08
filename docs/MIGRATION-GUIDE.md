# Spike Migration Guide

## Purpose

This document helps users move from their current operating system to Spike Linux. It covers preparation, the actual move, finding equivalent applications, transferring files and accounts, and what to expect in the first few weeks after switching.

Most users coming to Spike are leaving Windows 10 because support ended and their laptop cannot run Windows 11. Some are leaving macOS after their Mac became too old to update. A few are experienced Linux users looking for something simpler. This guide speaks to all three, but primarily to the Windows 10 refugee — the person who has never used Linux, doesn't want to learn a terminal, and just needs their old laptop to work again.

Spike was designed for exactly this person. The migration should feel less like learning a new operating system and more like getting a familiar laptop back after repairs.

## Before You Start

### Check Your Hardware

Spike needs:

```
├── A 64-bit processor (anything from roughly 2013 or later)
├── At least 2 CPU cores
├── At least 4GB of RAM
├── At least 128GB of storage (SSD strongly preferred)
├── A screen resolution of at least 1366×768
└── A USB 3.0 port (for the installer USB drive)
```

**How to check on Windows:**

```
├── Hold the Windows key and press Pause/Break (or right-click
│   Start → System)
├── Look at:
│   ├── "Processor" — must show "x64" or "64-bit operating system"
│   ├── "Installed RAM" — must be 4GB or more
│   └── "System type" — must say "64-bit operating system"
├── Storage: open File Explorer → This PC → check drive capacity
└── If you're not sure, download the Spike ISO and try it —
    the installer will tell you if anything is missing
```

**How to check on macOS:**

```
├── Apple menu → About This Mac
├── "Chip" or "Processor" shows the CPU
├── "Memory" shows RAM
└── Storage: Apple menu → About This Mac → Storage
```

**How to check on Linux:**

```
├── You probably already know
├── Run: lscpu, free -h, lsblk
└── But if you're on Linux, you can skip most of this guide
```

**What will NOT work:**

```
├── Laptops with less than 4GB of RAM
├── Laptops with eMMC storage only (common in HP Stream, some Lenovos)
│   └── These usually have a SATA slot — check your model online
├── Laptops with 32-bit processors (very old, pre-2010)
├── Chromebooks (different architecture, locked bootloader)
├── Tablets or phones (ARM, not x86)
└── Raspberry Pi (ARM, not x86)
```

### Back Up Your Data

This is the most important step. Installing Spike will erase everything on the target drive. There is no undo.

**What to back up:**

```
├── Documents (Word, Excel, PowerPoint, PDFs)
├── Photos and videos
├── Music
├── Downloads you want to keep
├── Browser bookmarks (export from browser settings)
├── Browser passwords (export from browser, or use a password manager)
├── Email (if using a desktop email client — see below)
├── Application-specific data:
│   ├── Game save files
│   ├── Office templates
│   ├── Financial software data (Quicken, etc.)
│   └── Any application that stores data locally
├── Product keys (Microsoft Office, other paid software)
└── Anything in unusual locations (Desktop, custom folders)
```

**Where to back up:**

```
├── External USB hard drive or SSD (recommended)
│   ├── Connect via USB 3.0 for speed
│   ├── Capacity: at least equal to your used storage
│   └── Format doesn't matter (Spike can read NTFS, FAT32, exFAT)
├── Cloud storage (Google Drive, OneDrive, Dropbox)
│   ├── Good for documents and photos
│   ├── Slow for large amounts of data
│   └── Requires internet connection
├── Another computer on your network (file sharing)
└── USB flash drives (for small amounts of data)
```

**How to back up on Windows:**

```
├── Simple: drag and drop files to external drive
├── Better: use File History (Settings → Update & Security → Backup)
├── Browser bookmarks:
│   ├── Chrome: menu → Bookmarks → Bookmark Manager → Export
│   ├── Edge: menu → Favorites → Manage favorites → Export
│   └── Firefox: menu → Bookmarks → Manage Bookmarks → Import and Backup → Export
├── Browser passwords:
│   ├── Chrome: chrome://settings/passwords → Export passwords
│   ├── Edge: edge://settings/passwords → Export passwords
│   └── Firefox: no built-in export (use Firefox Sync or a password manager)
└── Email:
    ├── Webmail (Gmail, Outlook.com): nothing to do — mail is on the server
    ├── Outlook (desktop): File → Open & Export → Import/Export → Export to .pst
    ├── Thunderbird: profile folder can be copied directly
    └── Windows Mail: check if IMAP (server-side) or POP3 (local)
```

**How to back up on macOS:**

```
├── Time Machine (best option): System Settings → Time Machine
├── Manual: drag and drop to external drive
├── Browser: same as Windows instructions above
├── Email:
│   ├── Webmail: nothing to do
│   ├── Apple Mail: Mailbox → Export Mailbox
│   └── Outlook for Mac: same as Windows Outlook export
└── Photos: drag from Photos app to external drive, or use iCloud
```

**Verify your backup:**

```
├── Open files from the backup to make sure they're not corrupted
├── Check that the total size looks right (roughly matches used space)
├── Count the number of photos/documents — does it look right?
├── If anything is missing, back it up before proceeding
└── When you're absolutely sure: you're ready to install Spike
```

### Create the Spike Installer USB

You need:

```
├── A USB flash drive (at least 8GB)
│   └── USB 3.0 strongly preferred (installation is much faster)
├── The Spike ISO file (downloaded from spike.bigrangatech.com)
└── A tool to write the ISO to the USB drive
```

**On Windows (use Rufus):**

```
├── Download Rufus from rufus.ie (free, no installation needed)
├── Insert USB drive
├── Select your USB drive in Rufus
├── Click "Select" and choose the Spike ISO file
├── Partition scheme: GPT (for UEFI) or MBR (for older BIOS laptops)
│   ├── If your laptop is from 2012 or later: use GPT
│   ├── If you're not sure: try GPT first, switch to MBR if it won't boot
│   └── Check: Windows key + R → type "msinfo32" → look for "BIOS Mode"
│       ├── "UEFI" → use GPT
│       └── "Legacy" → use MBR
├── Click "Start"
├── If prompted about ISO vs DD mode: try ISO mode first (works for most cases)
└── Wait for completion (2-5 minutes)
```

**On macOS (use balenaEtcher):**

```
├── Download balenaEtcher from balena.io/etcher (free)
├── Insert USB drive
├── Select the Spike ISO file
├── Select the USB drive
├── Click "Flash"
└── Wait for completion (5-10 minutes)
```

**On Linux (use dd or balenaEtcher):**

```
├── Using dd (advanced):
│   ├── Find the USB drive: lsblk (look for your USB device, e.g., /dev/sdb)
│   ├── Write: sudo dd if=spike.iso of=/dev/sdX bs=4M status=progress
│   │   └── Replace sdX with your actual device (NOT a partition like sdb1)
│   ├── Sync: sync
│   └── ⚠ Double-check the device name — dd will overwrite whatever you point it at
├── Using balenaEtcher: same as macOS instructions
└── Using GNOME Disks: Open Disks → select USB → menu → Restore Disk Image
```

**After writing:**

```
├── The USB drive will appear differently (may show as "Spike" or "EFI Boot")
├── Safely eject and re-insert the USB drive
├── The USB drive is now a Spike installer
└── Do NOT remove files from it or format it — it's ready to use as-is
```

### Boot From the USB

1. Insert the Spike USB drive into your laptop

2. Shut down the laptop completely (not sleep, not restart — shut down)

3. Turn on the laptop and immediately press the boot menu key:

**Common boot menu keys by manufacturer:**

```
├── Acer:     F12 (or F2 for BIOS setup)
├── ASUS:    Esc or F8 (or F2 for BIOS setup)
├── Dell:    F12 (or F2 for BIOS setup)
├── HP:      F9 (or F10 for BIOS setup)
├── Lenovo:  F12 or Novo button (or F1/F2 for BIOS setup)
├── Samsung: F2 or F12
├── Toshiba: F12
└── Other:   Try F12, F10, F8, Esc in order
```

4. A menu should appear listing boot options

5. Select the USB drive (may be labeled "Spike", "UEFI: USB", or the drive brand name)

6. Press Enter

**If the laptop boots to Windows instead of the USB:**

```
├── The USB drive may not be recognized — try another USB port
├── Secure Boot may be blocking the USB drive:
│   ├── Enter BIOS/UEFI setup (F2 or Del at boot)
│   ├── Find "Secure Boot" setting (usually under Security or Boot tab)
│   ├── Disable Secure Boot (or set to "Other OS")
│   ├── Save and exit, then try booting from USB again
│   └── Spike supports Secure Boot, but some laptops are picky about USB booting
├── Legacy/CSM vs UEFI:
│   ├── If the laptop is set to Legacy/CSM boot mode, try switching to UEFI
│   ├── If the laptop can only boot in Legacy mode: use MBR USB (see Rufus steps)
│   └── Most laptops from 2015+ support UEFI
└── Fast Boot may be skipping USB detection:
    ├── Enter BIOS/UEFI setup
    ├── Find "Fast Boot" or "Quick Boot" setting
    ├── Disable it
    └── This forces the laptop to scan USB devices at boot
```

## The Move

### What Happens During Installation

The Spike installer does the following:

**1. Hardware detection**

```
├── Scans your laptop's CPU, RAM, storage, GPU, Wi-Fi, etc.
├── Checks minimum requirements are met
├── Recommends Spike Standard or Spike Plus based on hardware
└── Explains any limitations (e.g., no hardware video decoding)
```

**2. Storage selection and partitioning**

```
├── Shows available drives
├── You select which drive to install on
├── WARNING: Everything on that drive will be erased
├── If multiple operating systems exist: you can choose to replace one
│   (Dual-boot is NOT supported — see "Why No Dual-Boot" below)
└── Partitioning is automatic (you don't manage partitions manually)
```

**3. User account creation**

```
├── Your name (display name, shown on login screen)
├── Username (computer login name — lowercase, no spaces)
├── Password (used for login and administrative tasks)
└── Computer name (identifies your laptop on a network)
```

**4. Locale and keyboard**

```
├── Language (detected from installer, changeable)
├── Time zone (detected from internet, changeable)
└── Keyboard layout (detected, changeable)
```

**5. Variant selection**

```
├── Installer recommends Standard or Plus based on hardware
├── You can accept the recommendation or override it
├── Explanation of what each variant means is shown
└── You can change variants later (Settings → System → Variant)
```

**6. Installation**

```
├── Copies files to disk (5-15 minutes, depending on drive speed)
├── Installs bootloader (GRUB)
├── Configures hardware drivers
├── Creates user account
├── Generates configuration files
└── Shows progress bar with estimated time remaining
```

**7. Post-install configuration**

```
├── First boot setup wizard runs
├── Wi-Fi configuration (if not done during install)
├── Updates check (if internet available)
├── Privacy settings review
└── Welcome screen with quick tips
```

**What the installer does NOT do:**

```
├── Does not preserve your old operating system
├── Does not migrate files automatically (use Move My Files /
│   spike-migration — see SPIKE-MIGRATION.md — or Transferring Files below)
├── Does not install additional software beyond the base system
├── Does not require an internet connection (but recommends one)
└── Does not ask technical questions (partition sizes, filesystem types, etc.)
```

### Why No Dual-Boot

Spike does not support dual-booting with Windows or another OS.

**Reasons:**

```
├── Target users are non-technical — dual-boot adds complexity and risk
├── Dual-boot requires manual partitioning (violates Golden Rule: no config files)
├── GRUB can overwrite Windows bootloader, causing panic
├── Windows updates can overwrite GRUB, causing Spike to be unbootable
├── Partition resizing can fail and destroy data
├── Users who want to "try" Spike should use the live USB environment
│   (the installer runs from USB — you can explore Spike without installing)
└── Users who truly need both OSes are not Spike's target audience
```

The live USB lets you test Spike before committing:

```
├── Boot from USB (as described above)
├── Choose "Try Spike" instead of "Install Spike"
├── Spike runs entirely from the USB drive
├── Your Windows/macOS installation is untouched
├── You can browse, test applications, check Wi-Fi and audio
├── Nothing is saved (changes are lost on reboot)
└── When you're ready: restart and choose "Install Spike"
```

## Transferring Files

Preferred path when the tools are available:

```
├── Disaster / broken system: Rescue My Files (spike-rescue) — recover and/or restore
├── Before install (live USB): Move My Files → SpikeBackup/ (when spike-migration ships)
├── After install: Move My Files → import from SpikeBackup/ or an old disk
├── Install-time: Installer Step 7 backup / Layer 4 restore (same SpikeBackup/ layout)
└── Manual fallback: drag-and-drop in Files (steps below still apply)
```

Specs: `SPIKE-MIGRATION.md`, `SPIKE-RECOVERY-TOOL-GENERAL.md`, `DISASTER-RECOVERY.md`. Until Move My Files ships, Rescue restore + installer restore (when built) cover bringing files back.

### From Windows

After Spike is installed, connect your backup drive:

1. Plug in the external USB drive containing your backup 

2. Spike automatically detects it and shows a notification: "USB drive connected. Open in Files?" 

3. Click "Open" (or open Files manually from the launcher) 

4. The drive appears in the left sidebar under "Devices" 

**Windows file locations → Spike equivalents:**

| **Windows location** | **Spike location** |
| :-: | :-: |
| C:\\Users&lt;name>\\Documents | ~/Documents |
| C:\\Users&lt;name>\\Pictures | ~/Pictures |
| C:\\Users&lt;name>\\Videos | ~/Videos |
| C:\\Users&lt;name>\\Music | ~/Music |
| C:\\Users&lt;name>\\Downloads | ~/Downloads |
| C:\\Users&lt;name>\\Desktop | ~/Desktop |
| C:\\Users&lt;name>\\Contacts | (use webmail or Thunderbird) |
| C:\\Users&lt;name>\\Favorites | (import into Firefox) |

**Copy your files:**

```
├── Open the USB drive in Files
├── Open your Home folder in another Files window
├── Drag and drop (or copy and paste) files to the matching folders
├── Large transfers: Spike shows a progress bar
├── NTFS drives: readable and writable (Spike supports NTFS)
└── exFAT drives: readable and writable
```

**Specific file types:**

```
├── Word documents (.docx): open in LibreOffice Writer (pre-installed)
├── Excel spreadsheets (.xlsx): open in LibreOffice Calc
├── PowerPoint (.pptx): open in LibreOffice Impress
├── PDFs: open in Firefox (built-in PDF viewer) or Document Viewer
├── Photos (.jpg, .png, .heic): open in Image Viewer
├── Videos (.mp4, .mkv): open in Video Player
├── Music (.mp3, .flac, .m4a): open in Music Player
├── ZIP archives: extract by right-click → "Extract Here"
└── Windows executables (.exe): will NOT run (see "Software Equivalents")
```

### From macOS

**macOS file locations → Spike equivalents:**

| **macOS location** | **Spike location** |
| :-: | :-: |
| /Users/<name>/Documents | ~/Documents |
| /Users/<name>/Pictures | ~/Pictures |
| /Users/<name>/Movies | ~/Videos |
| /Users/<name>/Music | ~/Music |
| /Users/<name>/Downloads | ~/Downloads |
| /Users/<name>/Desktop | ~/Desktop |
| Photos Library (app) | (export photos first — see below) |
| Notes app | (export as text/PDF) |
| Safari Bookmarks | (export as HTML, import to Firefox) |

**Photos library:**

```
├── macOS Photos stores photos in a library bundle, not as individual files
├── Before switching: open Photos → select all → File → Export → Export Items
├── Choose a destination on your backup drive
├── This creates individual .jpg/.heic files that Spike can read
└── HEIC files: Spike can convert these (Image Viewer opens them)
```

**Notes:**

```
├── Apple Notes has no Linux equivalent
├── Export notes before switching: select note → File → Export as PDF
├── Or: copy-paste note contents into a text file
└── Alternative: use a web-based note app (Google Keep, Simplenote)
```

**macOS backup drives:**

```
├── Time Machine drives use APFS or HFS+
├── Spike can read HFS+ (hfsprogs package)
├── Spike cannot read APFS Time Machine snapshots
├── If your backup is a Time Machine backup:
│   ├── You can browse the backup folder structure manually
│   ├── Copy files from within the Time Machine folders
│   └── But snapshot restore is not available
└── Best practice: use a simple file copy backup (not Time Machine)
```

**macOS-specific files:**

```
├── Pages (.pages): export to .docx or .pdf on macOS first
├── Numbers (.numbers): export to .xlsx or .pdf
├── Keynote (.key): export to .pptx or .pdf
├── GarageBand (.band): no Linux equivalent — export audio to .mp3/.wav
└── iMovie projects: no Linux equivalent — export to .mp4
```

### From Another Linux Distribution

If you're coming from Ubuntu, Fedora, Debian, or another Linux distro:

**File transfer:**

```
├── Home directory structure is largely the same
├── Copy ~/Documents, ~/Pictures, ~/Videos, ~/Music, ~/Downloads
├── Configuration files (.config, .local): DO NOT copy directly
│   ├── Application configs from other distros may cause conflicts
│   ├── Reconfigure applications fresh on Spike
│   └── Copy individual files if needed (e.g., ~/.bashrc additions)
├── SSH keys: copy ~/.ssh/ (set permissions: chmod 700 ~/.ssh, chmod 600 ~/.ssh/*)
├── GPG keys: export on old system, import on Spike
└── Browser: use Firefox Sync (easiest) or copy profile (riskier)
```

**Package differences:**

```
├── Spike uses Flatpak for user applications (not AUR, RPM Fusion, etc.)
├── System packages: standard Ubuntu LTS apt repository (base only)
├── You will NOT find: AUR, Snap (only Flatpak), RPM packages
├── If you relied on specific packages from another distro's repos:
│   ├── Check if a Flatpak version exists (flathub.org)
│   ├── Check if it's in the Ubuntu LTS repo
│   └── If neither: it may not be available on Spike
└── This is intentional — Spike trades package variety for simplicity
```

**Desktop environment differences:**

```
├── Spike Shell is NOT KDE Plasma, GNOME, XFCE, or Cinnamon
├── Spike Shell is a custom Qt6/Wayland desktop
├── If you customized your previous DE heavily: those settings won't transfer
├── Spike Shell has limited customization by design (see DESIGN-DECISIONS.md)
├── KDE applications still work (Dolphin, Konsole, Discover, etc.)
│   └── But KDE system settings are replaced by Spike Settings
└── GTK applications work but use Qt styling (may look slightly different)
```

**Terminal users:**

```
├── Terminal is available (Konsole) — you haven't lost your tools
├── Package manager: apt (for base system), flatpak (for applications)
├── Shell: bash (default), others installable
├── Your scripts will likely work (bash is bash)
├── systemd is the init system
└── But remember: Spike's Golden Rules say no user task should require
    the terminal. If you find yourself in the terminal for routine
    tasks, something is wrong with Spike, not with you.
```

### Browser Migration

Firefox is Spike's default and only pre-installed browser.

**From Firefox (any OS):**

```
├── Easiest path: use Firefox Sync
│   ├── On old computer: Firefox menu → Settings → Sync → Sign in
│   ├── On Spike: same — sign in to Firefox Sync
│   ├── Bookmarks, passwords, history, extensions all sync
│   └── Takes a few minutes to complete
├── Manual path: export bookmarks to HTML
│   ├── Old: Firefox → Bookmarks → Manage Bookmarks → Import and Backup → Export to HTML
│   ├── Spike: Firefox → Bookmarks → Manage Bookmarks → Import and Backup → Import from HTML
│   └── Passwords: use Firefox Sync (manual export was removed in recent versions)
```

**From Chrome:**

```
├── Bookmarks:
│   ├── Old: Chrome → menu → Bookmarks → Bookmark Manager → menu → Export
│   ├── Spike: Firefox → Bookmarks → Manage Bookmarks → Import and Backup → Import from HTML
│   └── All bookmark folders will be preserved
├── Passwords:
│   ├── Old: chrome://settings/passwords → Export passwords (saves .csv)
│   ├── Spike: Firefox → Settings → Privacy & Security → Logins and Passwords → Import from file
│   ├── Select the .csv file exported from Chrome
│   └── All saved passwords transfer
├── History: cannot be transferred directly (different format)
│   └── You'll start with a clean history
├── Extensions:
│   ├── Chrome extensions are NOT compatible with Firefox
│   ├── Find Firefox equivalents:
│   │   ├── uBlock Origin: same name, Firefox version
│   │   ├── Grammarly: Firefox extension available
│   │   ├── Honey: Firefox extension available
│   │   ├── Password managers (Bitwarden, 1Password): Firefox extensions
│   │   └── Most popular Chrome extensions have Firefox versions
│   └── Search addons.mozilla.org for replacements
└── Settings: must be reconfigured manually (privacy, homepage, etc.)
```

**From Microsoft Edge:**

```
├── Edge is Chromium-based — same export process as Chrome
├── Bookmarks: edge://favorites → Export (HTML file)
├── Passwords: edge://settings/passwords → Export (.csv)
├── Import into Firefox: same as Chrome instructions
└── Edge-specific features (Collections, Web Capture): no Firefox equivalent
```

**From Safari:**

```
├── Bookmarks:
│   ├── Safari → File → Export Bookmarks (produces HTML file)
│   ├── Transfer file to Spike
│   ├── Firefox → Bookmarks → Manage Bookmarks → Import from HTML
│   └── Bookmarks and folders transfer
├── Passwords:
│   ├── Safari stores passwords in Keychain
│   ├── On macOS: System Settings → Passwords → export (if available)
│   ├── Or: use iCloud Keychain and access via web (icloud.com)
│   └── Alternative: use a cross-platform password manager before switching
├── History: cannot be transferred
├── Reading List:
│   ├── Safari → export each item (manual, tedious)
│   ├── Or: use a read-later service (Pocket, Instapaper)
│   └── Firefox has a built-in Reader Mode
└── Extensions: Safari extensions are not Firefox-compatible
```

### Email Migration

**Webmail users (Gmail, Outlook.com, Yahoo, iCloud Mail):**

```
├── Nothing to do — your email lives on the server
├── On Spike: open Firefox → go to your webmail site → log in
├── Alternatively: install a desktop email client (Thunderbird)
│   ├── Thunderbird available via Discover (Flatpak)
│   ├── Add your account (Gmail, Outlook, etc.) — auto-configures
│   └── Mail, contacts, and calendar sync from server
└── No migration needed — email is cloud-based
```

**Desktop email client users:**

**From Microsoft Outlook (Windows):**

```
├── Export to .pst file (before leaving Windows):
│   ├── File → Open & Export → Import/Export → Export to a file
│   ├── Select "Outlook Data File (.pst)"
│   ├── Select the folders to export (or entire mailbox)
│   ├── Save to your backup drive
│   └── Note: .pst files can be large
├── Importing into Thunderbird on Spike:
│   ├── Thunderbird cannot directly import .pst files
│   ├── Option 1: If your email is IMAP — just add the account to Thunderbird
│   │   └── All server-side email appears automatically
│   ├── Option 2: If email is POP3 (stored locally):
│   │   ├── You need to convert .pst to mbox format
│   │   ├── Tool: readpst (install via Discover: pst-utils package)
│   │   ├── Command: readpst -M backup.pst
│   │   ├── Output: mbox files that Thunderbird can import
│   │   ├── Thunderbird: Tools → Import → Import from mbox file
│   │   └── This is the one case where the terminal may be needed
│   └── Option 3: Switch to IMAP before leaving Windows
│       ├── Configure Outlook to use IMAP (if not already)
│       ├── All email syncs to server
│       ├── Then on Spike: add IMAP account to Thunderbird
│       └── Cleanest path — plan ahead
```

**From Apple Mail (macOS):**

```
├── Export mailboxes:
│   ├── Mailbox → Export Mailbox (creates .mbox file/folder)
│   ├── Repeat for each mailbox you want to keep
│   └── Save to backup drive
├── Importing into Thunderbird:
│   ├── Thunderbird: Tools → Import → Import from Apple Mail
│   ├── Select the .mbox file(s)
│   └── Mail imports into local folders
├── Or: if using IMAP (most modern accounts):
│   ├── Just add the account to Thunderbird on Spike
│   └── Server-side email appears automatically
└── Contacts (if stored in Apple Contacts):
    ├── Export as vCard: Contacts → select all → File → Export → Export vCard
    └── Import into Thunderbird Address Book or a contacts app
```

**From Thunderbird (any OS):**

```
├── Easiest migration — just copy the profile
├── On old system: find profile folder
│   ├── Help → Troubleshooting Information → Open Directory (or "Show in Finder")
│   └── Copy the entire profile folder to backup drive
├── On Spike:
│   ├── Install Thunderbird from Discover
│   ├── Run once, then close (creates a default profile)
│   ├── Copy your old profile contents into the new profile folder
│   └── Restart Thunderbird — everything should be there
└── Or: use Thunderbird Sync (if configured) for simpler transfer
```

**From Windows Live Mail / Windows Mail:**

```
├── Windows Live Mail: Export → Email → select format
│   ├── Export as .eml files (individual emails)
│   └── Or export to Exchange (if account is Microsoft)
├── Windows 10 Mail app: no export function
│   ├── If IMAP: just add account to Thunderbird
│   └── If local only: email is stuck (consider switching to IMAP first)
└── These clients store email in proprietary formats — IMAP is the escape hatch
```

## Software Equivalents

### Common Windows Applications

| **Windows application** | **Spike equivalent** | **Notes** |
| :-: | :-: | :-: |
| Microsoft Word | LibreOffice Writer | Opens .docx files |
| Microsoft Excel | LibreOffice Calc | Opens .xlsx files |
| Microsoft PowerPoint | LibreOffice Impress | Opens .pptx files |
| Microsoft Outlook | Thunderbird (or webmail) | IMAP email |
| Microsoft Edge | Firefox | Import bookmarks |
| Google Chrome | Firefox | Import bookmarks |
| Windows File Explorer | Files (Dolphin) | Built-in |
| Windows Settings | Settings (Spike) | Built-in |
| Windows Photos | Image Viewer | Built-in |
| Windows Media Player | Music Player / Video Player | Built-in |
| Windows Calculator | Calculator | Built-in |
| Notepad | Text Editor (Kate) | Built-in |
| Snipping Tool | Screenshot (built-in) | PrtScn key |
| Task Manager | System Monitor | Built-in |
| Paint | Drawing app (Flatpak) | Try GIMP or Pinta |
| 7-Zip / WinRAR | Built-in (Files extracts) | Right-click → Extract |
| Adobe Photoshop | GIMP (Flatpak) | Powerful, different UI |
| Adobe Illustrator | Inkscape (Flatpak) | SVG editor |
| Adobe Acrobat | Firefox / Document Viewer | PDF reading built-in |
| Skype | Skype (web or Flatpak) | Web version works |
| Zoom | Zoom (Flatpak) | Available |
| Microsoft Teams | Teams (web) | No desktop Flatpak |
| Discord | Discord (Flatpak) | Available |
| Spotify | Spotify (Flatpak) | Available |
| VLC Player | Video Player (built-in) | Built-in, or VLC |
| Steam | Steam (Flatpak) | Limited games on Linux |
| iTunes | Music Player / Apple Music | Apple Music web only |
| CCleaner | Not needed | Linux doesn't need this |
| Antivirus | Not needed | Linux doesn't need this |
| Malwarebytes | Not needed | Linux doesn't need this |
| Defragmenter | Not needed | SSDs don't defrag |
| Device Manager | Settings → System Info | Built-in |
| Control Panel | Settings | Built-in |
| Registry Editor | (no equivalent) | Linux has no registry |
| PowerShell / CMD | Terminal (Konsole) | Available but optional |
| Driver installer | Built-in (drivers in kernel) | No driver downloads |

### Common macOS Applications

| **macOS application** | **Spike equivalent** | **Notes** |
| :-: | :-: | :-: |
| Pages | LibreOffice Writer | Opens .docx |
| Numbers | LibreOffice Calc | Opens .xlsx |
| Keynote | LibreOffice Impress | Opens .pptx |
| Safari | Firefox | Import bookmarks |
| Mail | Thunderbird | Import from .mbox |
| Photos | Image Viewer | View photos |
| Preview | Document Viewer / Firefox | PDF viewing |
| Finder | Files (Dolphin) | File manager |
| Spotlight | Launcher (search) | Super key |
| Dock | Panel (bottom) | Different style |
| System Preferences | Settings | Built-in |
| Activity Monitor | System Monitor | Built-in |
| TextEdit | Text Editor (Kate) | Built-in |
| QuickTime Player | Video Player | Built-in |
| Music (iTunes) | Music Player | No iTunes Store |
| FaceTime | (no equivalent) | Use web alternatives |
| Messages | (no equivalent) | iMessage is Apple-only |
| Notes | Text Editor / web app | Export before switch |
| GarageBand | (no equivalent) | Try LMMS (Flatpak) |
| iMovie | (no equivalent) | Try Pitivi (Flatpak) |
| Final Cut Pro | DaVinci Resolve (Flatpak) | Professional editor |
| Pixelmator | GIMP (Flatpak) | Different workflow |
| Sketch | Inkscape (Flatpak) | SVG-focused |
| Time Machine | Spike Rescue / manual | Different approach |
| Homebrew | apt / Flatpak | Different package mgmt |
| Terminal | Terminal (Konsole) | Bash |

### Things That Won't Transfer

Applications with no Linux equivalent and no viable alternative:

```
├── Adobe Creative Cloud (full suite):
│   ├── Photoshop: GIMP is a partial replacement (different workflow)
│   ├── Illustrator: Inkscape (partial, different paradigm)
│   ├── InDesign: Scribus (basic, not professional-grade)
│   ├── Premiere Pro: DaVinci Resolve (capable, different workflow)
│   ├── After Effects: no good Linux equivalent
│   ├── Lightroom: Darktable (raw processing, different workflow)
│   └── Audition: Audacity (audio editing, less features)
│   └── If your livelihood depends on Adobe: consider keeping a Mac/PC
│
├── Microsoft Office (full desktop):
│   ├── LibreOffice is capable but not identical
│   ├── Complex macros may not work
│   ├── Advanced Excel features (Power Query, Power Pivot): missing
│   ├── Complex formatting may shift in LibreOffice
│   ├── Alternative: Office 365 web (works in Firefox)
│   └── If you need exact MS Office: use web version or keep Windows
│
├── Games:
│   ├── Steam on Linux: many games work via Proton (compatibility layer)
│   ├── Not all games work (anti-cheat systems may block Linux)
│   ├── Epic Games: no Linux client (Heroic Games Launcher, unofficial)
│   ├── Battle.net: works via Wine/Proton (unofficial)
│   ├── Native Linux games: smaller catalog
│   └── If gaming is your primary use: Spike may not be right for you
│
├── iTunes (full):
│   ├── Apple Music web (music.apple.com) works in Firefox
│   ├── No iTunes app for Linux
│   ├── iTunes Store purchases: accessible via web
│   └── iPhone syncing: not available (use iCloud web)
│
├── Windows-specific software:
│   ├── .exe and .msi files: will NOT run on Spike
│   ├── Wine: can run some Windows apps (not installed by default, advanced)
│   ├── Bottles (Flatpak): easier Wine front-end for some apps
│   └── But reliability is not guaranteed — plan for Linux alternatives
│
└── macOS-specific software:
    ├── .app bundles: will NOT run on Spike
    ├── .dmg files: will NOT mount on Spike
    ├── iWork apps (Pages/Numbers/Keynote): export before switching
    ├── iMessage: Apple-only, no cross-platform solution
    ├── FaceTime: Apple-only (use Jitsi Meet, Zoom, etc.)
    └── AirDrop: no Linux equivalent (use USB, Bluetooth, or web transfer)
```

## Accounts And Services

### Microsoft Account

Your Microsoft account still works on Spike — it's web-based:

```
├── Email (Outlook.com/Hotmail): access via web (outlook.com) or Thunderbird
├── OneDrive: access via web (onedrive.live.com)
│   └── No desktop sync app (OneDrive Linux client exists but is unofficial)
├── Office Online: use in Firefox (office.com)
├── Xbox: web access for account, no game streaming
├── Skype: web version (web.skype.com) or Flatpak
├── Microsoft Store: irrelevant on Linux
├── Windows license: no longer needed (Spike is free)
└── Your Microsoft account itself is unchanged — just use it via browser
```

OneDrive files:

```
├── Download files you want to keep locally before switching
├── Or: keep accessing via web browser
└── Third-party OneDrive clients (rclone) work but are advanced
```

### Google Account

Your Google account works perfectly on Spike:

```
├── Gmail: web (gmail.com) or Thunderbird
├── Google Drive: web (drive.google.com)
│   └── Google Drive desktop sync (Google Drive for Desktop) is macOS/Windows only
├── Google Docs/Sheets/Slides: web (docs.google.com) — works fully in Firefox
├── Google Photos: web (photos.google.com)
├── YouTube: web (youtube.com) — works fully
├── Google Calendar: web (calendar.google.com)
├── Google Meet: web (meet.google.com) — works in Firefox
├── Google Play: irrelevant on Linux
├── Android phone: unaffected (your phone doesn't care what OS your PC runs)
└── Your Google account is unchanged
```

### Apple Account (Apple ID)

Your Apple ID still works, but some services are limited:

```
├── iCloud Mail: web (icloud.com) or Thunderbird (IMAP)
├── iCloud Drive: web (icloud.com) — no desktop sync
├── iCloud Photos: web (icloud.com) — can view and download
├── Apple Music: web (music.apple.com)
├── Apple TV+: web (tv.apple.com)
├── iCloud Keychain: access via web (icloud.com → Passwords)
│   └── Cannot sync to Firefox — export passwords before switching
├── Find My: web (icloud.com → Find My)
├── iMessage: no web access — lost on Spike
│   └── Continue using on your iPhone/iPad
├── FaceTime: no web access (except FaceTime Links, limited)
│   └── Use on your iPhone/iPad
├── App Store: irrelevant on Linux
├── iPhone/iPad syncing: no desktop syncing on Spike
│   ├── Photos: use iCloud Photos (web) or AirDrop to another Apple device
│   ├── Backups: must use iCloud backups (no Finder/iTunes backup)
│   └── Media transfer: use web or cloud storage
└── Your Apple ID is unchanged — but you'll rely on web interfaces
```

### Social Media

All social media works via Firefox on Spike:

```
├── Facebook: web (facebook.com) — full functionality
├── Instagram: web (instagram.com) — full functionality
├── Twitter/X: web (twitter.com or x.com) — full functionality
├── Reddit: web (reddit.com) — full functionality
├── TikTok: web (tiktok.com) — full functionality (including upload)
├── WhatsApp: web (web.whatsapp.com) — full functionality
│   └── Also available as Flatpak (WhatsApp Desktop, unofficial)
├── Telegram: web (web.telegram.org) or Flatpak (Telegram Desktop)
├── Signal: web or Flatpak (Signal Desktop)
├── Slack: web or Flatpak
├── Discord: web or Flatpak
├── Pinterest: web — full functionality
└── LinkedIn: web — full functionality
```

## First Week With Spike

### Day 1: Setup And Orientation

- Complete the first-boot setup wizard 

- Connect to Wi-Fi 

- Check for updates (Settings → Updates) 

- Review privacy settings (Settings → Privacy) 

- Open Files and verify it works 

- Open Firefox and sign in to your browser sync 

- Import your bookmarks 

- Connect your external drive and start copying files 

- Try opening a document (LibreOffice Writer) 

- Try opening a photo (Image Viewer) 

- Try playing a video (Video Player) 

- Connect Bluetooth headphones (Settings → Bluetooth) 

- Check sound output (Settings → Sound) 

- Set wallpaper (Settings → Appearance → Wallpaper) 

- Set timezone if incorrect (Settings → Time & Date) 

- Shut down and restart to verify boot works 

**Day 1 expectations:**

```
├── Everything should "just work" — if it doesn't, see TROUBLESHOOTING.md
├── The desktop may feel different — that's normal
├── Spend time clicking around Settings to learn what's available
├── Don't install additional software yet — explore what's built in
└── If something feels broken: it might be — report it (see SUPPORT.md)
```

### Days 2-3: Software Setup

- Install additional applications from Discover 

  - Music player (if you want something beyond built-in) 

  - Additional browsers (Chrome, Brave — if you must) 

  - Chat apps (Discord, Telegram, Zoom) 

  - Games (Steam, if interested) 

  - Creative tools (GIMP, Inkscape, DaVinci Resolve) 

- Set up email (Thunderbird or webmail) 

- Configure printer (Settings → Printers, if you have a printer) 

- Transfer remaining files from backup 

- Organize files in Home folder 

- Learn the launcher (Super key → type to search) 

- Learn keyboard shortcuts (Settings → Keyboard → Shortcuts) 

- Configure touchpad settings (Settings → Mouse & Touchpad) 

- Set up night light if you use it (Settings → Display → Night Light) 

**Days 2-3 expectations:**

```
├── Discover (app store) should feel familiar — like Google Play or App Store
├── Installing apps is one click (no downloading from websites)
├── Apps install to the launcher automatically
├── Updates are handled by Discover (no manual update checks)
└── If an app isn't in Discover: check flathub.org (the Flatpak repository)
```

### Days 4-7: Settling In

- Customize panel applets (right-click panel) 

- Set up keyboard layout if non-US (Settings → Keyboard) 

- Test suspend/resume (close lid, open lid) 

- Test external display (connect HDMI) 

- Test audio with headphones and speakers 

- Set up automated backups (Settings → Backups, or manual) 

- Remove any apps you installed but don't use 

- Explore Settings thoroughly — there's no hidden config files 

- Try the terminal if you're curious (it won't bite) 

- Join the community forum (forum.bigrangatech.com) 

**First week reality check:**

```
├── You will occasionally forget how to do something — that's normal
├── Some things are in different places than Windows/macOS — that's expected
├── You might miss a specific app — check for a Linux alternative
├── Performance should feel good — if it's slow, check variant
├── If you hit a wall: check FAQ.md, then TROUBLESHOOTING.md, then forum
└── By the end of week one, most users feel comfortable
```

### After Two Weeks

By this point you should:

```
├── Know where all settings are
├── Have all your applications installed
├── Have all your files organized
├── Feel comfortable with the launcher and panel
├── Understand how updates work (they just happen)
├── Not think about antivirus (it's not needed)
├── Not think about defragmenting (it's not needed)
├── Not think about driver updates (they come with system updates)
├── Rarely think about "which OS am I using" — it should feel natural
└── If you're still struggling after two weeks:
   ├── Check the forum — others may have had the same issue
   ├── Check FAQ.md — common questions are answered there
   └── The Spike team wants to know what's hard — file feedback
```

## Common Adjustment Points

### Things That Are Different (But Not Harder)

**1. Installing software**

```
├── Windows: go to website → download .exe → run installer → click Next 10 times
├── macOS: go to website → download .dmg → drag to Applications
└── Spike: open Discover → search → click Install → done
└── Spike's way is actually easier once you're used to it
```

**2. Updates**

```
├── Windows: Windows Update (slow, forces restart, annoying)
├── macOS: System Preferences → Software Update (monthly, big downloads)
└── Spike: Settings → Updates (checks weekly, downloads in background)
└── You get notified when updates are ready — no forced restarts
```

**3. File paths**

```
├── Windows: C:\\Users\\name\\Documents (backslashes, drive letters)
├── macOS: /Users/name/Documents (forward slashes, home is ~)
└── Spike: /home/name/Documents (same as macOS, home is ~)
└── You rarely need to type paths — use Files (file manager)
```

**4. Settings location**

```
├── Windows: Control Panel AND Settings app (confusing dual system)
├── macOS: System Settings (one place, but deeply nested)
└── Spike: Settings (one app, flat categories, no nesting beyond 2 levels)
└── If a setting exists, it's in Settings — there are no hidden config files
```

**5. Closing applications**

```
├── Windows: clicking X closes the application
├── macOS: clicking X closes the window but app keeps running
└── Spike: clicking X closes the window and the application
└── Spike behaves like Windows in this regard
```

**6. Window management**

```
├── Windows: taskbar at bottom, Snap (drag to edge)
├── macOS: Dock at bottom, Mission Control
└── Spike: panel at bottom, drag to snap (left half, right half, maximize)
└── Super + Left/Right arrow also snaps windows
```

**7. Keyboard shortcuts**

```
├── The Super key (Windows key) is your friend
├── Super alone: opens the launcher (like pressing Windows key on Windows)
├── Super + 1, 2, 3...: not mapped (Spike doesn't pin apps to numbers)
├── Alt + Tab: switch between open windows (same as Windows)
├── Super + D: show desktop (same as Windows)
├── Ctrl + C / V / X: copy/paste/cut (same as Windows)
├── Ctrl + F: find (same as Windows)
├── Alt + F4: close window (same as Windows)
├── PrtScn: screenshot (opens screenshot tool)
├── Super + L: lock screen
└── See Settings → Keyboard → Shortcuts for the full list
```

### Things That Might Surprise You (In A Good Way)

**1. No antivirus needed**

```
├── Linux desktop malware is extremely rare
├── Spike's software comes from a signed repository (trusted source)
├── Applications are sandboxed (Flatpak)
└── You can uninstall your antivirus — you don't need it
```

**2. No defragmentation needed**

```
├── SSDs don't need defragmenting (it harms them)
├── Spike detects SSDs and disables defrag automatically
└── If you have an HDD: Spike uses a different filesystem (ext4) that
    fragments less than NTFS — defrag is still unnecessary
```

**3. No driver downloads**

```
├── Drivers are built into the Linux kernel
├── Your Wi-Fi, GPU, audio, touchpad, webcam — all work out of the box
├── The only exception is NVIDIA GPUs (Spike handles this during install)
└── You never visit a manufacturer website to download a driver
```

**4. Updates don't nag you**

```
├── Spike checks for updates weekly
├── Downloads happen in the background
├── You choose when to restart (no forced restarts)
├── Updates don't interrupt your work
└── You can postpone indefinitely (though applying within a week is recommended)
```

**5. Software doesn't slow down over time**

```
├── No registry bloat (Linux has no registry)
├── No startup program accumulation (without your knowledge)
├── No hidden toolbars or system tray junk
└── A Spike system six months in runs as fast as day one
```

**6. Everything is in one place**

```
├── All settings: Settings app
├── All applications: Launcher
├── All files: Files (file manager)
├── All updates: Settings → Updates
└── No hunting through multiple control panels
```

## When Something Goes Wrong

### Common Migration Issues

**Issue: "My Wi-Fi doesn't work after installing Spike"**

```
├── Most Wi-Fi adapters are supported out of the box
├── If Wi-Fi doesn't work:
│   ├── Check the physical switch (some laptops have a Wi-Fi toggle)
│   ├── Settings → Network → check if adapter is detected
│   ├── If adapter is detected but no networks: airplane mode may be on
│   └── If adapter not detected: see TROUBLESHOOTING.md → Network
├── Broadcom Wi-Fi: may need firmware (installer should handle this)
└── Solution: connect via Ethernet temporarily, run updates
```

**Issue: "My touchpad doesn't work properly"**

```
├── Basic touchpad movement should work immediately
├── If multitouch (scroll, pinch) doesn't work:
│   ├── Settings → Mouse & Touchpad → check settings
│   ├── Some older touchpads are recognized as generic mice (no multitouch)
│   └── This is a hardware limitation, not a Spike bug
└── If touchpad doesn't work at all: use a USB mouse temporarily
```

**Issue: "My documents look different in LibreOffice"**

```
├── LibreOffice renders documents slightly differently than Microsoft Office
├── Fonts may differ (Calibri → Carlito, a metric-compatible substitute)
├── Complex formatting (columns, text boxes) may shift
├── Track changes and comments: mostly compatible but not perfect
├── Macros: VBA macros will NOT work in LibreOffice
└── Solutions:
    ├── For viewing: use Office Online (office.com) in Firefox
    ├── For editing: accept minor formatting differences
    └── For critical documents: test before relying on LibreOffice
```

**Issue: "I can't find an application I need"**

```
├── Check Discover (Spike's app store) — search by name
├── Check flathub.org (browse the full Flatpak catalog)
├── Check if a web version exists (many apps have web versions)
├── Ask on the forum (forum.bigrangatech.com)
└── If no Linux equivalent exists: you may need a different solution
```

**Issue: "My printer won't connect"**

```
├── Printers are not configured by default (CUPS not pre-installed)
├── Settings → Printers → "Enable printer support" (installs CUPS)
├── Connect printer via USB: should auto-detect
├── Network printers: discovered via Bonjour/Avahi
├── IPP-compatible printers: work without drivers (most modern printers)
└── Manufacturer-specific drivers: may need manual installation
```

**Issue: "My Bluetooth headphones connect but sound is bad"**

```
├── Spike Standard: uses SBC codec (lower quality, most compatible)
├── Spike Plus: supports AAC and LDAC (higher quality, if supported)
├── Settings → Sound → check output device
├── If audio is choppy: move closer to laptop (Bluetooth range is limited)
├── Try disconnecting and reconnecting
└── If persistent: check for Bluetooth firmware issues
```

**Issue: "The screen resolution looks wrong"**

```
├── Settings → Display → Resolution
├── If correct resolution not listed: it may be a GPU driver issue
├── Most issues resolved by applying updates
├── If you have an NVIDIA GPU: ensure driver is installed (see HARDWARE.md)
└── External displays: configure in Settings → Display
```

**Issue: "My old software doesn't run"**

```
├── .exe files do not run on Linux
├── Check for a Linux alternative (see Software Equivalents above)
├── Wine can run some Windows apps (advanced, not guaranteed)
├── Bottles (Flatpak) provides easier Wine setup for some apps
├── VirtualBox: run a full Windows inside Spike (heavy on resources)
├── For critical Windows-only software: consider keeping a Windows PC
└── For most users: a Linux alternative exists and is sufficient
```

### Getting Help

If you're stuck, here's where to look (in order):

1. **FAQ.md** (if shipped with Spike, or on the website)

   - Answers the most common questions from new users 

   - Written in plain language, no technical knowledge required 

2. **TROUBLESHOOTING.md** (shipped with Spike, or on the website)

   - Symptom-based problem solving 

   - Each problem has a GUI fix first, advanced fix second 

   - Covers the most common issues users encounter 

3. **Forum** (forum.bigrangatech.com)

   - Search existing posts — your question may already be answered 

   - Post a new question if not 

   - Community members and developers respond 

   - Be patient — volunteers help in their spare time 

4. **Settings → Diagnostics**

   - Generate a system report (hardware summary, no personal data) 

   - Include this when asking for help on the forum 

   - Helps others understand your specific setup 

5. **Contact BigRangaTech**

   - Website: spike.bigrangatech.com 

   - Forum: forum.bigrangatech.com 

   - See SUPPORT.md for support channels 

**What to include when asking for help:**

```
├── What you were trying to do
├── What happened instead
├── What you expected to happen
├── Your laptop model (if known)
├── Spike version (Settings → About)
├── Whether you have Spike Standard or Plus
└── The diagnostic report (Settings → Diagnostics → Generate Report)
```

**What NOT to include:**

```
├── Your password
├── Personal files
├── Your full home directory
└── Anything you wouldn't post publicly
```

## Should You Switch?

### Honest Assessment

**Spike is the right choice if:**

```
├── Your Windows 10 laptop can't upgrade to Windows 11
├── You mainly use a web browser
├── You do basic document editing (not complex Office macros)
├── You watch videos and listen to music
├── You do email and video calls
├── You're tired of forced updates and slowdowns
├── Your laptop feels slow and you want it to feel fast again
├── You don't need specific Windows-only software
└── You're willing to learn a few new ways of doing things
```

**Spike might NOT be the right choice if:**

```
├── You depend on Adobe Creative Suite for work
├── You use Microsoft Office with complex macros
├── You're a serious gamer (some games work, many don't)
├── You need iTunes for iPhone management
├── You use specialized Windows software with no Linux equivalent
├── You use iMessage or FaceTime regularly
├── You want to dual-boot with Windows (Spike doesn't support this)
└── You're not willing to learn anything new
```

**The honest truth:**

```
├── Spike is a rescue for old laptops — that's its job
├── It won't replace a high-end workstation for power users
├── It won't run every piece of Windows software
├── But for browsing, email, documents, media, and basic computing:
│   Spike is faster, safer, and more private than Windows 10 was
└── And it will keep your laptop usable for years to come
```

## What This Document Does Not Cover

- Installer step-by-step screens and dialog text: See `INSTALLER.md` 

- Hardware detection and compatibility details: See `HARDWARE.md` 

- Variant differences (Standard vs Plus) and selection: See `VARIANT-DIFFERENCES.md` 

- Performance expectations per hardware tier: See `PERFORMANCE-BASELINES.md` 

- Troubleshooting specific technical issues: See `TROUBLESHOOTING.md` 

- Frequently asked questions: See `FAQ.md` 

- Support channels and response times: See `SUPPORT.md` 

- Privacy settings and data collection policy: See `PRIVACY.md` 

- Desktop layout and panel/launcher usage: See `DESKTOP.md` 

- Software installation and Discover (app store): See `DESKTOP.md` 

- File manager features and configuration: See `DESKTOP.md` 

- Update system and update scheduling: See `CONFIGURATION.md` 

- System recovery and disaster recovery: See `DISASTER-RECOVERY.md` 

- Accessibility features for users with disabilities: See `ACCESSIBILITY.md` 

- End-of-life timeline and support duration: See `END-OF-LIFE-POLICY.md` 

🐕 BigRangaTech


