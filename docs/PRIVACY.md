# Spike Privacy

## Design Philosophy

Spike's privacy philosophy can be summarized in one sentence:

> The laptop belongs to the user. The data belongs to the user. Spike exists to serve, not to observe.

This means:

```
`├── No data leaves the device without explicit user consent`

`├── No background data collection of any kind`

`├── All logs stay local and are never transmitted`

`├── Application permissions are user-controlled and revocable`

`├── No advertising, no analytics, no tracking`

`└── Transparency: users can see exactly what connects where`
```

Privacy is not a toggle in Spike. It is the baseline. There is no "privacy mode" to enable because privacy is always on. There are only permissions to grant when the user chooses to share specific access with specific apps.

## Data Collection Policy

### What Spike Collects

Nothing.

Zero bytes. Zero metrics. Zero telemetry. Zero analytics. Zero crash reports. Zero usage statistics. Zero hardware surveys.

This is absolute and non-negotiable.

No "anonymous" statistics. No "opt-out" telemetry. No "we collect data to improve your experience." None of it.

If a future contributor proposes adding telemetry of any kind, the answer is no. This is a BDFL-level decision and is recorded in `DESIGN-DECISIONS.md`.

### Removed Components

The following Ubuntu data-collection components are NOT installed:

```
`ubuntu-report              → Hardware/instance reporting to Canonical`

`apport                      → Crash reporting (uploads crash data)`

`whoopsie                    → Error reporting daemon`

`popularity-contest         → Package popularity statistics`

`landscape-client           → Canonical management/monitoring`

`ubuntu-advantage-tools     → Ubuntu Pro (telemetry component)`

`motd-news                  → Fetches news/ads for terminal MOTD`

`cloud-init                  → Cloud instance setup (not applicable)`

`snapd                       → Snap includes Canonical telemetry`
```

All of these are stripped from the ISO at build time. None can be accidentally enabled.

### Network Connections Spike Makes

Spike makes exactly these outbound network connections:

**1. apt update (package list refresh)**

```
`├── Destination: archive.ubuntu.com (or configured mirror)`

`├── Data sent: Request for package lists (no user data)`

`├── Frequency: Every 6 hours (via spike-update.timer)`

`└── Includes: No user identifiers, no machine IDs`
```

**2. Flatpak remote check**

```
`├── Destination: flathub.org`

`├── Data sent: Request for app/update metadata`

`├── Frequency: Every 6 hours (via spike-update.timer)`

`└── Includes: No user identifiers`
```

**3. NTP time synchronization**

```
`├── Destination: pool.ntp.org (or configured NTP server)`

`├── Data sent: Time request packet (standard NTP protocol)`

`├── Frequency: On boot, then every 64 seconds (systemd-timesyncd default)`

`└── Includes: No user data (NTP protocol sends only timestamps)`
```

**4. Connectivity check**

```
`├── Destination: connectivity-check.ubuntu.com (or spike-branded URL)`

`├── Data sent: HTTP GET request (no body, no cookies)`

`├── Frequency: On network connection, then periodic`

`└── Includes: IP address (unavoidable — network requires it)`
```

**5. Captive portal detection**

```
`├── Destination: connectivity-check.ubuntu.com (redirect target)`

`├── Data sent: HTTP GET request`

`├── Frequency: On new network connection`

`└── Includes: IP address (unavoidable)`
```

That is the complete list. Nothing else connects anywhere. No Spike server receives user data. No analytics endpoint exists.

**Flathub privacy note:**

```
`├── When installing or updating a Flatpak application, Flathub's`

`│   server receives the application ID (e.g., "org.gimp.GIMP")`

`│   in the HTTP request`

`├── This is unavoidable (the server must know what to send)`

`├── Flathub does NOT receive: user identity, machine ID, or`

`│   usage statistics from Spike`

`├── Flathub may log the request (IP address, app ID, timestamp)`

`│   on their end — this is outside Spike's control`

`├── Mitigation: users concerned about Flathub tracking app`

`│   installs can use a VPN, or install apps from local .flatpak`

`│   files obtained via another method`

`└── Spike cannot prevent server-side logging by Flathub`
```

### Audit Trail

Users can verify that no unexpected connections are being made:

**Settings → Advanced → Diagnostics → Network Connections:**

```
`┌──────────────────────────────────────────────────┐`

`│  Active Network Connections                      │`

`│                                                  │`

`│  Process        Remote Address        Port  Type │`

`│  ───────────────┼────────────────────┼──────┼─────│`

`│  NetworkManager  —                    —     local│`

