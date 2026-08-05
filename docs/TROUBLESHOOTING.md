# Spike Troubleshooting

## Purpose

This document helps users and support staff diagnose and resolve problems with Spike. It is organized by symptom, not by subsystem — users don't know what PipeWire is, they know their sound doesn't work.

Every entry follows the same structure:

- **Problem:** What the user experiences (plain language) 

- **Likely cause:** What's probably happening (simple terms) 

- **GUI fix:** How to fix it without a terminal (Golden Rule 2) 

- **Advanced fix:** Terminal steps for developers and power users (clearly separated) 

If a problem has no GUI fix, that is a bug. Report it.

> **Note on Advanced Fixes:** Some commands in the Advanced Fix sections reference tools that may not be pre-installed on Spike (e.g., `htop`, `smartctl`, `powertop`, `wlr-randr`). These can be installed via `sudo apt install <package-name>` or through Discover.

## How To Use This Document

**For users:**

```
├── Find your symptom in the table of contents below
├── Follow the GUI fix steps
├── If the GUI fix doesn't work, use "Report a Problem" in Settings
└── Advanced fixes are for developers — you don't need them
```

**For support staff:**

```
├── Start with the GUI fix
├── If GUI fix fails, try the advanced fix
├── If advanced fix fails, escalate to bug report
└── Attach diagnostic information from Settings → Diagnostics
```

**For developers:**

```
├── Advanced fixes are the starting point
├── Check journalctl, dmesg, and Spike logs
├── Reference the relevant subsystem doc for architecture
└── File issues on GitLab with full diagnostic output
```

## Table Of Contents

```
1. Boot Problems
   ├── System won't boot at all
   ├── Boot menu appeared unexpectedly
   ├── Black screen after GRUB
   ├── System boots but login screen doesn't appear
   ├── System boots but desktop is black
   ├── Boot is very slow
2. Installation Problems
   ├── Installer won't start
   ├── Installer froze partway through
   ├── Installation failed at storage step
   ├── Wi-Fi not detected during install
   ├── Installer rejected my storage device
3. Display Problems
   ├── Screen resolution is wrong
   ├── Screen is stretched or distorted
   ├── External monitor not detected
   ├── Screen flickering or tearing
   ├── Brightness control not working
   ├── Night light won't turn off
4. Audio Problems
   ├── No sound at all
   ├── Sound only from one speaker
   ├── Bluetooth audio cuts out
   ├── Microphone not working
   ├── Audio is distorted or crackling
   ├── Volume is too quiet even at 100%
5. Network Problems
   ├── Wi-Fi won't connect
   ├── Wi-Fi not detected
   ├── Ethernet not working
   ├── VPN won't connect
   ├── Internet is very slow
   ├── DNS errors (sites don't load)
6. Memory And Performance
   ├── System is very slow
   ├── Application closed unexpectedly
   ├── System freezes under load
   ├── Applications take long to open
   ├── Swap warning in Settings
7. Software Problems
   ├── Can't install applications
   ├── Discover shows errors
   ├── Application won't launch
   ├── Firefox is slow or stuttering
   ├── Video playback is choppy
8. Peripheral Problems
   ├── Touchpad not working
   ├── Keyboard not working or wrong layout
   ├── Bluetooth device won't pair
   ├── USB device not recognized
   ├── Printer not working
   ├── Webcam not working
   ├── SD card not detected
9. Battery And Power
   ├── Battery drains quickly
   ├── System won't suspend
   ├── System won't hibernate
   ├── Battery percentage is wrong
   ├── System shuts down unexpectedly
10. File And Data Problems
    ├── Can't find my files
    ├── Disk is full
    ├── Files disappeared after update
    ├── Can't access USB drive
    ├── Recovering lost files
11. Account And Security
    ├── Forgot password
    ├── Can't log in
    ├── Screen won't unlock
    ├── Firewall warning in Settings
12. System Clock Problems
    ├── System clock is wrong
    ├── Time keeps drifting
13. Notification Problems
    ├── Notifications disappeared
    ├── No notification sound
    ├── Too many notifications
    ├── Notification badge won't clear
```

## 1. Boot Problems

### System Won't Boot At All

**Problem:** The laptop powers on but doesn't load Spike. Screen stays black or shows the manufacturer logo indefinitely.

**Likely causes:**

```
├── Boot device not selected in firmware
├── GRUB2 not installed or corrupted
├── Storage failure (drive dead)
└── Firmware boot order changed
```

**GUI fix:**

This requires firmware access, not Spike settings:

1. Power on the laptop 

2. Enter firmware setup (usually F2, F12, or Del — varies by manufacturer) 

3. Check boot order — "Spike" should be first 

4. If "Spike" is not listed: 

```
├── Boot from Spike USB installer
├── Reinstall Spike (data can be backed up first)
└── See "Recovering files" in Section 10
```

5. Save and exit firmware 

If the drive is not detected at all in firmware:

```
├── The storage drive may have failed
├── Check drive connection (if comfortable opening laptop)
└── Consult a repair shop — this is a hardware issue, not a Spike issue
```

**Advanced fix:**

```
# Boot from live USB, check if drive is visible
lsblk
sudo fdisk -l
# If drive is visible but GRUB is missing:
sudo mount /dev/sdaX /mnt        *# root partition
sudo mount /dev/sdaY /mnt/boot/efi  *# EFI partition (UEFI)
sudo grub-install --boot-directory=/mnt/boot /dev/sda
sudo update-grub
```

### Boot Menu Appeared Unexpectedly

**Problem:** The GRUB menu appeared on boot without the user pressing anything.

**Likely cause:** Spike's boot failure counter detected 3+ consecutive failed boots. This is a safety feature, not a malfunction.

**GUI fix:**

1. Select the current kernel entry (top option) and press Enter 

2. If Spike boots normally: 

```
├── The counter resets automatically
└── Next boot will be normal (menu hidden again)
```

3. If current kernel fails: 

```
├── Select "Spike (previous kernel)" — this is the last working version
└── If that boots, the recent kernel update is the problem
```

4. If neither works: 

```
├── Select "Spike (recovery mode)"
├── See "Recovery mode" below
└── Or boot from USB and use Spike Rescue
```

After booting successfully, check **Settings → Advanced → Boot → Boot failure counter**:

```
├── If value is 0: problem resolved
├── If value is non-zero: press "Reset"
└── Value shows "Healthy" when reset
```

**Advanced fix:**

```
# Check boot failure counter
cat /boot/grub/grubenv | grep spike_boot_count
# Reset manually
sudo grub-editenv /boot/grub/grubenv set spike_boot_count=0
```

### Black Screen After GRUB

**Problem:** GRUB loads, the kernel entry is selected, then the screen goes black and nothing happens.

**Likely causes:**

```
├── Plymouth fails to render (GPU driver issue)
├── Kernel panic (hardware incompatibility)
├── Root filesystem mount failure
└── Missing kernel module in initramfs
```

**GUI fix:**

1. Press ESC during the black screen — this reveals boot messages 

2. If you see error messages scrolling: 

```
├── Note what the last visible message says
├── Reboot, press ESC at GRUB, select "previous kernel"
└── If previous kernel boots: report the issue with the error message
```

3. If screen stays black even with ESC: 

```
├── Reboot from USB
├── Use Spike Rescue to recover files
└── Reinstall Spike
```

If recurring:

```
├── Boot previous kernel from GRUB menu
├── Go to Settings → Advanced → Boot
├── Change boot splash to "Minimal (text only)" — disables Plymouth
└── Reboot — if it boots, the problem is Plymouth/GPU related
```

**Advanced fix:**

```
# Temporarily disable Plymouth to see boot messages
# At GRUB menu, press 'e' to edit, add to kernel line:
# plymouth.enable=0
# Check for kernel panic in previous boot
journalctl -b -1 --no-pager | tail -50
# Check GPU driver
journalctl -b 0 | grep -i -E "i915|amdgpu|nouveau|drm"
# Rebuild initramfs (if missing module suspected)
sudo update-initramfs -u
```

### System Boots But Login Screen Doesn't Appear

**Problem:** The system seems to boot (Plymouth splash shows) but instead of the login screen, you get a black screen or cursor.

**Likely cause:** SDDM (display manager) failed to start.

**GUI fix:**

1. Wait 30 seconds — sometimes SDDM is slow to start on HDD 

2. If nothing appears after 30s: 

```
├── Press Ctrl+Alt+F2 (switches to text console)
├── If text console appears: log in with your username and password
├── Type: sudo systemctl restart sddm
└── If login screen appears: the issue may be a race condition
```

3. If Ctrl+Alt+F2 doesn't work: 

```
├── The system may be fully frozen
├── Hard reset (hold power button 5 seconds)
└── See "Boot menu appeared unexpectedly" on next boot
```

4. If SDDM crashes repeatedly: 

```
├── Boot from USB
├── Use Spike Rescue to recover files
└── Reinstall Spike
```

**Advanced fix:**

```
# Check SDDM status
systemctl status sddm
# Check SDDM logs
journalctl -u sddm --no-pager
# Check if KWin/Wayland is starting
journalctl -b 0 | grep -i kwin
journalctl -b 0 | grep -i wayland
# Restart display manager
sudo systemctl restart sddm
# Check if display is detected
journalctl -b 0 | grep -i drm
journalctl -b 0 | grep -i connector
```

### System Boots But Desktop Is Black

**Problem:** SDDM login works, but after entering password the screen goes black or shows a cursor with no desktop.

**Likely cause:** Spike Shell or KWin crashed after login.

**GUI fix:**

1. Press Ctrl+Alt+F2 to switch to a text console 

2. Log in with your username and password 

3. Try restarting the session: 

Type: `sudo systemctl restart sddm`

4. If the login screen appears, try logging in again 

5. If the desktop is still black after second login: 

```
├── The problem is persistent
├── Boot from USB, back up files with Spike Rescue
└── Reinstall Spike
```

If the problem started after a kernel or driver update:

```
├── Reboot, press ESC at GRUB
├── Select previous kernel
└── If previous kernel works: the update is the problem
```

**Advanced fix:**

```
# Check Spike Shell logs
journalctl --user -u spike-shell --no-pager
cat /tmp/spike-shell.log 2>/dev/null
# Check KWin logs
journalctl --user -u kwin_wayland --no-pager
journalctl -b 0 | grep -i kwin
# Check for compositor errors
journalctl -b 0 | grep -i -E "compositor|wayland|egl|gl"
# Try starting Spike Shell manually
systemctl --user start spike-shell
# Check XWayland if needed
journalctl --user | grep -i xwayland
```

### Boot Is Very Slow

**Problem:** Boot takes much longer than usual. The Plymouth splash shows for a long time before the login screen.

**Likely causes:**

```
├── HDD degradation or failure
├── Too many services starting at boot
├── Network wait (trying to connect to unavailable network)
└── Failing drive causing I/O timeouts
```

**GUI fix:**

1. Check storage health: **Settings → Advanced → Diagnostics → Storage** 

```
├── Look for SMART warnings
└── If SMART shows "Failing": replace the drive
```

