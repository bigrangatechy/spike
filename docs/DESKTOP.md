# Spike Desktop

## Purpose

This document specifies the Spike Shell — the custom desktop environment that defines the user experience. Spike Shell is the primary development effort of the project. It replaces a traditional desktop environment (like Plasma or GNOME) with a purpose-built, cohesive shell designed for beginners on older hardware.

## Overview

Spike Shell is built on Qt and runs as a Wayland client under KWin. It does not depend on plasma-workspace, plasma-session, Akonadi, Baloo, or any other Plasma infrastructure. All shell components communicate with system services through stable DBus interfaces.

Spike Shell consists of 20 components:

```
├── Panel                    (applet host, layout manager)
├── Launcher                 (application launcher, kickoff-style)
├── Notification Daemon      (popups, history, tray badge)
├── Settings Panel           (custom pages + KDE KCM loader)
├── Session Manager          (login, logout, shutdown, autostart)
├── Theme Engine             (colors, styles, icons, fonts)
└── Tray Applets (14 total, 4 conditional):
    ├── Network              (NetworkManager DBus)
    ├── Volume               (PipeWire DBus)
    ├── Battery              (UPower DBus)
    ├── Brightness           (/sys/class/backlight)
    ├── Notifications        (internal notify daemon)
    ├── Removable Devices    (udisks2 DBus)
    ├── Update Notifier      (apt + Flatpak polling)
    ├── Clock / Calendar     (system time, systemd-timesyncd)
    ├── Session Menu         (systemd loginctl)
    ├── Night Light          (KWin gamma)
    ├── Bluetooth            (BlueZ DBus — conditional)
    ├── Airplane Mode        (rfkill — conditional)
    ├── Keyboard Layout      (libinput — conditional)
    └── (4th conditional — determined during development)
```

## Technology Stack

| **Component** | **Technology** |
| :-: | :-: |
| Language | C++ (Qt6) |
| UI Toolkit | Qt Widgets (not QML — lower memory, deterministic layout) |
| Compositor | KWin (standalone Wayland) |
| IPC | DBus (system + session bus) |
| Theme | Custom Qt stylesheet + KWin decoration config |
| Windowing | Layer-shell protocol (wlr-layer-shell) for panel positioning |

### Why Qt Widgets, Not QML

Qt Widgets has a lower memory footprint and more predictable rendering behavior than Qt Quick/QML. QML's JavaScript engine and scene graph add overhead that Spike can't afford on 4GB systems. Qt Widgets also has richer standard widget set, meaning less custom drawing code.

## Panel

### Purpose

The panel is the horizontal bar that sits at the bottom (default) or top of the screen. It hosts applets, the launcher button, the clock, and the system tray. It is the primary interaction surface for the user.

### Layout

```
┌──────────────────────────────────────────────────────────────────┐
│                                                                  │
│  [Spike]  [launcher]  │ applets │     │ clock  │ tray applets   │
│  button   button       │         │     │        │                │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
← Left zone ──────→ ← Center ──→ ← Right zone ──────────────→
```

Three zones: left, center, right. Each zone contains configurable applet slots. Default layout:

**Left zone:**

```
├── Spike button (opens launcher)
├── Launcher button (quick-launch favorites)
└── Window list (running applications)
```

**Center zone:**

```
├── (empty by default — desktop wallpaper visible behind)
```

**Right zone (system tray area):**

```
├── Notifications (history badge)
├── Network
├── Volume
├── Battery (if present)
├── Brightness (if backlight present)
├── Bluetooth (if present)
├── Update Notifier (if updates available)
├── Removable Devices (if device mounted)
├── Night Light
├── Keyboard Layout (if multiple layouts)
├── Airplane Mode (if wireless present)
├── Clock / Calendar
└── Session Menu (logout, shutdown, restart)
```

### Position

Default: bottom of screen. Can be changed to top in **Settings → Appearance → Panel**. Cannot be set to left/right (vertical panels add complexity for minimal benefit on target laptops, which are widescreen).

### Height

Default: 32px. Adjustable in **Settings → Appearance → Panel** (range: 24-48px). Larger heights are useful for users with accessibility needs.

### Auto-Hide

Default: visible always. Can be set to auto-hide in **Settings → Appearance → Panel**. When auto-hiding, the panel slides off-screen and reappears when the cursor touches the screen edge. A 2px trigger zone prevents accidental triggering.

### Visibility In Mode

The panel uses the wlr-layer-shell protocol to position itself as an overlay layer. This ensures it is always rendered above application windows and below lock screens / full-screen exclusive surfaces.

### Spike Button

The Spike button in the left zone displays the Spike logo (herding dog emblem). Clicking it opens the launcher. It serves the same function as a "Start" button in Windows or an "Activities" button in GNOME.

**Visual:**

```
├── Circular emblem
├── Purple (#6d4aff) background
├── White Spike dog silhouette
├── Hover: subtle glow effect (Spike Plus only — disabled on Standard to save GPU cycles)
├── Click: launcher opens
└── Right-click: quick session menu (logout, shutdown, restart, lock)
```

## Launcher

### Purpose

The launcher is the application menu — how users find and open applications. It is modeled on KDE's Kickoff launcher, which organizes applications by category with search functionality.