`│  systemd-resolve 127.0.0.53           53    DNS │`

`│  firefox        140.82.121.4          443   HTTPS│`

`│  firefox        151.101.1.69          443   HTTPS│`

`│                                                  │`

`│  \[Refresh\]  \[Export report\]                     │`

`│                                                  │`

`│  Connection Log (last 24 hours):                 │`

`│  ───────────────────────────────────────────────│`

`│  14:32  apt update → archive.ubuntu.com:443     │`

`│  14:32  flatpak remote-ls → flathub.org:443      │`

`│  14:30  NTP sync → pool.ntp.org:123              │`

`│  14:28  Connectivity check → 204.79.197.200:80   │`

`│  14:28  Network connected (Wi-Fi: Home WiFi)      │`

`│                                                  │`

`└──────────────────────────────────────────────────┘`

`├── Data source: ss -tunp (parsed, displayed in GUI)`

`├── Connection log: /var/log/spike/connections.log`

`└── Log retention: 7 days (configurable in Settings → Privacy)`
```

## Application Permissions

### Portal-Based Permission System

Spike uses `xdg-desktop-portal` for application permission management. This is the Wayland-native mechanism for controlling what applications can access.

**Permission categories:**

```
`├── Files (filesystem access)`

`├── Camera`

`├── Microphone`

`├── Screen (capture/recording)`

`├── Notifications`

`├── Location (if GPS or geolocation available)`

`└── Background (run in background)`
```

**How it works:**

```
`├── Application requests access via portal DBus interface`

`├── xdg-desktop-portal-kde handles the request`

`├── Spike-themed dialog appears:`

`│   "Firefox wants to use your camera. Allow?"`

`│   \[Allow\] \[Deny\] \[Always allow\]`

`├── User's choice is stored per-application`

`├── Stored in ~/.local/share/flatpak/permissions/`

`└── Revocable at any time in Settings → Privacy`
```

**Permission flow:**

```
`┌──────────────┐     ┌─────────────────┐     ┌──────────┐`

`│   App        │────▶│  xdg-desktop-   │────▶│  User    │`

`│   requests   │     │  portal-kde     │     │  dialog  │`

`│   access     │     │  (intercepts)   │     │          │`

`└──────────────┘     └────────┬────────┘     └────┬─────┘`

`                              │                     │`

`                              │  ◀── response ──────┘`

`                              ▼`

`                     ┌─────────────────┐`

`                     │  Permission     │`

`                     │  stored in      │`

`                     │  permissions    │`

`                     │  store          │`

`                     └─────────────────┘`
```

### Permission States

Each permission for each application has one of these states:

```
`\[Allowed\]  → App can access without asking`

`\[Ask\]      → App prompts user each time (or first time only)`

`\[Denied\]   → App cannot access (silently denied, no prompt)`

`\[N/A\]      → App hasn't requested this permission (not applicable)`
```

Permission dialog options:

```
`├── \[Allow\]       → Permission granted for this session`

`├── \[Always Allow\] → Permission granted permanently (stored)`

`├── \[Deny\]        → Permission denied for this session`

`└── \[Always Deny\]  → Permission denied permanently (stored, app won't ask again)`
```

If user selects "Allow" (session only):

```
`├── Permission resets on app restart`

`├── App will ask again next launch`

`└── Useful for one-time access`
```

If user selects "Always Allow" or "Always Deny":

```
`├── Permission stored permanently`

`├── App does not ask again`

`├── User can change in Settings → Privacy → Application Permissions`

`└── Reset to defaults available`
```

### Settings → Privacy → Application Permissions

```
`┌──────────────────────────────────────────────────────────────┐`

`│  Application Permissions                                     │`

`│                                                              │`

`│  App              │ Microphone │ Camera │ Files │ Screen    │`

`│  ─────────────────┼────────────┼────────┼───────┼──────────  │`

`│  Firefox          │ \[Allowed\]  │ \[Ask\]  │ \[Allowed\] │ \[Ask\]  │`

`│  VLC              │ \[Denied\]   │ \[N/A\]  │ \[Allowed\] │ \[N/A\]  │`

`│  LibreOffice      │ \[N/A\]      │ \[N/A\]  │ \[Ask\]     │ \[N/A\]  │`

`│  Spectacle        │ \[N/A\]      │ \[N/A\]  │ \[Allowed\] │ \[Allowed\]│`

`│  Discover         │ \[N/A\]      │ \[N/A\]  │ \[N/A\]     │ \[N/A\]  │`

`│                                                              │`

