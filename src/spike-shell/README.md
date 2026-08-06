# Spike Shell

Stage 3 skeleton — **pre-alpha**. Spec: `docs/DESKTOP.md`.

## What this is

Spike Shell is the custom desktop shell (Qt6 **Widgets**, not QML) that runs as a Wayland client under **standalone KWin**. It does **not** use Plasma (`plasma-workspace`, Akonadi, Baloo, etc.).

MVP for this skeleton:

| Piece | Status |
|-------|--------|
| Bottom panel (32px) | ✅ |
| Spike button → Kickoff-style launcher | ✅ favorites + categories + power footer |
| Clock applet | ✅ |
| Session menu (Settings / logout / reboot / poweroff) | ✅ |
| Settings window (categories + About/Memory via D-Bus) | ✅ first wire |
| KCM pages | 📝 listed; opens via `kcmshell6` until in-window host |
| Theme (`spike.qss`) — white text on dark | ✅ |
| ISO / `.deb` packaging | ✅ via `scripts/package-spike-shell.sh` |
| `wlr-layer-shell` panel anchoring | ✅ via LayerShellQt (bottom edge) |
| Session start (seatd + cursor) | ✅ live smoke |
| `.desktop` app scanning + `Terminal=true` | ✅ |
| Notification daemon / tray applets | ⬜ later |

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
