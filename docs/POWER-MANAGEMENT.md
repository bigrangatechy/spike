# Spike Power Management

## Purpose

This document specifies all power management behavior in Spike — suspend, hibernate, screen blanking, CPU frequency scaling, display brightness, battery management, and peripheral power controls. The goal is to maximize battery life on laptops while preserving usability and responsiveness.

## Design Principles

Spike power management follows these principles:

**1. Conservative defaults**

```
├── Don't aggressively cut functionality to save power
└── Balance battery life with user experience
```

**2. User-visible controls for everything**

```
├── Every power setting has a GUI (Golden Rule 1)
└── No terminal commands required (Golden Rule 2)
```

**3. Hardware-aware optimization**

```
├── Adapt to storage type (HDD vs SSD power characteristics)
├── Adapt to GPU capabilities
└── Respect thermal constraints
```

**4. Progressive optimization**

```
├── Idle timeouts increase over time
├── More aggressive power savings when idle longer
└── Wake quickly when user interacts
```

## Power Profiles

### Automatic Profile Selection

Spike uses two power profiles that activate automatically:

**Profile: Performance**

```
├── Activated when: AC adapter connected OR battery > 50%
├── CPU governor: Depends on variant
│   ├── Standard: powersave (stable, efficient)
│   └── Plus: schedutil (responsive, adaptive)
├── Screen timeout: 10 minutes (AC) / 15 minutes (battery >50%)
├── Display dimming: Gradual, starts at 5 minutes idle
├── Disk spin-down: Disabled (SSD) / 15 minutes (HDD)
├── WiFi power saving: Off (keep connection strong)
├── Bluetooth power saving: Off (keep peripherals responsive)
└── USB autosuspend: Disabled (keep peripherals responsive)
```

**Profile: Battery Saver**

```
├── Activated when: AC disconnected AND battery ≤ 50%
├── CPU governor: powersave (both variants)
├── Screen timeout: 5 minutes
├── Display dimming: Starts at 3 minutes idle, faster ramp
├── Dimming target: 50% brightness (if user set higher)
├── Disk spin-down: 10 minutes (HDD only)
├── WiFi power saving: On (reduce radio power)
├── Bluetooth power saving: On (reduce radio power)
├── USB autosuspend: Enabled (suspend unused ports)
└── Night Light: Automatically enabled (reduces display power)
```

### Manual Override

Users can force a profile via **Settings → Power**:

Available options:

```
├── Performance (always)
├── Battery Saver (always)
├── Adaptive (default — switches automatically)
└── Critical (emergency — maximizes battery life)
```

Critical profile:

```
├── Activates at battery ≤ 20% (even if AC present)
├── Screen dims to 30% immediately
├── Screen off after 2 minutes
├── CPU locked to minimum frequency
├── All non-essential services paused
├── WiFi/Bluetooth reduced power or disabled
├── Warning: "Battery critically low. System entering critical mode."
└── Exit condition: AC connected OR battery recharged to 40%+
```

## Sleep Modes

### Suspend (Default)

Suspend puts the system into low-power standby (S3 state):

**What happens:**

```
├── Current session saved to RAM
├── CPU enters deep sleep (C-states)
├── DRAM in self-refresh mode (retains state)
├── Disks powered down
├── Peripherals powered down except wake triggers
├── Network adapters enter low-power mode
└── Wake via: lid open, power button, keyboard, mouse
```

| **Property** | **Value** |
| :-: | :-: |
| Power draw | 0.5–2W (varies by hardware) |
| Resume time | < 1 second (target) |
| Memory retained | Entire session (apps open, windows positioned) |

Wake triggers:

```
├── Open laptop lid (lid switch)
├── Press power button (short press)
├── Press any key on keyboard
├── Move/click mouse or touchpad
└── Incoming network packet (Wake-on-LAN — conditional)
```

Notes:

```
├── Suspend is the default "sleep" action
├── Users access it via Session Menu → Suspend
└── Keyboard shortcut: Super+S (configurable in Settings → Keyboard)
```

### Hibernate (Conditional)

Hibernate saves session to disk and powers off completely (S4 state):

**What happens:**

```
├── Entire RAM contents written to swap partition/file
├── System powers off completely (0W)
├── No power drain while hibernating
└── Resume restores exact session from disk
```

| **Property** | **Value** |
| :-: | :-: |
| Resume time | 5–15 seconds (depends on RAM size and disk speed) |
| Power draw | 0W (fully off) |

