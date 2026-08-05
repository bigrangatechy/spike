# Spike Accessibility

## Purpose

This document specifies the accessibility features of Spike Linux. Accessibility is not an add-on, a checkbox, or a "nice to have." It is a fundamental requirement of an operating system that claims to be for everyone.

Spike targets users who may be using old laptops precisely because they can't afford newer hardware. People with disabilities are disproportionately affected by financial barriers to technology — assistive hardware, specialized software, and modern devices are often priced out of reach. A budget operating system that ignores accessibility is failing the people who need it most.

Every accessibility feature in Spike must comply with the Golden Rules: no terminal required, no config file editing. Accessibility features are configured entirely through **Settings → Accessibility**.

## Design Principles

**1. Every setting has a GUI toggle**

```
├── No accessibility feature requires terminal or config editing
├── All features are in Settings → Accessibility (single location)
└── Settings persist across reboots and upgrades
```

**2. Sensible defaults**

```
├── Features are OFF by default (don't change the experience
│   for users who don't need them)
├── Once enabled, they stay enabled (persist as user preference)
├── First-boot welcome wizard can enable key features
└── Settings → Accessibility → "Quick Setup" detects needs
```

**3. Never break the base experience**

```
├── Enabling an accessibility feature must not crash the desktop
├── Disabling a feature returns to normal instantly
├── Features degrade gracefully on Tier 2 hardware
└── No accessibility feature should consume significant RAM
    (target: <20MB additional for any single feature)
```

**4. Qt6 and Wayland native where possible**

```
├── Use Qt6 accessibility framework (QAccessible) for Spike Shell
├── Use KWin Wayland accessibility hooks for compositor features
├── Use AT-SPI (Assistive Technology Service Provider Interface)
│   as the bridge to screen readers and external tools
└── Avoid custom implementations when standards exist
```

**5. Performance-conscious on target hardware**

```
├── Screen reader must work on Celeron N4020
├── Magnifier must not drop below 20fps on Tier 1
├── Visual alerts must not add perceptible latency
└── On-screen keyboard must be usable on touchpad (no touchscreen needed)
```

## Installed Accessibility Packages

All accessibility packages are installed on every Spike system — even if no accessibility features are enabled.

> **Design decision:** A user who needs accessibility features should never have to "install" them first (which requires internet, Discover, and knowing the package name). The cost is ~40MB of disk space (negligible) and ~0MB of RAM when not in use (daemons not started unless needed). This aligns with the principle that accessibility is fundamental, not an add-on.

**Base packages (always installed):**

```
├── orca                            → Screen reader
├── espeak-ng                       → Speech synthesis engine
├── brltty                          → Braille display daemon
├── maliit-keyboard                 → On-screen keyboard
├── at-spi2-core                    → AT-SPI bridge framework
└── qt-at-spi                       → Qt accessibility bridge
```

**BRLTTY lifecycle:**

```
├── Installed: Always (part of base package set)
├── Started: Only when braille display detected via USB
├── Detection: udev rule triggers BRLTTY start on USB device match
├── If no braille display: BRLTTY not running (0MB RAM)
├── If braille display connected: BRLTTY starts (~3MB RAM)
└── If braille display disconnected: BRLTTY stops after 30s grace period
```

## Accessibility Categories

**Settings → Accessibility** is organized into categories:

```
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
│     ├─ Bounce Keys                                │
│     ├─ Mouse Keys                                 │
│     ├─ Click Assist (dwell click)                  │
│     ├─ Switch Access (scanning)                   │
│     ├─ Repeat Keys (adjust/disable)               │
│     ├─ Alternative Keyboard Layouts               │
│     └─ Voice Control (future)                    │
│                                                  │
│  🧠 Cognitive                                      │
│     ├─ Simplified Interface (reduce clutter)       │
│     ├─ Focus Highlight                             │
│     ├─ Confirmation Dialogs (extra confirmations)  │
│     └─ Focus Animation (gentle pulse)             │
│                                                  │
│  ⚙ System                                          │
│     ├─ AT-SPI Bridge (status)                     │
│     ├─ Quick Setup Wizard                         │
│     └─ Export/Import Settings                     │
│                                                  │
└──────────────────────────────────────────────────┘
```

## Vision

### Screen Reader (Orca)

Spike integrates Orca as the screen reader. Orca is the standard Linux screen reader, supporting AT-SPI and providing speech output and braille display support.

**Settings → Accessibility → Vision → Screen Reader:**

```
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
```

**Implementation:**

```
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
```

### Magnifier

**Settings → Accessibility → Vision → Magnifier:**

```
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
```

**Implementation:**

```
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
```

### High Contrast

**Settings → Accessibility → Vision → High Contrast:**

```
[● Off]  [○ On]
```

**When enabled:**

```
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
```

**Implementation:**

```
├── Separate Qt stylesheet: /usr/share/spike/themes/high-contrast.qss
├── Applied by spike-shell theme engine (same mechanism as normal theme)
├── Overrides Spike Shell, KDE application styling (via Qt platform theme)
├── Firefox: follows system high-contrast setting via prefers-contrast media query
├── Does NOT require reboot (applied immediately)
├── Toggle shortcut: Super + Alt + C
└── Memory: 0 additional (replaces existing stylesheet)
```

### Large Text

**Settings → Accessibility → Vision → Large Text:**

```
[○ Off]  [● On]
Scaling:  ██████████░░░  (1.25x)
(Range: 1.0x to 2.0x in 0.25 increments)
Options: 1.0x (normal) / 1.25x / 1.5x / 1.75x / 2.0x (huge)
```

**When enabled:**

```
├── All Qt text scales by the selected factor
├── Panel height adjusts: 32px → 40px (1.25x) / 48px (1.5x)
├── Launcher icon size increases proportionally
├── Notifications: larger text, larger card
├── Settings: all text scales
├── KDE applications: scale via Qt font DPI
├── Firefox: respects system font scaling
├── SDDM (login screen): scales text
└── KWin window decorations: scale title text
```