2. Check if HDD (not SSD): 

```
├── If HDD: slow boot is expected (~30-50s)
├── Consider upgrading to SSD
└── See HDD upgrade warning
```

3. Check for pending updates: **Settings → Software Updates** 

```
├── If kernel update is pending: install and reboot
└── Pending updates can cause odd boot behavior
```

4. Check boot diagnostics: **Settings → Advanced → Boot → View boot log** 

```
└── Look for long pauses or errors in the log
```

**Advanced fix:**

```
# Analyze boot time
systemd-analyze
# Identify slow services
systemd-analyze blame | head -20
# Check for services waiting on network
systemd-analyze critical-chain
# Check disk health
sudo smartctl -a /dev/sda | grep -i -E "reallocated|pending|uncorrect"
sudo hdparm -tT /dev/sda
# Check for I/O errors
dmesg | grep -i -E "ata|error|timeout|i/o"
```

## 2. Installation Problems

### Installer Won't Start

**Problem:** The laptop boots from USB but the Spike installer doesn't appear — instead, a desktop with no installer icon, or a black screen, or a text prompt.

**Likely causes:**

```
├── ISO not written correctly to USB
├── USB drive too slow (USB 2.0)
└── Hardware incompatibility (GPU or storage)
```

**GUI fix:**

1. Verify USB was created correctly: 

```
├── Re-download the Spike ISO
├── Verify SHA256 checksum matches
└── Re-write to USB using a reliable tool (Balena Etcher, dd, Rosa)
```

2. Try a different USB port (use USB 3.0 if available) 

3. Try a different USB drive 

4. If laptop has both USB 2.0 and 3.0 ports: 

```
└── Use the 3.0 port (usually blue)
```

5. If no installer icon on desktop: 

```
└── Open the launcher and search "Install Spike"
```

**Advanced fix:**

```
# Verify ISO checksum
sha256sum spike-*.iso
# Compare with published checksum on spike.bigrangatech.com
# Check USB write
lsblk
sudo dmesg | grep -i usb
# Try booting with nomodeset (at GRUB prompt, press 'e', add):
# nomodeset
# Check if live environment started
systemctl status --no-pager
```

### Installer Froze Partway Through

**Problem:** The installation progress bar stopped moving partway through the installation process.

**Likely causes:**

```
├── Disk I/O hang (failing drive, bad sector)
├── Ran out of disk space
├── Package download failure (if network dropped)
└── Memory exhaustion during package installation
```

**GUI fix:**

1. Wait 2 minutes — sometimes package installation has long pauses 

2. If still frozen after 2 minutes: 

```
├── The installation is incomplete — do not restart yet
├── If data backup was done: your files are safe on USB
├── Hard reset (hold power button 5 seconds)
├── Boot from USB again
└── Reinstall (the incomplete install will be overwritten)
```

3. If it freezes again at the same point: 

```
├── The storage drive may be failing
├── Check SMART status in live environment (Settings → Diagnostics)
└── Try a different drive if possible
```

**Advanced fix:**

```
# Boot from live USB, check disk
sudo smartctl -a /dev/sda
sudo badblocks -sv /dev/sda
# Check if disk space was the issue
lsblk -f
df -h
# Check installer logs
cat /var/log/spike/installer.log
journalctl -b 0 | grep -i spike-installer
```

### Installation Failed At Storage Step

**Problem:** The installer showed an error about storage during installation.

**Likely causes:**

```
├── Storage device is eMMC (unsupported)
├── Storage device is USB 2.0 (unsupported)
├── Storage device is too small (< 128GB)
└── Partition table corrupt or unerasable
```

**GUI fix:**

The installer will tell you why it failed:

```
├── "eMMC storage detected" → Spike doesn't support eMMC.
│   └── Replace with an SSD if possible.
├── "USB 2.0 drive detected" → Too slow for Spike.
│   └── Use USB 3.0+ drive or internal storage.
├── "Storage too small" → Need at least 128GB.
│   └── Use a larger drive.
└── "Could not partition drive" → Drive may be failing or locked.
    ├── Boot from USB, use Spike Rescue to check for data
    └── Try a different drive
```

### Wi-Fi Not Detected During Installation

**Problem:** The Wi-Fi connection step shows no networks or no Wi-Fi adapter.

**Likely causes:**

```
├── Wi-Fi firmware not loaded
├── Wi-Fi adapter disabled (hardware switch or airplane mode)
└── Unsupported Wi-Fi adapter (rare — all major vendors are on ISO)
```

**GUI fix:**

1. Check for physical Wi-Fi switch or button on laptop: 

```
├── Some laptops have a hardware Wi-Fi toggle
└── Make sure it's in the "on" position
```

2. Check airplane mode: 

```
├── Some laptops have an airplane mode hotkey (Fn+F2, etc.)
├── Press it to toggle Wi-Fi on
└── Look for a Wi-Fi LED on the laptop
```

3. Skip Wi-Fi for now: 

```
├── Continue installation without Wi-Fi
├── Connect via Ethernet if available
└── Wi-Fi can be configured after installation
```

4. If Wi-Fi still doesn't work after installation: 

```
└── See "Wi-Fi not detected" in Section 5
```

**Advanced fix:**

```
# Check if Wi-Fi adapter is detected
lspci -nn | grep -i wireless
lsusb | grep -i wireless
# Check if firmware loaded
dmesg | grep -i firmware
dmesg | grep -i iwlwifi
# Check if module is loaded
lsmod | grep -i -E "iwlwifi|ath|rtl|brcm"
# Load module manually (if detected but not loaded)
sudo modprobe iwlwifi     *# Intel
sudo modprobe ath9k       *# Atheros
sudo modprobe rtl8xxxu    *# Realtek (varies)
# Check rfkill state
rfkill list
sudo rfkill unblock wifi
```

## 3. Display Problems

### Screen Resolution Is Wrong

**Problem:** The display shows the wrong resolution — everything is too big, too small, or stretched.

**GUI fix:**

**Settings → Display**

```
├── Select correct resolution from the dropdown
├── Common resolutions:
│   ├── 1366×768 (most 14" budget laptops)
│   ├── 1440×900 (some 14" laptops)
│   ├── 1920×1080 (Full HD)
│   └── 1600×900
├── Click "Apply"
├── If correct resolution is not listed:
│   ├── The GPU driver may not have loaded correctly
│   ├── Restart the computer
│   └── If still missing: see advanced fix
└── If resolution is correct but stretched:
    ├── Check for a "scaling" option in Display settings
    └── Set scaling to 100%
```

**Advanced fix:**

```
# List available display modes
wlr-randr
# Check GPU driver status
journalctl -b 0 | grep -i i915
journalctl -b 0 | grep -i amdgpu
journalctl -b 0 | grep -i nouveau
# Check EDID (display identification)
cat /sys/class/drm/card0-*/edid | edid-decode
# Check if modesetting is active
journalctl -b 0 | grep -i modeset
```

### External Monitor Not Detected

**Problem:** An external monitor connected via HDMI, VGA, or DisplayPort is not detected.

**GUI fix:**

1. Check cable connection: 

```
├── Unplug and replug the cable
├── Try a different cable
└── Try a different port
```

2. Check monitor power: 

```
├── Is the monitor turned on?
└── Is it set to the correct input?
```

3. **Settings → Display** 

```
├── Click "Detect Displays"
├── If detected: arrange displays (extend or duplicate)
└── If not detected: see advanced fix
```

4. Try plugging the monitor in before booting: 

```
├── Some laptops only detect monitors at boot
└── Reboot with monitor connected
```

**Advanced fix:**

```
# Check if display connectors are detected
cat /sys/class/drm/card0-*/status
# Check for HDMI audio/video
journalctl -b 0 | grep -i -E "hdmi|drm|connector"
# Check KWin display detection
journalctl --user | grep -i -E "output|display|screen"
# Force display rescan (Wayland)
wlr-randr
# Check GPU driver
vainfo
```

### Screen Flickering Or Tearing

**Problem:** The screen flickers, has horizontal lines, or tears during movement.

**Likely cause:** Compositor or GPU driver issue.

**GUI fix:**

1. **Settings → Display** 

```
├── Check if "VSync" or "Compositor" settings exist
└── Toggle available options
```

2. If on Spike Plus: 

```
├── Settings → Appearance → Animations
├── Try disabling animations
└── If flickering stops: the issue is compositor-related
```

3. Restart the system: 

```
└── A reboot often fixes transient GPU issues
```

4. If persistent: 

```
├── May be a GPU driver regression
├── Report with hardware details (GPU model)
└── Include diagnostic info from Settings → Diagnostics
```

**Advanced fix:**

```
# Check compositor status
journalctl --user -u kwin_wayland --no-pager
# Check for DRM errors
dmesg | grep -i drm
# Check GPU render node
ls /dev/dri/
# Check KWin debug
WAYLAND_DEBUG=1 kwin_wayland --replace 2>&1 | head -50
# Check for tearing-related settings
cat /etc/environment | grep -i LIBVA
```

### Brightness Control Not Working

**Problem:** The brightness slider or function keys don't change the screen brightness.

**GUI fix:**

1. Try the brightness applet in the panel (bottom-right area) 

2. Try **Settings → Display → Brightness** slider 

3. Try function keys (Fn + brightness up/down) 

```
├── On some laptops, try without Fn (just the brightness key)
└── Some laptops need Fn-lock toggled
```

4. If using an external monitor: 

```
├── External monitors have their own brightness controls
└── Use the buttons on the monitor itself
```

5. If nothing works: 

```
├── Report the issue with laptop model and GPU
└── The backlight interface may need a specific kernel parameter
```

**Advanced fix:**

```
# Check backlight interface
ls /sys/class/backlight/
cat /sys/class/backlight/*/brightness
cat /sys/class/backlight/*/max_brightness
# Try writing brightness directly
echo 50 | sudo tee /sys/class/backlight/intel_backlight/brightness
# Check for ACPI backlight issues (add to kernel params)
# acpi_backlight=video  or  acpi_backlight=vendor
# (Settings → Advanced → Boot → edit kernel parameters, or GRUB)
# Check kernel module
lsmod | grep -i backlight
```

## 4. Audio Problems

### No Sound At All

**Problem:** No audio from speakers or headphones.

**GUI fix:**

1. Check the volume applet in the panel: 

```
├── Is it muted? (click speaker icon, check mute toggle)
├── Is volume at 0%? (drag slider up)
└── Is the correct output device selected?
```

2. **Settings → Sound** 

```
├── Output Device: select "Speakers — Built-In Audio"
├── Volume: set above 0%
└── Try the "Test audio" button
```

3. Check headphones: 

```
├── Remove headphones — do speakers work?
├── Plug in headphones — do they work?
└── If neither works: see advanced fix
```

4. Restart the system: 

```
└── Audio issues often resolve on reboot
```

5. If still no sound after reboot: 

```
└── Report with diagnostic info from Settings → Diagnostics
```

**Advanced fix:**