`│  Click any permission to change it.                          │`

`│  Options: Allowed, Ask, Denied                              │`

`│                                                              │`

`│  \[Reset all to defaults\]                                    │`

`│                                                              │`

`└──────────────────────────────────────────────────────────────┘`
```

**Default permissions (pre-configured by Spike):**

**Firefox (Flatpak):**

```
`├── Microphone: Ask (for video calls)`

`├── Camera: Ask (for video calls)`

`├── Files: Allowed (downloads, uploads)`

`├── Screen: Ask (for screen sharing in calls)`

`└── Notifications: Allowed (download complete, etc.)`
```

**VLC (Flatpak):**

```
`├── Microphone: Denied (media player doesn't need mic)`

`├── Camera: Denied (media player doesn't need camera)`

`├── Files: Allowed (open media files)`

`├── Screen: Denied`

`└── Notifications: Denied`
```

**LibreOffice (Flatpak):**

```
`├── Microphone: Denied`

`├── Camera: Denied`

`├── Files: Ask (user should choose which files to open)`

`├── Screen: Denied`

`└── Notifications: Denied`
```

**Spectacle (system package):**

```
`├── Files: Allowed (save screenshots)`

`├── Screen: Allowed (capture screenshots — core function)`

`└── Microphone/Camera: Denied`
```

**Applications installed by user (after install):**

```
`├── All permissions default to: Ask`

`├── User prompted on first access attempt`

`├── No blanket permissions for newly installed apps`

`└── User maintains full control`
```

## Camera and Microphone Indicators

### Privacy Indicators

When an application is actively using the camera or microphone, Spike Shell displays an indicator in the panel:

**Camera active:**

```
`├── Camera icon (📷) appears in panel, right zone`

`├── Tooltip: "Camera is in use by Firefox"`

`├── Click: Shows details + "Disable camera" button`

`└── "Disable camera" immediately revokes access (app receives error)`
```

**Microphone active:**

```
`├── Microphone icon (🎙️) appears in panel, right zone`

`├── Tooltip: "Microphone is in use by Firefox"`

`├── Click: Shows details + "Mute microphone" button`

`└── "Mute microphone" immediately revokes access (app receives silence)`
```

**Both active:**

```
`├── Both icons displayed`

`├── Combined tooltip: "Camera and microphone in use by Firefox"`

`└── Individual disable buttons for each`
```

These indicators are **NON-NEGOTIABLE**:

```
`├── They cannot be hidden by applications`

`├── They cannot be disabled in Settings`

`├── They appear whenever hardware is in use`

`├── Only system-level kill switch (Airplane Mode) overrides`

`└── This is a user trust mechanism — users must always know`

`    when their camera or microphone is active`
```

### Kill Switch

Hardware kill switch for camera and microphone:

**Settings → Privacy → Camera & Microphone:**

```
`┌──────────────────────────────────────────────────┐`

`│  Camera & Microphone                              │`

`│                                                  │`

`│  Camera                                           │`

`│  ──────                                          │`

`│  Status: ● Available                              │`

`│  Currently in use by: (none)                      │`

`│                                                  │`

`│  \[Disable camera\]                                 │`

`│  When disabled, no application can access the     │`

`│  camera. A camera icon with a slash will appear   │`

`│  in the panel.                                   │`

`│                                                  │`

`│  Microphone                                       │`

`│  ───────────                                     │`

`│  Status: ● Available                              │`

`│  Currently in use by: (none)                      │`

`│                                                  │`

`│  \[Disable microphone\]                             │`

`│  When disabled, no application can access the     │`

`│  microphone. A microphone icon with a slash will  │`

`│  appear in the panel.                            │`

`│                                                  │`

`│  \[Disable both\] (equivalent to Airplane Mode     │`

`│   for audio/video devices only)                   │`

`│                                                  │`

`└──────────────────────────────────────────────────┘`
```

Kill switch behavior:

```
`├── Disabling camera: Unloads uvcvideo module`

`│   └── Applications receive "camera not available" error`

`├── Disabling microphone: Mutes all capture devices in PipeWire`

`│   └── Applications receive silence (no error, no data)`

`├── Re-enabling: Reloads module / unmutes devices`

`├── State persists across reboots`

`└── Panel shows disabled indicator until re-enabled`
```

Relationship to Airplane Mode:

```
`├── Airplane Mode disables Wi-Fi and Bluetooth (radio)`

`├── Camera/microphone kill switch is separate (not radio)`

`├── Both can be active simultaneously`