Requirements:

```
├── Swap space ≥ RAM size (8GB swap for 4GB RAM)
├── Disk must support hibernate (SSD preferred, HDD acceptable)
├── Kernel hibernation support (enabled by default)
└── No NVIDIA proprietary driver conflicts (known issue — warn user)
```

Availability:

```
├── Detected during install
├── If swap < RAM size: "Hibernate not available (insufficient swap)"
├── If swap sufficient: hibernate option appears in Session Menu
└── If hibernate fails: automatic fall back to suspend
```

**Settings → Power:**

```
├── [Enable Hibernate] checkbox
├── [Require password on resume] checkbox
└── [Hibernate timeout] slider: 10 min, 30 min, 1 hour, 2 hours, never
```

Hibernate timeout behavior:

```
├── After X minutes idle on battery: hibernate instead of suspend
├── Prevents RAM drain if laptop forgotten in bag
└── Default: 30 minutes on battery
```

### Hybrid Sleep (Recommended for Laptop)

Hybrid sleep combines suspend and hibernate:

**What happens:**

```
├── Session saved to both RAM and disk simultaneously
├── System enters suspend mode
├── If power loss occurs (RAM drains): hibernate kicks in
├── Resume from suspend: fast (< 1 sec)
└── Resume from hibernate: slower (5-15 sec) but still works
```

Benefits:

```
├── Fast resume normally (like suspend)
├── Survives power loss (like hibernate)
└── No manual action required
```

Drawbacks:

```
├── Slightly more disk I/O during sleep initiation
├── Takes both RAM AND swap space
└── Only worth enabling on battery-powered laptops
```

Spike default:

```
├── Standard: Hybrid sleep ON (safety net for low RAM systems)
├── Plus: Hybrid sleep OFF (trust RAM to survive short power loss)
└── User can change in Settings → Power
```

Implementation:

```
├── Uses systemd's hybrid-sleep.target
├── Configured via logind.conf
└── User interface via power button/lid settings
```

### Settings Integration

**Settings → Power → Sleep Behavior:**

```
Sleep Options:
├── [x] Allow suspend
├── [ ] Allow hibernate (grayed out if swap < RAM)
├── [x] Hybrid sleep (recommended for laptops)
Sleep Triggers:
├── Close lid: [Suspend] ▼
│   ├── Do nothing
│   ├── Suspend
│   ├── Hibernate
│   └── Shutdown
├── Power button: [Suspend] ▼
│   ├── Do nothing
│   ├── Suspend
│   ├── Hibernate
│   ├── Shutdown
│   └── Log out
├── Sleep key: [Suspend] ▼
│   ├── Do nothing
│   ├── Suspend
│   └── Hibernate (if available)
Automatic Hibernate:
├── After suspend for [30 minutes] on battery ▼
│   ├── 10 minutes
│   ├── 30 minutes (default)
│   ├── 1 hour
│   ├── 2 hours
│   └── Never
Security:
├── [ ] Require password on wake
└── [ ] Lock screen automatically when suspended
```

All changes take effect immediately (no reboot required).

## Screen Management

### Brightness Control

Hardware interface:

```
├── Primary: /sys/class/backlight/*/brightness (read; write when permitted)
├── Preferred write path: logind Session.SetBrightness (unprivileged seat)
└── Fallback: brightnessctl -d <device> set <value>
```

Controls:

```
├── Physical function keys (Fn + brightness up/down)
├── Brightness applet in tray (slide control — live via BrightnessClient)
├── Settings → Display → Brightness slider (when KCM exposes it)
└── Auto-adjust (if ambient light sensor present) — later
```

Range: 0–100% (hardware-dependent minimum) Minimum safe: 10% (below this, LCD may not turn on)

### Block sleep and screen locking (Plasma-equivalent)

Session-scoped switch (not an always-visible Power tray):

```
├── Battery tray popup → “Manually block sleep and screen locking”
├── Settings → Power → same checkbox (for AC-only machines without battery applet)
├── Implementation: logind Inhibit("sleep:idle", …, "block") via SleepInhibit
├── Persists in QSettings power/blockSleepAndLocking; restored on shell start
├── Blocks idle sleep and idle lock; manual Lock Screen still works
└── Manual Suspend: confirm “Sleep/locking block is on — suspend anyway?”
```

Auto-dim settings:

```
├── Start dimming after: 3-10 minutes idle
├── Target brightness: 50-100% (adjustable)
└── Ramp time: gradual over 30 seconds (not abrupt)
```

Night Light integration:

```
├── When Night Light active: minimum brightness = 50% of normal minimum
└── Prevents display from going too dim (color temp shift makes it appear darker)
```

### Screen Blank / Power Saving

Screen blanks (turns off backlight) after timeout.

Timeout schedule (adaptive based on power profile):

**Performance (AC):**

```
├── Dimming starts: 10 minutes idle
├── Blank: 15 minutes idle
└── Power down GPU: 30 minutes idle
```

**Battery Saver:**

```
├── Dimming starts: 3 minutes idle
├── Blank: 5 minutes idle
└── Power down GPU: 15 minutes idle
```

**Critical (battery ≤ 20%):**

```
├── Dimming starts: Immediately
├── Blank: 2 minutes idle
└── Power down GPU: 5 minutes idle
```

**Settings → Display:**

```
├── Screen blank: [15 minutes] ▼
│   ├── Never
│   ├── 1 minute
│   ├── 2 minutes
│   ├── 5 minutes
│   ├── 10 minutes
│   ├── 15 minutes (default on AC)
│   └── 30 minutes
├── Dim before blank: [On] checkbox
├── Dimming start time: [5 minutes] ▼
├── Dimming target: [50%] ▼ (of current brightness)
Security:
├── [x] Lock screen when blanked (default ON)
└── [ ] Require password to wake (optional — slows wake, more secure)
```

### Screensaver

Spike does **NOT** use traditional screensavers (animated idle screens).

Reason: Screensavers consume CPU/GPU cycles to display animations, wasting power.

Instead:

```
├── Screen blanks (backlight off) → power efficient
├── Optionally lock screen for security
└── If user wants visual feedback when idle:
    └── Suggest installing a dedicated screensaver (e.g., xscreensaver)
```

**Settings → Appearance → Screensaver:**

```
├── [ ] Use screensaver instead of blank (not recommended)
└── [None] (default — blank and lock)
```

Recommendation shown: *"Blanking the screen uses less power than a screensaver. We recommend keeping screensaver off for better battery life."*

## CPU Power Management

### Frequency Scaling

CPU governors (set at install time based on variant):

**Standard (Celeron-class CPUs):**

```
├── Governor: powersave
├── Behavior: Keeps CPU at lowest stable frequency
├── Peak frequencies accessed only under load
├── Optimized for battery life over burst performance
└── Best for: N4020, Pentium Silver, low-power Atom
```

**Plus (Modern i3/i5/i7/Ryzen):**

```
├── Governor: schedutil
├── Behavior: Frequencies scale with scheduler load hints
├── Faster frequency ramping than powersave
├── Better balance between performance and efficiency
└── Best for: 8th-gen Intel+, Ryzen 3000+
```

Manual override:

```
├── Settings → Advanced → Power → CPU governor
│   ├── Powersave (default Standard)
│   └── Schedutil (default Plus)
└── Performance (available on all systems):
    └── Forces highest frequency (decreases battery life)
```

Performance mode activation:

```
├── When AC connected AND user selects Performance profile
├── CPU governor temporarily set to performance
├── Reverts to variant default when profile changes
└── Notification: "Performance mode enabled (higher power usage)"
```

### Thermal Throttling

Spike respects hardware thermal management:

**What Spike does:**

```
├── Monitors CPU temperature via hwmon sensors
├── Alerts if temperature > 80°C sustained
├── Notification: "System hot. Consider improving ventilation."
└── Does NOT override thermal limits (unsafe)
```

**What the hardware does:**

```
├── CPU reduces frequency when approaching thermal limit (automatic)
├── Fans spin up automatically (if present)
└── Emergency shutdown if overheating persists
```

Temperature monitoring:

```
├── Settings → Advanced → Diagnostics → Temperatures
│   ├── Current CPU temp
│   ├── Max recorded temp (since boot)
│   └── Thermal headroom calculation
├── High temp warning shown in diagnostics if avg > 75°C
└── Fan curves configured by firmware (Spike does not override)
```

Fan control (conditional):

```
├── If laptop has software-controllable fans:
│   └── Settings → Advanced → Power → Fan curve (limited exposure)
└── Most laptops: fan control handled by firmware/EC only
```

### C-State Deep Sleep

C-states are hardware-level CPU sleep states:

```
C0: Running
C1: Idle (very shallow)
C2: Idle (slightly deeper)
C3: Idle (deep, longer wake latency)
C6: Idle (deepest, longest wake latency)
```

Spike configuration:

```
├── C-states enabled by kernel (default Ubuntu behavior)
├── No forced disabling of deep C-states
├── Power governors optimize C-state selection
└── No user-facing controls (too technical for beginners)
```

Monitoring:

```
├── Available via: cat /sys/devices/system/cpu/cpu*/cpuidle/state*/name
├── Visible in Settings → Advanced → Diagnostics (advanced tab only)
└── Not exposed to normal users (irrelevant to their workflow)
```

## Display Power Management

### DPMS (Display Power Management Signaling)

DPMS controls display power states:

State transitions:

```
├── On (active display)
├── Standby (blanks, backlight off, panel still powered)
├── Suspend (blank + reduce panel refresh rate)
└── Off (full power down, may need warm-up to turn on)
```

Spike behavior:

```
├── Uses DPMS through Wayland compositor (KWin)
├── Timeout set by screen blank settings
├── No separate DPMS timeout (screen blank controls it)
└── DPMS state queried via wlr-randr or xrandr (legacy)
```

**Settings → Display:**

```
├── DPMS timeout controlled by "Screen blank" setting (single control)
└── No separate DPMS configuration exposed to users
```

DPMS on external monitors:

```
├── If HDMI/VGA/DP monitor connected:
│   ├── Respects monitor's EDID power management capabilities
│   ├── Sends DPMS signals over display interface
│   └── Monitor turns off when blanking occurs
└── If monitor doesn't support DPMS:
    └── Falls back to simple display blank (may not power down monitor)
```

### External Display Power

Multiple display configurations:

**Single display (internal only):**

```
├── Power management applies to internal display
└── External monitor sleeps independently (if connected)
```

**Single display (external only):**

```
├── Internal display powered off (lid closed, external connected)
└── Power management applies to external monitor
```

**Duplicate mode:**

```
├── Both displays show same content
├── Power timeouts apply to both simultaneously
└── Dimming/blanking synchronized
```

**Extended mode:**

```
├── Independent power management per display (future enhancement)
├── Currently: both displays follow same timeout
└── Planned: Settings → Display → Per-display power settings
```

Lid close behavior with external display:

```
├── If external display detected AND lid closes:
│   ├── Option: Keep internal display on (clone/extend)
│   ├── Option: Turn off internal display (clamshell mode)
│   └── Default: Clamshell mode (internal display off)
└── Settings → Power → Lid close → "With external display"
```

## GPU Power Management

**Intel UHD Graphics 600 (target hardware):**

```
├── Integrated GPU shares memory with CPU
├── Power states tied to CPU power state
├── No separate power controls needed
└── Driver handles GPU idle automatically
```

**AMD integrated graphics:**

```
├── Similar to Intel, power tied to CPU
├── Driver manages GPU power states
└── Aggressive power saving on GPU clocks
```

**NVIDIA discrete graphics:**

```
├── Dedicated GPU with independent power state
├── nouveau driver: automatic power saving enabled
├── Proprietary driver: OptiMIX power management (if configured)
├── Settings → Advanced → Power → GPU power mode (if NVIDIA detected)
│   ├── Automatic (default)
│   ├── Performance (stay at high clock)
│   └── Power saving (aggressive downclocking)
└── Note: Proprietary NVIDIA driver has known hibernate issues
    └── Warning shown in Settings if hibernate selected
```

## Peripheral Power Management

### WiFi Power Saving

WiFi power states:

```
├── Full power (active scanning, frequent beacons)
├── Light sleep (reduce beacon listening)
└── Deep sleep (minimal activity, wake periodically)
```

Spike behavior:

**AC connected / Performance mode:**

```
├── WiFi power saving: Off
├── Frequent network checks for responsiveness
└── Best online performance
```

**Battery Saver / Critical:**

```
├── WiFi power saving: On
├── Reduce beacon listening interval
├── Longer sleep periods between checks
└── Slight latency increase for background traffic
```

**Settings → Power → Advanced:**

```
├── WiFi power saving: [Adaptive] ▼
│   ├── Off (best performance)
│   ├── On (best battery)
│   └── Adaptive (default — follows power profile)
```

Per-network settings:

```
├── Home network: [Off] (trusted, power-saving disabled)
├── Public networks: [On] (conservative, save power)
└── User configures per SSID in Settings → Network → Advanced
```