```
# Check PipeWire status
systemctl --user status pipewire
systemctl --user status wireplumber
# Restart audio stack
systemctl --user restart pipewire pipewire-pulse wireplumber
# List audio outputs
wpctl status
pactl list sinks
# Check if ALSA sees the sound card
aplay -l
# Check if kernel module loaded
lsmod | grep -i snd
dmesg | grep -i snd
# Check for HDMI audio (may be defaulting to HDMI)
pactl list sinks short
# Set default sink
pactl set-default-sink <sink_name>
```

### Sound Only From One Speaker

**Problem:** Audio plays from only one speaker or one side of headphones.

**GUI fix:**

1. **Settings → Sound** 

```
├── Check "Balance" slider — it should be centered
└── If balance is off-center: drag to center
```

2. Check physical connections: 

```
├── Headphones: try a different pair
├── Speakers: check for physical obstruction
└── Some laptops have one speaker broken (hardware issue)
```

3. If balance is centered and still one speaker: 

```
├── May be hardware failure
├── Test with headphones — if headphones are fine, it's the speaker
└── Hardware issue, not a Spike issue
```

**Advanced fix:**

```
# Check channel mapping
pactl list sinks
# Test left/right channels separately
speaker-test -c 2 -t wav
# Check WirePlumber default profile
wpctl status
```

### Bluetooth Audio Cuts Out

**Problem:** Bluetooth headphones or speakers connect but audio stutters, cuts out, or drops entirely.

**GUI fix:**

1. Move closer to the Bluetooth device: 

```
├── Range issues cause audio drops
└── Bluetooth range is typically 10 meters
```

2. Check for interference: 

```
├── Other Bluetooth devices nearby can cause issues
├── USB 3.0 devices can interfere with Bluetooth
└── Move USB devices away from the laptop
```

3. **Settings → Sound → Bluetooth device properties** 

```
├── Check codec selection
├── If on Plus: try different codec (SBC, AAC, LDAC)
└── SBC is most reliable (default on Standard)
```

4. Disconnect and reconnect the Bluetooth device: 

```
├── Settings → Bluetooth → Disconnect → Connect
└── Or use the Bluetooth applet
```

5. If persistent: 

```
└── The Bluetooth adapter or device may have firmware issues
```

**Advanced fix:**

```
# Check Bluetooth status
systemctl status bluetooth
bluetoothctl info <device_mac>
# Check audio routing
wpctl status
# Check Bluetooth audio module
journalctl --user | grep -i -E "bluez|bluetooth|a2dp"
# Restart Bluetooth + audio
sudo systemctl restart bluetooth
systemctl --user restart pipewire wireplumber
# Check Bluetooth codec negotiation
journalctl --user | grep -i codec
```

### Microphone Not Working

**Problem:** The microphone doesn't pick up sound during calls or recordings.

**GUI fix:**

1. Check the volume applet: 

```
├── Click the speaker icon
├── Look for microphone slider or mute toggle
└── Make sure microphone is not muted
```

2. **Settings → Sound** 

```
├── Input Device: select "Microphone — Built-In Audio"
├── Input volume: set above 0%
├── Test: speak and watch the input level bar
└── If level bar doesn't move: microphone not detected
```

3. Check application permissions: **Settings → Privacy → Application Permissions** 

```
├── Find the app (e.g., Firefox)
├── Check Microphone: should be "Allowed" or "Ask"
└── If "Denied": change to "Allowed"
```

4. If using an external microphone: 

```
├── Check it's plugged in correctly
└── Try a different port
```

**Advanced fix:**

```
# List audio inputs
wpctl status
pactl list sources
# Check if microphone is detected
arecord -l
# Test recording
arecord -d 5 test.wav
aplay test.wav
# Check source mute status
pactl list sources | grep -A 5 "Name:"
pactl set-source-mute <source_name> 0
pactl set-source-volume <source_name> 50%
```

### Audio Is Distorted Or Crackling

**Problem:** Sound is crackling, popping, or distorted.

**GUI fix:**

1. Lower the volume: 

```
├── Distortion at high volume is normal on small laptop speakers
└── Try keeping volume below 100%
```

2. Check if "Boost" is enabled: **Settings → Sound** 

```
├── If volume is above 100%: that's software boost — causes distortion
└── Keep at 100% or below
```

3. Try different audio output: 

```
├── If using speakers: try headphones
├── If using headphones: try speakers
└── If one is clear and the other distorted: hardware issue
```

4. Restart the system: 

```
└── Audio crackling can be a transient PipeWire issue
```

**Advanced fix:**

```
# Check for buffer underruns
journalctl --user -u pipewire | grep -i underrun
# Check sample rate
pw-cli info all | grep -i rate
# Check for CPU spikes causing audio issues
top -b -n 1 | head -20
# Try increasing buffer size (temporarily)
# Edit PipeWire config to increase quantum
# (This is a spike-config managed file — use Settings, not manual edit)
```

### Volume Is Too Quiet Even At 100%

**Problem:** Even at 100% volume, the audio is barely audible.

**GUI fix:**

1. **Settings → Sound** 

```
├── Check "Allow boosting above 100%" option
├── Enable it
└── Volume slider now extends to 150%
```

2. If 150% is still too quiet: 

```
├── The hardware may have low maximum volume
├── External speakers or headphones may be needed
└── Some laptops just have weak speakers (hardware limitation)
```

3. Check the application's own volume: 

```
├── Firefox, VLC, etc. have their own volume controls
├── Make sure the app volume is at 100%
└── Sometimes the app volume is low even when system is at 100%
```

## 5. Network Problems

### Wi-Fi Won't Connect

**Problem:** Wi-Fi adapter is detected but won't connect to a network.

**GUI fix:**

1. Check password: 

```
├── Re-enter the Wi-Fi password carefully
├── Passwords are case-sensitive
└── Try showing the password (eye icon) to verify
```

2. Check if other devices can connect: 

```
├── If other devices also fail: router issue, not Spike
├── Restart the router (unplug 30 seconds, plug back in)
└── Wait 2 minutes for router to fully restart
```

3. Forget and reconnect: **Settings → Network → Wi-Fi** 

```
├── Click the network → "Forget"
├── Search for networks again
└── Re-enter password
```

4. Try the network applet: 

```
├── Click the network icon in the panel
├── Select the network
└── Enter password
```

5. Restart NetworkManager (from SDDM or via reboot): 

```
└── A reboot often fixes connection issues
```

**Advanced fix:**

```
# Check NetworkManager status
nmcli device
nmcli connection show
# Restart NetworkManager
sudo systemctl restart NetworkManager
# Check for auth issues
journalctl -u NetworkManager | grep -i -E "auth|password|fail"
# Check Wi-Fi driver
dmesg | grep -i firmware
lsmod | grep -i iwlwifi
# Try disabling and re-enabling Wi-Fi
nmcli radio wifi off
sleep 2
nmcli radio wifi on
```

### Wi-Fi Not Detected

**Problem:** No Wi-Fi adapter appears in Settings or the panel applet. The system acts as if there's no Wi-Fi card.

**GUI fix:**

1. Check for hardware switch: 

```
├── Some laptops have a physical Wi-Fi switch or button
├── Look on the sides, front, or above the keyboard
└── Toggle it and wait 5 seconds
```

2. Check airplane mode: 

```
├── Panel applet → Airplane Mode icon (if present)
├── Make sure it's OFF
└── Some laptops toggle airplane mode with Fn + a function key
```

3. Restart the system: 

```
└── Firmware may not have initialized the adapter properly
```

4. If still not detected: 

```
├── The Wi-Fi adapter may be unsupported (very rare)
├── Check Settings → Diagnostics → Hardware info
└── Report with the adapter model (from diagnostics)
```

**Advanced fix:**

```
# Check if adapter is visible to the kernel
lspci -nn | grep -i wireless
lsusb | grep -i wireless
# Check if module is loaded
lsmod | grep -i -E "iwlwifi|ath|rtl|brcm|cfg80211"
# Check rfkill
rfkill list
rfkill unblock all
# Check firmware presence
ls /lib/firmware/ | grep -i iwl
# Try loading driver manually
sudo modprobe iwlwifi      *# Intel
sudo modprobe ath9k        *# Atheros
sudo modprobe rtl8xxxu     *# Realtek
# Check for USB Wi-Fi adapters
lsusb
```

### Ethernet Not Working

**Problem:** Ethernet cable is plugged in but there's no connection.

**GUI fix:**

1. Check the cable: 

```
├── Unplug and replug both ends
├── Try a different cable
└── Look for link lights on the port (green/amber LEDs)
```

2. Check the router/switch: 

```
├── Try a different port
└── Make sure the port is active
```

3. **Settings → Network** 

```
├── Look for "Wired" connection
├── If shown but disconnected: click "Connect"
└── If not shown: see advanced fix
```

4. Restart: 

```
└── Reboot the system
```

**Advanced fix:**

```
# Check ethernet interface
ip link
nmcli device
# Check if driver loaded
lsmod | grep -i -E "e1000|igb|r8169|atl"
# Check link status
ethtool eth0
cat /sys/class/net/en*/operstate
# Restart NetworkManager
sudo systemctl restart NetworkManager
# Try manual DHCP
sudo dhclient <interface>
```

### VPN Won't Connect

**Problem:** VPN connection fails or immediately disconnects.

**GUI fix:**

1. **Settings → Network → VPN** 

```
├── Select the VPN connection
├── Check credentials are correct
└── Try connecting again
```

2. Check the VPN type: 

```
├── WireGuard: ensure configuration is imported correctly
├── OpenVPN: ensure .ovpn file is imported correctly
└── Contact VPN provider if config is from a provider
```

3. Try reconnecting: 

```
├── Disconnect and reconnect
└── If it keeps failing: check VPN provider status
```

4. If using a corporate VPN: 

```
├── Contact IT department — VPN config may need updating
└── Corporate VPNs often have specific client requirements
```

**Advanced fix:**

```
# Check VPN status
nmcli connection show
nmcli connection up <vpn_name>
# Check VPN logs
journalctl -u NetworkManager | grep -i vpn
journalctl -u NetworkManager | grep -i wireguard
# Check if VPN modules loaded
lsmod | grep -i wireguard
lsmod | grep -i openvpn
```

### Internet Is Very Slow

**Problem:** Wi-Fi or Ethernet is connected but browsing is very slow.

**GUI fix:**

1. Check signal strength (Wi-Fi): 

```
├── Panel network applet shows signal bars
├── If 1-2 bars: move closer to router
└── If 3-4 bars: signal is fine, problem is elsewhere
```

2. Check if other devices are also slow: 

```
├── If yes: ISP or router issue, not Spike
└── Try restarting the router
```

3. Check for download activity: 

```
├── Are updates running? (Settings → Software Updates)
├── Is a Flatpak downloading? (Discover)
└── Wait for downloads to finish
```

4. Try a different DNS: **Settings → Network → Advanced → DNS** 

```
├── Try: 1.1.1.1 (Cloudflare) or 8.8.8.8 (Google)
└── If faster: DNS issue, keep the new setting
```

**Advanced fix:**