`└── Disabling camera/mic does NOT disable Wi-Fi`
```

## Browser Privacy

### Firefox Privacy Configuration

Spike configures Firefox with strong privacy defaults:

**Enhanced Tracking Protection: Strict**

```
`├── Blocks:`

`│   ├── Tracking cookies (cross-site)`

`│   ├── Social media trackers (Facebook, Twitter, LinkedIn)`

`│   ├── Crypto mining scripts`

`│   ├── Fingerprinting scripts`

`│   └── Tracker-containing scripts`

`├── Effect: Some websites may break (tracking-dependent features)`

`├── Per-site exception: User can click shield icon → "Disable for this site"`

`└── Cannot be globally disabled (user must do per-site)`
```

**HTTPS-Only Mode: Enabled**

```
`├── All HTTP requests upgraded to HTTPS`

`├── If site doesn't support HTTPS:`

`│   ├── Warning: "This site doesn't support HTTPS. Continue?"`

`│   └── User must explicitly accept the risk`

`└── Prevents downgrade attacks on public Wi-Fi`
```

**Do Not Track: Enabled**

```
`├── Sends DNT: 1 header`

`├── Most sites ignore it (acknowledged)`

`└── Supplemental measure, not primary defense`
```

**Cookie handling:**

```
`├── Cookies accepted from visited sites`

`├── Third-party cookies: Blocked (via ETP Strict)`

`├── Cookie deletion on close: Disabled (convenience)`

`└── User can enable in Firefox settings if desired`
```

**URL tracking stripping:**

```
`├── Strips known tracking parameters from URLs`

`│   (utm\_source, fbclid, gclid, etc.)`

`├── Reduces cross-site tracking via link parameters`

`└── Built into Firefox ETP Strict mode`
```

**DNS-over-HTTPS (DoH):**

```
`├── Disabled by default`

`├── Some routers and ISPs interfere with DoH`

`├── User can enable in Firefox settings → Privacy → DNS over HTTPS`

`├── If enabled: Uses Cloudflare DNS (1.1.1.1) by default`

`└── User can choose provider: Cloudflare, NextDNS, or custom`
```

### Firefox Data Storage

Firefox (Flatpak) stores data in:

```
`├── ~/.var/app/org.mozilla.firefox/.mozilla/firefox/`

`├── Isolated from system by Flatpak sandbox`

`├── Per-user (each user has separate Firefox data)`

`├── Not accessible by other applications (Flatpak isolation)`

`└── Cleared if user uninstalls Firefox (Flatpak removes app data)`
```

What Firefox stores locally:

```
`├── Browsing history`

`├── Bookmarks`

`├── Saved passwords (encrypted with Firefox master password, if set)`

`├── Cookies`

`├── Cache`

`├── Extensions and their data`

`└── Preferences`
```

What Firefox may sync (if user enables Firefox Sync):

```
`├── User must explicitly create Firefox account and enable sync`

`├── Data goes to Mozilla servers, not Spike servers`

`├── Spike has no involvement in Firefox Sync`

`├── Sync is user's choice — not enabled by default`

`└── Privacy implications are Firefox's responsibility, not Spike's`
```

### Alternative Browsers

Spike does not pre-install Chromium, Chrome, Brave, or other browsers. Only Firefox is included.

If user installs another browser via Discover (Flatpak):

```
`├── Same Flatpak sandbox permissions apply`

`├── Same portal-based permission system applies`

`├── Camera/microphone/screen access: Ask by default`

`├── No Spike-level privacy configuration applied to non-Firefox browsers`

`├── User responsible for configuring their browser's privacy settings`

`└── Firefox privacy prefs are Spike-managed because it's the default browser`
```

Spike does NOT recommend or endorse specific alternative browsers. The user guide notes: *"Firefox is pre-configured with strong privacy protections. If you install a different browser, you'll need to review its privacy settings yourself."*

## Location Services

### Geolocation

Spike uses limited geolocation for two purposes:

**1. Installer timezone detection (IP-based geolocation)**

```
`├── Happens during installation only`

`├── Uses IP address to determine approximate location`

`├── Data sent: IP address (unavoidable — network requires it)`

`├── Response: Approximate timezone (e.g., "Pacific/Auckland")`

`├── No precise location, no GPS, no address`

`└── User confirms timezone manually (can override)`
```

**2. Night Light sunset/sunrise schedule**

```
`├── If user enables "Sunset to sunrise" in Night Light settings`

`├── Uses approximate location for sunset/sunrise times`

`├── Source: geoclue2 (geolocation service)`

`├── geoclue2 can use:`