### Layout

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  [🔍 Search applications...]                      │
│                                                  │
├──────────────┬───────────────────────────────────┤
│              │                                   │
│  Favorites   │  All Applications                 │
│  ──────────  │  ────────────────────────        │
│  🌐 Firefox  │  Internet                        │
│  📁 Dolphin  │    Firefox                       │
│  📝 Kate     │    Chromium                      │
│  🎬 Videos   │                                  │
│              │  Office                          │
│  Recently    │    LibreOffice Writer             │
│  Used        │    LibreOffice Calc               │
│  ──────────  │                                  │
│  Firefox     │  Media                           │
│  Kate        │    VLC                            │
│              │    Spotify                       │
│              │                                  │
│              │  System                          │
│              │    Discover 🟢 (new)              │
│              │    Settings                       │
│              │    Konsole                        │
│              │                                  │
├──────────────┴───────────────────────────────────┤
│  [Spike]  [Logout]  [Shutdown]                    │
└──────────────────────────────────────────────────┘
```

### Categories

Standard freedesktop.org categories:

- Internet (web browsers, email, chat) 

- Office (word processors, spreadsheets, PDF readers) 

- Media (video players, music players, image viewers) 

- Graphics (image editors, drawing tools) 

- Games 

- System (settings, terminal, file manager) 

- Accessories (calculator, archive manager, text editor) 

- Development (if development tools installed) 

### Search

- Case-insensitive substring search across application names and descriptions 

- Searches all installed applications (system, user, and Flatpak) 

- Results update live as the user types 

- Enter key launches the first search result 

### Favorites

**Favorites sidebar:**

```
├── Pre-populated with common apps at install:
│   ├── Firefox (web browser)
│   ├── Thunderbird (email)
│   ├── VLC (media)
│   ├── Dolphin (file manager)
│   ├── Kate (text editor)
│   └── Discover (software center)
├── User can add/remove favorites:
│   ├── Right-click app in launcher → "Add to Favorites"
│   └── Right-click favorite → "Remove from Favorites"
├── Drag-and-drop reordering supported
└── Favorites persist in ~/.config/spike/launcher/favorites.json
```

### Recently Used

**Recently used section:**

```
├── Shows last 5 launched applications
├── Ordered by most recent first
├── Cleared on logout (privacy — prevents another user from seeing what you used)
└── Stored in ~/.local/share/spike/launcher/recent.json
```

### Recently Installed Badge

**New app badge:**

```
├── Green dot (🟢) appears next to newly installed applications
├── Persists until the user dismisses it:
│   ├── Right-click app → "Mark as seen"
│   └── Or simply launching the app marks it as seen
├── Tracks applications installed via Discover (Flatpak or apt)
├── Badge does NOT appear for pre-installed apps (those are known)
└── Stored in ~/.local/share/spike/launcher/new-apps.json
```

### Desktop File Sources

The launcher scans `.desktop` files from three locations:

**System applications:**

```
└── /usr/share/applications/
```

**User applications (installed manually):**

```
└── ~/.local/share/applications/
```

**Flatpak applications:**

```
└── ~/.local/share/flatpak/exports/share/applications/
└── /var/lib/flatpak/exports/share/applications/
```

An inotify watch monitors all three directories. When a new `.desktop` file appears (app installed via Discover), the launcher updates its list within 1-2 seconds without requiring a restart.

### Launch Behavior

- Single-click launches the application (not double-click) 

- Launcher closes after launching an app 

- If an app is already running and the user clicks it again, it focuses the existing window (does not launch a second instance) 

### Keyboard Navigation

- Super key (Windows key): Opens launcher 

- Escape: Closes launcher 

- Arrow keys: Navigate categories and applications 

- Enter: Launch selected application 

- Tab: Switch between favorites, categories, and search 

- Type to search: Any typing while launcher is open activates search 

## Notification Daemon

### Purpose

The notification daemon handles all system and application notifications. It is the most carefully engineered component in Spike Shell because notifications are a trust mechanism — if a user misses one, they lose faith in the system.

### Core Principle

**Disk-before-display invariant.** Every notification is written to persistent storage before it is shown on screen. If the display layer crashes, the notification is still in history. No notification is ever lost.

### DBus Interface

The daemon implements `org.freedesktop.Notifications` on the session DBus. This is the standard interface that applications use to send notifications. Any application or system service can send notifications via:

```
dbus-send --session --dest=org.freedesktop.Notifications \\
  /org/freedesktop/Notifications \\
  org.freedesktop.Notifications.Notify \\
  string:"app-name" uint32:0 string:"icon" \\
  string:"summary" string:"body" \\
  array:[] dict:string:string: int32:-1
```

### Notification Processing Pipeline

```
Notification arrives via DBus
        │
        ▼
┌──────────────────────────┐
│  1. Parse notification    │
│     fields (summary,     │
│     body, icon, actions,  │
│     urgency, timeout,    │
│     transient hint)      │
└──────────┬───────────────┘
           │
           ▼
┌──────────────────────────┐
│  2. Assign unique ID      │
│     (incrementing counter │
│     persisted to disk)    │
└──────────┬───────────────┘
           │
           ▼
┌──────────────────────────┐
│  3. WRITE TO DISK         │
│     (atomic write to      │
│     history.json)         │
│     ★ This step MUST      │
│       succeed before       │
│       proceeding ★         │
└──────────┬───────────────┘
           │
           ▼
┌──────────────────────────┐
│  4. Display popup         │
│     (if not in DND mode)  │
└──────────┬───────────────┘
           │
           ▼
