# Spike Security

## Purpose

This document specifies all security policies, mechanisms, and configurations in Spike. Security is approached from a practical standpoint: the target user is a beginner on a personal laptop, not an enterprise administrator managing a fleet. Security measures must protect without obstructing.

## Design Philosophy

Spike's security philosophy is grounded in the target user:

**1. Protect the user, not the system**

```
`├── Personal files matter more than system integrity`

`├── A wiped system can be reinstalled; lost photos cannot be replaced`

`└── Security measures prioritize data protection over system hardening`
```

**2. Sensible defaults, no security theater**

```
`├── Firewall on, AppArmor on, root disabled`

`├── No disk encryption (trade-off explained below)`

`├── No forced password complexity (see INSTALLER.md rationale)`

`└── No biometric requirements or multi-factor authentication`
```

**3. Automatic protection**

```
`├── Security updates install silently in the background`

`├── User does not need to "run antivirus" or "scan for threats"`

`└── No nagware, no fear-inducing warnings`
```

**4. No surveillance of the user**

```
`├── No telemetry, no crash reporting, no phoning home`

`├── No remote management capabilities`

`└── The laptop belongs to the user, not to BigRangaTech`
```

## Threat Model

### What Spike Protects Against

Threats Spike actively mitigates:

**1. Network-based attacks**

```
`├── Remote exploitation of services (firewall blocks all incoming)`

`├── Malicious websites (browser sandboxing, no Flash/Java plugins)`

`├── Rogue Wi-Fi networks (user must explicitly connect)`

`└── DNS poisoning (systemd-resolved with caching)`
```

**2. Local privilege escalation**

```
`├── AppArmor confines applications to expected behavior`

`├── sudo requires password (no passwordless sudo)`

`├── root login disabled (no root password to crack)`

`└── Polkit handles privilege escalation with GUI prompts`
```

**3. Malware via software installation**

```
`├── Only signed apt packages from Ubuntu repositories`

`├── Flatpak sandboxing isolates applications`

`├── No third-party apt repositories by default`

`└── No script execution from file manager (no .sh auto-run)`
```

**4. Physical access (casual)**

```
`├── Screen lock on idle and on suspend`

`├── Password required on wake`

`├── Root login disabled at SDDM`

`└── Recovery mode requires physical access (acceptable risk)`
```

**5. Data loss from system failure**

```
`├── Boot failure counter + previous kernel fallback`

`├── Spike Rescue tool for file recovery`

`└── Reinstall with restore preserves personal files`
```

### What Spike Does NOT Protect Against

Threats Spike does NOT mitigate (by design):

**1. Determined physical access**

```
`├── No disk encryption — attacker with physical access can read files`

`├── Recovery mode grants root shell (standard Ubuntu behavior)`

`├── USB boot bypasses all system-level security`

`└── Trade-off: Encryption adds complexity, password at boot,`

`    and recovery complexity — unacceptable for target users`
```

**2. Social engineering**

```
`├── User can install malicious Flatpak (sandboxed, but data accessible)`

`├── User can enter sudo password when prompted by convincing dialog`

`├── User can disable firewall via GUI`

`└── Mitigation: Spike never asks for password unexpectedly`
```

**3. Supply chain attacks**

```
`├── Compromised Ubuntu package could affect Spike`

`├── Compromised Flathub application could affect user data`

`└── Mitigation: Only official repositories, no 3rd party PPAs`
```

**4. Firmware-level attacks**

```
`├── SPI flash, Intel ME, AMD PSP — out of scope`

`├── Bootkit/rootkit — no Secure Boot enforcement by default`

`└── Trade-off: Secure Boot is supported but not required`

`    (Broadcom drivers need MOK enrollment, adds friction)`
```

**5. Zero-day exploits**

```
`├── No proactive exploit mitigation beyond AppArmor`

`└── Mitigation: Security updates applied automatically`
```

### Why No Disk Encryption

This is a deliberate, documented decision:

**Against encryption:**

```
`├── Target users are beginners — encryption adds boot-time password`

`├── Lost encryption password = permanent data loss (no recovery possible)`

`├── Spike Rescue tool cannot read encrypted partitions`

`├── Reinstall with restore cannot work on encrypted disks`

`├── Performance overhead on Celeron N4020 (AES-NI present but still overhead)`

`├── Recovery mode and rescue tooling become useless`

`└── The recovery-first design philosophy is incompatible with encryption`
```

**For encryption (acknowledged):**

```
`├── Protects data if laptop is stolen`

`├── Industry best practice for sensitive data`

`└── Some users may genuinely need it`
```

**Decision:**

```
`├── No disk encryption by default`

`├── Not offered during installation`

`├── If a future version adds it: must be opt-in, not default`

`│   and must integrate with rescue/recovery tooling`

`└── If users ask: "Should I encrypt?" the answer is:`

`    "If you have highly sensitive data and you're confident`

`    you'll never forget the password, encryption adds protection.`

`    However, if you forget the password, your data is permanently`

`    lost. Spike does not currently support disk encryption."`
```

This decision is recorded in **DESIGN-DECISIONS.md**.

## User Authentication

### User Account Model

Spike uses a standard Linux user/sudo model:

**Account types:**

```
`├── Regular user: Member of sudo group, can elevate via sudo/polkit`