**Implementation:**

```
├── Qt font scaling: QT_FONT_DPI adjusted (96 * scale factor)
├── Panel and launcher: pixel sizes multiplied by scale factor
├── Wayland logical size: unaffected (buffer scaling handles it)
├── Performance: no impact (font rendering is cached)
├── Memory: minimal increase (larger font glyph cache)
├── Applies immediately (no reboot)
└── Toggle shortcut: Super + Alt + L
```

### Color Filters

**Settings → Accessibility → Vision → Color Filters:**

```
[○ Disabled]  [● Enabled]
Filter type: [Deuteranopia correction] ▼
Correction filters (shift colors for users with the condition):
  ├── Deuteranopia correction (for green-blind users)
  ├── Protanopia correction (for red-blind users)
  ├── Tritanopia correction (for blue-blind users)
  ├── Deuteranomaly correction (for green-weak users)
  ├── Protanomaly correction (for red-weak users)
  └── Tritanomaly correction (for blue-weak users)
Simulation filters (simulate how others see — for testing):
  ├── Simulate deuteranopia
  ├── Simulate protanopia
  └── Simulate tritanopia
Display adjustments:
  ├── Achromatopsia (grayscale — removes all color)
  ├── Invert colors
  └── Custom (manual hue shift)
Intensity:  ██████████░░░  (100%)
```

**What this does:**

```
├── Correction filters: Shift color palette so affected colors
│   become distinguishable for users with that condition
│   (e.g., red and green shifted apart so a deuteranope can tell
│   them apart)
├── Simulation filters: Shift colors to simulate how someone with
│   that condition perceives the screen (useful for developers
│   testing accessibility, or for educators demonstrating
│   color blindness)
├── Applies to entire screen (compositor-level)
├── Firefox and all applications see the filtered output
└── Screenshots: capture unfiltered (original colors)
```

> **Note:** Correction and simulation are opposite operations. Correction helps someone *with* a condition see more clearly; simulation helps someone *without* a condition understand the condition. The naming makes this distinction explicit to avoid confusion.

**Implementation:**

```
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
```

### Reduce Motion

**Settings → Accessibility → Vision → Reduce Motion:**

```
[● On]  [○ Off]
```

**When enabled:**

```
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
```

**Implementation:**

```
├── Equivalent to Spike Standard's animation behavior (already minimal)
├── If user is on Spike Plus with animations enabled:
│   ├── This setting overrides variant default
│   └── All animations are disabled
├── KWin: sets animation speed factor to 0 (instant)
├── Spike Shell: animation durations set to 0ms
├── No performance benefit (animations are cheap) — this is accessibility
└── Toggle shortcut: Super + Alt + R
```

### Cursor Size

**Settings → Accessibility → Vision → Cursor Size:**

```
Size:  [● Normal] [○ Large] [○ Extra Large] [○ Huge]
Normal:      24px (default)
Large:       32px
Extra Large: 48px
Huge:        64px
```

**When changed:**

```
├── Cursor scales immediately (no reboot)
├── All applications use the new cursor size
├── SDDM (login screen) uses the selected size
└── High-contrast cursor variant used if high contrast is also enabled
```

## Hearing

### Visual Alerts

**Settings → Accessibility → Hearing → Visual Alerts:**

```
[● Enabled]  [○ Disabled]
Alert type:
  [○ Flash screen]
  [● Flash window border]
  [○ Banner notification]
Flash color: [■ White] [■ Yellow] [● Custom]
  (If custom: color picker)
Flash duration: 300ms (default, range 200-1000ms)
```

**What triggers a visual alert:**

```
├── System notification with "critical" urgency
├── Error dialog appearance
├── Battery critically low warning
├── Update failure notification
├── Application crash notification
└── Timer/alarm (if clock applet supports)
```

**What does NOT trigger:**

```
├── Normal notifications (too frequent)
├── Information popups
└── Background events (non-visible to user)
```

**Implementation:**

```
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
```

### Caption Support (Future — Specification)

**Settings → Accessibility → Hearing → Caption Support:**

```
[○ Disabled — Future Feature]
Status: "System-wide captioning is planned for a future Spike release."
```

**What is planned:**

```
├── System-wide audio captions:
│   ├── Real-time captioning of system audio output
│   ├── Uses local speech-to-text (Vosk, same engine as Voice Control)
│   ├── Captions appear as floating overlay (similar to subtitles)
│   └── User can reposition caption window
├── Media captions (video playback):
│   ├── Already supported: SRT, ASS, VTT subtitle files (VLC, Firefox)
│   ├── Already supported: Embedded subtitle tracks in MKV/MP4
│   └── Planned: Auto-caption for videos without subtitle tracks
│       (via local speech recognition on audio track)
└── Communication captions (video calls):
    ├── Live captions for incoming audio in WebRTC calls
    ├── Caption overlay on video call window
    └── Uses same Vosk engine for transcription
```

**Planned caption display:**

```
┌──────────────────────────────────────────────────┐
│  Caption Display                                  │
│                                                  │
│  Position: [Bottom center] ▼                     │
│    ├── Bottom center (subtitle-style)            │
│    ├── Top center                                │
│    ├── Bottom left                               │
│    └── Floating (user drags position)            │
│                                                  │
│  Font: [Noto Sans] ▼                             │
│  Font size: [18pt] ▼ (range: 12-36pt)            │
│  Background: [Semi-transparent black] ▼           │
│    ├── Semi-transparent black (subtle)           │
│    ├── Solid black (high contrast)              │
│    └── No background (text only, with shadow)    │
│                                                  │
│  Max lines: [2] ▼ (1, 2, 3)                      │
│  Max characters per line: [40] ▼                 │
│                                                  │
│  [x] Show speaker name (if identifiable)        │
│  [ ] Include sound effects [bracketed]            │
│  [x] Highlight current word being spoken         │
│                                                  │
└──────────────────────────────────────────────────┘
```