`│   ├── Wi-Fi network proximity (Mozilla Location Service)`

`│   ├── IP address (approximate)`

`│   └── GPS (if hardware present — rare on laptops)`

`└── No precise tracking, only approximate time zone level location`
```

**Settings → Privacy → Location:**

```
`┌──────────────────────────────────────────────────┐`

`│  Location Services                                │`

`│                                                  │`

`│  Location services are used for:                  │`

`│  • Night Light sunset/sunrise scheduling          │`

`│  • Time zone detection (during installation)       │`

`│                                                  │`

`│  \[x\] Allow location services                     │`

`│                                                  │`

`│  Location accuracy: \[Approximate (city-level)\]    │`

`│  (Precise location is not available on this       │`

`│   hardware — no GPS detected)                     │`

`│                                                  │`

`│  Applications that have requested location:        │`

`│  (none)                                           │`

`│                                                  │`

`└──────────────────────────────────────────────────┘`
```

If user disables location services:

```
`├── Night Light: User must set manual schedule`

`├── Installer: User must select timezone manually`

`├── geoclue2 daemon: does not start`

`└── Applications cannot request location (denied at portal level)`
```

No GPS hardware:

```
`├── Most Celeron laptops have no GPS`

`├── Location accuracy is city-level at best (IP-based)`

`├── No street-level tracking is possible`

`└── This is clearly communicated to the user`
```

## Data Retention

### What Spike Stores Locally

Spike stores the following data on the user's device:

**System logs:**

```
`├── Location: /var/log/ (journald)`

`├── Retention: 7 days (journald SystemMaxUse=200M, MaxRetentionSec=7day)`

`├── Contents: Boot messages, service status, errors`

`├── Contains: No personal data (no browsing history, no file contents)`

`├── Never transmitted off-device`

`└── User can clear in Settings → Privacy → Clear Logs`
```

**Spike-specific logs:**

```
`├── /var/log/spike/updates.log → Update history (package names, timestamps)`

`├── /var/log/spike/connections.log → Network connection history`

`├── Retention: 7 days (configurable: 1-30 days)`

`├── Contains: Package names, network addresses, timestamps`

`├── Never transmitted off-device`

`└── User can clear in Settings → Privacy → Clear Spike Logs`
```

**Notification history:**

```
`├── ~/.local/share/spike/notifications/history.json`

`├── Retention: 3 days (configurable: 1-31 days)`

`├── Max count: 500 (configurable: 10-1000)`

`├── Contains: Notification text (from apps), timestamps, app names`

`├── Per-user (not accessible by other users)`

`├── Never transmitted off-device`

`└── User can clear in Settings → Notifications → Clear All`
```

**Launcher recently used:**

```
`├── ~/.local/share/spike/launcher/recent.json`

`├── Retention: Until logout (cleared on every logout)`

`├── Contains: App names launched, timestamps`

`├── Never transmitted off-device`

`└── Auto-cleared (no manual action needed)`
```

**Battery health history:**

```
`├── /var/lib/spike/power/battery-health.json`

`├── Retention: Unlimited (until user resets)`

`├── Contains: Monthly battery health readings (percentage, date)`

`├── Never transmitted off-device`

`└── User can clear in Settings → Power → Battery → Clear History`
```

**Clipboard history:**

```
`├── NOT stored — Spike does not keep clipboard history`

`├── Clipboard contents: in-memory only (RAM)`

`├── Cleared on: reboot, logout, or user-initiated clear`

`├── No persistent clipboard storage`

`└── No clipboard manager installed (deliberate privacy choice)`
```

### Settings → Privacy → Data Retention

```
`┌──────────────────────────────────────────────────┐`

`│  Data Retention                                   │`

`│                                                  │`

`│  SYSTEM LOGS                                      │`

`│  ──────────                                      │`

`│  Retention: \[7 days\] ▼ (1, 3, 7, 14, 30 days)    │`

`│  Current size: 24 MB                              │`

`│  \[Clear system logs now\]                          │`

`│                                                  │`

`│  NETWORK CONNECTION LOG                           │`

`│  ────────────────────────                        │`

`│  Retention: \[7 days\] ▼ (1, 3, 7, 14, 30 days)    │`

`│  Current size: 1.2 MB                             │`

`│  \[Clear connection log now\]                       │`

`│                                                  │`

`│  NOTIFICATION HISTORY                             │`

`│  ──────────────────                              │`

`│  Retention: \[3 days\] ▼ (1, 3, 7, 14, 31 days)    │`

