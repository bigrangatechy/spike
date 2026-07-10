Spike Accessibility
Purpose

This document specifies the accessibility features of Spike Linux. Accessibility is not an add-on, a checkbox, or a "nice to have." It is a fundamental requirement of an operating system that claims to be for everyone.

Spike targets users who may be using old laptops precisely because they can't afford newer hardware. Disability correlates with poverty. A budget operating system that ignores accessibility is failing the people who need it most.

Every accessibility feature in Spike must comply with the Golden Rules: no terminal required, no config file editing. Accessibility features are configured entirely through Settings → Accessibility.
Design Principles

1. Every setting has a GUI toggle
   ├── No accessibility feature requires terminal or config editing
   ├── All features are in Settings → Accessibility (single location)
   └── Settings persist across reboots and upgrades

2. Sensible defaults
   ├── Features are OFF by default (don't change the experience
   │   for users who don't need them)
   ├── Once enabled, they stay enabled (persist as user preference)
   ├── First-boot welcome wizard can enable key features
   └── Settings → Accessibility → "Quick Setup" detects needs

3. Never break the base experience
   ├── Enabling an accessibility feature must not crash the desktop
   ├── Disabling a feature returns to normal instantly
   ├── Features degrade gracefully on Tier 2 hardware
   └── No accessibility feature should consume significant RAM
       (target: <20MB additional for any single feature)

4. Qt6 and Wayland native where possible
   ├── Use Qt6 accessibility framework (QAccessible) for Spike Shell
   ├── Use KWin Wayland accessibility hooks for compositor features
   ├── Use AT-SPI (Assistive Technology Service Provider Interface)
   │   as the bridge to screen readers and external tools
   └── Avoid custom implementations when standards exist

5. Performance-conscious on target hardware
   ├── Screen reader must work on Celeron N4020
   ├── Magnifier must not drop below 20fps on Tier 1
   ├── Visual alerts must not add perceptible latency
   └── On-screen keyboard must be usable on touchpad (no touchscreen needed)

Accessibility Categories

Settings → Accessibility is organized into categories:

┌──────────────────────────────────────────────────┐
│  Accessibility                                    │
│                                                  │
│  👁 Vision                                        │
│     ├─ Screen Reader (Orca)                       │
│     ├─ Magnifier                                  │
│     ├─ High Contrast                              │
│     ├─ Large Text                                 │
│     ├─ Color Filters                              │
│     ├─ Reduce Motion (disable animations)         │
│     └─ Cursor Size                                │
│                                                  │
│  👂 Hearing                                        │
│     ├─ Visual Alerts                              │
│     ├─ Caption Support (future)                   │
│     └─ Mono Audio                                  │
│                                                  │
│  🖐 Motor                                          │
│     ├─ On-Screen Keyboard                         │
│     ├─ Sticky Keys                                │
│     ├─ Slow Keys                                  │
│     ├── Bounce Keys                               │
│     ├─ Mouse Keys                                 │
│     ├─ Click Assist (dwell click)                  │
│     └─ Repeat Keys (adjust/disable)               │
│                                                  │
│  🧠 Cognitive                                      │
│     ├─ Simplified Interface (reduce clutter)       │
│     ├─ Reading Assistance (highlight focus)        │
│     ├─ Confirmation Dialogs (extra confirmations)  │
│     └─ Focus Animation (gentle pulse)             │
│                                                  │
│  ⚙ System                                          │
│     ├─ AT-SPI Bridge (status)                     │
│     ├─ Quick Setup Wizard                         │
│     └─ Export/Import Settings                     │
│                                                  │
└──────────────────────────────────────────────────────────┘

Vision
Screen Reader (Orca)

Spike integrates Orca as the screen reader. Orca is the standard Linux screen reader, supporting AT-SPI and providing speech output and braille display support.

Screen Reader — Settings → Accessibility → Vision → Screen Reader:

┌──────────────────────────────────────────────────┐
│  Screen Reader                                    │
│                                                  │
│  [● Enabled]  [○ Disabled]                       │
│                                                  │
│  Voice                                            │
│  Engine: [espeak-ng] ▼                            │
│  Voice: [English (default)] ▼                    │
│  Rate:  ████████░░░░░  (180 WPM)                  │
│  Pitch:  ██████░░░░░░░░  (50)                     │
│  Volume: ██████████░░░  (80%)                    │
│  [Test voice]                                    │
│                                                  │
│  Braille Display                                  │
│  [ ] Enable braille output (if display connected)  │
│  Device: [Auto-detect] ▼                         │
│                                                  │
│  Behavior                                         │
│  [x] Read tooltips aloud                         │
│  [x] Read notifications aloud                    │
│  [x] Announce window title on focus change       │
│  [ ] Speak typed characters                       │
│  [ ] Speak typed words                            │
│  [x] Speak navigation keys (Tab, arrow keys)     │
│  [x] Read full dialog when it appears            │
│                                                  │
│  [Apply]  [Reset to defaults]                    │
│                                                  │
└──────────────────────────────────────────────────┘

Implementation:

Orca integration:
├── orca is installed on all Spike installs (not conditional)
├── AT-SPI bridge runs when Orca is enabled
├── Spike Shell exposes QAccessible interfaces for all widgets:
│   ├── Panel: announces applet names and states
│   ├── Launcher: reads application names as user navigates
│   ├── Notifications: reads notification content when displayed
│   ├── Settings: reads page names, section labels, control labels
│   └── Dialogs: reads full dialog content on appearance
├── KWin exposes window titles, states, and focus changes
├── Firefox: has its own accessibility support (built-in)
├── KDE applications: have Qt accessibility support (built-in)
│
├── Speech synthesis engine: espeak-ng
│   ├── Lightweight (~5MB RAM)
│   ├── Works on Celeron N4020 without noticeable CPU impact
│   ├── Supports multiple voices and languages
│   └── No GPU or audio hardware acceleration needed
│
├── Braille display support: BRLTTY
│   ├── Installed but not started unless braille display detected
│   ├── Auto-detected via USB
│   ├── If detected: BRLTTY starts, Orca routes braille output
│   └── If not detected: no overhead (process not running)
│
├── Keyboard shortcut: Super + Alt + S toggles screen reader
└── Status: shown in panel as an accessibility indicator

Magnifier

Magnifier — Settings → Accessibility → Vision → Magnifier:

┌──────────────────────────────────────────────────┐
│  Screen Magnifier                                 │
│                                                  │
│  [● Enabled]  [○ Disabled]                       │
│                                                  │
│  Zoom level:  ██████░░░░░░░░  (2x)               │
│  (Range: 1.5x to 8x)                             │
│                                                  │
│  Mode: [Follow mouse] ▼                          │
│    ├── Follow mouse (magnifier tracks cursor)    │
│    ├── Follow caret (tracks text cursor)         │
│    ├── Follow focus (tracks focused widget)     │
│    └── Full screen (entire screen zoomed)        │
│                                                  │
│  Lens shape (when not full screen):              │
│    ├── [Circle] [Rectangle]                       │
│    Lens size:  ██████░░░░  (300px radius)         │
│                                                  │
│  Smoothing: [x] Smooth magnified image           │
│  Mouse pointer: [Scale with zoom] ▼              │
│    ├── Scale with zoom (pointer grows with zoom)  │
│    ├── Keep original size                        │
│    └── Hide pointer (use crosshair)               │
│                                                  │
│  Edge resistance: [x] Stop magnifier at screen   │
│    edges                                          │
│                                                  │
│  [Apply]                                         │
│                                                  │
└──────────────────────────────────────────────────┘

Implementation:

Magnifier implementation:
├── KWin Wayland magnifier (built-in)
├── Uses compositor's scene graph (GPU-accelerated on Plus,
│   software-rendered on Standard)
├── Performance:
│   ├── Tier 1 (Standard, software): 20-30fps at 2x zoom (acceptable)
│   ├── Tier 1 (Plus, OpenGL): 60fps at 2x zoom (smooth)
│   └── Tier 2 (AMD A4): 15-25fps at 2x zoom (usable, may stutter)
├── Memory: <10MB additional
├── Toggle shortcut: Super + Alt + M
└── Zoom adjustment: Super + Alt + +/- (zoom in/out)