**Planned implementation:**

```
├── Audio capture: PipeWire captures system audio stream
├── Speech recognition: Vosk processes audio in real-time
├── Caption overlay: KWin compositor draws caption text
│   (floating layer above all windows)
├── Performance target: <2 second latency from speech to caption
├── RAM usage: ~80-100MB (Vosk model + audio buffer)
├── CPU usage: ~10-15% on Celeron N4020 (during active captioning)
├── Limitation: accuracy depends on audio clarity and speaker accent
└── Limitation: not suitable for music or non-speech audio
```

**Why not available at launch:**

```
├── Requires Vosk integration (same dependency as Voice Control)
├── CPU/RAM overhead exceeds Spike Standard's budget when combined
│   with other accessibility features
├── Accuracy on target hardware internal speakers: moderate
│   (microphone loopback has echo without headphones)
└── Planned for post-1.0 release (see ROADMAP.md)
```

### Mono Audio

**Settings → Accessibility → Hearing → Mono Audio:**

```
[● Enabled]  [○ Disabled]
```

**When enabled:**

```
├── Left and right audio channels are mixed to mono
├── Output is identical on both speakers/headphones
├── Useful for users with unilateral hearing loss
├── Applies to all audio output (speakers, headphones, Bluetooth)
└── Does NOT affect microphone input (mic stays stereo if available)
```

**Implementation:**

```
├── PipeWire module: module-mono-mix
├── Loaded when mono audio is enabled
├── Unloaded when disabled
├── No perceptible latency or CPU impact
└── Toggle shortcut: Super + Alt + N
```

## Motor

### On-Screen Keyboard

**Settings → Accessibility → Motor → On-Screen Keyboard:**

```
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
```

**Why word prediction is OFF by default:**

```
├── Word prediction requires a language model loaded in memory (~10-15MB)
├── On Celeron N4020 (4GB RAM target), 10-15MB is significant
│   when combined with other accessibility features
├── Prediction quality with lightweight models is poor
│   (frequent irrelevant suggestions, frustrating user experience)
├── Users who need prediction can enable it (toggle is prominent)
├── When enabled: uses presage or predictive library
│   ├── Model: lightweight n-gram (~5MB)
│   └── RAM overhead: ~10MB when enabled
└── Future consideration: ML-based prediction if model size/RAM decreases
```

**Implementation:**

```
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
```

### Sticky Keys

**Settings → Accessibility → Motor → Sticky Keys:**

```
[● Enabled]  [○ Disabled]
```

**How it works:**

```
├── Modifier keys (Shift, Ctrl, Alt, Super) "stick" when pressed
├── User presses modifier, releases, then presses the next key
├── Example: Press Shift, release, press A → types "A"
├── Two modifiers: Press Ctrl, release, press Alt, release,
│   press Del → Ctrl+Alt+Del
```

**Options:**

```
  [x] Show sticky key status in panel (visual indicator)
  [x] Beep when modifier is stuck
  [ ] Lock modifier if pressed twice
  [ ] Turn off sticky keys if two keys pressed simultaneously
    (returns to normal mode — useful for sharing keyboard)
```

**Implementation:**

```
├── KWin keyboard filter (intercepts modifier key events)
├── Qt input handling (modifiers applied to subsequent keypress)
├── Status indicator: panel applet shows active sticky modifiers
│   ├── Small icons: [Shift] [Ctrl] [Alt] [Super]
│   └── Highlighted when that modifier is "stuck"
├── Works in all applications (system-level interception)
├── No performance impact
├── Toggle shortcut: Press Shift 5 times (classic accessibility shortcut)
└── Persists across reboots
```

### Slow Keys

**Settings → Accessibility → Motor → Slow Keys:**

```
[● Enabled]  [○ Disabled]
Acceptance delay:  ██████░░░░  (300ms)
(Range: 50ms - 1000ms)
(How long a key must be held before it registers)
```

**How it works:**

```
├── Key must be held down for the acceptance delay before registering
├── Brief key taps (tremors, accidental touches) are ignored
└── Useful for users with tremors who hit keys accidentally
```

**Options:**

```
  [x] Beep when key is accepted
  [x] Show acceptance indicator (visual feedback during hold)
  [ ] Beep when key is rejected
```

**Implementation:**

```
├── KWin keyboard filter (delays key event emission)
├── Qt input handling (events delayed until acceptance threshold met)
├── Visual indicator: progress ring around cursor or panel indicator
├── Works in all applications
├── No performance impact
├── Toggle shortcut: Super + Alt + Q
└── Persists across reboots
```

### Bounce Keys

**Settings → Accessibility → Motor → Bounce Keys:**

```
[● Enabled]  [○ Disabled]
Rejection delay:  ████░░░░░░  (200ms)
(Range: 50ms - 1000ms)
(Ignores repeat presses of the same key within this window)
```

**How it works:**

```
├── After a key is pressed, the same key is ignored for the
│   rejection delay period
├── Prevents double-presses from tremor (bounce)
├── Different from slow keys: bounce keys filter repeats, slow keys
│   filter short presses
└── Can be used together with slow keys
```

**Options:**

```
  [ ] Beep when key is rejected
```

**Implementation:**

```
├── KWin keyboard filter (deduplicates rapid key repeats)
├── Works in all applications
├── No performance impact
├── Toggle shortcut: Super + Alt + B
└── Persists across reboots
```

### Mouse Keys

**Settings → Accessibility → Motor → Mouse Keys:**

```
[● Enabled]  [○ Disabled]
```

**How it works:**

```
├── Numpad keys (or custom keys) control the mouse pointer
├── Move: 8/4/6/2 (or numpad arrows)
├── Diagonal: 7/9/1/3
├── Left click: 5
├── Right click: + or right-click key
├── Drag: 0 to start drag, . to release
└── Speed: adjustable acceleration curve
```

**Pointer speed:**