`├── Root: Locked (no password set, login disabled)`

`└── No separate administrator account (user IS the administrator)`
```

**User creation (during install):**

```
`├── One user account created`

`├── Added to groups: user, sudo, audio, video, render, pipewire, plugdev`

`├── Shell: /bin/bash`

`├── Home directory: /home/\[username\]`

`└── No additional users created by installer`
```

**Additional users (post-install):**

```
`├── Settings → Users → Add User`

`├── New user can be standard or admin (sudo group)`

`├── Admin users have same privileges as original user`

`└── Each user has separate home directory, settings, and files`
```

### Password Policy

**Password requirements:**

```
`├── Minimum: 6 characters`

`├── No complexity requirements (no forced uppercase, numbers, symbols)`

`├── No password strength meter (doesn't judge the user)`

`├── No password expiration (no forced periodic changes)`

`├── No password history (user can reuse old passwords)`

`└── No lockout policy (no account lock after failed attempts)`
```

**Rationale:**

```
`├── Forcing complexity leads to written-down passwords`

`├── Forced periodic changes lead to predictable patterns (Password1!)`

`├── Lockout policies can lock out the legitimate user`

`├── Target users are single-user laptops, not shared systems`

`└── A memorable password the user doesn't write down beats a complex one taped to the lid`
```

**Password usage:**

```
`├── Login at SDDM`

`├── sudo (command-line privilege escalation)`

`├── Polkit (GUI privilege escalation — e.g., installing software)`

`├── Screen unlock`

`└── Disk decryption (N/A — no encryption)`
```

### sudo Configuration

Configuration: `/etc/sudoers` (managed by spike-config, never user-edited)

**Spike sudoers configuration:**

```
*`\# Spike sudo configuration`*

*`\# Users in the sudo group may run any command`*

`%sudo ALL=(ALL:ALL) ALL`


*`\# No passwordless sudo anywhere`*

*`\# No NOPASSWD entries`*


*`\# sudo lecture (first-time message):`*

`Defaults lecture = always`

`Defaults lecture\_file = /etc/sudo\_lecture`


*`\# Spike sudo lecture:`*

`"This is a privileged operation. Your password is required.`

`Type carefully — changes here affect the entire system."`


*`\# Timestamp timeout: 5 minutes (default)`*

`Defaults timestamp\_timeout = 5`


*`\# Insults disabled (professional, not snarky)`*

`Defaults !insults`


*`\# Requiretty disabled (allows sudo from scripts)`*

`Defaults !requiretty`
```

### Polkit (GUI Privilege Escalation)

Polkit handles GUI privilege escalation:

**When an action requires admin privileges:**

```
`├── Polkit intercepts the request`

`├── Displays GUI dialog: "Authentication required"`

`├── Shows: "\[Action\] requires administrative privileges."`

`├── Shows: "User: \[username\] — enter your password"`

`├── Password field with "Show password" toggle`

`├── On success: action proceeds`

`├── On failure: 3 attempts, then dialog closes with "Authentication failed"`

`└── On cancel: action aborts, no error shown to user`
```

**Polkit rules (Spike defaults):**

```
`├── org.freedesktop.policykit.exec: requires admin auth`

`├── org.freedesktop.packagekit.install: requires admin auth`

`├── org.freedesktop.packagekit.remove: requires admin auth`

`├── org.freedesktop.udisks2.filesystem-mount: requires admin auth for system drives`

`├── org.freedesktop.udisks2.filesystem-mount-system: auth\_admin\_keep (remember for 5 min)`

`├── org.freedesktop.NetworkManager.settings.modify: auth\_admin\_keep`

`└── com.github.spike.\*: various rules per component`
```

**Authentication agent:**

```
`├── polkit-kde-authentication-agent-1`

`├── Runs as part of spike-session (autostart)`

`├── Themed with Spike visual identity`

`└── No separate configuration needed`
```

### Root Account

Root account is LOCKED:

```
`├── No root password set (account locked)`

`├── root cannot log in at SDDM`

`├── root cannot log in via TTY`

`├── root cannot SSH in (SSH not installed anyway)`

`├── sudo -i or sudo su provides root shell when needed`

`└── Recovery mode: sulogin provides root shell without password`

`    (standard Ubuntu behavior — requires physical access)`
```

This is standard Ubuntu Server behavior. Spike inherits it.

## Login Security

SDDM (display manager) configuration:

**Login behavior:**

```
`├── Username field: pre-filled with last logged-in user`

`├── Password field: empty, password masking on`

`├── Failed login: shake animation (Plus) or red border (Standard), clear password`

`├── No "hint" display (no password hints)`

`├── No guest session`

`├── No remote login`

`├── Auto-login: optional (Settings → Users)`

`└── Session: spike-session (only option, no dropdown)`
```

**SDDM security settings:**

```
`├── Maximum login attempts: unlimited (no lockout — see password policy)`

`├── Login delay: none (immediate)`

`├── User list: show all users (single-user systems typically)`

`├── Session type: Wayland only (no X11 session option)`

`└── Greeter runs as separate user (sddm) — no access to user files`
```

## AppArmor

### Configuration

AppArmor is enabled with Ubuntu defaults. No custom profiles.

**State:** Active and enforcing at boot.

**Packages installed:**

```
`├── apparmor`