High Contrast

High Contrast — Settings → Accessibility → Vision → High Contrast:

[● Off]  [○ On]

When enabled:
├── Qt stylesheet switches to high-contrast theme
│   ├── Background: pure black (#000000)
│   ├── Foreground: pure white (#ffffff)
│   ├── Selection: pure yellow (#ffff00) on black
│   ├── Borders: thick (3px) white borders on all panels and dialogs
│   ├── Buttons: black background, white text, thick white border
│   ├── Text fields: white background, black text, thick black border
│   └── Links: bright cyan (#00ffff) underlined
├── KWin window borders: thick, high-contrast colors
├── Firefox: high-contrast mode triggered (browser respects system theme)
├── Cursors: high-contrast cursor theme (black/white, large)
├── Plymouth (next boot): uses high-contrast variant if available
└── SDDM (login screen): high-contrast variant

Implementation:

High contrast theme:
├── Separate Qt stylesheet: /usr/share/spike/themes/high-contrast.qss
├── Applied by spike-shell theme engine (same mechanism as normal theme)
├── Overrides Spike Shell, KDE application styling (via Qt platform theme)
├── Firefox: follows system high-contrast setting via prefers-contrast media query
├── Does NOT require reboot (applied immediately)
├── Toggle shortcut: Super + Alt + C
└── Memory: 0 additional (replaces existing stylesheet)

Large Text

Large Text — Settings → Accessibility → Vision → Large Text:

[○ Off]  [● On]

Scaling:  ██████████░░░  (1.25x)
(Range: 1.0x to 2.0x in 0.25 increments)
Options: 1.0x (normal) / 1.25x / 1.5x / 1.75x / 2.0x (huge)

When enabled:
├── All Qt text scales by the selected factor
├── Panel height adjusts: 32px → 40px (1.25x) / 48px (1.5x)
├── Launcher icon size increases proportionally
├── Notifications: larger text, larger card
├── Settings: all text scales
├── KDE applications: scale via Qt font DPI
├── Firefox: respects system font scaling
├── SDDM (login screen): scales text
└── KWin window decorations: scale title text

Implementation:

Large text implementation:
├── Qt font scaling: QT_FONT_DPI adjusted (96 * scale factor)
├── Panel and launcher: pixel sizes multiplied by scale factor
├── Wayland logical size: unaffected (buffer scaling handles it)
├── Performance: no impact (font rendering is cached)
├── Memory: minimal increase (larger font glyph cache)
├── Applies immediately (no reboot)
└── Toggle shortcut: Super + Alt + L

Color Filters

Color Filters — Settings → Accessibility → Vision → Color Filters:

[○ Disabled]  [● Enabled]

Filter type: [Deuteranopia (green-blind)] ▼
  ├── Protanopia (red-blind)
  ├── Deuteranopia (green-blind)
  ├── Tritanopia (blue-blind)
  ├── Protanomaly (red-weak)
  ├── Deuteranomaly (green-weak)
  ├── Tritanomaly (blue-weak)
  ├── Achromatopsia (no color)
  ├── Invert colors
  └── Custom (select hue shift)

Intensity:  ██████████░░░  (100%)

What this does:
├── Shifts color palette so affected colors are distinguishable
├── Applies to entire screen (compositor-level)
├── Firefox and all applications see the filtered output
└── Screenshots: capture unfiltered (original colors)

Implementation:

Color filter implementation:
├── KWin shader-based color transformation
├── Applied at compositor level (after rendering, before display)
├── Shader is a fragment shader (GPU on Plus, CPU on Standard)
├── Performance:
│   ├── Tier 1 (Plus, GPU): no perceivable impact
│   ├── Tier 1 (Standard, software): <2% CPU overhead
│   └── Tier 2 (AMD A4): <5% CPU overhead
├── Memory: <1MB (shader only)
├── Applies immediately (no reboot)
└── Toggle shortcut: Super + Alt + F

Reduce Motion

Reduce Motion — Settings → Accessibility → Vision → Reduce Motion:

[● On]  [○ Off]

When enabled:
├── All animations are disabled or replaced with instant transitions
├── Spike Shell:
│   ├── Panel show/hide: instant (no slide animation)
│   ├── Launcher open/close: instant (no fade)
│   ├── Notification popups: instant (no slide/fade)
│   ├── Workspace switch: instant (no slide)
│   └── Button hover: no hover animation (color change only)
├── KWin:
│   ├── Window open/close: instant (no animation)
│   ├── Minimize: instant (window disappears, no genie effect)
│   ├── Workspace switching: instant
│   └── Dialog transitions: instant
├── Plymouth (next boot): static (no animated progress, dots only)
└── SDDM: instant transitions

Implementation:

Reduce motion implementation:
├── Equivalent to Spike Standard's animation behavior (already minimal)
├── If user is on Spike Plus with animations enabled:
│   ├── This setting overrides variant default
│   └── All animations are disabled
├── KWin: sets animation speed factor to 0 (instant)
├── Spike Shell: animation durations set to 0ms
├── No performance benefit (animations are cheap) — this is accessibility
└── Toggle shortcut: Super + Alt + R

Cursor Size

Cursor Size — Settings → Accessibility → Vision → Cursor Size:

Size:  [● Normal] [○ Large] [○ Extra Large] [○ Huge]

Normal:      24px (default)
Large:       32px
Extra Large: 48px
Huge:        64px

When changed:
├── Cursor scales immediately (no reboot)
├── All applications use the new cursor size
├── SDDM (login screen) uses the selected size
└── High-contrast cursor variant used if high contrast is also enabled

Hearing
Visual Alerts

Visual Alerts — Settings → Accessibility → Hearing → Visual Alerts:

[● Enabled]  [○ Disabled]

Alert type:
  [○ Flash screen]
  [● Flash window border]
  [○ Banner notification]

Flash color: [■ White] [■ Yellow] [● Custom]
  (If custom: color picker)

Flash duration: 300ms (default, range 200-1000ms)

What triggers a visual alert:
├── System notification with "critical" urgency
├── Error dialog appearance
├── Battery critically low warning
├── Update failure notification
├── Application crash notification
└── Timer/alarm (if clock applet supports)

What does NOT trigger:
├── Normal notifications (too frequent)
├── Information popups
└── Background events (non-visible to user)

Implementation:

Visual alert implementation:
├── Spike Shell hooks into notification daemon
│   ├── Filters for critical urgency notifications
│   ├── Triggers visual alert effect
│   └── Alert plays simultaneously with notification
├── Flash screen:
│   ├── KWin overlay: full-screen translucent flash (300ms)
│   ├── Does not obscure content (translucent, brief)
│   └── Does not interfere with running applications
├── Flash window border:
│   ├── KWin draws colored border around active window (300ms)
│   ├── More subtle than full-screen flash
│   └── Good for users who find full-screen flash disorienting
├── Banner notification:
│   ├── Large, high-contrast banner at top of screen
│   ├── Includes the alert text
│   └── Auto-dismisses after duration
├── Audio alert (optional, can accompany visual):
│   ├── Plays a distinct sound alongside the visual alert
│   └── Useful for users with partial hearing
├── Performance: <1% CPU, no memory impact
├── Toggle shortcut: Super + Alt + V
└── Persists across reboots

Mono Audio

Mono Audio — Settings → Accessibility → Hearing → Mono Audio:

[● Enabled]  [○ Disabled]

When enabled:
├── Left and right audio channels are mixed to mono
├── Output is identical on both speakers/headphones
├── Useful for users with unilateral hearing loss
├── Applies to all audio output (speakers, headphones, Bluetooth)
└── Does NOT affect microphone input (mic stays stereo if available)

Implementation:
├── PipeWire module: module-mono-mix
├── Loaded when mono audio is enabled
├── Unloaded when disabled
├── No perceptible latency or CPU impact
└── Toggle shortcut: Super + Alt + N

Motor
On-Screen Keyboard

On-Screen Keyboard — Settings → Accessibility → Motor → On-Screen Keyboard:

[● Enabled]  [○ Disabled]

Layout: [Compact] ▼
  ├── Compact (letters + numbers, symbols on secondary pane)
  └── Full (numpad, function keys, all symbols)

Position: [Bottom] ▼
  ├── Bottom (docked at bottom of screen)
  ├── Floating (movable window)
  └── Top (docked at top — rare but available)

Auto-show: [● When text field is focused]  [○ Always visible]  [○ Manual]

Key behavior:
  [x] Key preview (show pressed key enlarged briefly)
  [x] Audible click (play sound on key press)
  [ ] Word prediction (suggests words as user types)
  [x] Auto-capitalization
  [x] Double-tap for shift (caps lock)

Size:  ████████░░░  (60% of screen height when docked)
Opacity: ██████████  (100% — semi-transparent may be hard to see)

Implementation:

On-screen keyboard implementation:
├── maliit-keyboard (Qt6-based, Wayland native)
├── Integrates with Qt input method framework
├── Appears when text fields gain focus (auto-show mode)
├── Works with mouse, touchpad, or touch input
│   ├── Mouse: click keys individually
│   ├── Touchpad: drag to key, tap to press
│   └── Touchscreen: tap to press (if touchscreen present)
├── Spike Shell: text fields report focus to keyboard
├── Firefox: supports Wayland text protocol (keyboard appears)
├── KDE applications: native Qt input method support
├── SDDM (login screen): on-screen keyboard available
│   ├── Toggle button on login screen
│   └── Useful if user can't type password (motor difficulty)
├── Performance:
│   ├── Memory: ~15-20MB (when visible, 0 when hidden)
│   ├── CPU: negligible (event-driven, not polling)
│   └── Tier 2: usable (may have slight input lag)
├── Toggle shortcut: Super + Alt + K
└── Memory freed when keyboard is dismissed

Sticky Keys

Sticky Keys — Settings → Accessibility → Motor → Sticky Keys:

[● Enabled]  [○ Disabled]

How it works:
├── Modifier keys (Shift, Ctrl, Alt, Super) "stick" when pressed
├── User presses modifier, releases, then presses the next key
├── Example: Press Shift, release, press A → types "A"
├── Two modifiers: Press Ctrl, release, press Alt, release, press Del → Ctrl+Alt+Del

Options:
  [x] Show sticky key status in panel (visual indicator)
  [x] Beep when modifier is stuck
  [ ] Lock modifier if pressed twice
  [ ] Turn off sticky keys if two keys pressed simultaneously
    (returns to normal mode — useful for sharing keyboard)

Implementation:
├── KWin keyboard filter (intercepts modifier key events)
├── Qt input handling (modifiers applied to subsequent keypress)
├── Status indicator: panel applet shows active sticky modifiers
│   ├── Small icons: [Shift] [Ctrl] [Alt] [Super]
│   └── Highlighted when that modifier is "stuck"
├── Works in all applications (system-level interception)
├── No performance impact
├── Toggle shortcut: Press Shift 5 times (classic accessibility shortcut)
└── Persists across reboots

Slow Keys

Slow Keys — Settings → Accessibility → Motor → Slow Keys:

[● Enabled]  [○ Disabled]

Acceptance delay:  ██████░░░░  (300ms)
(Range: 50ms - 1000ms)
(How long a key must be held before it registers)

How it works:
├── Key must be held down for the acceptance delay before registering
├── Brief key taps (tremors, accidental touches) are ignored
├── Useful for users with tremors who hit keys accidentally

Options:
  [x] Beep when key is accepted
  [x] Show acceptance indicator (visual feedback during hold)
  [ ] Beep when key is rejected

Implementation:
├── KWin keyboard filter (delays key event emission)
├── Qt input handling (events delayed until acceptance threshold met)
├── Visual indicator: progress ring around cursor or panel indicator
├── Works in all applications
├── No performance impact
├── Toggle shortcut: Super + Alt + Q
└── Persists across reboots

Bounce Keys

Bounce Keys — Settings → Accessibility → Motor → Bounce Keys:

[● Enabled]  [○ Disabled]

Rejection delay:  ████░░░░░░  (200ms)
(Range: 50ms - 1000ms)
(Ignores repeat presses of the same key within this window)

How it works:
├── After a key is pressed, the same key is ignored for the
│   rejection delay period
├── Prevents double-presses from tremor (bounce)
├── Different from slow keys: bounce keys filter repeats, slow keys
│   filter short presses
└── Can be used together with slow keys

Options:
  [ ] Beep when key is rejected

Implementation:
├── KWin keyboard filter (deduplicates rapid key repeats)
├── Works in all applications
├── No performance impact
├── Toggle shortcut: Super + Alt + B
└── Persists across reboots

Mouse Keys

Mouse Keys — Settings → Accessibility → Motor → Mouse Keys:

[● Enabled]  [○ Disabled]

How it works:
├── Numpad keys (or custom keys) control the mouse pointer
├── Move: 8/4/6/2 (or numpad arrows)
├── Diagonal: 7/9/1/3
├── Left click: 5
├── Right click: + or right-click key
├── Drag: 0 to start drag, . to release
├── Speed: adjustable acceleration curve

Pointer speed:
  Acceleration:  ██████░░░░  (medium)
  Max speed:     ████████░░  (fast)
  (If too slow: increase max speed. If too fast: decrease acceleration)

Options:
  [x] Show mouse keys indicator in panel
  [ ] Hold Shift to slow down (precise positioning)

Implementation:
├── KWin pointer emulation (translates keyboard to pointer events)
├── Works even if mouse/touchpad is disconnected
├── Pointer appears and moves normally
├── Works in all applications
├── No performance impact
├── Toggle shortcut: Super + Alt + G
└── Persists across reboots

Click Assist (Dwell Click)

Click Assist — Settings → Accessibility → Motor → Click Assist:

[● Enabled]  [○ Disabled]

Mode: [Dwell click] ▼
  ├── Dwell click: pointer auto-clicks after dwelling on a spot
  └── Simulated secondary click: hold primary button for right-click

Dwell click settings:
  Dwell delay:  ████░░░░░░  (1.0 second)
  (How long the pointer must stay still to trigger a click)

  Click type: [Auto] ▼
    ├── Auto (detect based on context: link=left, text=double, etc.)
    ├── Left click only
    ├── Double click only
    └── Right click only

  Dwell tolerance:  ██████░░░░  (10px)
  (How much movement is allowed during dwell before restart)

  Visual feedback:
    [x] Show dwell timer (progress ring around cursor)
    [x] Change cursor shape during dwell

Simulated secondary click settings:
  Delay:  ██████░░░░  (500ms)
  (Hold left mouse button this long to trigger right-click)

Implementation:
├── KWin dwell click plugin (Wayland native)
├── Progress ring rendered as KWin overlay
├── Works with mouse, touchpad, or mouse keys
├── Performance: negligible (timer-based, not polling)
├── Toggle shortcut: Super + Alt + D
└── Persists across reboots

Repeat Keys

Repeat Keys — Settings → Accessibility → Motor → Repeat Keys:

[● Default]  [○ Disabled]  [○ Custom]

If custom:
  Delay:  ████░░░░░░  (500ms before repeat starts)
  Rate:   ██████░░░░  (25 repeats per second)

If disabled:
├── Key repeat is completely turned off
├── Each key press registers once
├── To type repeated characters: press the key multiple times
└── Useful for users who accidentally hold keys

Implementation:
├── Standard XKB/Wayland keyboard repeat settings
├── Applied via KWin input configuration
├── Works in all applications
├── No performance impact
└── Persists across reboots

Cognitive
Simplified Interface

Simplified Interface — Settings → Accessibility → Cognitive → Simplified Interface:

[● Enabled]  [○ Disabled]

When enabled:
├── Launcher: only shows pinned apps + search (no category browse)
├── Panel: only essential applets (clock, network, volume, session)
│   └── Conditional and advanced applets hidden
├── Settings: hides "Advanced" sections (shows only basic settings)
│   └── Advanced still accessible via "Show advanced settings" toggle
├── Notifications: simpler grouping (fewer categories)
├── File manager: simplified sidebar (only main folders)
│   ├── Home, Documents, Downloads, Pictures, Videos, Music, Trash
│   └── Network, Removable Devices hidden (accessible via menu)
├── Desktop: no widgets or shortcuts
└── Right-click context menus: reduced to essential items only

Implementation:

Simplified interface implementation:
├── Spike Shell: configuration flag "simplified_mode": true
├── Stored in spike-config state store (/var/lib/spike/config/state.json)
├── Panel: only loads essential applet subset
├── Launcher: hides category view, shows search + favorites only
├── Settings: "Advanced" sections collapsed by default
├── Dolphin: custom configuration profile (simplified.sidebar)
├── Memory impact: LESS than normal (fewer applets loaded)
├── Disabling returns to full interface immediately
└── Persists across reboots

Reading Assistance (Focus Highlight)

Reading Assistance — Settings → Accessibility → Cognitive → Reading Assistance:

[● Enabled]  [○ Disabled]

Focus highlight:
├── A colored ring is drawn around the currently focused widget
├── Ring color: [Purple] (default) [Yellow] [Blue] [Custom]
├── Ring thickness: [3px] (default, range 2-6px)
├── Ring animation: [Gentle pulse] ▼
│   ├── Gentle pulse (subtle breathing effect)
│   ├── Static (no animation)
│   └── Solid outline
└── Helps users track which element has keyboard focus

Focus tracking:
├── When focus changes, the highlight moves smoothly to the new element
├── In text fields: highlight follows caret position
├── In lists/trees: highlight follows selected item
└── In dialogs: highlight follows the default button

Implementation:
├── Qt QAccessible focus tracking
├── KWin overlay draws the highlight ring (compositor-level)
├── Works in all Qt applications (Spike Shell, KDE apps)
├── Firefox: uses its own focus indication (standard web focus rings)
├── Performance: negligible (single overlay element)
├── Memory: <1MB
└── Persists across reboots

Confirmation Dialogs

Confirmation Dialogs — Settings → Accessibility → Cognitive → Confirmation Dialogs:

[● Enabled]  [○ Disabled]

When enabled:
├── Additional confirmation dialogs appear for potentially destructive actions:
│   ├── Deleting multiple files (confirmation even if in trash)
│   ├── Emptying trash (always confirmed — this is default anyway)
│   ├── Formatting a USB drive
│   ├── Uninstalling an application
│   ├── Disabling the firewall
│   └── Changing system-wide settings (power, boot, firewall)
├── Dialogs are clearer and more descriptive:
│   ├── Title: "Are you sure?" → "Confirm deletion of 5 files"
│   ├── Body: explains what will happen in plain language
│   └── Buttons: "Delete" / "Cancel" (not "OK" / "Cancel")
└── Extra 3-second delay before destructive button is clickable
    (prevents impulsive clicks)

Implementation:
├── Spike Shell: global confirmation dialog system
├── Intercepts destructive actions and inserts confirmation
├── Dolphin: uses its built-in confirmation (enhanced by Spike config)
├── Settings: all system-changing actions wrapped in confirmation
├── No performance impact
└── Persists across reboots

Focus Animation

Focus Animation — Settings → Accessibility → Cognitive → Focus Animation:

[○ Off]  [● On]

When enabled:
├── When focus changes between windows, a gentle pulse animation
│   highlights the newly focused window
├── Animation: soft glow pulse (200ms, purple edge)
├── Helps users with ADHD or attention difficulties track focus changes
├── Subtle enough not to be distracting for neurotypical users
└── Automatically disabled when "Reduce Motion" is enabled

Implementation:
├── KWin window animation (focus change event)
├── Compositor overlay: soft purple glow on window border
├── 200ms duration, single pulse (not repeating)
├── Performance: negligible (only triggers on focus change)
└── Persists across reboots

System
AT-SPI Bridge

AT-SPI Bridge — Settings → Accessibility → System → AT-SPI Bridge:

Status: [● Running]  [○ Stopped]

The AT-SPI (Assistive Technology Service Provider Interface) bridge
is the system that allows assistive technologies (screen reader,
magnifier) to communicate with applications.

├── Starts automatically when any accessibility tool is enabled
├── Stops when no accessibility tools are running
├── Status visible here for diagnostic purposes
├── Memory: ~5MB when running
└── Cannot be manually stopped while accessibility tools are active

Diagnostics:
├── Shows which applications are connected via AT-SPI
├── Shows which accessibility tools are active
└── [Export diagnostic report] (for troubleshooting)

Quick Setup Wizard

Quick Setup Wizard — Settings → Accessibility → System → Quick Setup:

The Quick Setup Wizard helps users quickly identify which accessibility
features they need. It is also presented during first-boot welcome
(optional).

Wizard flow:
├── Step 1: "Do you have trouble seeing the screen?"
│   ├── Yes → Offers: Large Text, High Contrast, Screen Reader
│   ├── Somewhat → Offers: Large Text, Cursor Size
│   └── No → Continue
├── Step 2: "Do you have trouble hearing sounds?"
│   ├── Yes → Offers: Visual Alerts, Mono Audio
│   ├── Somewhat → Offers: Visual Alerts
│   └── No → Continue
├── Step 3: "Do you have trouble using the keyboard or mouse?"
│   ├── Yes → Offers: On-Screen Keyboard, Sticky Keys, Mouse Keys,
│   │          Slow Keys, Bounce Keys, Click Assist
│   ├── Somewhat → Offers: Sticky Keys, Repeat Keys adjustment
│   └── No → Continue
├── Step 4: "Would you like a simpler interface?"
│   ├── Yes → Offers: Simplified Interface
│   ├── No → Continue
│   └── (This question is optional / can be skipped)
├── Step 5: Review selected features
│   ├── List of features to enable
│   ├── Each with a checkbox (user can deselect)
│   └── [Apply selected features] button
└── Step 6: "Accessibility is configured. You can change these
    settings anytime in Settings → Accessibility."

Export/Import Settings

Export/Import — Settings → Accessibility → System → Export/Import:

Export:
├── Saves all accessibility settings to a file
├── File: ~/Documents/spike-accessibility-settings.json
├── Contains: all enabled features, all parameters
├── Useful for: transferring settings to a new install
└── Can be backed up with other user data

Import:
├── Load accessibility settings from exported file
├── Applies all settings immediately
├── Useful after reinstall or upgrade
└── Validation: checks for valid format before applying

Keyboard Shortcuts

All accessibility features have keyboard shortcuts for quick toggling. These use Super + Alt as the modifier combination to avoid conflicts with application shortcuts.

Feature                    Shortcut               Notes
──────────────────────────────────────────────────────────────────
Screen Reader              Super + Alt + S        Toggles Orca
Magnifier                  Super + Alt + M        Toggles magnifier
High Contrast              Super + Alt + C        Toggles high-contrast theme
Large Text                 Super + Alt + L        Toggles 1.25x text scaling
Color Filters             Super + Alt + F        Toggles current filter
Reduce Motion             Super + Alt + R        Toggles animation reduction
Cursor Size                Super + Alt + Shift+C   Cycles: normal→large→XL→huge
Visual Alerts              Super + Alt + V        Toggles visual alerts
Mono Audio                Super + Alt + N        Toggles mono audio mix
On-Screen Keyboard         Super + Alt + K        Toggles on-screen keyboard
Sticky Keys               Shift × 5              Classic sticky keys shortcut
Slow Keys                  Super + Alt + Q        Toggles slow keys
Bounce Keys               Super + Alt + B        Toggles bounce keys
Mouse Keys                Super + Alt + G        Toggles mouse keys
Click Assist              Super + Alt + D        Toggles dwell click
Simplified Interface      Super + Alt + I        Toggles simplified mode

Shortcuts are system-wide — they work regardless of which application has focus. They are active from the login screen (SDDM) onward.
First Boot Integration

First-boot welcome wizard (post-install):

After "Get started" button (existing welcome flow):
├── Optional step: "Set up accessibility"
│   ├── "Would you like to configure accessibility features?"
│   ├── [Yes, run setup wizard] → Quick Setup Wizard runs
│   ├── [Not now] → Continues to desktop
│   └── [Skip this step permanently] → Never shows again
│
├── If user skips: accessibility is available in Settings later
├── If user runs wizard: features are enabled before desktop appears
└── Accessibility shortcut hints shown on first login:
    "Tip: Press Super + Alt + S for screen reader, Super + Alt + K
     for on-screen keyboard. See Settings → Accessibility for more."

Login Screen Accessibility

SDDM (login screen) has accessibility features available before login:

SDDM accessibility:
├── On-screen keyboard:
│   ├── Toggle button on login screen (keyboard icon)
│   ├── Uses same maliit-keyboard as desktop
│   └── Available for password entry
├── Screen reader:
│   ├── Toggle button on login screen (speaker icon)
│   ├── Reads username field, password field prompts
│   ├── Announces login errors
│   └── Turns off after login (Orca takes over if enabled)
├── High contrast:
│   ├── Toggle button (contrast icon)
│   ├── Applies high-contrast SDDM theme
│   └── Carries over to desktop if enabled in Settings
├── Large text:
│   ├── Toggle button (text size icon)
│   └── Scales login screen text by 1.5x
├── Cursor size:
│   └── Uses cursor size from accessibility settings (if set)
└── All toggle buttons are in a row at the bottom-left of the
    login screen, with clear icons and tooltips

SDDM login screen with accessibility controls:

┌──────────────────────────────────────────────────┐
│                                                  │
│              [Spike Logo]                        │
│                                                  │
│         ┌──────────────────────────┐             │
│         │  Username                 │             │
│         └──────────────────────────┘             │
│         ┌──────────────────────────┐             │
│         │  Password             👁 │             │
│         └──────────────────────────┘             │
│                                                  │
│              [ Log In ]                          │
│                                                  │
│  ♿ 🔊 ⌨️ 🔶 📝                                   │
│  (accessibility toggle buttons:                  │
│   screen reader, on-screen keyboard, etc.)      │
│                                                  │
│  [⟳ Restart]            [⏻ Shut Down]           │
│                                                  │
└──────────────────────────────────────────────────┘

Performance Impact Summary

Feature                     Memory (additional)   CPU impact      Tier 1 OK?  Tier 2 OK?
──────────────────────────────────────────────────────────────────────────────────────────
Screen Reader (Orca)         ~15-25MB             <3%             ✓           ✓ (degraded)
Magnifier (2x)               <10MB               <2% (SW)        ✓           ✓ (15-25fps)
High Contrast                0MB                 0%              ✓           ✓
Large Text (1.5x)            <2MB (glyph cache)   0%              ✓           ✓
Color Filters                <1MB                 <2% (SW)        ✓           ✓
Reduce Motion                0MB                 0%              ✓           ✓
Cursor Size (Huge)           <1MB                 0%              ✓           ✓
Visual Alerts                0MB                 <1%             ✓           ✓
Mono Audio                   <1MB                 0%              ✓           ✓
On-Screen Keyboard           ~15-20MB             negligible     ✓           ✓ (slight lag)
Sticky Keys                  0MB                 0%              ✓           ✓
Slow Keys                    0MB                 0%              ✓           ✓
Bounce Keys                  0MB                 0%              ✓           ✓
Mouse Keys                   0MB                 0%              ✓           ✓
Click Assist (Dwell)         0MB                 negligible     ✓           ✓
Simplified Interface         NEGATIVE             0%              ✓           ✓
Reading Assistance           <1MB                 negligible     ✓           ✓
Confirmation Dialogs         0MB                 0%              ✓           ✓
Focus Animation              <1MB                 negligible     ✓           ✓
──────────────────────────────────────────────────────────────────────────────────────────
All enabled simultaneously:  ~50-60MB           <5%             ✓           ✓ (degraded)
Maximum target:              <100MB              <10%            ✓           ✓

Key observations:

    High Contrast, Reduce Motion, and all keyboard modifications cost zero additional memory
    Simplified Interface actually REDUCES memory usage (fewer applets loaded)
    Screen Reader is the heaviest feature (~25MB) but well within budget
    All features work on Tier 1 (Celeron N4020, 4GB RAM)
    On Tier 2 (AMD A4), all features work but some may be visually degraded (lower fps on magnifier, slight input lag on on-screen keyboard)

AT-SPI And Application Compatibility
Which Applications Support AT-SPI

Application              AT-SPI Support        Notes
──────────────────────────────────────────────────────────────────
Spike Shell (panel,      Full                   QAccessible interfaces exposed
launcher, settings)
KWin (window manager)    Full                   Focus, window state events
Dolphin (file manager)   Full                   Qt accessibility
Konsole (terminal)       Partial                Text content may not be fully readable
Kate (text editor)       Full                   Qt accessibility
Ark (archive)            Full                   Qt accessibility
Spectacle (screenshot)   Full                   Qt accessibility
KCalc (calculator)       Full                   Qt accessibility
Firefox                   Full                   Built-in accessibility (Gecko)
LibreOffice               Full                   UNO accessibility bridge
VLC                       Partial                Main interface readable, video content N/A
Discover (software center) Full                  Qt accessibility
Settings (KCM modules)    Full                   Qt accessibility
Flatpak applications      Varies                 Depends on individual app's toolkit support

Applications With Limited Accessibility

Known limitations:
├── Konsole (terminal):
│   ├── Terminal content is not exposed to screen reader
│   ├── Orca reads the menu bar and window title but not terminal output
│   └── Users who need an accessible terminal: use a GUI alternative
│       (Spike's goal is no terminal needed — see Golden Rule 2)
├── VLC:
│   ├── Interface is partially accessible
│   ├── Video content is obviously not "readable"
│   └── Keyboard navigation works for playback controls
├── Games and emulators:
│   ├── No AT-SPI support (typically)
│   └── Not a Spike concern (gaming is not a target use case)
└── Non-Qt/non-GTK applications:
    ├── May not expose AT-SPI interfaces
    ├── Flatpak apps using Electron: limited (Electron AT-SPI support varies)
    └── Flatpak apps using GTK: good (GTK has native AT-SPI support)

Internationalization And Accessibility

Accessibility features work across all 11 supported languages:
├── Screen reader: espeak-ng supports all 11 languages
│   ├── Voice quality varies by language
│   ├── espeak-ng voices are robotic but functional
│   └── Higher-quality voices (if desired): user installs via Discover
│       (e.g., RHVoice, Pico TTS — not pre-installed due to size)
├── On-screen keyboard: adapts to current keyboard layout
│   ├── Layout matches system keyboard layout
│   └── Input method support (e.g., Chinese, Japanese) works
├── Visual alerts: language-independent (visual, not text)
├── High contrast: language-independent
├── Magnifier: language-independent
└── Quick Setup Wizard: translated into all 11 languages

What This Document Does Not Cover

    Spike Shell component architecture (panel, launcher, settings): See DESKTOP.md
    KWin configuration and compositor details: See DESKTOP.md and BOOT-PROCESS.md
    PipeWire audio configuration: See MULTIMEDIA.md
    Qt6 framework and QAccessible: See dev-guide (to be written)
    Hardware tier definitions and performance targets: See PERFORMANCE-BASELINES.md
    Settings panel architecture and KCM integration: See DESKTOP.md (Settings Panel section) and CONFIGURATION.md
    Translation system and language support: See user guide (to be written)
    SDDM configuration: See BOOT-PROCESS.md
    First-boot welcome wizard: See BOOT-PROCESS.md (First Boot section) and INSTALLER.md
    Code of conduct (community accessibility): See CODE_OF_CONDUCT.md (to be written)

🐕 BigRangaTech
