# Spike Shell

Stage 3 skeleton — **pre-alpha**. Spec: `docs/DESKTOP.md`.

## What this is

Spike Shell is the custom desktop shell (Qt6 **Widgets**, not QML) that runs as a Wayland client under **standalone KWin**. It does **not** use Plasma (`plasma-workspace`, Akonadi, Baloo, etc.).

MVP for this skeleton:

| Piece | Status |
|-------|--------|
| Bottom panel (32px) | ✅ stub |
| Spike button → launcher | ✅ stub |
| Clock applet | ✅ stub |
| Session menu (logout/reboot/poweroff) | ✅ stub (`loginctl` / `systemctl`) |
| Theme (`spike.qss`) — white text on dark | ✅ stub |
| ISO / `.deb` packaging | ✅ via `scripts/package-spike-shell.sh` |
| `wlr-layer-shell` panel anchoring | ⬜ later (runs as normal window for now) |
| `.desktop` app scanning | ⬜ later |
| Notification daemon / tray applets | ⬜ later |

## Build (host smoke)

```bash
cd src/spike-shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./build/spike-shell
```

Needs: `cmake`, `qt6-base-dev` (Widgets). On a full Spike session you will also need `kwin-wayland`; for host smoke-tests the panel opens as a normal window.

## Package + ISO

```bash
./scripts/package-spike-shell.sh
# or as part of a full image:
sudo ./scripts/build-iso.sh
```

`build-iso.sh` packages both `spike-config` and `spike-shell`, stages newest `spike-*.deb` under `includes.chroot/var/cache/spike-local/`, and installs them via hook `0600-spike-config.chroot`. Live package list includes Qt6 Widgets runtime, `qt6-wayland`, `kwin-wayland`, and `xwayland`.

## Session stub

`session/spike-session` starts `kwin_wayland` then `spike-shell`. On the live image: `spike-shell` alone for a quick panel smoke, or pick the **Spike** Wayland session (`spike.desktop`) when a display manager is present.

## License

GPLv2+ (code). Specs/branding remain CC-BY-SA 4.0.