`├── apparmor-utils`

`└── apparmor-profiles-extra (Ubuntu default profiles)`
```

**Profiles loaded:**

```
`├── Ubuntu default profiles (100+ profiles)`

`│   ├── firefox\_profile (if Firefox installed as apt package)`

`│   ├── Various system utilities (ping, traceroute, etc.)`

`│   └── Service profiles (NetworkManager, dhclient, etc.)`

`├── Flatpak sandbox (Flatpak uses its own sandbox, not AppArmor)`

`└── No Spike-specific AppArmor profiles`
```

**Why no custom profiles:**

```
`├── Spike Shell components run as the user, not as services`

`├── KWin, PipeWire, NetworkManager already have Ubuntu profiles`

`├── Writing custom AppArmor profiles requires extensive testing`

`├── A poorly written profile can break functionality`

`└── Ubuntu defaults provide meaningful confinement without risk`
```

**Verification:**

```
`├── Settings → Advanced → Diagnostics → Security`

`│   ├── AppArmor status: Active (Enforcing)`

`│   ├── Profiles loaded: \[count\]`

`│   ├── Profiles in enforce mode: \[count\]`

`│   ├── Profiles in complain mode: 0`

`│   └── \[View profiles\] (read-only list)`

`└── Command-line: aa-status (for developers)`
```

## Flatpak Sandboxing

Flatpak provides its own application sandboxing, separate from AppArmor. This is the primary isolation mechanism for user-installed applications.

**Flatpak sandbox layers:**

```
`├── Bubblewrap (bwrap): PID namespace, mount namespace`

`├── xdg-desktop-portal: File access negotiation, screen capture`

`├── seccomp: System call filtering`

`└── AppArmor: Additional confinement (Ubuntu default)`
```

**Default permissions for Flatpak apps:**

```
`├── App must request: filesystem access, camera, microphone`

`├── User approves via portal dialog`

`├── Permissions stored per-app, revocable in Settings`

`└── Apps cannot access other apps' data by default`
```

**Settings → Privacy → Application Permissions:**

```
`├── Lists all installed Flatpak applications`

`├── Shows current permissions (files, camera, microphone, network)`

`├── User can revoke permissions per app`

`├── User can reset to defaults`

`└── Changes take effect on next app launch`
```

## Firewall

### Default Configuration

**Firewall:** ufw (Uncomplicated Firewall)  
**Backend:** nftables (modern, via iptables-nft)  
**State:** Enabled, active at boot

**Default policies:**

| **Direction** | **Policy** |
| :-: | :-: |
| Incoming | DENY |
| Outgoing | ALLOW |
| Forward | DENY |
| Routed | DENY |

**Pre-configured rules:**

```
`├── 5353/udp ALLOW (mDNS — printer discovery, Chromecast)`

`└── No other incoming rules`
```

See **NETWORKING.md** for full firewall specification including GUI controls, rule management, and user-facing settings.

## Automatic Updates

### Update Policy

Spike separates updates into three categories:

**1. Security updates (apt)**

```
`├── Source: Ubuntu security repository`

`├── Behavior: INSTALLED AUTOMATICALLY`

`├── Schedule: Every 6 hours (via spike-update.timer)`

`├── Conditions:`

`│   ├── Network connected`

`│   ├── System idle 10+ minutes`

`│   └── Memory pressure below threshold (earlyoom check)`

`├── User notification: "X security updates were installed."`

`├── Reboot: Never forced`

`│   ├── If kernel updated: "Restart to apply security updates"`

`│   └── User can restart at their convenience`

`└── No user intervention required (silent)`
```

**2. Non-security updates (apt)**

```
`├── Source: Ubuntu updates repository`

`├── Behavior: NOTIFY ONLY (not auto-installed)`

`├── User notification: "X updates are available."`

`├── Installation: User clicks "Update" in Discover`

`└── User decides when to apply`
```

**3. Flatpak updates**

```
`├── Source: Flathub repository`

`├── Behavior: NOTIFY ONLY (not auto-downloaded, not auto-installed)`

`├── User notification: "X app updates are available."`

`├── Installation: User clicks "Update" in Discover`

`└── User decides when to apply`
```

**Rationale:**

```
`├── Security updates protect the user without requiring action`

`├── Non-security updates may change behavior (user should choose)`

`├── Flatpak updates may change app UI (user should choose)`

`└── Forced updates erode user trust ("my computer changed overnight")`
```

### Update Execution

`spike-update.service` (runs via timer):

**Execution flow:**

```
`1. Check conditions:`

`   ├── Is network connected? (NetworkManager DBus)`

`   ├── Is system idle 10+ minutes? (logind idle hint)`

`   └── Is memory pressure acceptable? (earlyoom threshold check)`

`   If any condition fails: skip this cycle, retry in 6 hours.`


`2. Refresh package lists:`

`   ├── apt update`

`   └── If apt update fails (network issue): skip, retry later`


`3. Check for security updates:`

`   ├── apt list --upgradable`

`   ├── Filter for security-origin packages`

`   └── If none: done for this cycle`


`4. Install security updates:`

`   ├── apt install --only-upgrade \[security-packages\]`