```
  Acceleration:  ██████░░░░  (medium)
  Max speed:     ████████░░  (fast)
  (If too slow: increase max speed. If too fast: decrease acceleration)
```

**Options:**

```
  [x] Show mouse keys indicator in panel
  [ ] Hold Shift to slow down (precise positioning)
```

**Implementation:**

```
├── KWin pointer emulation (translates keyboard to pointer events)
├── Works even if mouse/touchpad is disconnected
├── Pointer appears and moves normally
├── Works in all applications
├── No performance impact
├── Toggle shortcut: Super + Alt + G
└── Persists across reboots
```

### Click Assist (Dwell Click)

**Settings → Accessibility → Motor → Click Assist:**

```
[● Enabled]  [○ Disabled]
Mode: [Dwell click] ▼
  ├── Dwell click: pointer auto-clicks after dwelling on a spot
  └── Simulated secondary click: hold primary button for right-click
```

**Dwell click settings:**

```
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
```

**Simulated secondary click settings:**

```
  Delay:  ██████░░░░  (500ms)
  (Hold left mouse button this long to trigger right-click)
```

**Implementation:**

```
├── KWin dwell click plugin (Wayland native)
├── Progress ring rendered as KWin overlay
├── Works with mouse, touchpad, or mouse keys
├── Performance: negligible (timer-based, not polling)
├── Toggle shortcut: Super + Alt + D
└── Persists across reboots
```

### Switch Access (Scanning)

**Settings → Accessibility → Motor → Switch Access:**

```
[○ Disabled]  [● Enabled]
```

**How it works:**

```
├── User operates the interface via one or more switches
│   (external buttons, sip-and-puff, foot pedals, etc.)
├── The system cycles (scans) through interactive elements
│   on screen, highlighting each in sequence
├── When the desired element is highlighted, user activates
│   the switch to select it
├── For hierarchical interfaces (like the Launcher):
│   ├── First scan: groups/categories
│   ├── Second scan: items within selected group
│   └── Third scan: actions (open, pin, uninstall)
└── Fully navigable with a single switch
```

**Scan modes:**

```
├── Automatic scanning:
│   ├── System advances highlight at a fixed interval
│   ├── User presses switch to select
│   └── Scan speed: adjustable (0.5s to 5s per item)
├── Step scanning:
│   ├── One switch: advances to next item
│   ├── Second switch (or same switch, different action): selects
│   └── More precise but requires more switch activations
└── Inverse scanning:
    ├── Switch held down: scanning progresses
    ├── Switch released: current item selected
    └── Good for users who can hold but not tap precisely
```

**Configuration:**

```
Switch input:
├── Number of switches: [1] ▼ (1, 2, or 3)
├── Switch 1 input:
│   ├── [Spacebar] ▼ (keyboard key as switch — common)
│   ├── [Enter] ▼
│   ├── [Mouse button 1] ▼
│   ├── [USB switch device] ▼ (auto-detected)
│   └── [GPIO pin] ▼ (for custom hardware — advanced)
├── Switch 2 input: (if 2+ switches)
│   └── Same options as Switch 1
└── Switch debounce: 50ms (prevents double-activation)
Scanning:
├── Scan mode: [Automatic] ▼
│   ├── Automatic
│   ├── Step (1 switch: advance+select)
│   ├── Step (2 switch: advance / select)
│   └── Inverse
├── Scan interval:  ████░░░░░░░░  (1.5 seconds)
│   (Range: 0.5s to 5.0s)
├── Scan pattern: [Row-column] ▼
│   ├── Row-column (rows first, then items in row)
│   ├── Linear (item by item)
│   └── Group (groups first, then items)
├── First item highlighted: [Top-left] ▼
└── Wrap around: [On] (scanning loops back to start)
Visual feedback:
├── Highlight style: [Colored border] ▼
│   ├── Colored border (3px purple ring around scanned item)
│   ├── Filled background (semi-transparent purple overlay)
│   └── Crosshair (targets the item center)
├── Sound on advance: [ ] (beep on each item — may be annoying)
├── Sound on select: [x] (confirmation beep on selection)
└── Pause on group boundary: [x] (brief pause when entering
    a new group, helps with row-column orientation)
```

**Implementation:**

```
├── KWin input filter (intercepts switch inputs at system level)
├── AT-SPI traversal (walks the accessibility tree of focused window)
├── Spike Shell integration:
│   ├── Panel: scans applets left-to-right
│   ├── Launcher: scans categories, then apps within category
│   ├── Settings: scans pages, then controls within page
│   └── Notifications: scans action buttons in notifications
├── KDE applications: traversed via QAccessible tree
├── Firefox: traversed via Gecko accessibility tree
├── Scanning overlay: KWin compositor draws highlight (same as
│   focus highlight ring)
├── USB switch devices:
│   ├── Detected as HID devices (keyboard, joystick, or mouse)
│   ├── Common switch interfaces supported (AbleNet, Pretorian, etc.)
│   ├── Custom GPIO: via /dev/gpiochip (advanced, documented in dev guide)
│   └── No proprietary driver needed (standard HID)
├── Performance: negligible (timer + traversal, no continuous polling)
├── Memory: ~3MB (scanning engine + overlay)
├── Toggle shortcut: Super + Alt + X
└── Persists across reboots
```

**Hardware support:**

```
├── USB switch adaptors (HID-compatible): auto-detected
├── Adaptive joysticks (as switch input): supported via joystick-to-key
├── Sip-and-puff devices: supported (serial-to-USB adaptors)
├── Foot pedals (USB): detected as keyboard/mouse switches
├── Custom GPIO switches: supported (documented in dev guide)
└── Bluetooth switches: supported (if Bluetooth adapter present)
```

**Known limitations:**

```
├── Scanning in Firefox web pages: depends on page accessibility
│   └── Well-structured pages (ARIA roles) scan well; poorly structured
│       pages may present a confusing scan order
├── Games and emulators: not supported (not a target use case)
└── Custom (non-ATK) widget toolkits: may not be scannable
```

