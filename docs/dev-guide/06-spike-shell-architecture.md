# Spike Dev Guide — Spike Shell Architecture

## Purpose

Implementation sketch of **spike-shell** — the live/installed desktop shell (not Plasma as a full DE). Product UI requirements: `DESKTOP.md`. Update this page as modules land or IPC changes.

## Status

Shipping on the live ISO (version in `src/spike-shell/CMakeLists.txt` / `STATE.md`). Pre-alpha: expect polish gaps vs Plasma branding docs.

## High-level processes

```
tty / display manager
        │
        ▼
spike-session (wayland session entry)
        │
        ├── compositor / layer-shell stack (as packaged)
        └── spike-shell
                  ├── Panel (applets: network, volume, battery, …)
                  ├── Launcher (Kickoff-style; scans *.desktop)
                  ├── Settings host (KCMs + custom pages via org.spike.Config)
                  └── Power / audio / network helpers
```

## Source layout

```
src/spike-shell/src/
├── main.cpp
├── panel/
├── launcher/          → Launcher.cpp (categoryFor, desktop scan)
├── settings/
├── network/
├── audio/
└── power/
```

Session / desktop entry bits also under `src/spike-shell/session/` (packaged into the `.deb`).

## Launcher categories

The shell does **not** read `applications.menu` for Kickoff buckets. It parses `Categories=` from `.desktop` files in `Launcher.cpp` → `categoryFor()`.

Spike-owned tools:

```
Categories=…;X-Spike-Tools;…
```

must be checked **before** generic `System`, returning the label **Spike Tools**.

Freedesktop menus (Dolphin / KService) still use:

- `etc/xdg/menus/applications.menu`
- `usr/share/desktop-directories/spike-tools.directory`

Both live under `build/iso-build/config/includes.chroot/`.

## Packaging

```
./scripts/package-spike-shell.sh
```

ISO hook installs the deb and may seed skel Desktop / xdg configs. See `04-building-spike.md` and `05-building-components.md`.

## Related product docs

- `DESKTOP.md` — pages, applets, Settings map  
- `BRANDING.md` — look (shell QSS ≠ full Plasma theme)  
- `CONFIGURATION.md` / `MEMORY.md` — settings that call into spike-config  

## Related

- Repo map: `02-repo-structure.md`
- Branding build notes: `10-branding-and-theming.md` (stub until themes solidify)