`│  Max notifications: \[500\] ▼ (10, 50, 100, 500,   │`

`│                       1000)                      │`

`│  Current count: 23                                │`

`│  \[Clear all notifications now\]                    │`

`│                                                  │`

`│  BATTERY HEALTH HISTORY                           │`

`│  ────────────────────────                        │`

`│  \[Clear battery history\]                          │`

`│                                                  │`

`│  LAUNCHER RECENTLY USED                            │`

`│  ──────────────────────                           │`

`│  Cleared on every logout                          │`

`│  \[Clear now\]                                      │`

`│                                                  │`

`│  CACHE                                            │`

`│  ─────                                           │`

`│  Thumbnail cache: 12 MB                           │`

`│  Package cache: 145 MB                            │`

`│  Flatpak cache: 23 MB                             │`

`│  \[Clear all caches\]                              │`

`│                                                  │`

`└──────────────────────────────────────────────────┘`
```

All retention settings are per-user (except system logs which are system-wide).

## Multi-User Privacy

### User Isolation

Spike supports multiple user accounts with strong isolation:

**Home directory isolation:**

```
`├── Each user: /home/\[username\]/ with permissions 750`

`├── Owner: \[username\]:\[username\]`

`├── Other users cannot read, write, or list files`

`├── Even if two users are in the sudo group`

`└── Only root (via sudo) can access other users' files`
```

**External drive caveat:**

```
`├── USB drives formatted as FAT32 or exFAT do NOT support`

`│   Linux file permissions`

`├── Files on these drives are readable by all users`

`│   (no per-user isolation)`

`├── ext4 USB drives DO support permissions (isolated per user)`

`├── Mitigation: user can format USB drive as ext4 if privacy`

`│   between users is required`

`└── User guide notes this limitation`
```

**Notification isolation:**

```
`├── Each user has their own notification history`

`├── Notifications sent to the active session only`

`├── User A's notifications are not visible to User B`

`└── Notification daemon runs per-session (not system-wide)`
```

**Launcher isolation:**

```
`├── Favorites: per-user (~/.config/spike/launcher/favorites.json)`

`├── Recently used: per-user, cleared on each user's logout`

`├── New app badges: per-user`

`└── User A's app history is invisible to User B`
```

**Settings isolation:**

```
`├── All user preferences stored in ~/.config/spike/`

`├── Wallpaper, panel position, brightness: per-user`

`├── System-wide settings (network, firewall): admin-only`

`└── Non-admin users cannot change system settings`
```

**Browser isolation:**

```
`├── Firefox Flatpak data: per-user`

`├── Each user has separate: history, bookmarks, cookies, passwords`

`├── No shared browser state between users`

`└── Uninstalling Firefox removes all users' Firefox data`
```

**Clipboard isolation:**

```
`├── Clipboard is per-session`

`├── When user switches (SDDM user switch), clipboard is cleared`

`├── User A's clipboard contents are not available to User B`

`└── This prevents clipboard-based data leakage between users`
```

### User Switching

When a user switches to another account (without logging out):

```
`├── SDDM user switch (not full logout)`

`├── Current user's session is suspended (not terminated)`

`├── Lock screen activates for current user`

`├── SDDM shows login screen for new user`

`├── New user logs in → their session starts`

`├── Original user's session: locked, suspended, inaccessible`

`└── Original user can return by switching back and unlocking`
```

Security implications:

```
`├── Suspended session's RAM retains state (files, browser tabs)`

`├── If the new user has sudo: theoretically can access original user's files`

`├── This is standard Linux behavior (admin can access all files)`

`├── Mitigation: Spike defaults to single-user (most target users)`

`└── Multi-user systems should have trusted users only`
```

Guest session:

```
`├── NOT available`

`├── No guest login option at SDDM`

`├── No temporary account creation`

`└── Rationale: Guest sessions add complexity and potential data leakage`
```

## Network Privacy

### MAC Address

MAC address handling:

```
`├── Default behavior: Real MAC address used (cloned-mac-address=preserve)`

`└── Why not randomize:`

`    ├── Some older consumer routers reject randomized MAC addresses`

`    ├── Network whitelisting (MAC filtering) is common on older routers`

`    ├── Randomized MAC addresses can confuse DHCP assignments`

`    ├── Target users likely have simple home networks`

`    └── Randomized MAC doesn't provide meaningful privacy on home network`
```

Per-network MAC randomization (optional):

```
`├── Settings → Network → Advanced → MAC Randomization`

`├── \[ \] Randomize MAC address on public networks`

`├── If enabled: generates random MAC for networks marked "Public"`