`   ├── Run unattended-upgrades for configured packages`

`   ├── If kernel updated: update GRUB, rebuild initramfs`

`   └── Log all actions to /var/log/spike/updates.log`


`5. Post-update actions:`

`   ├── If kernel updated: notification "Restart to apply updates"`

`   ├── If services restarted: notification "Background services updated"`

`   └── If no restart needed: notification "Security updates installed"`


`6. Check for non-security updates:`

`   ├── apt list --upgradable (remaining packages)`

`   ├── Count non-security upgradable packages`

`   ├── If any: notify Discover to show update badge`

`   └── Discover shows update count in tray`


`7. Check for Flatpak updates:`

`   ├── flatpak remote-ls --updates`

`   ├── Count available updates`

`   ├── If any: notify Discover to show Flatpak update badge`

`   └── Do NOT download or install (user choice)`
```

### Update Failure Handling

**If a security update fails to install:**

```
`1. Retry (up to 3 attempts with 5-second delays)`

`2. If still failing:`

`   ├── Log the error to /var/log/spike/updates.log`

`   ├── Notification: "A security update could not be installed.`

`   │   Spike will try again later."`

`   ├── Next timer cycle: retry the failed package`

`   └── If package fails 3 consecutive cycles:`

`       ├── Notification: "Spike has been unable to install a security`

`       │   update for \[package\] over several attempts. This may`

`       │   indicate a network or disk issue."`

`       └── User can manually install via Discover`
```

**If apt itself is broken:**

```
`├── spike-update.service cannot fix this`

`├── User guided to recovery mode → dpkg --configure -a`

`├── Or: live USB → chroot → apt repair`

`└── See DISASTER-RECOVERY.md for full recovery flow`
```

**If disk is full during update:**

```
`├── apt detects insufficient space`

`├── Update aborts gracefully (no partial install)`

`├── Notification: "Not enough disk space to install updates.`

`│   Free up space and try again."`

`├── Settings → Advanced → Storage shows disk usage`

`└── No automatic cleanup (user decides what to delete)`
```

### Never Force Reboot

This is a non-negotiable design decision:

```
`├── Spike NEVER forces a reboot for updates`

`├── Spike NEVER schedules automatic reboots`

`├── Spike NEVER reboots outside of user-initiated action`

`├── Reboot notifications are gentle:`

`│   ├── "Security updates were installed. Please restart when`

`│   │   convenient to complete the update."`

`│   ├── Appears once after update, not repeated`

`│   ├── No countdown, no deadline, no urgency`

`│   ├── Can be dismissed`

`│   └── Does not reappear until next kernel update`

`└── User can restart whenever they want`
```

**Rationale:**

```
`├── Forced reboots lose unsaved work`

`├── Unexpected reboots destroy user trust`

`├── Users may have processes running (downloads, renders, prints)`

`├── Target users may not understand why their laptop restarted`

`└── The security benefit of immediate reboot is minimal compared`

`    to the trust cost`
```

## Secure Boot

### Policy

Spike supports Secure Boot but does not require it.

**Behavior:**

```
`├── If Secure Boot is enabled in firmware:`

`│   ├── Ubuntu's signed GRUB2 shim boots normally`

`│   ├── Ubuntu-signed kernel boots normally`

`│   ├── All standard functionality works`

`│   ├── Broadcom Wi-Fi (bcmwl): MOK enrollment required`

`│   │   ├── User prompted during installer: "Your Broadcom Wi-Fi`

`│   │   │   driver requires Secure Boot key enrollment."`

`│   │   ├── Password set during install, enrollment on next reboot`

`│   │   └── If enrollment missed: Wi-Fi doesn't work, fix via reinstall`

`│   └── NVIDIA proprietary driver: DKMS signs module with MOK`

`│       (only if user installs proprietary driver post-install)`

`├── If Secure Boot is disabled:`

`│   ├── Everything works without MOK enrollment`

`│   ├── No driver signing issues`

`│   └── Standard boot path`

`└── If Secure Boot status unknown:`

`    └── Installer detects and handles accordingly`
```

**User-facing information:**

```
`├── Settings → Advanced → Diagnostics → Security`

`│   ├── Secure Boot: Enabled / Disabled / Not Supported`

`│   ├── MOK list: (count of enrolled keys)`

`│   └── "Secure Boot \[protects/helps\] prevent unauthorized boot`

`│       software. It is \[enabled/disabled\] on your system."`

`└── No recommendation to enable/disable (user choice, hardware-specific)`
```

## Browser Security

### Firefox Configuration

Firefox is the default and only pre-installed browser. Shipped as Flatpak (sandboxed, isolated from system).

Spike applies default preferences (via Flatpak override):

**Privacy:**

```
`├── Enhanced Tracking Protection: Strict`

`│   ├── Blocks: tracking cookies, social trackers, crypto miners,`

`│   │   fingerprinting scripts`

`│   └── May break some sites (user can disable per-site)`

`├── Do Not Track: Enabled`

`├── Cookie deletion on close: Disabled (convenience over privacy)`

`├── HTTPS-Only Mode: Enabled`

`│   ├── All connections upgraded to HTTPS`

`│   └── If site doesn't support HTTPS: warning before connecting`

`└── DNS-over-HTTPS: Disabled by default`