┌──────────────────────────┐
│  5. Update tray badge     │
│     (increment unread     │
│     count)                │
└──────────────────────────┘
```

### History Storage

**Location:** `~/.local/share/spike/notifications/history.json`

**Format:** JSON array, grouped by date

**Structure:**

```
{
  "notifications": [
    {
      "id": 1042,
      "timestamp": "2026-07-09T14:32:00Z",
      "date": "2026-07-09",
      "app_name": "Discover",
      "app_icon": "discover",
      "summary": "Updates available",
      "body": "3 applications can be updated.",
      "urgency": "normal",
      "actions": [
        { "id": "update-all", "label": "Update All" },
        { "id": "dismiss", "label": "Dismiss" }
      ],
      "transient": false,
      "displayed": true
    }
  ],
  "counter": 1042,
  "last_read_id": 1040
}
```

### Atomic Writes

History is written using atomic write-and-rename:

```
# Conceptual atomic write process:
1. Write to history.json.tmp
2. fsync(history.json.tmp)
3. rename(history.json.tmp, history.json)
4. fsync(directory containing the file)
```

This ensures that if the system loses power or crashes mid-write, the history file is either the old version or the new version — never a corrupted half-written file.

### History Retention

- Default: 3 days 

- Adjustable: 1-31 days (**Settings → Notifications**) 

- Maximum count: 500 notifications (adjustable: 10-1000) 

- Cleanup: Runs at daemon startup and every hour 

**When cleanup runs:**

- Delete entries older than retention period 

- If count exceeds maximum, delete oldest entries first 

- Atomic rewrite of history.json 

- Tray badge updated if any unread notifications were removed 

### History Viewer

Accessible by clicking the notification tray icon:

```
┌──────────────────────────────────────────────────┐
│  Notifications                                     │
│                                                    │
│  ── Today ────────────────────────────────────── │
│  📦 Updates available                 2:30 PM     │
│     3 applications can be updated.                 │
│     [Update All]  [Dismiss]                       │
│                                                    │
│  🔔 Battery low                       1:45 PM     │
│     Battery at 15%. Connect your charger.          │
│                                                    │
│  ── Yesterday ────────────────────────────────── │
│  🖨️ Print job complete               6:12 PM     │
│     Document.pdf printed successfully.             │
│                                                    │
│  ── 2 days ago ───────────────────────────────── │
│  💾 Backup complete                   8:00 AM     │
│     Your files were backed up to USB.              │
│                                                    │
│  [Clear all]                        [Settings]     │
└──────────────────────────────────────────────────┘
```

#### History Viewer Features

**Grouping:**

```
├── Notifications grouped by day (Today, Yesterday, date headers)
├── Within each day, ordered newest first
└── Infinite scroll (loads more entries as user scrolls up)
```

**Actions:**

```
├── Action buttons (e.g., "Update All", "Dismiss") remain functional from history
├── Clicking an action sends the DBus reply to the originating application
└── If the originating application is no longer running, action is disabled with tooltip
```

**Search:**

```
├── Search bar at top of history viewer
├── Searches summary and body text
└── Results filtered live as user types
```

**Clearing:**

```
├── "Clear all" button removes all notifications from history
└── Confirmation dialog: "Clear all notifications? This cannot be undone."
```

### Tray Badge

**Tray icon (bell):**

```
├── No badge: all notifications read
├── Number badge: unread count (e.g., "3")
├── Badge persists until user opens history viewer
├── Opening history viewer marks all as read
├── Badge does NOT disappear when popup disappears
└── Badge does NOT disappear on timeout
```

### Do Not Disturb Mode

**Settings → Notifications → Do Not Disturb**

```
├── Toggle: On/Off
├── When ON:
│   ├── Popups do not display
│   ├── Notifications still written to disk (history invariant maintained)
│   ├── Tray badge still updates
│   ├── Sound is suppressed
│   └── User can review in history viewer later
├── Schedule (optional):
│   ├── Start time: (e.g., 10:00 PM)
│   └── End time: (e.g., 7:00 AM)
└── Critical notifications override DND:
    └── Urgency=critical notifications display and play sound regardless
```

### Transient Hints

The transient hint in the freedesktop.org spec indicates that a notification should not persist in any permanent display. Spike interprets this as follows:

**transient=true:**

```
├── Popup disappears on timeout (normal behavior)
├── History entry IS still created (Spike overrides spec here — trust over compliance)
└── Entry appears in history viewer
```

**transient=false (or absent):**

```
├── Popup disappears on timeout
├── History entry created
└── Entry appears in history viewer
```

**Reason:** The transient hint only affects display behavior, not persistence. If a user heard a notification sound, they should be able to find it.

### Replacement Notifications

When an application sends a notification with the `replaces_id` parameter, indicating it replaces a previous notification:

**Spike behavior:**

```
├── Creates a NEW entry in history (does not overwrite the old one)
├── Marks the old entry as "superseded" in history
├── Superseded entries are dimmed in the history viewer
├── Popup replaces the existing popup on screen (normal behavior)
└── Reason: Overwriting history loses the original context.
    Example: "Downloading: 50%" → "Downloading: 51%" should show
    both in history, not just the last one.