### Repeat Keys

**Settings → Accessibility → Motor → Repeat Keys:**

```
[● Default]  [○ Disabled]  [○ Custom]
If custom:
  Delay:  ████░░░░░░  (500ms before repeat starts)
  Rate:   ██████░░░░  (25 repeats per second)
```

**If disabled:**

```
├── Key repeat is completely turned off
├── Each key press registers once
├── To type repeated characters: press the key multiple times
└── Useful for users who accidentally hold keys
```

**Implementation:**

```
├── Standard XKB/Wayland keyboard repeat settings
├── Applied via KWin input configuration
├── Works in all applications
├── No performance impact
└── Persists across reboots
```

### Alternative Keyboard Layouts

**Settings → Accessibility → Motor → Keyboard Layouts:**

```
┌──────────────────────────────────────────────────┐
│  Keyboard Layout                                  │
│                                                  │
│  Current layout: [US (QWERTY)] ▼                │
│                                                  │
│  Standard layouts:                               │
│    US (QWERTY)                                   │
│    US (Dvorak)                                   │
│    US (Colemak)                                  │
│    UK                                            │
│    (and all standard international layouts —     │
│     see Settings → Keyboard → Layouts)           │
│                                                  │
│  Accessibility layouts:                          │
│    One-handed (Left)                            │
│    One-handed (Right)                           │
│                                                  │
│  [x] Show layout preview                        │
│  [Apply layout]                                 │
│                                                  │
└──────────────────────────────────────────────────┘
```

**One-handed layouts:**

```
├── Left-handed layout:
│   ├── Mirrors QWERTY for single-hand typing
│   ├── Common keys reachable with left hand only
│   └── Designed for users with right-hand mobility impairment
├── Right-handed layout:
│   ├── Mirrors QWERTY for single-hand typing
│   ├── Common keys reachable with right hand only
│   └── Designed for users with left-hand mobility impairment
└── Both layouts: standard XKB layouts (built into Linux)
```

**Dvorak and Colemak:**

```
├── Available as alternative layouts (reduces finger travel)
├── Sometimes used by users with RSI or repetitive strain injuries
├── Not accessibility-exclusive (power users use them too)
├── Listed in Settings → Keyboard → Layouts (general keyboard settings)
└── Also surfaced in Accessibility → Motor → Keyboard Layouts
    (because some users find Dvorak/Colemak reduces strain)
```

**Implementation:**

```
├── Uses standard XKB keyboard layout definitions
├── Layout change applies immediately (no reboot)
├── On-screen keyboard: mirrors the selected physical layout
├── SDDM (login screen): uses system keyboard layout
├── Keyboard layout indicator in panel (shows current layout)
└── Standard international layouts also accessible via
    Settings → Keyboard → Layouts (not just accessibility)
```

### Voice Control (Future — Specification)

**Settings → Accessibility → Motor → Voice Control:**

```
[○ Disabled — Future Feature]
Status: "Voice control is planned for a future Spike release.
        See ROADMAP.md for timeline."
```

**Why voice control is not yet available:**

```
├── Speech recognition is computationally expensive
├── On Celeron N4020 (2 cores, 2.8GHz turbo):
│   ├── Online recognition (cloud-based): feasible but requires
│   │   internet, raises privacy concerns, and adds latency
│   ├── Offline recognition (local): CPU usage 15-30% continuous
│   │   during active listening — unacceptable on battery
│   └── Whisper (open-source model): small model ~400MB RAM,
│       would push idle memory budget past target
├── Privacy constraint: Spike does not send audio to cloud services
│   (core principle — see PRIVACY.md)
├── Microphone quality on target hardware: often poor
│   (budget laptop internal mics have high noise floor)
└── Integration complexity: requires Vosk + AT-SPI command mapping
```

**Planned features (when implemented):**

```
├── Voice dictation:
│   ├── Speech-to-text in any text field
│   ├── Operates entirely offline (local model)
│   ├── Model: Vosk (small, ~50MB model)
│   ├── Languages: English at launch, expanding with Vosk model availability
│   └── Activation: Super + Alt + D (dictation toggle) or
│       microphone button on on-screen keyboard
├── Voice commands:
│   ├── "Open [application]" — launches apps from Launcher
│   ├── "Scroll down" / "Scroll up" — page navigation
│   ├── "Click [element name]" — AT-SPI element targeting
│   ├── "Go back" / "Go forward" — browser navigation
│   ├── "Close window" — window management
│   └── "Show desktop" — minimize all
├── Voice cursor control:
│   ├── "Move mouse up/down/left/right" — directional pointer control
│   ├── "Click" / "Right click" / "Double click" — mouse actions
│   └── "Drag" / "Drop" — drag and drop via voice
└── Wake word: "Spike" (configurable, or push-to-talk)
```

**Planned implementation:**

```
├── Engine: Vosk (offline speech recognition)
│   ├── Model size: ~50MB (English small model)
│   ├── RAM usage: ~80-100MB during active listening
│   ├── CPU usage: ~5-10% on Celeron N4020 (continuous)
│   └── Push-to-talk mode: CPU usage drops to ~0% between activations
├── Command parsing:
│   ├── Local grammar-based parser (no cloud NLU)
│   ├── Predefined command vocabulary (extensible)
│   └── Context-aware: "Click 'Save'" targets buttons labelled "Save"
├── AT-SPI integration:
│   ├── Enumerates accessible elements in focused window
│   ├── Matches spoken names to element names
│   └── Triggers action on matched element
├── Privacy:
│   ├── All processing local (no audio leaves the device)
│   ├── No audio recording stored (transient processing only)
│   └── Model stored locally (no network dependency)
└── Hardware recommendation:
    ├── External USB microphone recommended (better SNR)
    └── Internal microphone: functional but higher error rate
```

**Settings UI (placeholder, shown when feature is available):**