```
# Test DNS resolution speed
dig google.com
nslookup google.com
# Test bandwidth
speedtest-cli  *# if installed, or use browser-based speed test
# Check for DNS issues
systemd-resolve --status
journalctl -u systemd-resolved
# Check connection speed
iwconfig  *# Wi-Fi link rate
ethtool eth0  *# Ethernet link rate
```

### DNS Errors (Sites Don't Load)

**Problem:** Connected to Wi-Fi/Ethernet but websites don't load. Browser shows "site can't be reached" or DNS errors.

**GUI fix:**

1. Restart NetworkManager via reboot: 

```
└── A reboot usually resolves DNS cache issues
```

2. Try a different DNS server: **Settings → Network → Advanced → DNS** 

```
├── Set to 1.1.1.1 or 8.8.8.8
└── Apply and try browsing again
```

3. Check if the issue is specific to one site: 

```
├── If only one site: that site may be down
└── Try downforeveryoneorjustme.com (if other sites work)
```

4. Flush DNS: **Settings → Network → Advanced → "Clear DNS Cache"** 

```
└── If button not available: restart the system
```

**Advanced fix:**

```
# Check DNS resolver
resolvectl status
cat /etc/resolv.conf
# Restart DNS resolver
sudo systemctl restart systemd-resolved
# Test DNS
dig @127.0.0.53 google.com
dig @1.1.1.1 google.com
# Flush DNS cache
sudo resolvectl flush-caches
```

## 6. Memory And Performance

### System Is Very Slow

**Problem:** The system is generally sluggish — windows take long to respond, typing has delay, animations stutter.

**Likely causes:**

```
├── Memory pressure (too many apps open)
├── ZRAM not enabled or not functioning
├── Disk I/O bottleneck (HDD)
├── CPU thermal throttling
└── Failing storage device
```

**GUI fix:**

1. Close applications you're not using: 

```
├── The most common cause of slowness is too many open apps
├── Close Firefox tabs (Firefox is the biggest memory consumer)
└── Save and close LibreOffice documents you're not editing
```

2. Check memory status: **Settings → Memory** 

```
├── If "Available" is very low: memory pressure is the cause
├── Check if ZRAM is active — should say "Active (zstd)"
└── If ZRAM is "Disabled": the CPU may be too weak (see MEMORY.md)
```

3. Check storage type: **Settings → Advanced → Diagnostics → Storage** 

```
├── If HDD: slowness is expected — consider SSD upgrade
└── If SSD: check SMART status for drive health
```

4. Check CPU temperature: **Settings → Advanced → Diagnostics → Temperatures** 

```
├── If above 80°C: thermal throttling may be occurring
└── Improve ventilation (don't use on bed/couch)
```

5. Restart the system: 

```
└── A reboot clears memory and restarts all services cleanly
```

**Advanced fix:**

```
# Check memory usage
free -h
cat /proc/meminfo
# Check ZRAM
zramctl
swapon --show
# Check CPU usage
top
htop  *# if installed
# Check I/O wait
iostat 1 5  *# if installed
vmstat 1 5
# Check for OOM events
dmesg | grep -i oom
journalctl -b 0 | grep -i -E "oom|killed|earlyoom"
# Check disk health
sudo smartctl -a /dev/sda
```

### Application Closed Unexpectedly

**Problem:** An application disappeared while you were using it, and a notification appeared saying Spike closed it to free memory.

**Likely cause:** Earlyoom killed the application because the system ran low on memory.

**GUI fix:**

1. The notification explains what happened — read it 

2. This is a safety feature: 

```
├── The app was closed to keep the system responsive
└── Your other apps are still running
```

3. Reopen the closed application: 

```
├── Launcher → search for the app
└── Your work may or may not be recovered (depends on the app)
```

4. If this happens frequently: 

```
├── You may need to keep fewer apps open at once
├── Firefox with many tabs is the most common cause
└── Consider closing tabs you're not actively using
```

5. Check memory settings: **Settings → Memory → Advanced** 

```
├── Earlyoom threshold: try lowering to 5% (triggers earlier)
└── This gives earlier warning, may save more work
```

**Advanced fix:**

```
# Check earlyoom logs
journalctl -u earlyoom --no-pager
# Check what was killed
dmesg | grep -i -E "killed|oom"
# Check memory state when it happened
journalctl -b 0 | grep -i earlyoom
# Adjust earlyoom threshold
# (This should be done through Settings, not command line)
```

### System Freezes Under Load

**Problem:** The system becomes completely unresponsive when running heavy applications.

**Likely cause:** Memory exhaustion without earlyoom intervention, or CPU/IO saturation.

**GUI fix:**

1. Wait 30 seconds: 

```
├── Sometimes the system recovers after catching up
└── If it recovers: close the heavy application immediately
```

2. If frozen for more than 30 seconds: 

```
├── Hard reset (hold power button 5+ seconds)
└── On reboot, the boot failure counter will have incremented
```

3. After reboot: 

```
├── Settings → Advanced → Boot → check failure counter
├── Settings → Memory → check if earlyoom is active
└── If earlyoom was not running: investigate why
```

4. Preventive: 

```
├── Don't run multiple heavy apps simultaneously on 4GB RAM
├── Firefox tabs are the biggest memory drain
└── Check Settings → Memory regularly
```

**Advanced fix:**

```
# Check if earlyoom is running
systemctl status earlyoom
# Check earlyoom configuration
cat /etc/default/earlyoom
# Check for OOM killer (kernel-level, means earlyoom didn't act in time)
dmesg | grep -i "out of memory"
# Check memory + swap state
free -h
swapon --show
zramctl
# Check if ZRAM is actually working
cat /proc/swaps
```

### Applications Take Long To Open

**Problem:** Clicking an application takes a long time before the window appears.

**GUI fix:**

1. Check if this is the first launch after boot: 

```
├── First launch is slower (cold cache)
└── Second launch should be faster (warm cache)
```

2. Check storage type: **Settings → Advanced → Diagnostics → Storage** 

```
├── HDD: slow app launch is expected (~3-5s)
├── SSD: should be faster (~1-2s)
└── If SSD is slow: check SMART health
```

3. Check memory pressure: **Settings → Memory** 

```
├── If available memory is low: close some apps
└── The system may be swapping heavily
```

4. Check if updates are running: **Settings → Software Updates** 

```
└── Background updates can slow app launches
```

5. Check CPU temperature: **Settings → Advanced → Diagnostics → Temperatures** 

```
└── If hot: thermal throttling is slowing everything down
```

### Swap Warning In Settings

**Problem:** **Settings → Memory** shows a warning about swap usage.

**GUI fix:**

1. Check what the warning says: 

```
├── "High swap usage": system is using disk swap heavily
│   ├── Close some applications
│   └── Restart the system to clear swap
├── "Swap file error": swap file may be corrupted
│   ├── Restart the system (fsck will check it)
│   └── If warning persists: report the issue
└── "ZRAM disabled": CPU was classified as low-end
    └── This is expected behavior, not a problem
```

2. If swap usage is persistently high: 

```
├── You may need to keep fewer apps open
├── Consider whether the workload fits on 4GB RAM
└── The system is doing its best with available hardware
```

## 7. Software Problems

### Can't Install Applications

**Problem:** Installing applications from Discover fails or shows an error.

**GUI fix:**

1. Check network connection: 

```
├── Discover needs internet to install apps
└── If offline: connect to Wi-Fi first
```

2. Try refreshing Discover: 

```
├── Open Discover
├── Click the refresh button (or wait for auto-refresh)
└── Try installing again
```

3. Check for disk space: **Settings → Advanced → Storage** 

```
├── If disk is full: delete files you don't need
└── Flatpak apps need space for runtimes
```

4. Check Flatpak repository: **Settings → Software Sources → Flathub** 

```
└── Make sure Flathub is enabled
```

5. If a specific app fails: 

```
├── Try a different app (to check if Discover itself is broken)
└── If only one app fails: that app may have a packaging issue
```

6. If Discover shows an error message: 

```
├── Read the message
├── Report it via Settings → Advanced → "Report a Problem"
└── Include the error text in the report
```

**Advanced fix:**

```
# Check Flatpak status
flatpak list
flatpak remotes
# Check if Flathub is configured
flatpak remote-list
# Try installing via command line
flatpak install flathub <app-id>
# Check apt status
sudo apt update
sudo apt install -f
# Check for broken packages
dpkg --audit
sudo dpkg --configure -a
# Check disk space
df -h
```

### Discover Shows Errors

**Problem:** Discover (software center) displays error messages or warnings.

**GUI fix:**

1. Most Discover errors are transient: 

```
├── Close Discover
├── Wait 10 seconds
└── Reopen Discover — error may be gone
```

2. If the error persists: 

```
├── Note the error message
├── Settings → Software Sources → check repository status
└── Try toggling Flathub off and on
```

3. If "cache mismatch" or "refresh failed": 

```
├── Network issue — check connection
└── Try again when network is stable
```

4. If "dependency error": 

```
├── There may be a broken package
├── Reboot, then try Discover again
└── If persistent: report with the error text
```

**Advanced fix:**

```
# Check package manager state
sudo apt update
sudo apt --fix-broken install
sudo dpkg --configure -a
# Check Flatpak
flatpak repair
flatpak update
# Check Discover logs
journalctl --user -u discover --no-pager
# Clear package cache (careful)
sudo apt clean
flatpak uninstall --unused
```

### Application Won't Launch

**Problem:** Clicking an application in the launcher does nothing, or the app starts and immediately closes.

**GUI fix:**

1. Try launching from the panel: 

```
├── If the app is pinned to the panel favorites, click it there
└── If it launches from panel but not launcher: launcher cache issue
```

2. Restart the launcher: 

```
├── Log out and log back in (fastest fix)
└── Or reboot
```

3. If the app crashes immediately: 

```
├── The app may be missing a dependency
├── Try reinstalling via Discover
└── If Flatpak: the runtime may be corrupted
```

4. Check for updates: 

```
├── The app may need an update
├── Discover → Updates
└── Install available updates and try again
```

5. If it's a specific Flatpak app: **Settings → Software Sources → Flatpak** 

```
└── Try removing and reinstalling the app
```

**Advanced fix:**

```
# Try launching from terminal to see error messages
<application-command> 2>&1
# For Flatpak apps
flatpak run <app-id> 2>&1
# Check for missing libraries
ldd $(which <application>) | grep "not found"
# Check Flatpak runtime
flatpak info <app-id>
flatpak list --runtime
# Reinstall Flatpak runtime
flatpak uninstall <runtime>
flatpak install flathub <runtime>
```

### Firefox Is Slow Or Stuttering

**Problem:** Firefox is slow, pages take long to load, or scrolling stutters.

**GUI fix:**

1. Check number of open tabs: 

```
├── Each tab uses memory — close tabs you don't need
├── 10+ tabs on 4GB RAM will cause slowness
└── Consider bookmarking instead of keeping tabs open
```

2. Check if VA-API is working: **Settings → Advanced → Multimedia** 

```
├── "Video acceleration: Active" should be shown
├── If "Inactive" or "Not working": see advanced fix
└── Click "Test hardware video decode"
```