Implementation:

```
├── Controlled via iwconfig power setting
├── Or via NetworkManager's Wi-Fi power save property
└── Changes applied immediately (no disconnection required)
```

### Bluetooth Power Saving

Similar to WiFi but with additional considerations:

Battery Saver behavior:

```
├── Reduce advertising interval (if acting as peripheral)
├── Reduce polling frequency for connected devices
├── Disconnect non-critical peripherals (mouse, keyboard stay)
└── Audio streaming: maintain quality (don't throttle)
```

**Settings → Power → Advanced:**

```
├── Bluetooth power saving: [Adaptive] ▼
│   ├── Off
│   ├── On
│   └── Adaptive
```

Per-device settings:

```
├── Keyboard: [On] (low bandwidth, can sleep)
├── Mouse: [On] (intermittent use)
├── Headphones: [Off] (audio quality priority)
├── Phone pairing: [On] (background sync ok)
└── User configures per device in Settings → Bluetooth
```

Autodisconnect:

```
├── If device idle for 15 minutes: disconnect (optional)
├── Settings → Power → "Disconnect idle Bluetooth devices"
├── Checkbox: [On] or [Off]
└── Default: Off (convenience over battery)
```

### USB Autosuspend

USB devices can be put into low-power state when idle:

**Which devices:**

```
├── USB keyboards and mice (input devices)
├── USB flash drives (when not accessed)
├── USB webcams (when not in use)
└── USB hubs (downstream ports if upstream idle)
```

**Exclusions:**

```
├── Internal devices (trackpoints, fingerprint readers)
├── Always-connected peripherals (external drives)
└── Devices with critical data (USB-attached printers with buffers)
```

**Settings → Power → Advanced:**

```
├── USB autosuspend: [Adaptive] ▼
│   ├── Off (all devices always on)
│   ├── On (aggressive power saving)
│   └── Adaptive (follow power profile)
```

Whitelist/blacklist:

```
├── [Add device to whitelist] — never suspend
├── [Add device to blacklist] — always suspend (even on AC)
└── Device listed by vendor/product ID + friendly name
```

Example:

```
├── "Logitech MX Master Mouse" — Whitelist (keep responsive)
├── "Generic USB Hub" — Blacklist (always suspend if idle)
└── "SanDisk Ultra Flash Drive" — Blacklist (keep ready)
```

### HDD Spin-Down