`    ├── Some routers/ISPs break DoH`

`    ├── User can enable in Firefox settings`

`    └── If enabled: uses Cloudflare DNS (1.1.1.1)`
```

**Security:**

```
`├── Popup blocking: Enabled`

`├── Warning on dangerous downloads: Enabled`

`├── Add-on installation: Only from addons.mozilla.org (AMO)`

`├── DRM content: Enabled (Widevine for Netflix, Spotify)`

`│   ├── User prompted on first DRM site visit`

`│   └── Enables: Netflix, Spotify, Disney+, Amazon Prime`

`├── JavaScript: Enabled (required for modern web)`

`├── WebGL: Enabled (required for maps, 3D content)`

`└── Flash/Java/NPAPI plugins: NOT installed (deprecated, insecure)`
```

**Password management:**

```
`├── Firefox built-in password manager: Available`

`├── Master password: Optional (user can set)`

`├── No Spike-managed password storage`

`└── User responsible for their own password security`
```

**WebRender:**

```
`├── gfx.webrender.all = true`

`├── GPU-accelerated compositing enabled`

`├── Improves rendering performance and reduces CPU usage`

`└── Falls back to software rendering if GPU unsupported`
```

### Sandbox Isolation

Firefox Flatpak sandbox:

**What Firefox CAN access:**

```
`├── User's home directory (via portal — file picker)`

`├── Network (outgoing connections)`

`├── Audio (playback and recording — via portal permission)`

`├── Camera (via portal permission)`

`├── Clipboard (read/write)`

`├── Downloads directory (read/write)`

`└── Its own Flatpak data directory (~/.var/app/org.mozilla.firefox/)`
```

**What Firefox CANNOT access:**

```
`├── System configuration files (/etc/)`

`├── Other applications' data`

`├── Other users' home directories`

`├── Raw device access`

`├── System logs`

`└── Process listing`
```

**Permissions shown in:**

```
`├── Settings → Privacy → Application Permissions → Firefox`

`└── User can revoke: camera, microphone, files`
```

## Privacy Protection

### No Telemetry

Spike collects ZERO data from users. This is absolute.

No telemetry, no analytics, no crash reports, no usage statistics, no phone-home features, no background data transmission.

**Specifically disabled/removed:**

```
`├── Ubuntu telemetry (ubuntu-report) — not installed`

`├── Apport (crash reporter) — not installed`

`├── Whoopsie (error reporting) — not installed`

`├── Popularity Contest (popcon) — not installed`

`├── Landscape (Ubuntu management) — not installed`

`├── Canonical snap telemetry — snapd not installed`

`├── MOTD news (ads in terminal MOTD) — motd-news.timer disabled`

`├── Ubuntu Pro/Advantage — ubuntu-advantage-tools not installed`

`└── Cloud-init — not installed (not a cloud image)`
```

**Network connections Spike makes:**

```
`├── apt update (package list refresh) — to archive.ubuntu.com`

`├── Flatpak remote check — to flathub.org`

`├── NTP time sync — to pool.ntp.org`

`├── Connectivity check — to connectivity-check.ubuntu.com (or spike URL)`

`├── Captive portal detection — to configured check URL`

`└── That's it. Nothing else connects anywhere.`
```

All network connections are documented and auditable:

```
`├── Settings → Advanced → Diagnostics → Network Connections`

`│   (shows active outbound connections with process names)`

`└── Full connection log in /var/log/spike/connections.log (if enabled)`
```

### Local Data Privacy

Spike protects local user data through these mechanisms:

**Multi-user isolation:**

```
`├── Each user has separate home directory`

`├── Home directories: chmod 750 (owner+rwx, group+r-x, others=none)`

`├── Users cannot read other users' files by default`

`└── Only admin (sudo) can access other users' files`
```

**Recently used list:**

```
`├── Launcher's "Recently Used" section: cleared on logout`

`├── Prevents subsequent user from seeing what previous user opened`

`└── Stored in ~/.local/share/spike/launcher/recent.json (deleted on logout)`
```

**Notification history:**

```
`├── Per-user notification history`

`├── Stored in ~/.local/share/spike/notifications/history.json`

`├── Each user only sees their own notifications`

`└── History is cleared per user's retention setting`
```

**Browser data:**

```
`├── Firefox Flatpak data isolated per user`

`├── No shared browser data between users`

`└── User can clear browser data within Firefox`
```

**Thumbnail cache:**

```
`├── Per-user thumbnail cache (~/.cache/thumbnails/)`

`├── Not accessible by other users`

`└── Cleared on disk cleanup (Settings → Storage → Clear cache)`
```

**Trash:**

```
`├── Per-user trash (~/.local/share/Trash/)`

`├── Users cannot see other users' trashed files`

`└── Trash auto-empty: configurable (off by default)`
```

### Microphone and Camera Access

Microphone and camera access is controlled via Wayland portals:

**Microphone:**

```
`├── First access by an app: permission dialog`

`│   "\[App name\] wants to use your microphone. Allow?"`

`├── Permission stored per application`

`├── Revocable in Settings → Privacy → Application Permissions`

`├── Indicator: Microphone icon in panel when active`

`│   ├── Shows which app is using microphone`

`│   └── User can click to disable immediately`