3. Check if AV1 is disabled: **Settings → Advanced → Multimedia** 

```
├── "Disable AV1" should be checked (recommended)
└── If unchecked: check it and restart Firefox
```

4. Check system memory: **Settings → Memory** 

```
├── If available memory is low: Firefox is competing for memory
└── Close other apps to give Firefox more room
```

5. Try restarting Firefox (not the whole system): 

```
├── Close Firefox completely
└── Reopen from launcher
```

**Advanced fix:**

```
# Check VA-API status
vainfo
# Check Firefox about:support
# Navigate to about:support in Firefox
# Look for "HARDWARE_VIDEO_DECODING" → should be "available"
# Check Firefox prefs
# about:config
# Search for: media.ffmpeg.vaapi.enabled → should be true
# Search for: media.av1.enabled → should be false
# Check WebGL
# about:support
# Look for "WebGL Renderer" → should show GPU info
# Check CPU usage during browsing
top -b -n 1 | grep -i firefox
```

### Video Playback Is Choppy

**Problem:** Video on YouTube, Netflix, or in local files plays with stuttering or frame drops.

**GUI fix:**

1. Check VA-API status: **Settings → Advanced → Multimedia → "Test hardware video decode"** 

```
├── If test fails: hardware video acceleration isn't working
└── CPU is doing all the work → choppy playback
```

2. Check if AV1 is disabled (Firefox): **Settings → Advanced → Multimedia** 

```
└── "Disable AV1" must be checked for Gemini Lake GPUs
```

3. Check network speed: 

```
├── Choppy video can be buffering, not decoding
├── Try lowering the video quality (YouTube gear icon)
└── If lower quality is smooth: network issue, not decoding
```

4. Check if it's a specific video format: 

```
├── H.264/MP4: should be smooth (hardware decoded)
├── VP9/WebM: should be smooth (hardware decoded on Intel)
├── AV1: will be choppy (software decoded — this is why it's disabled)
└── If AV1 video specifically is choppy: that's expected
```

5. Check system load: 

```
├── Close other apps while watching video
└── Video decode competes with other CPU work
```

## 8. Peripheral Problems

### Touchpad Not Working

**Problem:** The touchpad doesn't respond — cursor doesn't move, or tap-to-click doesn't work, or gestures aren't recognized.

**Likely causes:**

```
├── Touchpad disabled in settings
├── Touchpad disabled by function key (Fn-lock)
├── libinput driver not loaded
├── Hardware failure (rare but possible on old laptops)
└── Palm rejection configured too aggressively
```

**GUI fix:**

1. Check for a function key toggle: 

```
├── Many laptops have Fn+F-key combination to toggle touchpad
├── Common keys: Fn+F5, Fn+F7, Fn+F9 (varies by manufacturer)
├── Look for a touchpad icon on one of the function keys
└── Press it and wait 3 seconds
```

2. Check touchpad settings: **Settings → Mouse/Touchpad** 

```
├── Look for "Touchpad enabled" toggle — make sure it's ON
├── Check "Tap to click" if taps aren't registering
├── Check pointer speed — if too slow, increase sensitivity
└── Check "Disable while typing" — if enabled, touchpad pauses during typing
```

3. Check palm rejection: 

```
├── If the touchpad works intermittently: palm rejection may be too aggressive
├── Settings → Mouse/Touchpad → Palm Detection
└── Try lowering palm detection sensitivity or disabling temporarily
```

4. Try an external USB mouse: 

```
├── If USB mouse works but touchpad doesn't: touchpad hardware or driver issue
└── If neither works: deeper input system issue
```

5. Restart the system: 

```
└── libinput may need to re-initialize the device
```

**Advanced fix:**

```
# Check if touchpad is detected
libinput list-devices
cat /proc/bus/input/devices | grep -i touch
# Check if libinput driver is loaded
journalctl -b 0 | grep -i libinput
dmesg | grep -i -E "input|touchpad|synaptics|i2c-hid"
# Check kernel module
lsmod | grep -i -E "libinput|synaptics|i2c_hid|psmouse"
# Try reloading input module
sudo rmmod psmouse
sudo modprobe psmouse
# Check XWayland input (if applicable)
journalctl --user | grep -i -E "input|touchpad|pointer"
```

### Keyboard Not Working Or Wrong Layout

**Problem:** The keyboard doesn't respond, or keys produce the wrong characters (e.g., pressing `"` produces `@`).

**Likely causes:**

```
├── Wrong keyboard layout selected during installation
├── Layout switched accidentally (Super+Space)
├── libinput or kernel keyboard driver issue
├── Hardware failure (liquid damage, worn membrane)
└── Num Lock or Fn-lock in unexpected state
```

**GUI fix:**

1. Check keyboard layout indicator: 

```
├── Look in the panel (right zone) for a layout indicator (e.g., "US", "UK")
├── If wrong: click it and select the correct layout
└── If indicator not present: only one layout is configured
```

2. Change keyboard layout: **Settings → Keyboard Layout** 

```
├── Check current layout matches your physical keyboard
├── If wrong: select the correct layout from the list
├── Add additional layouts if you use multiple languages
└── Layout change is instant (no re-login required)
```

3. Check if layout was switched accidentally: 

```
├── Super+Space cycles between configured layouts
├── If you have multiple layouts, you may have pressed it by accident
└── Press Super+Space to cycle back
```

4. Check Num Lock and Fn-lock: 

```
├── If number keys don't work: check Num Lock (Fn+Num Lock on some laptops)
├── If function keys behave oddly: check Fn-lock (Fn+Esc on many laptops)
└── Try toggling each and testing the keyboard
```

5. If the keyboard doesn't respond at all: 

```
├── Try an external USB keyboard
├── If USB keyboard works: laptop keyboard hardware issue
└── If neither works: deeper input system issue
```

6. Restart the system: 

```
└── Keyboard driver may need to re-initialize
```

> **Note for recovery mode / text console:** When using recovery mode or a text console (Ctrl+Alt+F2), the keyboard defaults to US/QWERTY layout regardless of your graphical settings. If your password contains characters that differ between layouts (e.g., `@` vs `"`, `\\` vs `#`), type it as if using a US keyboard.

**Advanced fix:**

```
# Check if keyboard is detected
libinput list-devices
cat /proc/bus/input/devices
# Check kernel keyboard driver
dmesg | grep -i -E "input|keyboard|atkbd|i8042"
# Check current layout
localectl status
# List available layouts
localectl list-x11-keymap-layouts
# Set keyboard layout
sudo localectl set-x11-keymap <layout>
sudo localectl set-keymap <layout>
# Check libinput for keyboard events
journalctl -b 0 | grep -i -E "input|keyboard|key"
```

### Bluetooth Device Won't Pair

**Problem:** A Bluetooth device (headphones, mouse, keyboard) won't pair with the laptop.

**GUI fix:**

1. Check if Bluetooth is enabled: 

```
├── Panel → Bluetooth applet (if present)
├── Settings → Bluetooth
└── Toggle Bluetooth on
```

2. Put the device in pairing mode: 

```
├── Check the device's manual for pairing instructions
├── Usually a button hold until LED flashes rapidly
└── Device must be discoverable
```

3. **Settings → Bluetooth** 

```
├── Click "Scan" or wait for device to appear
├── Click the device → "Pair"
├── Accept pairing on both devices if prompted
└── If it asks for a PIN: try 0000 or 1234
```

4. If the device was previously paired: 

```
├── Remove it: Settings → Bluetooth → device → "Remove"
└── Pair again from scratch
```

5. If Bluetooth applet is not in the panel: 

```
├── The laptop may not have Bluetooth
├── Try a USB Bluetooth dongle
└── If dongle is plugged in: reboot to trigger detection
```

**Advanced fix:**

```
# Check Bluetooth status
systemctl status bluetooth
hciconfig -a
# Check if adapter detected
lsusb | grep -i bluetooth
rfkill list bluetooth
# Use bluetoothctl
bluetoothctl
# Inside bluetoothctl:
#   power on
#   scan on
#   pair <MAC>
#   trust <MAC>
#   connect <MAC>
# Check logs
journalctl -u bluetooth --no-pager
```

### USB Device Not Recognized

**Problem:** A USB device (flash drive, mouse, keyboard) is plugged in but not detected.

**GUI fix:**

1. Try a different USB port: 

```
├── Use USB 3.0 (usually blue) if available
└── Some front-panel ports on desktops can be unreliable
```

2. Try a different cable (if applicable): 

```
├── Cables fail more often than devices
└── Try the device with a known-good cable
```

3. Unplug, wait 5 seconds, replug: 

```
└── Sometimes the USB enumeration needs a retry
```

4. Restart the system: 

```
└── Firmware may need to re-enumerate USB
```

5. If the device was previously working: 

```
├── Check if it shows in Settings → Removable Devices
└── If listed but not accessible: filesystem issue
```

6. If it's a USB hub: 

```
├── Try the device directly in a laptop port
└── The hub may not provide enough power
```

**Advanced fix:**

```
# Check USB devices
lsusb
lsusb -t
# Check USB device tree
ls /sys/bus/usb/devices/
# Check for USB errors
dmesg | grep -i usb
dmesg | tail -20
# Check if device is mounted
lsblk
mount | grep -i media
# Try manual mount (for storage devices)
sudo mount /dev/sdX /mnt
```

### Printer Not Working

**Problem:** A printer is connected but printing doesn't work.

**GUI fix:**

1. **Settings → Printers** (if available) 

```
├── Click "Add Printer"
├── Wait for detection
└── Select the printer and follow setup
```

2. Check connection: 

```
├── USB: unplug and replug
├── Network: ensure printer is on same network
└── Check printer is powered on
```

3. Check printer status: 

```
├── Many printers have a status display
├── Look for error lights or messages
└── Check paper jams, ink levels
```

4. If printer is network-connected: 

```
├── Try the printer's IP address
├── Settings → Printers → Add → Network
└── Enter the printer IP
```

5. If the Printers page is not available: 

```
├── Printer support may not be fully configured yet (alpha/beta)
└── Report as a feature request
```

**Advanced fix:**

```
# Check CUPS status
systemctl status cups
lpstat -t
# Check if printer detected
lpinfo -v
# Check USB printers
lsusb | grep -i printer
# Check CUPS logs
journalctl -u cups --no-pager
# Add printer manually (requires CUPS web interface)
# Visit: http://localhost:631
```

### Webcam Not Working

**Problem:** The webcam doesn't work in Firefox or other applications.

**GUI fix:**

1. Check camera permissions: **Settings → Privacy → Application Permissions** 

```
├── Find the app (e.g., Firefox)
├── Camera: should be "Allowed" or "Ask"
└── If "Denied": change to "Allowed"
```

2. Check for privacy shutter or switch: 

```
├── Many laptops have a physical shutter or switch
├── Make sure it's open
└── Look for a small slider near the camera
```

3. Try a different application: 

```
├── If Firefox: try Cheese (install from Discover if needed)
└── If one works and other doesn't: permission issue
```

4. Restart the system: 

```
└── Camera may not have been initialized at boot
```

5. If external USB camera: 

