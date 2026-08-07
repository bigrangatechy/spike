# Spike Shell

Stage 3 skeleton — **pre-alpha**. Spec: `docs/DESKTOP.md`.

## What this is

Spike Shell is the custom desktop shell (Qt6 **Widgets**, not QML) that runs as a Wayland client under **standalone KWin**. It does **not** use Plasma (`plasma-workspace`, Akonadi, Baloo, etc.).

MVP for this skeleton:

| Piece | Status |
|-------|--------|
| Bottom panel (24–48px, top/bottom, auto-hide) | ✅ live from desktop module |
| Spike button → Kickoff-style launcher | ✅ favorites + categories + power footer |
| Clock applet | ✅ |
| Network tray applet | ✅ |
| Volume tray applet | ✅ PipeWire-Pulse via pactl |
| Battery tray applet | ✅ UPower (hidden if no battery) |
| Session menu (Settings / logout / reboot / poweroff) | ✅ |
| Settings window | ✅ custom pages + KCM hosts |
| Appearance (panel live-apply) | ✅ accent/font/animations saved only |
| Date & Time / Keyboard / Mouse / Layout | ✅ Spike-native |
| Language / Users / VPN | ✅ localectl / thin accounts / NM VPN |
| Notifications (prefs) | ✅ DND/retention/sound saved; daemon later |
| Accessibility (started) | ✅ AccessX + Orca/OSK; high contrast saved |
| Software Sources (started) | ✅ APT list + properties/drivers/update tools |
| Memory / Boot forms | ✅ SetSetting + generate |
| KCM pages | ✅ in-window host (`KcmHost` + Apply/Reset); packages added as pages flesh out |
| Theme (`spike.qss`) — white text on dark | ✅ |
| Icons (`spike-icons` inherits Breeze Dark) | ✅ |
| ISO / `.deb` packaging | ✅ via `scripts/package-spike-shell.sh` |
| `wlr-layer-shell` panel anchoring | ✅ via LayerShellQt (top or bottom) |
| Session start (seatd + cursor) | ✅ live smoke |
| `.desktop` app scanning + `Terminal=true` | ✅ |
| Notification daemon / magnifier / APT edit UI | ⬜ later |

## Build (host smoke)

```bash
cd src/spike-shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./build/spike-shell
```

Needs: `cmake`, `qt6-base-dev`, `liblayershellqtinterface-dev`. On a full Spike session you will also need `kwin-wayland` + `layer-shell-qt`; for host smoke-tests without a compositor the panel opens as a normal window (geometry fallback). Settings talks to `org.spike.Config` on the system bus (`spike-config`).

## Package + ISO

```bash
./scripts/package-spike-shell.sh
# or as part of a full image:
sudo ./scripts/build-iso.sh
```

`build-iso.sh` packages both `spike-config` and `spike-shell`, stages newest `spike-*.deb` under `includes.chroot/var/cache/spike-local/`, and installs them via hook `0600-spike-config.chroot`. Live package list includes Qt6 Widgets, `layer-shell-qt`, `kwin-wayland`, and `xwayland`.

## Session

`session/spike-session` starts **DRM** `kwin_wayland` via **`seatd-launch`** (so keyboard/mouse are granted from a text VT), then `spike-shell`. Refuses bare root; if you `sudo`, it re-execs as `SUDO_USER`. Logs tee to `$XDG_RUNTIME_DIR` and `/var/log/spike/session-latest.log` (casper copies that to USB).

**Important:** run from a text VT as the live user. Prefer plain `spike-session` (not sudo).

## License

GPLv2+ (code). Specs/branding remain CC-BY-SA 4.0.