```

### Notification Sound

- **Sound:** Custom Spike notification chime (short, pleasant, not jarring) 

- **Playback:** via PipeWire (via libcanberra or direct PipeWire API) 

- **Volume:** Follows system volume 

- **Suppressed:** In Do Not Disturb mode (unless urgency=critical) 

### Crash Recovery

If the notification daemon crashes:

```
├── systemd restarts it (Restart=always, 1-second delay)
├── On restart:
│   ├── Reads history.json from disk
│   ├── All notifications preserved
│   ├── Tray badge restored from last_read_id
│   └── History viewer available immediately
├── Notifications sent during downtime are lost (no daemon to receive them)
│   └── Acceptable: if the daemon is down, the system has bigger problems
└── Counter continues from last persisted value (no ID collision)
```

## Tray Applets

### General Architecture

All tray applets share a common architecture:

**Each applet:**

```
├── Implements the Spike::Applet interface
├── Has an icon (shown in panel tray)
├── Has a popup (shown when clicked)
├── Has a settings entry (if user-configurable)
├── Communicates via DBus or sysfs (never direct library calls to system daemons)
├── Loads dynamically at panel startup
└── Can be conditional (loaded only if hardware is present)
```

### Conditional Applet Loading

At panel startup:

```
├── Check hardware for each conditional applet
├── If hardware present → load applet
├── If hardware absent → skip (no tray icon shown)
└── If hardware added later (hot-plug) → udev triggers applet load via DBus signal
```

**Conditional applets:**

```
├── Bluetooth: Check for BlueZ adapter presence
├── Airplane Mode: Check for rfkill devices
├── Keyboard Layout: Check if multiple layouts configured
└── (4th conditional: TBD during development)
```

### 1. Network Applet

- **DBus interface:** `org.freedesktop.NetworkManager` 

- **Tray icon:** Signal strength bars (Wi-Fi) or Ethernet icon 

**Popup contents:**

```
├── Current network name (SSID)
├── Signal strength
├── "Disconnect" button
├── Available networks list (scan results)
│   └── Click to connect (prompts for password if needed)
├── "Network Settings" button
└── "Turn Off Wi-Fi" toggle
```

- **Settings entry:** **Settings → Network** (Spike custom NetworkManager UI — same panel as the tray applet; not plasma-nm / KCM)

- **Hotspot:** Available in popup → "Create Hotspot" 

- **VPN:** Shown in popup if configured (OpenVPN, WireGuard) 

- **Mobile broadband:** Shown if modem detected 

### 2. Volume Applet

- **DBus interface:** `org.freedesktop.portal.Audio` (PipeWire) or direct PipeWire DBus 

- **Tray icon:** Speaker icon (muted state shows crossed-out speaker) 

**Popup contents:**

```
├── Output volume slider (0-150%)
├── Mute toggle button
├── Output device selector (dropdown)
│   └── Lists speakers, headphones, HDMI audio, Bluetooth
├── Input volume slider (microphone)
├── Input device selector
└── "Sound Settings" button
```

- **Settings entry:** **Settings → Sound** (KCM module loaded) 

- **Scroll wheel:** Scrolling over the tray icon adjusts volume 

- **Middle-click:** Toggles mute 

### 3. Battery Applet

- **DBus interface:** `org.freedesktop.UPower` 

- **Tray icon:** 

```
├── Battery percentage + charging icon
├── States: Discharging, Charging, Full, Low (<20%), Critical (<10%)
```

**Popup contents:**

```
├── Battery percentage
├── State (Charging / Discharging / Full)
├── Time remaining (estimate)
└── "Power Settings" button
```

- **Conditional:** Only shown if battery present (detected via UPower) 

**Notifications:**

```
├── Low battery (20%): "Battery is running low. Connect your charger soon."
├── Critical battery (10%): "Battery critically low. Plug in now to avoid shutdown."
└── Fully charged: "Battery fully charged. You can unplug your charger."
```

### 4. Brightness Applet

- **Interface:** `/sys/class/backlight/*/brightness` 

- **Tray icon:** Sun icon 

**Popup contents:**

```
├── Brightness slider (0-100%)
└── "Auto-adjust brightness" toggle (if ambient light sensor present)
```

- **Conditional:** Only shown if backlight device present 

### 5. Notifications Applet

- **Interface:** Internal (spike-notify daemon) 

- **Tray icon:** Bell with unread count badge 

**Popup contents:**

```
├── Full history viewer (as described in Notification Daemon section)
├── "Clear all" button
└── "Notification Settings" button
```

- **Badge persists until user opens history viewer** 

### 6. Removable Devices Applet

- **DBus interface:** `org.freedesktop.UDisks2` 

- **Tray icon:** USB drive icon (shown when a removable filesystem is present)

**Popup contents:**

```
├── List of removable volumes (mounted or not)
│   ├── Device name / label
│   ├── Mount point (or “not mounted”)
│   ├── Open / Mount / Eject
└── Auto-mount via UDisks2 on plug-in
```

- **Conditional:** Shown when UDisks2 reports at least one non-system filesystem volume

**Notifications:**

```
├── Device connected: "USB drive detected" (+ mount path when auto-mount succeeds)
└── Device safely removed / ejected: tray Notify via org.freedesktop.Notifications
```

### 7. Update Notifier Applet

- **Interface:** Polls apt and Flatpak repositories 

- **Tray icon:** Package icon with update badge (green dot if updates available) 

**Popup contents:**

```
├── "X system updates available"
├── "Y application updates available (Flatpak)"
├── "Update All" button → opens Discover
└── "View updates" button → opens Discover update page
```

**Polling schedule:**

```
├── Check every 6 hours
├── Check on network connection establishment
├── Delayed if system under memory pressure (check skipped, retried when idle)
└── Never runs during user activity (waits for idle period)
```

**Notifications:**

```
├── Updates available: "Updates are available. Click to review."
├── Security update available: "Important security update available."
└── Update failure: "An update could not be installed. Try again later."
```

### 8. Clock / Calendar Applet

- **Interface:** `systemd-timesyncd` (NTP sync status) 

- **Tray icon:** Digital clock (time display) 

**Popup contents:**

```
├── Large clock display (time and date)
├── Mini calendar (current month, navigable)
├── Next 3 calendar events (if KDE calendar integration available — future)
└── "Date & Time Settings" button
```

**Format:**

```
├── Time format follows locale setting (12h or 24h)
├── Date format follows locale setting
└── NTP sync indicator (small icon if time is syncing)
```

### 9. Session Menu Applet

- **DBus interface:** `org.freedesktop.login1` (systemd loginctl) 

- **Tray icon:** Power icon 

**Popup contents:**

```
├── "Lock Screen"
├── "Log Out"
├── "Restart"
├── "Shut Down"
└── "Suspend"
```

**Each option shows confirmation dialog:**

```
├── Log Out: "Are you sure you want to log out? Unsaved work may be lost."
├── Restart/Shut Down: "Are you sure you want to [restart/shut down]? Unsaved work may be lost."
├── Lock: No confirmation (immediate)
└── Suspend: No confirmation (immediate)
```

**Spike Standard:** "Fast" power options (suspend preferred over full shutdown)

### 10. Night Light Applet

- **Interface:** KWin gamma control (`org.kde.KWin.gamma`) 

- **Tray icon:** Moon/sun icon 

**Popup contents:**

```
├── Night Light toggle (On/Off)
├── Color temperature slider (2500K - 6500K)
├── "Sunset to sunrise" schedule toggle
└── Manual schedule (start time, end time)
```

- **Default:** Off (user discovers and enables if desired) 

- **Effect:** Reduces blue light by lowering color temperature at night 

### 11. Bluetooth Applet (Conditional)

- **DBus interface:** `org.bluez` 

- **Tray icon:** Bluetooth icon 

- **Conditional:** Only shown if Bluetooth adapter detected 

**Popup contents:**

```
├── Bluetooth toggle (On/Off)
├── Paired devices list
│   ├── Device name, type, battery level (if reported)
│   ├── Connected/disconnected status
│   └── "Connect"/"Disconnect" button per device
├── "Pair new device" button → opens pairing wizard
└── "Bluetooth Settings" button
```

**Pairing wizard:**

```
├── Scans for discoverable devices
├── Lists found devices with signal strength
├── PIN entry (if required by device)
├── Confirmation on both devices (for SSP pairing)
└── Success/failure notification
```

### 12. Airplane Mode Applet (Conditional)

- **Interface:** `/sys/class/rfkill/*` 

- **Tray icon:** Airplane icon 

- **Conditional:** Only shown if rfkill devices present (Wi-Fi and/or Bluetooth) 

**Popup contents:**

```
├── Airplane Mode toggle
└── Shows affected radios: "This will turn off Wi-Fi and Bluetooth"
```

**Behavior:**

```
├── Toggle on: Blocks all rfkill devices (Wi-Fi off, Bluetooth off)
├── Toggle off: Unblocks all rfkill devices
├── Persists across reboots (if toggled on, stays on after restart)
└── Notification: "Airplane mode [on/off]. Wi-Fi and Bluetooth are [disabled/enabled]."
```

### 13. Keyboard Layout Applet (Conditional)

- **Interface:** libinput + xkbcommon 

- **Tray icon:** Current layout flag or abbreviation (e.g., "US", "UK", "FR") 

- **Conditional:** Only shown if more than one keyboard layout is configured 

**Popup contents:**

```
├── Current layout (highlighted)
├── List of configured layouts
├── Click to switch layout
├── Keyboard shortcut: Super+Space to cycle layouts
└── "Keyboard Settings" button
```

**Layout configuration:**

```
├── Set during installer (timezone-based suggestion)
├── Additional layouts added in Settings → Keyboard
└── Layout switching is instant (no re-login required)
```

### 14. Fourth Conditional Applet

To be determined during development. Possible candidates:

**Candidates:**

```
├── Cloud sync status (if Proton Drive or similar integration added)
├── Printer queue status (if CUPS detected)
├── Screen recording indicator (when Spectacle or OBS is active)
└── External display manager (when monitor connected via HDMI)
```

Will be decided based on user feedback during alpha testing.

## Settings Panel

### Purpose

The settings panel is the central location for all system configuration. It embodies Spike's golden rule: users never edit config files. Everything has a GUI.

### Architecture

The settings panel is a hybrid: custom Spike pages for Spike-specific settings and for NetworkManager UI, and KDE KCM (KDE Configuration Modules) for hardware modules whose **standalone** packages do not pull `plasma-desktop` / `plasma-workspace` (no plasmashell). All modules are loaded inside a single Spike-themed window.

**KCM packaging rule:** Never install `plasma-desktop` or `plasma-workspace` on Spike. Prefer standalone KCM providers (`kscreen`, `plasma-pa`, `powerdevil`, `bluedevil`, `print-manager`). If an upstream KCM only exists inside Plasma Desktop/Workspace, implement a Spike custom page instead.

**Settings Window:**

```
┌──────────────────────┬───────────────────────────────────┐
│                      │                                   │
│  [🔍 Search settings..]                                   │
│                      │                                   │
│  PERSONAL            │  (Selected page content)          │
│  ├ Appearance        │                                   │
│  ├ Notifications     │                                   │
│  ├ Keyboard Layout   │                                   │
│  └ Language          │                                   │
│                      │                                   │
│  HARDWARE            │                                   │
│  ├ Display           │                                   │
│  ├ Sound             │                                   │
│  ├ Power             │                                   │
│  ├ Keyboard          │                                   │
│  ├ Mouse/Touchpad    │                                   │
│  ├ Bluetooth         │  [Context-aware help button →]   │
│  └ Printer           │  (links to relevant user guide    │
│                      │   section)                        │
│  NETWORK             │                                   │
│  ├ Network           │                                   │
│  └ VPN               │                                   │
│                      │                                   │
│  SYSTEM              │                                   │
│  ├ Users             │                                   │
│  ├ Date & Time       │                                   │
│  ├ Accessibility     │                                   │
│  ├ Software Sources  │                                   │
│  └ About             │                                   │
│                      │                                   │
│  ADVANCED            │                                   │
│  ├ Memory            │                                   │
│  ├ Boot              │                                   │
│  ├ Kernel Modules    │                                   │
│  ├ Updates           │                                   │
│  ├ Storage           │                                   │
│  └ Diagnostics       │                                   │
│                      │                                   │
└──────────────────────┴───────────────────────────────────┘
```

### Organization

**Personal:**

```
├── Appearance (custom) — accent/font/wallpaper live; Aurorae later
├── Notifications (custom) — DND / retention; tray owns fdo Notifications when free
├── Keyboard Layout (custom) — XKB layout via setxkbmap / localectl / kxkbrc
└── Language (custom) — system language / region
    └── Note: upstream kcm_regionandlang lives in plasma-workspace; Spike does not ship plasmashell
```

**Panel:**

```
├── Panel (custom) — position / height / auto-hide (moved from Appearance)
├── Tray Applets (custom) — visibility toggles for optional applets
└── Night Light (custom) — enable + temperature (KWin ColorCorrect when available)
```

**Hardware:**

```
├── Display (KCM) — kscreen / kcm_kscreen (standalone package)
├── Sound (KCM) — plasma-pa / kcm_pulseaudio (standalone; needs PipeWire)
├── Power (custom) — org.spike.Config power + logind drop-in (not powerdevil KCM); Apply does not restart logind mid-session
│   └── Lid / power button / idle suspend / governor; dimming & charge limits later
├── Keyboard (custom) — repeat rate via ~/.config/kcminputrc (KWin); shortcuts later
│   └── Note: upstream kcm_keyboard lives in plasma-desktop; not shipped
├── Mouse/Touchpad (custom) — speed / tap-to-click via kcminputrc
│   └── Note: upstream kcm_touchpad lives in plasma-desktop; not shipped
├── Bluetooth (KCM) — bluedevil / kcm_bluetooth (standalone)
└── Printer (KCM) — print-manager / kcm_printer_manager (standalone)
```

**Network:**

```
├── Network (custom) — Wi-Fi, Ethernet, hotspot via NetworkManager D-Bus
│   └── Shared UI with the panel Network tray applet; nmcli used as connect helper when needed
│   └── Do not use plasma-nm (pulls QtWebEngine ~200MB — too heavy for Tier-1)
└── VPN (custom) — OpenVPN / WireGuard list + import + nm-connection-editor
```

**System:**

```
├── Users (custom) — account management
│   └── Note: upstream kcm_users lives in plasma-workspace; not shipped
├── Date & Time (custom) — timezone, NTP via timedatectl / systemd-timedated
│   └── Note: upstream kcm_clock lives in plasma-desktop; not shipped
├── Accessibility (custom) — AccessX, magnifier launcher, Spike Shell high-contrast chrome
│   └── Note: upstream kcm_access lives in plasma-desktop; not shipped
├── Software Sources (custom) — APT view + universe/multiverse enable + external tools
└── About (custom) — version / hardware summary; raw state collapsible
```

**Advanced (hidden behind "Show Advanced"):**

```
├── Memory (custom) — ZRAM, swap, earlyoom, swappiness
├── Boot (custom) — GRUB behavior, boot failure counter
├── Kernel Modules (custom) — blacklist editor → spike-blacklist.conf
├── Updates (custom) — update schedule / auto-security prefs
├── Storage (custom) — lsblk inventory + UDisks2 Removable/USB Mount/Eject/Open
└── Diagnostics (custom) — sectioned DetectHardware + copy report
```

### Search

Search bar at top of settings window:

```
├── Searches all settings page titles and descriptions
├── Also searches keywords:
│   ├── "volume" → finds Sound
│   ├── "password" → finds Users
│   ├── "WiFi" → finds Network
│   └── "brightness" → finds Display (or Brightness applet)
├── Results update live
└── Enter navigates to first result
```

### Context-Aware Help

Every settings page has a "?" (help) button in the top-right corner. Clicking it opens the integrated user guide reader, scrolled to the relevant section:

**Help button behavior:**

```
├── Appearance → user-guide/07-system-settings.md#appearance
├── Network → user-guide/02-connecting-wifi.md
├── Sound → user-guide/07-system-settings.md#sound
├── Memory → dev-guide/09-spike-config-internals.md#memory
└── etc.
```

The user guide is stored offline on disk:

```
├── Location: /usr/share/spike/user-guide/
├── Rendered from Markdown to formatted display
├── Searchable within the reader
└── Same content as the website (single source of truth)
```

### Custom Pages Detail

#### Appearance Page

**Appearance:**

```
├── Theme: Spike (purple/cyan) — only theme, no theme switching
├── Accent color: Purple (#6d4aff) default, Cyan (#00bcd4) optional
├── Panel position: Bottom / Top
├── Panel height: 32px (slider: 24-48)
├── Panel auto-hide: On/Off
├── Window animations: Off (Spike Standard) / On (Spike Plus)
├── Font: Noto Sans (default), size adjustable
├── Icons: Breeze (default), size adjustable
└── Wallpaper: (selection from /usr/share/spike/wallpapers/)
```

#### Boot Page

**Boot:**

```
├── Boot failure counter: Current value, reset button
├── GRUB menu timeout: 3 seconds (slider: 1-10)
├── Show GRUB menu: Always / On failure only (default: On failure only)
├── Boot splash: Spike (default), Minimal (text only)
└── "View boot log" button (opens Konsole with journalctl -b)
```

#### Storage Page

**Storage:**

```
├── Disk overview:
│   ├── Device: /dev/sda (model, serial)
│   ├── Type: SSD / HDD / NVMe / SD / USB
│   ├── Total: 240GB
│   ├── Used: 45GB
│   └── Free: 195GB
├── Mount options (read-only display):
│   ├── / → noatime (or noatime,commit=60 for HDD)
│   └── /boot/efi → defaults
├── Swap file status:
│   ├── Size: 8GB
│   └── In use: 0.2GB
├── Health check:
│   └── SMART status (if supported): OK / Degraded / Failing
└── "Open in Dolphin" button
```

#### Diagnostics Page

**Diagnostics:**

```
├── System Information
│   ├── CPU: Intel Celeron N4020 @ 1.10GHz (2 cores)
│   ├── RAM: 4GB
│   ├── GPU: Intel UHD Graphics 600
│   ├── Storage: 240GB SSD (SATA)
│   ├── Kernel: 7.0.0-xx-generic
│   ├── Spike version: 0.1.0-alpha
│   └── Install date: 2026-07-09
├── Performance
│   ├── CPU usage: 12%
│   ├── RAM usage: 2.1GB / 4.0GB
│   ├── Swap usage: 0.2GB / 8.0GB
│   ├── Disk I/O: 2 MB/s read, 0 MB/s write
│   └── Temperature: 52°C (if sensor available)
├── Logs
│   └── "Open system log" button → opens Konsole with journalctl
└── Hardware Inventory
    └── "Generate hardware report" button → creates text file for support
```

## Session Manager

### Purpose

The session manager handles the lifecycle of the desktop session: login, autostart, logout, shutdown, restart, and suspend.

### Session Startup

Boot sequence (after kernel and systemd):

```
├── systemd reaches graphical.target
├── spike-session service starts
│   ├── Starts KWin (Wayland compositor)
│   ├── Starts Spike Shell:
│   │   ├── Panel (loads applets)
│   │   ├── Notification daemon
│   │   ├── Theme engine (applies stylesheet)
│   │   └── Settings (on-demand, not loaded at startup)
│   ├── Starts PipeWire + WirePlumber
│   ├── Starts NetworkManager (already running from systemd)
│   ├── Starts earlyoom
│   ├── Processes autostart entries:
│   │   ├── ~/.config/autostart/*.desktop
│   │   └── /etc/xdg/autostart/*.desktop (filtered — only Spike-approved entries)
│   └── Clears boot failure counter (successful boot)
└── Desktop ready
```

### Autostart Management

**Settings → Advanced → Startup Applications**

```
├── Lists all autostart entries
├── Toggle on/off per entry
├── Add custom entry (browse for .desktop file or executable)
├── Remove custom entries
├── Cannot remove system-critical entries (greyed out)
└── Entries sorted: System (locked) → User (editable)
```

### Login

**Display manager:** SDDM (Simple Desktop Display Manager)

```
├── Themed with Spike branding (purple/cyan, Spike logo)
├── Username + password fields
├── Auto-login: Optional, set during installer or in Settings → Users (default off)
├── Graphical login: spike-greeter on tty1 when auto-login is off (Alpha; SDDM later)
├── Session: spike-session (only option — no dropdown)
└── Power options: Restart, Shut Down (accessible without login)
```

### Logout / Shutdown Flow

User triggers logout/shutdown:

```
├── Session manager intercepts
├── Checks for running applications with unsaved work:
│   ├── Sends SIGTERM to all applications
│   ├── Waits 5 seconds for graceful shutdown
│   ├── If apps still running: shows "Waiting for [app] to close..." dialog
│   ├── "Force quit" button after 10 seconds
│   └── On force: SIGKILL remaining applications
├── Saves session state (open windows, positions — for future restore feature)
├── Stops Spike Shell components in reverse order:
│   ├── Settings (if open)
│   ├── Applets (save state)
│   ├── Notification daemon (flush history to disk)
│   ├── Panel
│   └── KWin
├── systemd handles actual shutdown/restart
└── Boot failure counter preserved (only cleared on successful boot)
```

### Boot Failure Counter

- **Location:** `/boot/.spike/boot-count` 

- **Purpose:** Track consecutive boot failures 

**Behavior:**

```
├── Incremented at early boot (initramfs stage)
├── Cleared when spike-session successfully starts
├── If count >= 3:
│   ├── GRUB shows boot menu on next attempt
│   ├── Previous kernel highlighted
│   └── Recovery entry available
└── Counter resets to 0 after successful boot
```

**File format:**

```
3     # just an integer, no newline
```

## Theme Engine

### Purpose

The theme engine applies Spike's visual identity consistently across all shell components and KDE applications.

### Color Palette

| **Component** | **Color** |
| :-: | :-: |
| Primary | #6d4aff (Spike Purple) |
| Secondary | #00bcd4 (Cyan/Teal) |
| Background | #1a1a2e (Dark background) |
| Surface | #16213e (Panel, popups) |
| Text | #e0e0e0 (Light gray) |
| Text muted | #8888aa (Dimmed text) |
| Success | #4caf50 (Green) |
| Warning | #ff9800 (Orange) |
| Error | #f44336 (Red) |
| Accent hover | #8b6fff (Lighter purple) |

### Components

**Theme Engine applies:**

```
├── Qt Stylesheet (QSS)
│   ├── Panel background, borders
│   ├── Applet popup styling
│   ├── Launcher styling
│   ├── Settings window styling
│   ├── Notification popup styling
│   └── Scrollbars, buttons, sliders
├── KWin Window Decoration
│   ├── Title bar color
│   ├── Border color
│   ├── Button styling (close, minimize, maximize)
│   └── Spike theme config applied via kwalletrc or KWin config
├── Icon Theme
│   ├── spike-icons on live ISO (`/usr/share/icons/spike-icons`, Inherits=breeze-dark,breeze,hicolor)
│   ├── Breeze Dark as the glyph source (no custom SVGs yet)
│   └── Panel / Settings nav use QIcon::fromTheme
├── Fonts
│   ├── Default: Noto Sans
│   ├── Monospace: Noto Sans Mono (for Konsole)
│   ├── Sizes: 10pt default, adjustable in Settings → Appearance
│   └── Anti-aliasing enabled (subpixel RGB, slight hinting)
├── Cursor Theme
│   ├── Default: Breeze cursor theme
│   └── Size: 24px default, adjustable
├── Plymouth (boot splash)
│   ├── Spike logo on dark background
│   ├── Minimal animation (Spike Standard) / animated (Spike Plus)
│   └── Located in src/spike-branding/plymouth/
└── GRUB Theme
    ├── Dark background with circuit traces
    ├── Spike logo
    ├── Purple/cyan color scheme
    └── Located in src/spike-branding/grub-theme/
```

### Wallpaper

Default wallpapers shipped in `/usr/share/spike/wallpapers/`:

```
├── Coastal-Run.png (temporary default — Pixel Archive Dark; stretch-to-fit)
└── spike-default.png (same asset for older paths)
```

- **Fit mode (pre-alpha):** stretch to fill the screen (`IgnoreAspectRatio`)
- **Wallpaper selection:** **Settings → Appearance → Wallpaper** 

- **Custom wallpaper:** User can browse and select any image 

## User Guide Integration

### Offline Reader

The user guide (17 documents from `docs/user-guide/`) is stored on disk and accessible from within the Settings application:

**Integrated reader:**

```
├── Location in Settings: About → User Guide
├── Also accessible via help (?) buttons on each settings page
├── Stored at: /usr/share/spike/user-guide/
├── Rendered from Markdown to formatted display
├── Table of contents navigation (sidebar)
├── Search within the guide
├── Breadcrumb navigation
├── Previous/next page buttons
└── No internet required — fully offline
```

### Single Source of Truth

```
docs/user-guide/ (in repository)
    │
    ├── Builds to /usr/share/spike/user-guide/ (on installed system)
    ├── Renders to spike.bigrangatech.com/user-guide/ (on website)
    └── Displays in Settings reader (offline)
```

One content source, three presentation layers. Changes to the Markdown files propagate everywhere.

## Accessibility

### Built-In Accessibility Features

Accessibility is provided through KDE KCM modules:

```
├── Screen magnifier
├── Screen reader (Orca, if installed)
├── Sticky keys
├── Slow keys
├── Bounce keys
├── Mouse keys (numeric keypad controls cursor)
├── High contrast mode
├── Visual alerts (screen flash for sounds)
└── On-screen keyboard (maliit, if installed)
```

Accessible via: **Settings → Accessibility**

See `ACCESSIBILITY.md` for full accessibility specification.

## Performance Considerations

### Memory Budget

Spike Shell component memory usage (idle, approximate):

| **Component** | **Memory** |
| :-: | :-: |
| Panel | ~15-20MB |
| Applets (14 loaded) | ~25-35MB (includes DBus polling) |
| Notification daemon | ~5-8MB |
| Settings (not running) | 0MB (loaded on-demand) |
| Session manager | ~3-5MB |
| Theme engine | ~2-3MB (stylesheet, cached) |
| KWin (Wayland) | ~100-150MB |
| **Total Shell overhead** | **~150-220MB** |
| System + kernel | ~80-120MB |
| **Total idle target** | **<400MB (Spike Standard)** |

### Optimization Strategies

**Spike Standard optimizations:**

```
├── Animations disabled (saves GPU memory and rendering cycles)
├── Compositor effects: minimal (no blur, no transparency)
├── Qt rendering: software rasterizer (if GPU acceleration weak)
├── Applet polling intervals: conservative (network every 10s, updates every 6h)
├── Notification history loaded lazily (only when viewer opened)
└── Settings pages loaded on-demand (not pre-loaded)
```

**Spike Plus optimizations:**

```
├── Animations enabled (GPU can handle it)
├── Compositor effects: blur, transparency, fade
├── Qt rendering: OpenGL (GPU-accelerated)
├── Applet polling intervals: standard (network every 5s, updates every 4h)
└── Everything else identical
```

## What This Document Does Not Cover

- Memory management configuration details: See `MEMORY.md` (ZRAM, swap, earlyoom, swappiness) 

- Kernel and GPU driver configuration: See `KERNEL.md` (boot parameters, module blacklisting, NVIDIA) 

- Installer flow and data backup/restore: See `INSTALLER.md` 

- Boot sequence and GRUB configuration: See `BOOT-PROCESS.md` 

- Audio and video configuration: See `MULTIMEDIA.md` (PipeWire, VA-API, Firefox tuning) 

- Network configuration and firmware: See `NETWORKING.md` 

- Security policies: See `SECURITY.md` (AppArmor, firewall, updates) 

- Accessibility specification: See `ACCESSIBILITY.md` 

- Branding assets and logos: See `BRANDING.md` 

- User guide content: See `docs/user-guide/` directory 

🐕 BigRangaTech