`└── Applies to: Firefox (video calls), any Flatpak app requesting audio`
```

**Camera:**

```
`├── First access by an app: permission dialog`

`│   "\[App name\] wants to use your camera. Allow?"`

`├── Permission stored per application`

`├── Revocable in Settings → Privacy → Application Permissions`

`├── Indicator: Camera icon in panel when active`

`│   ├── Shows which app is using camera`

`│   └── User can click to disable immediately`

`└── Applies to: Firefox (video calls), any app requesting camera`
```

**Settings → Privacy → Application Permissions:**

| **App** | **Microphone** | **Camera** | **Files** |
| :-: | :-: | :-: | :-: |
| Firefox | \[Allowed\] | \[Ask\] | \[Allowed\] |
| VLC | \[Denied\] | \[N/A\] | \[Allowed\] |
| Cheese | \[N/A\] | \[Ask\] | \[Allowed\] |

\[Reset to defaults\]

**Permission states:**

```
`├── Allowed: App can access without asking`

`├── Ask: App prompts each time (or first time)`

`├── Denied: App cannot access (silently denied)`

`└── N/A: App hasn't requested this permission`
```

## Screen Locking

### Lock Screen Behavior

Spike Shell provides its own screen locker (not xscreensaver, not KDE's):

**Lock triggers:**

```
`├── Manual: Super+L (keyboard shortcut, configurable)`

`├── Session Menu → Lock Screen`

`├── Screen blank timeout (if "Lock when blanked" enabled)`

`├── Suspend/hibernate (if "Lock on suspend" enabled)`

`├── SDDM lock on session switch`

`└── No proximity-based locking`
```

**Lock screen appearance:**

```
`├── Dark background (\#1a1a2e)`

`├── Spike logo (subtle, centered)`

`├── Clock (large, centered)`

`├── Date (below clock)`

`├── Password field`

`├── "Unlock" button`

`├── "Switch User" button (if multiple users)`

`└── "Restart" and "Shut Down" buttons (accessible without unlock)`
```

**Lock screen security:**

```
`├── Password required to unlock`

`├── No "switch to terminal" escape (Ctrl+Alt+F2 disabled when locked)`

`├── No application switching when locked`

`├── No notification previews (shows "Notification" only, not content)`

`├── Notifications accessible after unlock`

`└── Emergency call: N/A (not a phone)`
```

**Automatic lock settings:**

```
`├── Settings → Power → Screen → Lock screen when blanked: \[On\]`

`├── Settings → Power → Lock screen on suspend: \[On\]`

`├── Settings → Power → Require password on wake: \[On\]`

`└── If auto-login enabled AND lock on suspend: screen still locks on wake`

`    (auto-login only bypasses SDDM, not the lock screen)`
```

## File Permissions

### Default Permissions

Spike uses standard Linux file permissions with umask 022:

**Home directory:** `/home/\[username\]/`

```
`├── Created with: chmod 750 (owner+rwx, group+r-x, others=none)`

`├── Owned by: \[username\]:\[username\]`

`└── Other users cannot access`
```

**User files (~/Documents, ~/Pictures, etc.):**

```
`├── Directories: chmod 755 (owner+rwx, group+rx, others+rx)`

`├── Files: chmod 644 (owner+rw, group+r, others+r)`

`└── Standard umask: 022`
```

**Sensitive files:**

```
`├── ~/.ssh/: chmod 700 (owner only)`

`├── ~/.gnupg/: chmod 700 (owner only)`

`├── ~/.config/spike/: chmod 700 (owner only)`

`├── NetworkManager connection files: /etc/NetworkManager/system-connections/`

`│   └── chmod 600 (root only — contains Wi-Fi passwords)`

`└── Notification history: ~/.local/share/spike/notifications/history.json`

`    └── chmod 600 (owner only)`
```

**Spike-installed system files:**

```
`├── /usr/share/spike/: chmod 755/644 (world-readable, system-owned)`

`├── /etc/spike/: chmod 755/644 (world-readable, system-owned)`

`├── /boot/.spike/boot-count: chmod 644 (readable, root-owned)`

`├── /var/lib/spike/: chmod 755 (readable, root-owned)`

`└── /var/log/spike/: chmod 755 (readable, root-owned)`
```

## Security Auditing

### Settings → Advanced → Diagnostics → Security

| **Section** | **Details** |
| :-: | :-: |
| **FIREWALL** | Status: ● Active | Default incoming: Deny | Default outgoing: Allow | Rules: 1 (mDNS) |
| **APPARMOR** | Status: ● Active (Enforcing) | Profiles loaded: 112 | Profiles enforcing: 112 | Profiles complaining: 0 |
| **SECURE BOOT** | Status: Enabled | MOK keys enrolled: 2 |
| **USER ACCOUNTS** | Root login: Disabled | Sudo group members: 1 (john) | Auto-login: Disabled | Password required on wake: Yes |
| **UPDATES** | Last security update: 2026-07-09 14:32 | Last update check: 2026-07-10 08:00 | Pending security updates: 0 | Pending non-security updates: 3 | Pending Flatpak updates: 1 |
| **PRIVACY** | Telemetry: Disabled (none collected) | Crash reporting: Disabled | Remote access: Disabled (SSH not installed) |
| **ACTIVE CONNECTIONS** | Process | Remote Address | Port   
firefox | 140.82.121.4 | 443   
pipewire | (local) | -   
NetworkManager | (local) | - |

\[Export security report\]

This dashboard gives users (and support staff) a complete security overview without any terminal commands. (Golden Rule 2 compliance)

## SSH

### Policy

SSH server is NOT installed by default.

**Rationale:**

```
`├── Target users don't know what SSH is`