```
├── Try a different USB port
└── Try without USB hub
```

**Advanced fix:**

```
# Check if camera detected
lsusb | grep -i camera
ls /dev/video*
# Check if uvcvideo module loaded
lsmod | grep uvcvideo
dmesg | grep -i -E "uvc|video"
# Test camera
cheese  *# if installed
ffplay /dev/video0  *# alternative
# Check camera permissions (Wayland portal)
journalctl --user | grep -i -E "camera|portal"
```

### SD Card Not Detected

**Problem:** An SD card is inserted but doesn't appear.

**GUI fix:**

1. Try reseating the card: 

```
├── Remove and reinsert the SD card
└── Wait 5 seconds after insertion
```

2. Try a different SD card: 

```
├── If another card works: the first card may be damaged
└── If neither works: the card reader may have an issue
```

3. Check if it appears in the file manager: 

```
├── Open Dolphin (file manager)
└── Look under "Removable Devices" in the sidebar
```

4. Restart the system: 

```
└── Card reader may need firmware re-initialization
```

5. If the SD card works on another computer: 

```
├── Format the card on the other computer (FAT32 or exFAT)
└── Try again on Spike
```

**Advanced fix:**

```
# Check if card reader detected
lsusb | grep -i card
dmesg | grep -i mmc
dmesg | grep -i sd
# Check if card shows as block device
lsblk
# Check card reader module
lsmod | grep -i -E "rtsx|sdhci|mmc"
```

## 9. Battery And Power

### Battery Drains Quickly

**Problem:** Battery life is shorter than expected.

**GUI fix:**

1. Check power profile: **Settings → Power** 

```
├── Profile should be "Adaptive" (switches automatically)
├── If "Performance" on battery: switch to "Adaptive" or "Battery Saver"
└── Performance profile uses more power
```

2. Check screen brightness: 

```
├── Lower brightness to extend battery
├── 50-70% is usually a good balance
└── Battery Saver profile dims automatically
```

3. Check running applications: 

```
├── Firefox with video playback uses significant power
├── Close apps you're not using
└── Heavy CPU usage drains battery fast
```

4. Check Bluetooth: 

```
├── Bluetooth uses power even when idle
├── Turn off Bluetooth if not using it (panel applet)
└── Bluetooth audio uses more power than wired
```

5. Check Wi-Fi power saving: **Settings → Power → Advanced** 

```
└── Wi-Fi power saving should be "Adaptive" or "On"
```

6. Check battery health: **Settings → Power → Battery → Health** 

```
├── If health is below 70%: battery is aged
└── Aged batteries drain faster
```

7. Check for runaway processes: **Settings → Advanced → Diagnostics** 

```
└── Look for high CPU usage processes
```

**Advanced fix:**

```
# Check power consumption
upower -d
cat /sys/class/power_supply/BAT0/status
cat /sys/class/power_supply/BAT0/current_now
# Check CPU governor
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Check for high-power processes
top -o %CPU
powertop  *# if installed
# Check USB autosuspend
cat /sys/bus/usb/devices/*/power/control
```

### System Won't Suspend

**Problem:** Closing the lid or selecting Suspend doesn't put the system to sleep.

**GUI fix:**

1. Check lid settings: **Settings → Power → Lid Close** 

```
├── Make sure action is set to "Suspend"
└── If set to "Do nothing": that's why it doesn't suspend
```

2. Try manual suspend: 

```
├── Session Menu → Suspend
└── If this works: lid switch may be faulty
```

3. Check if something is preventing suspend: 

```
├── An application may be inhibiting suspend
│   (e.g., a download, a video playing)
└── Close all applications and try again
```

4. Check for USB devices: 

```
├── Some USB devices prevent suspend
├── Unplug USB devices and try suspending
└── If it suspends: a USB device was the issue
```

5. If suspend used to work but stopped: 

```
├── May be a kernel regression
├── Try previous kernel (GRUB menu → previous kernel)
└── If previous kernel suspends: report the regression
```

**Advanced fix:**

```
# Check what's preventing suspend
systemd-inhibit --list
# Try manual suspend
systemctl suspend
# Check lid switch
cat /proc/acpi/button/lid/LID/state
# Or:
cat /proc/device-tree/... (on some hardware)
# Check systemd power settings
cat /etc/systemd/logind.conf
# Check for suspend errors
journalctl -b 0 | grep -i suspend
```

### System Won't Hibernate

**Problem:** Hibernate doesn't work — the system shuts down but doesn't restore the session.

**GUI fix:**

1. Check if hibernate is available: **Settings → Power** 

```
├── If "Hibernate" is grayed out: swap is too small
├── Hibernate needs disk swap >= RAM size (not counting ZRAM)
└── This is a limitation, not a bug
```

> **ZRAM and hibernation:** ZRAM is a compressed swap device in RAM — it is **volatile** (contents are lost when power is removed). ZRAM cannot be used for hibernation. Only the on-disk swap file (`/swapfile`) can store hibernation data. Spike creates an 8GB swap file, which is sufficient for 4GB RAM systems. However, if the swap file was reduced (e.g., on SD card or USB installations with 4GB swap), hibernate may be unavailable because disk swap is smaller than RAM.

2. If hibernate is available but doesn't restore: 

```
├── The system may be falling back to a normal boot
├── Check if boot failure counter increased (it shouldn't for hibernate)
└── Report the issue with diagnostic info
```

3. If using NVIDIA proprietary driver: 

```
├── NVIDIA has known hibernate issues
├── Settings → Power may show a warning
└── Use suspend instead (more reliable)
```

4. Alternative: use suspend instead of hibernate 

```
├── Suspend is faster to resume
└── Suspend uses a small amount of battery
```

**Advanced fix:**

```
# Check swap size vs RAM
free -h
swapon --show
# Swap must be >= RAM for hibernate
# Check hibernate support
systemctl hibernate
# Check hibernate resume
journalctl -b 0 | grep -i hibernate
journalctl -b 0 | grep -i resume
# Check kernel parameters (resume= should point to swap)
cat /proc/cmdline
# Check if NVIDIA driver conflicts
lsmod | grep nvidia
journalctl -b 0 | grep -i nvidia | grep -i hibernate
```

### Battery Percentage Is Wrong

**Problem:** The battery percentage jumps around or shows an incorrect value.

**GUI fix:**

1. Calibrate the battery: **Settings → Power → Battery → Calibrate** 

```
├── Follow the guided wizard
├── Takes 2-4 hours
└── Improves accuracy of percentage
```

2. Check battery health: **Settings → Power → Battery → Health** 

```
├── If health is low (< 70%): percentage may be unreliable
└── Old batteries don't report accurately
```

3. Restart the system: 

```
├── Sometimes the battery gauge just needs a refresh
└── ACPI battery readings can get stuck
```

4. If the percentage shows 0% but laptop is running: 

```
├── The battery gauge may be broken (hardware issue)
└── Try calibration
```

### System Shuts Down Unexpectedly

**Problem:** The system turns off without warning, even with battery remaining.

**Likely causes:**

```
├── Overheating (thermal shutdown)
├── Battery calibration issue (reports higher than actual)
└── Hardware fault (failing battery or power management)
```

**GUI fix:**

1. Check if it's thermal: **Settings → Advanced → Diagnostics → Temperatures** 

```
├── If temperatures were high (> 90°C): thermal shutdown
├── Don't use the laptop on soft surfaces (bed, couch)
└── Clean dust from vents if possible
```

2. Check battery calibration: **Settings → Power → Battery → Calibrate** 

```
└── Bad calibration causes sudden shutdown (battery is emptier than reported)
```

3. Check battery health: **Settings → Power → Battery → Health** 

```
└── If health is poor (< 50%): replace battery
```

4. If on AC power and it still shuts down: 

```
├── Power supply may be faulty
├── DC jack may be loose
└── This is a hardware issue
```

**Advanced fix:**

```
# Check for thermal shutdown
journalctl -b -1 | grep -i -E "thermal|temperature|shutdown"
dmesg | grep -i -E "thermal|critical"
# Check battery stats
upower -d
cat /sys/class/power_supply/BAT0/uevent
# Check for hardware errors
journalctl -b -1 | grep -i -E "error|fault|panic"
```

## 10. File And Data Problems

### Can't Find My Files

**Problem:** Files that existed before installing Spike can't be found.

**GUI fix:**

1. If you backed up files before installation: 

```
├── Files were copied to USB during installation (SpikeBackup/)
├── Plug in the USB drive
├── Open Dolphin (file manager)
├── Find the USB drive under "Removable Devices"
└── Navigate to SpikeBackup/ folder
```

2. If you did NOT back up: 

```
├── Installation erased the drive — files are gone
├── Spike Rescue cannot recover files after a full install
└── This is why backup is offered during installation
```

3. If files were on a different drive (not the install target): 

```
├── They should still be there
├── Open Dolphin and check the drive
└── The drive may need to be mounted (click it in sidebar)
```

4. If files were in a specific location: 

```
├── Check: /home/[your-username]/Documents
├── Check: /home/[your-username]/Pictures
└── If restored: they're in the same folders they were in before
```

### Disk Is Full

**Problem:** "Disk full" error, or can't save files, or updates fail with space error.

**GUI fix:**

1. Check disk usage: **Settings → Advanced → Storage** 

```
├── Shows what's taking up space
├── Categories: System, Applications, User files, Cache
└── Click a category to see details
```

2. Delete files you don't need: 

```
├── Downloads folder often has large files
├── Trash: Settings → Storage → "Empty Trash"
└── Old videos/images you've backed up elsewhere
```

3. Clear caches: **Settings → Advanced → Storage → "Clear Cache"** 

```
├── Thumbnail cache: safe to clear
├── Flatpak unused runtimes: safe to remove
└── Package cache: safe to clear
```

4. Uninstall applications you don't use: 

```
├── Open Discover
├── Go to Installed tab
└── Remove apps you don't need
```

5. Check Flatpak runtimes: 

```
├── Unused runtimes take significant space
├── Discover → Settings → "Remove unused runtimes"
└── Or: Settings → Storage → Flatpak → Clean
```

6. If disk is still full after cleaning: 

```
├── You may need a larger drive
└── 128GB minimum is tight if you store many videos/photos
```

**Advanced fix:**

```
# Check disk usage
df -h
du -sh /* 2>/dev/null | sort -rh | head -10
# Find large files
find / -type f -size +500M 2>/dev/null | head -20
# Clean package cache
sudo apt clean
sudo apt autoremove
# Clean Flatpak
flatpak uninstall --unused
# Check user directory sizes
du -sh ~/Downloads ~/Documents ~/Pictures ~/Videos 2>/dev/null
```

### Files Disappeared After Update

**Problem:** Some files or settings changed after a system update.

**GUI fix:**

1. Check if files are in Trash: 

```
├── Open Dolphin
├── Check Trash in the sidebar
└── If found: restore them
```

2. Check if the home directory changed: 

```
├── Updates should never delete user files
├── If files are truly gone: this is a serious bug
└── Report immediately via Settings → "Report a Problem"
```

3. Check if it was a kernel update: 