```
┌──────────────────────────────────────────────────┐
│  Voice Control                                    │
│                                                  │
│  [○ Disabled]                                    │
│  "Voice control is coming soon. This feature     │
│   is being developed for a future Spike release." │
│                                                  │
│  Planned capabilities:                           │
│  ✓ Offline speech-to-text dictation              │
│  ✓ Voice commands for app launching and          │
│    window management                             │
│  ✓ Voice cursor control (move, click, drag)      │
│  ✓ Push-to-talk or wake word activation          │
│  ✓ 100% offline — no audio sent to any server    │
│                                                  │
│  [Notify me when available]                      │
│  (Sends no data — just enables a local flag      │
│   to highlight the feature in future updates)    │
│                                                  │
└──────────────────────────────────────────────────┘
```

## Cognitive

### Simplified Interface

**Settings → Accessibility → Cognitive → Simplified Interface:**

```
[● Enabled]  [○ Disabled]
```

**When enabled:**

```
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
```

**Implementation:**

```
├── Spike Shell: configuration flag "simplified_mode": true
├── Stored in spike-config state store (/var/lib/spike/config/state.json)
├── Panel: only loads essential applet subset
├── Launcher: hides category view, shows search + favorites only
├── Settings: "Advanced" sections collapsed by default
├── Dolphin: custom configuration profile (simplified.sidebar)
├── Memory impact: LESS than normal (fewer applets loaded)
├── Disabling returns to full interface immediately
└── Persists across reboots
```

### Focus Highlight

> **Naming note:** Previously called "Reading Assistance (highlight focus)." Renamed to "Focus Highlight" for accuracy — the feature draws a colored ring around the currently focused widget. "Reading Assistance" implied text-to-speech functionality, which was misleading.

**Settings → Accessibility → Cognitive → Focus Highlight:**

```
[● Enabled]  [○ Disabled]
```

**Focus highlight:**

```
├── A colored ring is drawn around the currently focused widget
├── Ring color: [Purple] (default) [Yellow] [Blue] [Custom]
├── Ring thickness: [3px] (default, range 2-6px)
├── Ring animation: [Gentle pulse] ▼
│   ├── Gentle pulse (subtle breathing effect)
│   ├── Static (no animation)
│   └── Solid outline
└── Helps users track which element has keyboard focus
```

**Focus tracking:**

```
├── When focus changes, the highlight moves smoothly to the new element
├── In text fields: highlight follows caret position
├── In lists/trees: highlight follows selected item
└── In dialogs: highlight follows the default button
```

**Implementation:**

```
├── Qt QAccessible focus tracking
├── KWin overlay draws the highlight ring (compositor-level)
├── Works in all Qt applications (Spike Shell, KDE apps)
├── Firefox: uses its own focus indication (standard web focus rings)
├── Performance: negligible (single overlay element)
├── Memory: <1MB
└── Persists across reboots
```

### Confirmation Dialogs

**Settings → Accessibility → Cognitive → Confirmation Dialogs:**

```
[● Enabled]  [○ Disabled]
```

**When enabled:**

```
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
```

**Implementation:**

```
├── Spike Shell: global confirmation dialog system
├── Intercepts destructive actions and inserts confirmation
├── Dolphin: uses its built-in confirmation (enhanced by Spike config)
├── Settings: all system-changing actions wrapped in confirmation
├── No performance impact
└── Persists across reboots
```

### Focus Animation

**Settings → Accessibility → Cognitive → Focus Animation:**

```
[○ Off]  [● On]
```

**When enabled:**

```
├── When focus changes between windows, a gentle pulse animation
│   highlights the newly focused window
├── Animation: soft glow pulse (200ms, purple edge)
├── Helps users with ADHD or attention difficulties track focus changes
├── Subtle enough not to be distracting for neurotypical users
└── Automatically disabled when "Reduce Motion" is enabled
```

**Implementation:**

```
├── KWin window animation (focus change event)
├── Compositor overlay: soft purple glow on window border
├── 200ms duration, single pulse (not repeating)
├── Performance: negligible (only triggers on focus change)
└── Persists across reboots
```

## System

### AT-SPI Bridge

**Settings → Accessibility → System → AT-SPI Bridge:**

```
Status: [● Running]  [○ Stopped]
```

The AT-SPI (Assistive Technology Service Provider Interface) bridge is the system that allows assistive technologies (screen reader, magnifier, switch access) to communicate with applications.

```
├── Starts automatically when any accessibility tool is enabled
├── Stops when no accessibility tools are running
├── Status visible here for diagnostic purposes
├── Memory: ~5MB when running
└── Cannot be manually stopped while accessibility tools are active
```

**Crash recovery:**

```
├── AT-SPI is monitored by spike-session (watchdog)
├── If AT-SPI crashes or exits unexpectedly:
│   ├── spike-session detects exit within 2 seconds
│   ├── AT-SPI automatically restarted
│   ├── All active accessibility tools reconnect automatically
│   ├── Notification: "An accessibility service restarted.
│   │   You may notice a brief interruption."
│   └── Crash logged in /var/log/spike/accessibility.log
├── If AT-SPI fails to restart (3 attempts):
│   ├── Notification: "Accessibility service could not restart.
│   │   Some features may be unavailable. Please save your work
│   │   and log out, then log back in."
│   ├── Screen reader: switches to basic mode (reads panel only)
│   ├── Magnifier: continues (compositor-level, no AT-SPI dependency)
│   └── Switch access: disabled (requires AT-SPI for element traversal)
├── If Orca (screen reader) crashes:
│   ├── spike-session detects exit
│   ├── Orca automatically restarted
│   ├── Brief silence (1-2 seconds during restart)
│   ├── Notification: "Screen reader restarted."
│   └── Crash logged
└── Auto-restart is always enabled (cannot be disabled)
    — users who depend on these tools cannot afford silent failures
```

**Diagnostics:**