`├── OpenSSH server adds attack surface (listening port)`

`├── Remote access is not a use case for rescued laptops`

`└── If needed: user can install via Discover (openssh-server)`
```

**If user installs openssh-server:**

```
`├── ufw does NOT automatically allow port 22`

`├── User must manually open port in Settings → Advanced → Firewall`

`├── Notification: "SSH server installed but firewall is blocking`

`│   port 22. Open Settings → Advanced → Firewall to allow`

`│   incoming SSH connections."`

`├── Root SSH login: disabled (PermitRootLogin no in sshd\_config)`

`├── Password authentication: enabled (no key-only enforcement)`

`└── X11 forwarding: disabled (Wayland, no X11 forwarding)`
```

**SSH client:**

```
`├── openssh-client IS installed (for outgoing SSH connections)`

`├── Useful for: developers, power users`

`└── No security risk (client-only, no listening port)`
```

## Network Services Audit

### Services That Listen On Ports

By default, Spike has NO services listening on external ports.

**Internal/listening services (localhost only):**

```
`├── PipeWire (audio server): /run/user/\[uid\]/pipewire-0`

`│   └── Unix socket, not network-accessible`

`├── DBus session bus: /run/user/\[uid\]/bus`

`│   └── Unix socket, not network-accessible`

`├── DBus system bus: /run/dbus/system\_bus\_socket`

`│   └── Unix socket, not network-accessible`

`├── systemd-resolved: 127.0.0.53:53`

`│   └── DNS resolver, localhost only`

`└── Avahi daemon: 0.0.0.0:5353 (mDNS)`

`    └── Allowed through firewall (printer discovery)`
```

**Network-facing services: NONE**

```
`├── No SSH (not installed)`

`├── No HTTP server (not installed)`

`├── No VNC/RDP (not installed)`

`├── No Samba/NFS (not installed)`

`├── No Telnet (not installed)`

`└── No custom daemons listening on TCP/UDP ports`
```

**Verification:**

```
`├── Settings → Diagnostics → Active Connections shows all`

`├── Command: ss -tlnp (for developers)`

`└── This can be verified by the user without terminal (Golden Rule 2)`
```

## Malware Protection

### Approach

Spike does NOT include antivirus software.

**Rationale:**

```
`├── Linux desktop malware is rare (not zero, but rare)`

`├── Traditional antivirus (ClamAV) is resource-heavy on Celeron`

`├── False positives cause user confusion and distrust`

`├── The primary malware vector is the browser (handled by sandboxing)`

`├── Flatpak sandboxing limits application damage`

`├── AppArmor confines system services`

`└── No executable email attachments (no email client installed by default)`
```

**Defense in depth:**

```
`├── Layer 1: User education (user guide covers safe browsing)`

`├── Layer 2: Firefox sandbox + Enhanced Tracking Protection`

`├── Layer 3: Flatpak application sandboxing`

`├── Layer 4: AppArmor service confinement`

`├── Layer 5: ufw firewall (no inbound connections)`

`├── Layer 6: No auto-execution of downloaded files`

`├── Layer 7: Automatic security updates`

`└── Layer 8: Standard file permissions (user isolation)`
```

**If a user wants antivirus:**

```
`├── ClamAV available via Discover (Flatpak or apt)`

`├── User guide: "Spike includes multiple layers of protection`

`│   against malicious software. Antivirus software is generally`

`│   not needed on Linux. If you want additional protection,`

`│   you can install ClamAV from Discover."`

`└── No nagware, no fear-mongering`
```

**Executable download protection:**

```
`├── Firefox warns on executable downloads (.deb, .rpm, .AppImage)`

`├── Dolphin does NOT auto-execute files`

`├── Double-clicking a .sh file opens it in Kate (text editor), not executes it`

`├── Running executables requires:`

`│   ├── Right-click → Properties → Permissions → "Allow executing`

`│   │   file as program"`

`│   └── Then double-click or right-click → Run`

`└── This deliberate friction prevents accidental execution`
```

## Kernel Security Features

### Enabled Features

Spike inherits Ubuntu's kernel security defaults:

**1. Kernel Address Space Layout Randomization (KASLR)**

```
`├── Enabled by default (CONFIG\_RANDOMIZE\_BASE=y)`

`├── Randomizes kernel memory layout at boot`

`└── Makes kernel exploits harder`
```

**2. SMEP (Supervisor Mode Execution Prevention)**

```
`├── Hardware feature (CPU-level)`

`├── Prevents kernel from executing user-space code`

`└── Enabled if CPU supports it (N4020 supports SMEP)`
```

**3. SMAP (Supervisor Mode Access Prevention)**

```
`├── Hardware feature (CPU-level)`

`├── Prevents kernel from reading user-space memory`

`└── Enabled if CPU supports it (N4020 supports SMAP)`
```

**4. seccomp**

```
`├── Filters system calls per process`