`├── Does NOT randomize on networks marked "Home" or "Trusted"`

`├── Random MAC regenerated per network (stable per SSID)`

`└── Useful on public Wi-Fi (prevents cross-network tracking)`
```

### DNS Privacy

Default DNS:

```
`├── Router-provided DNS via DHCP (most common)`

`├── Fallback: 1.1.1.1, 8.8.8.8 (if router doesn't provide DNS)`

`└── systemd-resolved caches queries locally`
```

DNS-over-HTTPS (DoH):

```
`├── Not enabled system-wide (router/ISP compatibility concerns)`

`├── Available in Firefox (user can enable)`

`├── Future consideration: system-wide DoH if compatibility improves`

`└── User guide explains DoH benefits for advanced users`
```

DNS-over-TLS (DoT):

```
`├── systemd-resolved supports DoT`

`├── Disabled by default (DNSOverTLS=no)`

`├── Can be enabled in Settings → Network → Advanced → DNS over TLS`

`├── Uses server-provided certificate (no manual certificate management)`

`└── If DoT fails: falls back to plaintext DNS (no breakage)`
```

DNS leak prevention:

```
`├── All DNS queries go through systemd-resolved`

`├── No application can bypass the system DNS resolver`

`├── Flatpak applications use the system DNS (no alternative path)`

`└── This prevents DNS leaks that could reveal browsing activity`
```

### VPN Privacy

VPN support: See `NETWORKING.md` for protocol details.

Privacy aspects of VPN usage:

```
`├── When VPN is connected:`

`│   ├── All outbound traffic routes through VPN tunnel`

`│   ├── DNS queries route through VPN (prevents ISP DNS snooping)`

`│   ├── ISP can see: VPN connection exists (but not contents)`

`│   ├── VPN provider can see: all traffic (depends on provider policy)`

`│   └── Spike does not log VPN traffic`

`├── VPN kill switch (optional):`

`│   ├── Settings → Network → VPN → \[connection\] → Advanced`

`│   ├── Blocks all non-VPN traffic when VPN is disconnected`

`│   ├── Prevents data leakage during VPN disconnection`

`│   └── Implemented via ufw rules (deny outgoing when VPN down)`

`└── Spike does not recommend specific VPN providers`

`    └── User guide: "If you want a VPN, research providers carefully.`

`        Look for no-logging policies and independent audits."`
```

Spike and VPNs:

```
`├── Spike has no affiliation with any VPN provider`

`├── No VPN is pre-configured`

`├── No VPN referral links or affiliate codes anywhere`

`└── VPN configuration is entirely user-driven`
```

### Printer Privacy

Network printers can log print jobs:

```
`├── When printing to a network printer (via mDNS/CUPS):`

`│   ├── The printer receives: document name, print time, user name`

`│   ├── The printer may store: print logs on its internal storage`

`│   └── Network administrators can access these logs`

`├── Spike does not control what the printer logs`

`├── Mitigation: user aware of network printer logging via user guide`

`└── Local USB printers: same behavior (printer may log internally)`
```

This is not a Spike-specific issue — all operating systems have this characteristic. It is documented here for completeness and transparency.

## Cloud Services

### Spike's Relationship To Cloud

Spike does NOT include any cloud integration.

**No cloud storage sync:**

```
`├── No Dropbox, Google Drive, OneDrive integration`

`├── No auto-sync of files to any cloud service`

`├── User can install cloud clients via Discover if desired`

`└── Spike has no cloud account requirement`
```

**No cloud backup:**

```
`├── No automatic cloud backup of user files`

`├── Backup is user's responsibility (USB drive recommended)`

`├── User guide covers manual backup to USB`

`└── See DISASTER-RECOVERY.md for backup procedures`
```

**No cloud authentication:**

```
`├── No Microsoft Account, Google Account, or Apple ID required`

`├── No cloud-based login`

`├── All authentication is local (username + password)`

`└── No "sign in with..." prompts anywhere in the system`
```

**No cloud settings sync:**

```
`├── Settings do not sync across devices`

`├── Each Spike installation is independent`

`├── If user has two Spike laptops: each is configured separately`

`└── No Spike cloud account exists to sync to`
```

### Third-Party Cloud Clients

If a user installs a cloud storage client (e.g., Dropbox) via Discover:

```
`├── Treated as a regular Flatpak application`

`├── Standard Flatpak sandbox permissions apply`

`├── User must grant filesystem access via portal`

`├── Cloud client can only access files the user explicitly grants`

`├── Spike does not integrate with the cloud client in any way`