```
├── Shows which applications are connected via AT-SPI
├── Shows which accessibility tools are active
├── Shows crash/restart history (last 10 events)
└── [Export diagnostic report] (for troubleshooting)
```

### Quick Setup Wizard

**Settings → Accessibility → System → Quick Setup:**

The Quick Setup Wizard helps users quickly identify which accessibility features they need. It is also presented during first-boot welcome (optional).

**Wizard flow:**

```
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
│   │          Slow Keys, Bounce Keys, Click Assist, Switch Access
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
```

### Export/Import Settings

**Settings → Accessibility → System → Export/Import:**

**Export:**

```
├── Saves all accessibility settings to a file
├── File: ~/Documents/spike-accessibility-settings.json
├── Contains: all enabled features, all parameters
├── Useful for: transferring settings to a new install
└── Can be backed up with other user data
```

**Import:**

```
├── Load accessibility settings from exported file
├── Applies all settings immediately
├── Useful after reinstall or upgrade
└── Validation: checks for valid format before applying
```

## Keyboard Shortcuts

All accessibility features have keyboard shortcuts for quick toggling. These use Super + Alt as the modifier combination to avoid conflicts with application shortcuts.

| **Feature** | **Shortcut** | **Notes** |
| :-: | :-: | :-: |
| Screen Reader | Super + Alt + S | Toggles Orca |
| Magnifier | Super + Alt + M | Toggles magnifier |
| High Contrast | Super + Alt + C | Toggles high-contrast theme |
| Large Text | Super + Alt + L | Toggles 1.25x text scaling |
| Color Filters | Super + Alt + F | Toggles current filter |
| Reduce Motion | Super + Alt + R | Toggles animation reduction |
| Cursor Size | Super + Alt + Shift+C | Cycles: normal→large→XL→huge |
| Visual Alerts | Super + Alt + V | Toggles visual alerts |
| Mono Audio | Super + Alt + N | Toggles mono audio mix |
| On-Screen Keyboard | Super + Alt + K | Toggles on-screen keyboard |
| Sticky Keys | Shift × 5 | Classic sticky keys shortcut |
| Slow Keys | Super + Alt + Q | Toggles slow keys |
| Bounce Keys | Super + Alt + B | Toggles bounce keys |
| Mouse Keys | Super + Alt + G | Toggles mouse keys |
| Click Assist | Super + Alt + D | Toggles dwell click |
| Switch Access | Super + Alt + X | Toggles switch scanning |
| Simplified Interface | Super + Alt + I | Toggles simplified mode |

Shortcuts are system-wide — they work regardless of which application has focus. They are active from the login screen (SDDM) onward.

## First Boot Integration

First-boot welcome wizard (post-install):

```
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
```

## Login Screen Accessibility

SDDM (login screen) has accessibility features available before login:

```
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
```

SDDM login screen with accessibility controls:

```
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
```

## AT-SPI and Application Compatibility

### Which Applications Support AT-SPI

| **Application** | **AT-SPI Support** | **Notes** |
| :-: | :-: | :-: |
| Spike Shell (panel, launcher, settings) | Full | QAccessible interfaces exposed |
| KWin (window manager) | Full | Focus, window state events |
| Dolphin (file manager) | Full | Qt accessibility |
| Konsole (terminal) | Partial | Text content may not be fully readable |
| Kate (text editor) | Full | Qt accessibility |
| Ark (archive) | Full | Qt accessibility |
| Spectacle (screenshot) | Full | Qt accessibility |
| KCalc (calculator) | Full | Qt accessibility |
| Firefox | Full | Built-in accessibility (Gecko) |
| LibreOffice | Full | UNO accessibility bridge |
| VLC | Partial | Main interface readable, video content N/A |
| Discover (software center) | Full | Qt accessibility |
| Settings (KCM modules) | Full | Qt accessibility |
| Flatpak applications | Varies | Depends on individual app's toolkit support |

### Applications With Limited Accessibility

**Known limitations:**

```
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
```

## Internationalization and Accessibility

Accessibility features work across all 11 supported languages:

```
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
```

## Accessibility Testing & Verification

### Testing Methodology

Accessibility is validated through multiple approaches:

**1. Automated AT-SPI audits**

```
├── Tool: axe-core (web) + custom AT-SPI validator (native)
├── Every Spike Shell widget is checked for:
│   ├── Accessible name (non-empty, human-readable)
│   ├── Accessible description (meaningful)
│   ├── Accessible role (correct: button, label, list, etc.)
│   ├── Focusable state (keyboard navigable)
│   ├── Action list (what actions are available)
│   └── Parent-child relationships (logical tree structure)
├── Run in CI pipeline on every code change affecting Spike Shell
└── Failures block merge (same as security checklist)
```

**2. Screen reader walk-throughs**

```
├── Developer task: Navigate entire Settings panel with Orca
│   ├── Every page reachable via keyboard
│   ├── Every control announced by Orca
│   ├── Every dialog read aloud
│   └── No "unlabelled widget" warnings
├── Developer task: Navigate Launcher with Orca
│   ├── Search: type and hear results
│   ├── Categories: tab through and hear names
│   └── Favorites: hear app names
├── Developer task: Navigate panel with Orca
│   ├── Every applet: hear name and current state
│   └── Notifications: hear content
└── Documented in CONTRIBUTING.md as required step before merge
```

**3. Keyboard-only navigation tests**

```
├── Every GUI task completable without mouse:
│   ├── Open launcher, search, launch app
│   ├── Close window, switch workspace
│   ├── Open Settings, change a setting, apply
│   ├── Connect to Wi-Fi
│   ├── Pair Bluetooth device
│   └── Take a screenshot
├── Tab order is logical (left-to-right, top-to-bottom)
├── No keyboard traps (focus never stuck in a widget)
└── Escape closes dialogs (every dialog)
```

**4. Contrast verification**

