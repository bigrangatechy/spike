# Spike Shell

Stage 3 skeleton — **Alpha**. Spec: `docs/DESKTOP.md`.

## What this is

Spike Shell is the custom desktop shell (Qt6 **Widgets**, not QML) that runs as a Wayland client under **standalone KWin**. It does **not** use Plasma (`plasma-workspace`, Akonadi, Baloo, etc.).

MVP for this skeleton (**0.0.49**):

| Piece | Status |
|-------|--------|
| Bottom panel (24–48px, top/bottom, auto-hide) | ✅ Settings → Panel (live Apply) |
| Desktop icons (`~/Desktop` .desktop) | ✅ Install / Rescue / Move My Files |
| Spike button → Kickoff-style launcher | ✅ favorites + categories + power footer |
| Window list | ✅ best-effort (`wmctrl` when available) |
| Clock / calendar popup | ✅ |
| Network tray | ✅ + active VPN row |
| Volume tray | ✅ 0–150%, middle-click mute |
| Battery tray | ✅ UPower + ETA; **block sleep/locking** switch (logind inhibit) |
| Notifications tray | ✅ in-process `org.freedesktop.Notifications` |
| Brightness | ✅ brightnessctl/sysfs/logind + `brightness-udev` / spike udev rules |
| Removable / Updates / Night Light | ✅ UDisks2 hotplug + Notify + automount; Updates/Night Light as before |
| Bluetooth / Airplane / Keyboard Layout | ✅ conditional |
| Session menu | ✅ Spike lock screen + suspend confirm if inhibit on |
| Lock screen / greeter | ✅ SpikeLockScreen; greeter stays visible; **Meta+L** global |
| Keyboard shortcuts | ✅ Fn volume/brightness/media via evdev; Meta+L/Space via KWin |
| Window list / task icons | ✅ KWin script → panel icons (minimize/restore) |
| System Monitor | ✅ `plasma-systemmonitor` on ISO |
| Settings → PANEL | ✅ Panel / Tray Applets / Night Light (live Apply) |
| Settings window | ✅ custom pages + KCM hosts |
| Power (Settings) | ✅ spike-config + logind + same block-sleep switch |
| Appearance | ✅ accent/font/wallpaper live |
| Date & Time / Keyboard / Mouse / Layout | ✅ Spike-native |
| Language / Users / VPN | ✅ |
| Notifications prefs | ✅ DND/retention |
| Accessibility / Software Sources / Advanced forms | ✅ |
| Post-install first-run wizard | ✅ Welcome/TZ/Wi‑Fi/import/done + placeholders |
| Theme / icons / layer-shell / packaging | ✅ |

## Build (host smoke)

```bash
cd src/spike-shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./scripts/package-spike-shell.sh
# → build/packages/spike-shell_0.0.49-1_amd64.deb
```

## First-run

Installed only (`/etc/spike/installed`). Completes to `~/.config/spike/first_run_completed`.
Hooks: `src/firstrun/FirstRunHooks.*` — tour / Flatpak / updates / a11y drop-ins.