> **Note:** Only applies if HDD detected (SSDs don't spin down).

Spin-down timing:

**Performance mode (AC):**

```
├── Spindown: 30 minutes idle
└── Cache: Aggressive (more writes buffered to RAM)
```

**Battery Saver mode:**

```
├── Spindown: 10 minutes idle
└── Cache: Moderate (balance safety vs power)
```

**Critical mode:**

```
├── Spindown: 5 minutes idle
└── Cache: Minimal (writes flushed immediately)
```

**Settings → Power → Hard Drive:**

```
├── Spin down after: [10 minutes] ▼
│   ├── Never
│   ├── 1 minute
│   ├── 5 minutes
│   ├── 10 minutes (default on battery)
│   ├── 15 minutes
│   ├── 20 minutes
│   └── 30 minutes (default on AC)
```

⚠️ Warning shown: *"Aggressive spin-down may cause HDD wear. Frequent spin-up/spin-down cycles stress the drive. If you hear the drive frequently starting and stopping, increase the timeout."*

SMART monitoring:

```
├── Disk health checked during SMART scan (Settings → Storage)
├── Spin-up count tracked
├── Warning if spin-up count unusually high
└── Recommendation: "Increase spin-down timeout to reduce wear"
```

## Battery Management

### Charge Thresholds (Conditional)

Some laptops support charge threshold configuration:

**Purpose:** Limit max charge to extend battery lifespan (100% charge stresses lithium-ion cells).

Spike support:

If hardware supports thresholds (via `acpi_call` or `tpacpi-bat`):

```
├── Settings → Power → Battery → Charge limits
│   ├── Start charging when: [20%] ▼ (threshold)
│   ├── Stop charging when: [80%] ▼ (threshold)
│   ├── [x] Enable charge limiting
│   └── Current status: "Charging stopped at 80%"
```

If hardware doesn't support thresholds:

```
├── Settings shows: "Your laptop doesn't support charge limiting."
└── Recommendation shown: "For maximum battery lifespan:
    - Avoid leaving plugged in 24/7
    - Try to keep battery between 20-80% when possible
    - Occasional full discharge/recharge helps calibration"
```

Threshold implementation:

```
├── Checks kernel module availability (acpi_call, thinkpad_acpi, etc.)
├── If available: expose UI for threshold configuration
├── If unavailable: hide advanced option
└── Graceful degradation (feature not available, not error)
```

### Battery Calibration

Periodic calibration recommended for accurate battery reporting:

**What calibration does:**

```
├── Drains battery to near-zero
├── Records actual capacity vs reported capacity
├── Updates firmware battery gauge
└── Improves remaining time estimates
```

Calibration schedule:

```
├── Recommended: Once every 3-6 months
├── Spike: Reminds via notification quarterly
└── User can initiate manually
```

**Calibration procedure (Guided Wizard):**

```
├── Settings → Power → Battery → Calibrate
├── Wizard explains process (takes 2-4 hours)
├── Steps:
│   1. Ensure AC unplugged
│   2. Unplug charger
│   3. Use laptop normally until it suspends/hibernates
│   4. Leave off for 5 hours
│   5. Plug in charger uninterrupted
│   6. Charge to 100% without using laptop
│   7. Report completion
└── After completion: "Battery calibrated. Time estimates improved."
```

User guidance:

```
├── Warning: "Keep laptop plugged in during charge phase.
    Interrupting may require repeating calibration."
├── Notify at each critical battery stage (50%, 30%, 20%, 10%, 5%)
└── Resume if calibration interrupted, but recalibrate recommended
```

### Battery Health Monitoring

Continuous monitoring of battery health:

**Metrics tracked:**

```
├── Design capacity (original factory rating)
├── Full charge capacity (current max)
├── Cycle count (charge/discharge cycles)
├── Current voltage
├── Average current draw
└── Temperature
```

Health calculation:

```
├── Health % = Full charge capacity / Design capacity
└── Example: 3000 mAh / 4000 mAh = 75% healthy
```

**Settings → Power → Battery:**

```
├── Current level: 85% (3400 mAh / 4000 mAh)
├── Health: 85% (Good) ▼
│   ├── Excellent (> 90%)
│   ├── Good (70-90%)
│   ├── Fair (50-70%)
│   └── Poor (< 50%)
├── Cycle count: 156 cycles
├── Replace recommendation: [No] or [Yes] (if < 50%)
└── Last calibration: [3 months ago]
```

Replace recommendation:

```
├── If health < 50%: "Consider replacing battery for best performance"
├── Link to manufacturer's battery replacement guide
└── If warranty: check eligibility for free replacement
```

Degradation tracking:

```
├── Health recorded monthly in history
├── Graph showing health decline over time
├── Alert if health drops suddenly (possible defect)
└── Data stored in /var/lib/spike/power/battery-health.json
```

## Power Events

### Lid Switch

Lid switch events trigger configurable actions:

**Open lid:**

```
├── Resume from suspend/hibernate (if suspended)
├── Unlock screen (if locked)
└── Wake display
```

**Close lid:**

Action depends on power profile and state:

| **AC Connected** | **On Battery** | **Action** |
| :-: | :-: | :-: |
| Yes | N/A | Suspend (default) |
| No | Yes | Suspend (default) |
| No | Yes | Hibernate (if enabled + timeout reached) |

**Settings → Power → Lid close:**

```
├── AC: [Suspend] ▼
├── Battery: [Suspend] ▼
└── Options for each: Do nothing, Suspend, Hibernate, Shutdown
```

Special case: External display

```
├── If external display detected AND lid closed:
│   ├── Option: Stay awake (continue using external monitor)
│   └── Option: Suspend (clamshell mode)
└── Settings → Power → Lid with external display: [Stay awake] ▼
```

### Power Button

Short press power button:

```
├── Behavior configurable
└── Settings → Power → Power button: [Suspend] ▼
```

Options:

```
├── Do nothing (ignore short press)
├── Suspend (default)
├── Hibernate (if available)
├── Shutdown (shows confirmation dialog)
└── Log out (shows confirmation dialog)
```

Long press power button (forced power off):

```
├── Hold for 5+ seconds: immediate shutdown (hardware level)
├── No software intervention
├── Last resort if system frozen
└── Risk of data loss if unsaved work exists
```

### AC Adapter Events

Plug in AC adapter:

```
├── Power profile: Battery Saver → Performance
├── Screen timeout restored (not dimmed)
├── WiFi/Bluetooth power saving: Off
└── Notification: "AC adapter connected. Performance restored."
```

Unplug AC adapter:

```
├── Check battery level:
│   ├── > 50%: Battery Saver mode
│   ├── 20-50%: Critical mode
│   └── < 20%: Critical mode + urgent low battery warning
├── Power profile: Performance → Battery Saver/Critical
├── Screen timeout shortened
├── Dimming starts sooner
├── Notification: "Running on battery. Power saving enabled."
└── If critical: "Battery low. Connect charger soon."
```

### Low Battery Warnings

Warning levels:

**Level 1: 20% (Warning)**

```
├── Notification: "Battery running low. Connect your charger soon."
├── Sounds alert chime
├── Battery icon shows yellow warning triangle
└── No action forced
```

**Level 2: 10% (Critical)**

```
├── Notification: "Battery critically low. Plug in now to avoid shutdown."
├── Sounds urgent chime (louder)
├── Battery icon shows red warning
├── If hibernate enabled: prepare to hibernate
└── If no hibernate: countdown to auto-shutdown
```

**Level 3: 5% (Emergency)**

```
├── Notification: "Battery almost drained. System will shut down in 2 minutes."
├── Countdown timer displayed
├── Save all work immediately
├── 2 minutes: system shuts down (if not recharged)
└── Can cancel if AC plugged in before shutdown
```

## Settings Integration

### Settings → Power (Main Page)

```
┌─────────────────────────────────────────────────────────────┐
│                                                              │
│  Power                                                       │
│                                                              │
│  CURRENT STATUS                                              │
│  ──────────────                                              │
│  Power source: [AC adapter connected]                        │
│  Current profile: [Adaptive]                                 │
│  Battery: [85% remaining] — [Good health]                    │
│                                                              │
│  SCREEN                                                      │
│  ───────                                                     │
│  Screen blank: [15 minutes] ▼                                │
│  Dim before blank: [On]                                      │
│  Dimming start: [5 minutes]                                  │
│  Dimming target: [50% brightness]                            │
│                                                              │
│  SUSPEND AND HIBERNATE                                       │
│  ─────────────────────────                                   │
│  Allow suspend: [✓]                                          │
│  Allow hibernate: [ ] (grayed out if swap < RAM)             │
│  Hybrid sleep: [✓] (recommended for laptops)                 │
│                                                              │
│  Lid close action:                                           │
│  ── On AC: [Suspend] ▼                                       │
│  ── On battery: [Suspend] ▼                                  │
│  ── With external display: [Clamshell mode] ▼                │
│                                                              │
│  After suspend on battery:                                   │
│  ── Hibernate after: [30 minutes] ▼                          │
│                                                              │
│  REQUIRE PASSWORD ON WAKE: [✓]                               │
│                                                              │
│  CPU                                                         │
│  ───                                                         │
│  CPU governor: [Schedutil] ▼                                 │
│  (Powersave/Schedutil/Performance)                           │
│                                                              │
│  ADVANCED                                                    │
│  ───────                                                     │
│  WiFi power saving: [Adaptive] ▼                             │
│  Bluetooth power saving: [Adaptive] ▼                        │
│  USB autosuspend: [Adaptive] ▼                               │
│  Hard drive spin-down: [10 minutes] ▼                        │
│                                                              │
│  BATTERY                                                     │
│  ───────                                                     │
│  Health: [85%]                                               │
│  Cycles: 156                                                 │
│  [Calibrate battery]  [View battery history]                 │
│                                                              │
│  CHARGE LIMITS (if supported)                                │
│  ─────────────────                                           │
│  [x] Enable charge limiting                                  │
│  Start charging at: [20%]                                    │
│  Stop charging at: [80%]                                     │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

All changes applied immediately. No restart required.

## What This Document Does Not Cover

- **Memory management (ZRAM, swap):** See `MEMORY.md` 

- **Kernel CPU parameters and governor configuration:** See `KERNEL.md` 

- **Thermal monitoring and overheating diagnostics:** See `TROUBLESHOOTING.md` 

- **Hardware detection and component specification:** See `HARDWARE.md` 

- **Spike Rescue tool and disaster recovery:** See `DISASTER-RECOVERY.md` 

- **Accessibility features (magnification, high contrast):** See `ACCESSIBILITY.md` 

🐕 BigRangaTech