```
├── Automated: WCAG 2.1 AA contrast ratio checked programmatically
│   ├── Normal text: ≥ 4.5:1
│   ├── Large text: ≥ 3.1:1
│   └── UI components: ≥ 3:1
├── High contrast theme: WCAG AAA (7:1 where possible)
├── Checked at every theme change
└── Tool: contrast-ratio checker in CI
```

**5. User testing (planned)**

```
├── Phase 1 (Alpha): Developer walk-throughs with simulated impairments
│   ├── Blindfolded navigation (screen reader only)
│   ├── One-handed operation (test one-handed layout)
│   └── Keyboard-only (no mouse/touchpad)
├── Phase 2 (Beta): Community testing with disabled users
│   ├── Reach out to disability advocacy groups
│   ├── Provide test laptops with Spike installed
│   ├── Collect structured feedback:
│   │   ├── What worked
│   │   ├── What was frustrating
│   │   ├── What was missing
│   │   └── Comparative rating vs previous OS
│   └── Iterate based on feedback
└── Phase 3 (Post-release): Continuous feedback channel
    ├── Accessibility feedback tag in issue tracker
    ├── Priority handling for accessibility bugs (P1)
    └── Regular accessibility review (quarterly)
```

### Bug Priority for Accessibility Issues

```
├── P1 (Critical): Feature doesn't work, no workaround
│   ├── Screen reader can't read a core panel
│   ├── Keyboard navigation trap in Settings
│   └── On-screen keyboard doesn't appear in text field
├── P2 (High): Feature works but poorly
│   ├── Screen reader announces wrong label
│   ├── Contrast ratio fails WCAG AA
│   └── Tab order illogical
├── P3 (Medium): Cosmetic or minor
│   ├── Focus highlight doesn't appear on one widget type
│   ├── Visual alert too subtle
│   └── Cursor size doesn't apply to one application
└── P4 (Low): Enhancement request
    ├── Additional voice/language for screen reader
    ├── New color filter type
    └── Improved prediction model
```

## Performance Impact Summary

| Feature | Memory (additional) | CPU impact | Tier 1 OK? | Tier 2 OK? |
| :-: | -: | -: | :-: | :-: |
| Screen Reader (Orca) | ~25MB | <3% | ✓ | ✓ (degraded) |
| Magnifier (2x) | <10MB | <2% (SW) | ✓ | ✓ (15–25fps) |
| High Contrast | 0MB | 0% | ✓ | ✓ |
| Large Text (1.5x) | <2MB (glyph cache) | 0% | ✓ | ✓ |
| Color Filters | <1MB | <2% (SW) | ✓ | ✓ |
| Reduce Motion | 0MB | 0% | ✓ | ✓ |
| Cursor Size (Huge) | <1MB | 0% | ✓ | ✓ |
| Visual Alerts | 0MB | <1% | ✓ | ✓ |
| Mono Audio | <1MB | 0% | ✓ | ✓ |
| On-Screen Keyboard | ~20MB | negligible | ✓ | ✓ (slight lag) |
| Sticky Keys | 0MB | 0% | ✓ | ✓ |
| Slow Keys | 0MB | 0% | ✓ | ✓ |
| Bounce Keys | 0MB | 0% | ✓ | ✓ |
| Mouse Keys | 0MB | 0% | ✓ | ✓ |
| Click Assist (Dwell) | 0MB | negligible | ✓ | ✓ |
| Switch Access | ~3MB | negligible | ✓ | ✓ |
| Simplified Interface | **NEGATIVE** | 0% | ✓ | ✓ |
| Focus Highlight | <1MB | negligible | ✓ | ✓ |
| Confirmation Dialogs | 0MB | 0% | ✓ | ✓ |
| Focus Animation | <1MB | negligible | ✓ | ✓ |
| Voice Control (future) | ~100MB (active) | ~10% | ⚠️ TBD | ❌ |
| Caption Support (future) | ~100MB (active) | ~15% | ⚠️ TBD | ❌ |

**Totals:**

```
All current features enabled simultaneously:
├── Total additional memory: ~62-68MB
│   (shared AT-SPI bridge: ~5MB counted once, not per-feature)
├── Total additional CPU: <5%
├── Maximum target: <100MB, <10% CPU
└── Well within budget for Tier 1 hardware (4GB RAM)
```

> **Reconciliation note:** Previous version listed "~50-60MB" which did not account for AT-SPI bridge memory being shared across features (counted once) versus the sum of individual feature estimates (which appeared to count it multiple times). Corrected total: ~62-68MB with AT-SPI counted once. Still well within the <100MB budget.

**Key observations:**

```
├── High Contrast, Reduce Motion, and all keyboard modifications
│   cost zero additional memory
├── Simplified Interface actually REDUCES memory usage
│   (fewer applets loaded)
├── Screen Reader is the heaviest feature (~25MB) but well within budget
├── All features work on Tier 1 (Celeron N4020, 4GB RAM)
└── On Tier 2 (AMD A4), all features work but some may be visually
    degraded (lower fps on magnifier, slight input lag on OSK)
```

## What This Document Does Not Cover

- **Spike Shell component architecture (panel, launcher, settings):** See `DESKTOP.md` 

- **KWin configuration and compositor details:** See `DESKTOP.md` and `BOOT-PROCESS.md` 

- **PipeWire audio configuration:** See `MULTIMEDIA.md` 

- **Qt6 framework and QAccessible:** See dev guide (to be written) 

- **Hardware tier definitions and performance targets:** See `PERFORMANCE-BASELINES.md` 

- **Settings panel architecture and KCM integration:** See `DESKTOP.md` and `CONFIGURATION.md` 

- **Translation system and language support:** See user guide (to be written) 

- **SDDM configuration:** See `BOOT-PROCESS.md` 

- **First-boot welcome wizard:** See `BOOT-PROCESS.md` and `INSTALLER.md` 

- **Code of conduct (community accessibility):** See `CODE_OF_CONDUCT.md` 

🐕 BigRangaTech