`├── Used by Flatpak (bubblewrap) and Firefox`

`└── Reduces attack surface for sandboxed apps`
```

**5. Yama ptrace restrictions**

```
`├── /proc/sys/kernel/yama/ptrace\_scope = 1`

`├── Processes can only ptrace their descendants`

`├── Prevents process inspection by unrelated programs`

`└── Ubuntu default, inherited by Spike`
```

**6. Hardened usercopy**

```
`├── Kernel validates memory regions during copy\_to\_user/copy\_from\_user`

`├── Prevents certain kernel exploitation techniques`

`└── Ubuntu default, inherited by Spike`
```

**7. Module signing (if Secure Boot enabled)**

```
`├── Kernel modules must be signed with trusted key`

`├── Ubuntu signs all in-tree modules`

`├── Third-party modules (Broadcom, NVIDIA) require MOK enrollment`

`└── Only enforced when Secure Boot is active`
```

### Kernel Parameters (Security-Relevant)

**Boot parameters set by Spike (see KERNEL.md for full list):**

```
`zswap.enabled=0                    → Prevents ZRAM conflict (memory)`

`transparent\_hugepage=madvise       → Prevents THP bloat (memory/performance)`
```

**Security parameters NOT modified by Spike (Ubuntu defaults kept):**

```
`├── slab\_merge (default on)`

`├── page\_alloc.shuffle (default on)`

`├── init\_on\_alloc (default on)`

`├── init\_on\_free (default on)`

`├── randomize\_kstack\_offset (default on)`

`└── These are all Ubuntu kernel defaults — Spike doesn't disable them`
```

**sysctl tunables (Spike customizations):**

**`/etc/sysctl.d/99-spike-network.conf`:**

```
*`\# IPv6 privacy extensions`*

`net.ipv6.conf.all.use\_tempaddr = 2`

`net.ipv6.conf.default.use\_tempaddr = 2`


*`\# SYN flood protection`*

`net.ipv4.tcp\_syncookies = 1`


*`\# Ignore ICMP broadcasts (smurf attacks)`*

`net.ipv4.icmp\_echo\_ignore\_broadcasts = 1`


*`\# Reverse path filtering`*

`net.ipv4.conf.all.rp\_filter = 1`

`net.ipv4.conf.default.rp\_filter = 1`


*`\# Log martian packets`*

`net.ipv4.conf.all.log\_martians = 1`


*`\# Ignore ARP redirects`*

`net.ipv4.conf.all.send\_redirects = 0`

`net.ipv4.conf.default.send\_redirects = 0`


*`\# Accept ICMP redirects (disabled)`*

`net.ipv4.conf.all.accept\_redirects = 0`

`net.ipv4.conf.default.accept\_redirects = 0`
```

## Security Checklist For Contributors

Before merging code, verify:

```
`□ Does this open a network port?`

`  → If yes, is it absolutely necessary? Default to not opening.`


`□ Does this execute user-supplied input without sanitization?`

`  → If yes: fix before merge. No exceptions.`


`□ Does this add a new background service?`

`  → If yes: justify the memory cost. Is it sandboxed?`


`□ Does this transmit data over the network?`

`  → If yes: is it a documented, user-visible connection?`

`  → If it's telemetry/analytics: REJECTED. No exceptions.`


`□ Does this weaken any existing security measure?`

`  → If yes: justify with threat model analysis.`


`□ Does this require elevated privileges?`

`  → If yes: does it use polkit (GUI) or sudo (terminal)?`

`  → Does it work without root for normal usage?`


`□ Does this handle user passwords or secrets?`

`  → If yes: never log, never store in plaintext, never transmit.`


`□ Does this add a new DBus interface?`

`  → If yes: is it on the session bus or system bus?`

`  → Are the permissions appropriate (not overly permissive)?`


`□ Does this trust input from untrusted sources?`

`  → If yes: validate and sanitize all input.`


`□ Is this code reachable from a web page (via browser/Firefox)?`

`  → If yes: ensure sandboxing is not weakened.`
```

## What This Document Does Not Cover

- **Firewall GUI controls and rule management:** See **NETWORKING.md** (Section: Firewall) 

- **Network firmware and driver configuration:** See **NETWORKING.md** (Section: Wi-Fi Firmware) 

- **Boot failure counter and kernel rollback:** See **BOOT-PROCESS.md** 

- **Memory pressure and earlyoom:** See **MEMORY.md** 

- **NVIDIA proprietary driver and MOK enrollment:** See **KERNEL.md** (Section: NVIDIA Driver/DKMS Flow) 

- **Privacy permissions (camera, microphone, file access):** See **PRIVACY.md** 

- **Disaster recovery procedures:** See **DISASTER-RECOVERY.md** 

- **Kernel parameters and sysctl tunables:** See **KERNEL.md** (Section: sysctl Tunables) 

- **Update schedule and spike-update.timer:** See **BOOT-PROCESS.md** (Section: systemd Timer Replacements) 

- **Installer security (password creation, root locking):** See **INSTALLER.md** (Step 4) 

- **Flatpak sandboxing and portal API:** See **ARCHITECTURE.md** and **MULTIMEDIA.md** 

🐕 BigRangaTech