`└── Privacy of cloud-stored data is the cloud provider's responsibility`
```

Spike does not recommend or endorse specific cloud providers. The user guide notes: *"Cloud storage is not built into Spike. If you want cloud backup, you can install a cloud storage app from Discover. Research the privacy policy of any cloud service before trusting them with your files."*

## User-Controllable Privacy Settings

### Settings → Privacy (Main Page)

```
`┌──────────────────────────────────────────────────┐`

`│  Privacy                                          │`

`│                                                  │`

`│  DATA COLLECTION                                  │`

`│  ───────────────                                 │`

`│  Telemetry: ● Disabled (Spike collects no data)   │`

`│  Crash reporting: ● Disabled                      │`

`│  Usage statistics: ● Disabled                     │`

`│  (These cannot be enabled — privacy is the       │`

`│   default state, not a setting)                   │`

`│                                                  │`

`│  APPLICATION PERMISSIONS                          │`

`│  ────────────────────────                        │`

`│  \[Manage application permissions\]                │`

`│  (Camera, microphone, files, screen per app)     │`

`│                                                  │`

`│  CAMERA & MICROPHONE                              │`

`│  ────────────────────────                        │`

`│  Camera: ● Available                              │`

`│  \[Disable camera\]                                 │`

`│  Microphone: ● Available                          │`

`│  \[Disable microphone\]                             │`

`│                                                  │`

`│  LOCATION SERVICES                                │`

`│  ──────────────────                              │`

`│  \[x\] Allow location services                     │`

`│  Accuracy: Approximate (city-level)              │`

`│                                                  │`

`│  BROWSER PRIVACY                                  │`

`│  ───────────────                                 │`

`│  Firefox tracking protection: Strict              │`

`│  HTTPS-Only mode: Enabled                         │`

`│  DNS-over-HTTPS: Disabled                         │`

`│  \[Open Firefox privacy settings\]                  │`

`│                                                  │`

`│  DATA RETENTION                                  │`

`│  ───────────────                                 │`

`│  \[Configure log retention\]                        │`

`│  \[Configure notification retention\]               │`

`│  \[Clear all local data\]                           │`

`│                                                  │`

`│  NETWORK PRIVACY                                  │`

`│  ───────────────                                 │`

`│  MAC randomization: \[Off\] ▼                      │`

`│  DNS-over-TLS: \[Off\] ▼                            │`

`│  \[View network connections\]                       │`

`│  \[View connection log\]                            │`

`│                                                  │`

`└──────────────────────────────────────────────────┘`
```

## Privacy vs Security Distinction

This document (`PRIVACY.md`) covers:

> "Who is watching the user?"

```
`├── Is Spike collecting data? No.`

`├── Is the browser tracking the user? Mitigated.`

`├── Can apps access camera/mic without consent? No.`

`└── Is the user's location being tracked? Minimized.`
```

`SECURITY.md` covers:

> "Who is attacking the system?"

```
`├── Is the firewall blocking external threats? Yes.`

`├── Are applications confined (AppArmor/Flatpak)? Yes.`

`├── Is the system patched against vulnerabilities? Yes.`

`└── Are user files protected from other users? Yes.`
```

**Overlap areas (covered in both, from different angles):**

```
`├── Application sandboxing: Security (containment) + Privacy (data access)`

`├── Browser configuration: Security (malware) + Privacy (tracking)`

`├── Network connections: Security (attack surface) + Privacy (data leaving)`

`└── User isolation: Security (privilege escalation) + Privacy (data visibility)`
```

## What This Document Does Not Cover

- **Firewall configuration and rules:** See `NETWORKING.md` (Section: Firewall) 

- **AppArmor and system hardening:** See `SECURITY.md` (Section: AppArmor) 

- **Automatic security updates:** See `SECURITY.md` (Section: Automatic Updates) 

- **Sandboxing technical implementation (Flatpak, bubblewrap):** See `ARCHITECTURE.md` 

- **VPN protocols and configuration:** See `NETWORKING.md` (Section: VPN) 

- **Notification daemon internals (history storage):** See `DESKTOP.md` (Notification Daemon) 

- **Disaster recovery and data backup:** See `DISASTER-RECOVERY.md` 

- **Firefox media tuning (VA-API, AV1):** See `MULTIMEDIA.md` 

- **Camera/microphone indicator applet UI:** See `DESKTOP.md` (Tray Applets) 

- **Connection log and diagnostics:** See `DESKTOP.md` (Settings → Diagnostics) 

🐕 BigRangaTech