```
├── Kernel updates don't touch user files
└── If files disappeared: something else is wrong
```

4. Check if Flatpak data was reset: 

```
├── Flatpak updates shouldn't reset app data
├── If an app's data was lost: the app may have been reinstalled
└── Report the specific app
```

### Can't Access USB Drive

**Problem:** A USB drive is plugged in but can't be opened in the file manager.

**GUI fix:**

1. Open Dolphin (file manager) 

```
├── Look for the USB drive in the left sidebar
├── Under "Removable Devices"
└── Click it to mount and open
```

2. If it doesn't appear: 

```
├── Try a different USB port
├── Try unplugging and replugging
└── Wait 5 seconds after plugging in
```

3. If it appears but won't open: 

```
├── The filesystem may be corrupt
├── A dialog may appear: "This device has an unrecognized filesystem"
└── Click "Format" if you don't need the data on it
```

4. If you need the data: 

```
├── Do NOT format
├── Try on another computer
└── If it works elsewhere: Spike filesystem driver issue (report it)
```

5. If it's a Windows-formatted drive (NTFS): 

```
├── NTFS should work (ntfs-3g)
├── If not: the drive may not have been safely ejected from Windows
└── Plug into a Windows machine, safely eject, try again
```

**Advanced fix:**

```
# Check if device is detected
lsblk
lsusb
# Try mounting manually
sudo mount /dev/sdX /mnt
# Check filesystem type
sudo blkid /dev/sdX
# Check for filesystem errors
sudo fsck /dev/sdX
# (WARNING: do not run fsck on a mounted drive)
# Check udisks2 status
dmesg | grep -i usb
journalctl | grep -i udisks
```

### Recovering Lost Files

**Problem:** Files need to be recovered from a broken Spike installation.

**GUI fix:**

1. Boot from Spike USB installer: 

```
├── Plug in the Spike USB
├── Power on, enter firmware boot menu (F12 usually)
├── Select USB device
└── Wait for live environment
```

2. On the desktop, click "Rescue My Files" 

3. Spike Rescue will: 

```
├── Scan for installed operating systems
├── Mount the broken Spike installation READ-ONLY
├── Scan for user data (Documents, Photos, etc.)
├── Ask for a USB destination
├── Copy files with verification
└── Show a summary: "N files recovered, M files failed"
```

4. After recovery: 

```
├── Files are on the destination USB
├── The broken installation is untouched (read-only mount)
└── You can reinstall Spike and restore from the backup USB
```

5. If Spike Rescue can't find your installation: 

```
├── The partition table may be damaged
├── Try testdisk (advanced tool, not included on ISO)
└── Consult a data recovery professional if critical
```

See `DISASTER-RECOVERY.md` for the full 4-layer recovery model.

## 11. Account And Security

### Forgot Password

**Problem:** User can't log in because they forgot their password.

**GUI fix:**

This is handled through recovery mode:

1. Reboot the computer 

2. Press ESC during boot (within the 3-second window) 

3. Select "Spike (recovery mode)" from the GRUB menu 

4. The system boots into a root shell 

5. Type: `passwd [your-username]` 

```
├── Enter a new password (you won't see characters — that's normal)
└── Confirm the new password
```

6. Type: `reboot` 

7. Log in with the new password 

> **Security note:** Recovery mode allows password reset without authentication — this means **anyone with physical access to the laptop can reset passwords**. This is standard Linux behavior and is an accepted security trade-off for a personal laptop. For multi-user systems or devices in shared spaces, consider disk encryption during installation.

If you can't access recovery mode:

```
├── Boot from USB
├── Use chroot to reset password (advanced — see below)
└── Or reinstall Spike (files can be rescued first)
```

**Advanced fix:**

```
# Boot from live USB, chroot into system:
sudo mount /dev/sdaX /mnt
for dir in proc sys dev run; do
  sudo mount --bind /$dir /mnt/$dir
done
sudo chroot /mnt
# Check PAM configuration
cat /etc/pam.d/sddm
cat /etc/pam.d/system-auth
# Check shadow file
cat /etc/shadow | grep [username]
# Reset password from chroot
passwd [username]
# Check SDDM configuration
cat /etc/sddm.conf
```

### Can't Log In

**Problem:** The correct password doesn't work at the login screen, or SDDM rejects every attempt.

**GUI fix:**

1. Check keyboard layout: 

```
├── SDDM uses the default keyboard layout
├── If you set a non-default layout during installation, SDDM may not reflect it
├── Look for a keyboard layout indicator at the login screen
└── Try typing the password assuming a US/QWERTY layout
```

2. Check Caps Lock: 

```
├── SDDM does not always show a Caps Lock warning
├── Press Caps Lock to toggle it off
└── Try the password again
```

3. Check number lock (Num Lock): 

```
├── If your password uses numbers, check Num Lock is on
└── Laptops may have a Fn+Num Lock combination
```

4. If still not working: 

```
├── Use recovery mode to reset the password (see "Forgot Password" above)
└── This rules out whether it's a password issue or an auth system issue
```

5. If recovery mode password reset works but SDDM still fails: 

```
├── The PAM configuration may be broken
├── Report immediately via Settings (from another user account, if available)
└── Or boot from USB and investigate
```

**Advanced fix:**

```
# Boot from live USB, chroot into system:
sudo mount /dev/sdaX /mnt
for dir in proc sys dev run; do
  sudo mount --bind /$dir /mnt/$dir
done
sudo chroot /mnt
# Check PAM configuration
cat /etc/pam.d/sddm
cat /etc/pam.d/system-auth
# Check shadow file
cat /etc/shadow | grep [username]
# Reset password from chroot
passwd [username]
# Check SDDM configuration
cat /etc/sddm.conf
```

### Screen Won't Unlock

**Problem:** After suspend, the screen is locked but entering the password doesn't unlock it.

**GUI fix:**

1. Wait a few seconds after resume: 

```
├── After suspend, the lock screen may take 2-3 seconds to accept input
└── The display is still initializing
```

2. Press a key or move the mouse: 

```
├── The lock screen may need input to activate
└── Wait for the cursor to appear
```

3. Check Caps Lock and keyboard layout (same as "Can't Log In")

4. Try Ctrl+Alt+F2:

```
├── If text console appears: the session is alive, lock screen is stuck
├── Log in, type: loginctl unlock-session
└── If desktop appears: the lock screen was hung
```

5. If nothing responds: 

```
├── Hard reset (hold power button 5 seconds)
├── On reboot: check if unsaved work was lost
└── Report if this happens repeatedly
```

**Advanced fix:**

```
# Check lock screen process
ps aux | grep -i -E "kscreenlocker|spike-lock"
loginctl list-sessions
loginctl session-status
# Unlock session
loginctl unlock-session <session_id>
# Check for display issues after resume
journalctl -b 0 | grep -i -E "drm|display|backlight|resume"
# Check if suspend itself had issues
journalctl -b 0 | grep -i suspend
```

### Firewall Warning In Settings

**Problem:** **Settings → Diagnostics** shows a firewall warning, or a yellow/red indicator next to "Firewall Status".

**GUI fix:**

1. Check what the warning says: **Settings → Advanced → Diagnostics → Security** 

```
├── If "Firewall inactive": ufw is not running
│   ├── Click "Enable Firewall"
│   └── If it won't enable: see advanced fix
├── If "Firewall misconfigured": rules may be incorrect
│   ├── Click "Reset Firewall to Defaults"
│   └── This restores the deny-incoming, allow-outgoing baseline
└── If "Unknown firewall state": ufw may not be installed
    └── This shouldn't happen on Spike — report it
```

2. After enabling or resetting: 

```
└── Warning should turn green within a few seconds
```

3. If the warning persists after reset: 

```
├── Something is modifying ufw rules
├── Check if a VPN client is installed that might override rules
└── Report with diagnostic info attached
```

**Advanced fix:**

```
# Check ufw status
sudo ufw status verbose
# Enable ufw
sudo ufw enable
# Check default rules
sudo ufw status numbered
# Reset to defaults
sudo ufw --force reset
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw enable
# Check if iptables is conflicting
sudo iptables -L -n
```

## 12. System Clock Problems

### System Clock Is Wrong

**Problem:** The clock in the panel or system shows the wrong time, or timezones are incorrect.

**Likely causes:**

```
├── NTP (network time) not enabled or not syncing
├── Wrong timezone selected
├── CMOS battery dead (old laptop) — hardware clock loses time
└── Dual-boot with Windows (clock conflict)
```

**GUI fix:**

1. Check NTP sync: **Settings → Date & Time** 

```
├── "Network time synchronization" should be ON
├── If OFF: toggle it ON
└── Wait 1-2 minutes for sync
```

2. Check timezone: **Settings → Date & Time** 

```
├── "Timezone" should match your location
├── Click "Change" to select the correct region/city
└── "Auto-detect timezone" available via IP geolocation
```

3. Check if clock drifts after reboot: 

```
├── If time is wrong immediately after boot: CMOS battery may be dead
├── CMOS battery replacement requires opening the laptop
└── Consult a repair shop if uncomfortable
```

4. If dual-booting with Windows: 

```
├── Windows expects hardware clock in local time
├── Linux expects hardware clock in UTC
├── This causes clock to be wrong when switching OSes
└── Solution: Set Windows to use UTC (registry change) or always boot into Linux first
```

**Advanced fix:**

```
# Check NTP status
timedatectl status
# Check current timezone
timedatectl
# Manually set timezone
sudo timedatectl set-timezone "America/New_York"  *# example
# Force NTP sync
sudo systemctl restart systemd-timesyncd
# Check hardware clock
sudo hwclock --show
# Sync system time to hardware clock
sudo hwclock --systohc
```

### Time Keeps Drifting

**Problem:** The clock stays correct initially but gradually drifts over days or weeks.

**Likely causes:**

```
├── NTP sync interval is too long
├── CMOS battery failing — hardware clock loses time between boots
├── VM or container with virtualized clock issues
└── Thermal issues affecting motherboard crystal oscillator
```

**GUI fix:**

1. Ensure NTP is enabled: **Settings → Date & Time** 

```
└── "Network time synchronization" must be ON continuously
```

2. Check if drift happens after reboot: 

```
├── If time is wrong immediately after boot: CMOS battery issue
├── If time drifts while system is running: NTP not syncing frequently enough
└── Try rebooting — if problem resolves, NTP may have missed a sync
```

3. Replace CMOS battery if old: 

```
├── Most laptops have a coin-cell battery on the motherboard
├── Typical lifespan: 5-10 years
└── On 10-year-old laptops, this is a common failure point
```

4. Check if system is in a VM: 

```
├── Virtual machines sometimes have clock drift issues
└── Install VM guest tools for better time sync
```

**Advanced fix:**

```
# Check NTP sync status
timedatectl status
# Check systemd-timesyncd logs
journalctl -u systemd-timesyncd --no-pager
# Force immediate sync
sudo systemctl stop systemd-timesyncd
sudo systemd-timedated
sudo systemctl start systemd-timesyncd
# Check hardware clock battery (if accessible)
# (Requires opening laptop — CMOS battery test/replacement)
```

## 13. Notification Problems

### Notifications Disappeared

**Problem:** Notifications that were previously visible are now gone, or the notification tray shows nothing.

**Likely causes:**

```
├── Notification retention period expired (default: 3 days)
├── history.json was corrupted and reset
├── Notification daemon restarted and reloaded from disk
└── User cleared notifications accidentally
```

**GUI fix:**

1. Check retention settings: **Settings → Notifications → Retention** 

```
├── Default: 3 days (older notifications auto-deleted)
├── Increase retention if needed (up to 31 days)
└── This is expected behavior, not a bug
```

2. Check if "Clear All" was pressed: 

```
├── The notification tray has a "Clear All" button
├── If pressed: all notifications are removed
└── This is intentional (user-initiated action)
```

3. Check Do Not Disturb: 

```
├── If DND is ON: notifications are received but not displayed
├── They are still written to history.json
└── Turn off DND to see them again
```

4. Check notification history: 

```
├── Click the notification icon → "View All"
├── This shows history.json entries
└── If history is empty: notifications expired or were cleared
```

5. If history.json was corrupted: 

```
├── The daemon resets to an empty file
├── New notifications will be recorded normally
└── Old ones are lost (no recovery — disk-before-display doesn't help here)
```

**Advanced fix:**

```
# Check history.json
cat ~/.local/share/spike/notifications/history.json | python3 -m json.tool
# Check notification daemon status
systemctl --user status spike-notifications
# Check daemon logs
journalctl --user -u spike-notifications --no-pager
# Check if history.json is valid JSON
python3 -c "import json; json.load(open('~/.local/share/spike/notifications/history.json'))"
# Restart notification daemon
systemctl --user restart spike-notifications
```

### No Notification Sound

**Problem:** Notifications appear visually but don't play a sound.

**GUI fix:**

1. Check system volume: 

```
├── Panel volume applet → make sure not muted
└── Volume should be above 0%
```

2. Check notification sound setting: **Settings → Notifications** 

```
├── "Play sound for notifications": should be checked
└── If unchecked: check it
```

3. Check Do Not Disturb: 

```
├── DND suppresses notification sounds
└── Turn DND off
```

4. Check if it's a specific app: 

```
├── Some apps send silent notifications by design
├── Firefox download notifications may be silent
└── System notifications (updates, power) should have sound
```

5. Check notification urgency: 

```
├── Low-urgency notifications don't play sound
├── Normal and critical urgency do
└── This is per-app behavior, not configurable by the user
```

**Advanced fix:**

```
# Check PipeWire is running
systemctl --user status pipewire
# Check if notification sound file exists
ls /usr/share/sounds/spike/
# Play sound manually
paplay /usr/share/sounds/spike/notification.ogg
# Check notification daemon sound config
cat ~/.config/spike/notifications/settings.json
# Check DBus notification (inspect hints)
dbus-monitor "interface='org.freedesktop.Notifications'"
```

### Too Many Notifications

**Problem:** The notification tray is constantly filling up with too many notifications.

**GUI fix:**

1. Identify the source: **Click the notification icon → View All** 

```
├── Look at which apps are generating the most notifications
└── The app name is shown with each notification
```

2. Per-app notification settings: **Settings → Notifications → Application Settings** 

```
├── Find the offending app
├── Toggle "Notifications" off for that app
└── Or set to "Important only" (suppresses low-urgency)
```

3. Reduce retention: **Settings → Notifications → Retention** 

```
├── Lower to 1 day if tray fills too quickly
└── Max count can be lowered (e.g., 100 instead of 500)
```

4. Enable Do Not Disturb for focused work: 

```
├── Panel notification icon → toggle DND
├── Or set a schedule:
│   Settings → Notifications → Do Not Disturb → Schedule
│   └── e.g., "10:00 PM to 7:00 AM"
└── Notifications are still saved to history, just not displayed
```

5. For browser notifications: 

```
├── Firefox → Settings → Privacy → Notifications
├── Block sites that send too many notifications
└── Or set to "Ask" instead of "Allow"
```

### Notification Badge Won't Clear

**Problem:** The notification tray badge (red dot with count) won't disappear even after viewing notifications.

**GUI fix:**

1. Click the notification icon: 

```
├── This marks all current notifications as "read"
├── The badge should clear
└── If it doesn't clear: see below
```

2. Check if new notifications arrived: 

```
├── Between opening and viewing, a new notification may have arrived
└── The badge reflects unread count, not total count
```

3. "Clear All" button: 

```
├── Click the notification icon → "Clear All"
├── This removes all notifications from the tray
└── The badge should clear immediately
```

4. If badge persists after Clear All: 

```
├── This is a bug (the badge should never auto-clear — it's a design
│   feature — but Clear All should reset it)
├── Report it
└── Include: what app(s) were in the tray, when it happened
```

**Advanced fix:**

```
# Check notification daemon state
journalctl --user -u spike-notifications | tail -20
# Check badge count vs history count
cat ~/.local/share/spike/notifications/history.json | \\
  python3 -c "import json,sys; d=json.load(sys.stdin); print(len(d))"
# Restart notification daemon
systemctl --user restart spike-notifications
# Check DBus for lingering notifications
gdbus call --session --dest=org.freedesktop.Notifications \\
  --object-path /org/freedesktop/Notifications \\
  --method org.freedesktop.Notifications.GetServerInformation
```

## General Diagnostic Procedures

### When No Solution Above Works

If none of the specific fixes resolve the problem, use these general procedures:

**Collect Diagnostic Information**

**Settings → Advanced → Diagnostics → "Export Report"**

This generates a text file containing:

```
├── System information (CPU, RAM, storage, GPU)
├── Spike version and variant
├── Kernel version
├── Service status (running/failed list)
├── Recent journal entries (last 200 lines)
├── Network adapter list
├── Audio device list
├── Display configuration
├── Memory usage snapshot
├── Disk usage snapshot
├── Boot time analysis
└── SMART status (if available)
```

Attach this file to any bug report. It contains no personal data (no browsing history, no file contents, no passwords).

**Report A Problem**

**Settings → Advanced → "Report a Problem"**

This opens the Spike issue tracker in Firefox:

```
├── URL: https://git.bigrangatech.com/spike/spike/-/issues/new
├── Template auto-filled with:
│   ├── Spike version
│   ├── Variant (Standard/Plus)
│   ├── Hardware (CPU, RAM, storage)
│   └── Kernel version
├── User fills in:
│   ├── Problem description (what happened, what you expected)
│   ├── Steps to reproduce (if known)
│   └── Diagnostic report (attach exported file)
└── No account required to view issues (account needed to submit)
```

**Boot Into Recovery Mode**

When the system can't boot normally:

1. Reboot 

2. Press ESC during the 3-second GRUB window 

3. Select "Spike (recovery mode)" 

4. Root shell appears (no graphics, no network) 

5. Available recovery actions: 

```
├── passwd [username]              → Reset password
├── spike-config --rollback [id]    → Undo config change
├── dpkg --configure -a            → Fix broken packages
├── apt --fix-broken install       → Fix broken dependencies
├── journalctl -b -1               → View previous boot logs
├── fsck /dev/sda2                 → Check filesystem
└── systemctl reboot               → Reboot normally
```

6. Fix the issue, then reboot 

See `DISASTER-RECOVERY.md` for the full 4-layer recovery model.

**Boot From Live USB (Spike Rescue)**

When the system can't boot at all:

1. Plug in Spike USB installer 

2. Power on, enter firmware boot menu (F12/F2/Del — varies) 

3. Select USB device 

4. Live environment boots 

5. Click "Rescue My Files" on desktop 

6. Spike Rescue: 

```
├── Scans for installed operating systems
├── Mounts the broken system READ-ONLY
├── Copies user data to a USB drive with SHA256 verification
└── Shows a summary
```

7. After rescue: 

```
├── Reinstall Spike (files are safe on USB)
└── Or try to fix the problem manually (advanced)
```

**Try Previous Kernel**

When problems started after a kernel update:

1. Reboot 

2. Press ESC during GRUB window 

3. Select "Spike (previous kernel)" 

4. If the problem goes away: 

```
├── The kernel update caused the issue
├── Report it with kernel version info
└── Continue using previous kernel until fix is released
```

5. The previous kernel is kept for exactly this scenario 

6. Spike only keeps ONE previous kernel (disk space conservation) 

## Common Error Messages

| **Error Message** | **Meaning** | **What To Do** |
| :-: | :-: | :-: |
| "Spike closed [app] to free memory" | earlyoom killed an app | Close other apps, reopen the killed app |
| "Cannot connect to network" | Wi-Fi/Ethernet not connected | Check connection, restart router |
| "Disk space low" | Less than ~5GB free | Delete files, clear cache, remove unused apps |
| "USB device not recognized" | USB enumeration failed | Try different port, different cable, reboot |
| "A system error occurred" | Generic error | Check Settings → Diagnostics, report |
| "Bluetooth device failed to pair" | Pairing timeout or rejection | Retry, move closer, check device is in pairing mode |
| "Filesystem read-only" | Disk corruption or protection | Reboot (fsck will run automatically) |
| "Authentication failed" | Wrong password or PAM issue | Check Caps Lock, try recovery mode |
| "Update failed" | apt or Flatpak update error | Check network, retry, report if persistent |
| "Compositor crashed" | KWin crashed | Session will restart automatically; report if repeated |
| "Plymouth failed to start" | Boot splash issue | Disable splash (Settings → Boot → Minimal) |
| "Boot failure detected" | Boot failure counter triggered | Select previous kernel or recovery mode |

> **Note:** Error messages shown in English. Localized versions may differ. If reporting a bug with non-English messages, translate them to English or include screenshots.

## What This Document Does Not Cover

- Boot process technical details (GRUB configuration, initramfs, boot stages): See `BOOT-PROCESS.md` 

- Memory management architecture (ZRAM, swap, earlyoom thresholds): See `MEMORY.md` 

- Kernel module loading and boot parameters: See `KERNEL.md` 

- Installer flow and steps: See `INSTALLER.md` 

- Network configuration details (Wi-Fi, VPN, DNS): See `NETWORKING.md` 

- Audio system architecture (PipeWire, WirePlumber): See `MULTIMEDIA.md` 

- Display and compositor details (KWin, Wayland): See `DESKTOP.md` 

- Security hardening details (ufw, AppArmor, root lock): See `SECURITY.md` 

- Privacy settings and data collection policy: See `PRIVACY.md` 

- Disaster recovery procedures (4-layer model, Spike Rescue): See `DISASTER-RECOVERY.md` 

- Configuration system (spike-config, state store, changelog): See `CONFIGURATION.md` 

- Variant differences (animations, effects, polling intervals): See `VARIANT-DIFFERENCES.md` 

- Power management (suspend, hibernate, battery calibration): See `POWER-MANAGEMENT.md` 

- Hardware tier definitions and detection: See `HARDWARE.md` 

- Performance baselines (expected timings, targets): See `PERFORMANCE-BASELINES.md` 

🐕 BigRangaTech


