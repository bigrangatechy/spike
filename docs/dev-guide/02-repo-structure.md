# Spike Dev Guide — Repository Structure

## Purpose

Map of the Spike monorepo as it exists today. Update this page when you add a top-level package, script, or docs tree — do not invent paths that are not on disk.

Product behaviour lives in the top-level specs (`DESKTOP.md`, `INSTALLER.md`, `DISASTER-RECOVERY.md`, …). This page is **where the code and build glue live**.

## Top level

```
spike/
├── src/                    → Spike-owned packages (apps, libs, branding)
├── scripts/                → Build / package / USB / smoke helpers
├── build/
│   ├── iso-build/          → live-build recipe (includes.chroot, hooks, package-lists)
│   └── packages/           → Built .debs (gitignored binaries; produced by package-*.sh)
├── docs/                   → Product + agent-ops + this dev-guide
├── licences/               → License texts
└── README.md / LICENSE*
```

## `src/` packages

| Path | Status | Role |
| :-: | :-: | :-: |
| `spike-shell/` | ✅ Shipping on live ISO | Qt6 desktop shell (panel, launcher, settings host, session) |
| `spike-config/` | ✅ Shipping | Variant / hardware profile engine (Python) |
| `spike-rescue/` | ✅ Shipping | Rescue My Files — recover + restore GUI |
| `spike-common/` | ✅ Used by rescue | Shared SpikeBackup layout / find / path map |
| `spike-installer/` | 📝 **0.0.1** wizard UI | Custom Qt installer (wipe engine not enabled yet) |
| `spike-migration/` | 🔲 Scaffold | Move My Files (detailed migration wizard) |
| `spike-branding/` | ✅ **0.0.1** packaged | Plymouth + GRUB + logos (`package-spike-branding.sh`) |

### spike-rescue (Layer 3)

```
src/spike-rescue/
├── CMakeLists.txt          → project version (bump with releases)
├── src/
│   ├── main.cpp
│   ├── MainWindow.*        → wizard UI (mode → recover | restore)
│   ├── RescueEngine.*      → scan, mount, inventory, copy, restore
│   └── RescueTypes.hpp
├── data/
│   ├── spike-rescue-mount  → privileged helper (/usr/lib/spike/)
│   ├── spike-rescue.sudoers
│   ├── spike-rescue.desktop
│   └── spike-rescue-desktop.desktop  → live Desktop shortcut template
└── resources/
```

Package script: `scripts/package-spike-rescue.sh` → `build/packages/spike-rescue_*.deb`.

### spike-common (shared SpikeBackup)

```
src/spike-common/
├── SpikeBackupLayout.hpp/.cpp   → find sessions, map restore → ~/…
└── README.md
```

Canonical on-disk layout:

```
SpikeBackup/<utc-stamp>/<os-label>/home/<user>/Documents/…
```

Consumed today by `spike-rescue`. Installer Step 7 / Layer 4 and `spike-migration` must reuse this — do not invent a second tree format.

### spike-installer / spike-migration

```
src/spike-installer/          → 0.0.1 wizard (see README); package-spike-installer.sh
src/spike-migration/          → scaffold + spec only
```

Specs: `INSTALLER.md`, `SPIKE-MIGRATION.md`, `SPIKE-RECOVERY-TOOL-GENERAL.md`.

### spike-shell

```
src/spike-shell/
├── src/
│   ├── main.cpp
│   ├── panel/ launcher/ settings/ network/ audio/ power/
│   └── …
├── session/                    → spike-session / wayland session bits
└── CMakeLists.txt
```

Package: `scripts/package-spike-shell.sh`. Launcher category **Spike Tools** matches `X-Spike-Tools` in `.desktop` files (see `06-spike-shell-architecture.md`).

### spike-config

```
src/spike-config/
├── spike_config/               → Python package
├── modules/ boot/ memory/ …    → profile / apply modules
├── data/ templates/ tests/
└── pyproject.toml
```

Package: `scripts/package-spike-config.sh`. Details: `09-spike-config-internals.md` (when filled).

## `scripts/`

| Script | Purpose |
| :-: | :-: |
| `build-iso.sh` | Package local .debs + run live-build |
| `package-spike-{shell,config,rescue}.sh` | Build component .debs into `build/packages/` |
| `spike-iso-hybridize.sh` / `spike-usb-fat32.sh` | USB write helpers |
| `spike-collect-usb-logs.sh` | Pull capture / writable logs from a stick |
| `install-dev-env.sh` | Host deps helper |
| `hardware-test.sh` / `test-installer.sh` | Smoke / future installer tests |

## `build/iso-build/` (live-build)

Important Spike-owned bits:

```
config/
├── package-lists/spike-live.list.chroot
├── spike-archives/             → Mozilla APT (staged → archives/ by build-iso.sh)
├── hooks/                      → e.g. 0600 inject debs + Desktop shortcuts
├── includes.chroot/
│   ├── etc/apt/…               → Mozilla sources/prefs/key (installed + live)
│   ├── etc/xdg/menus/applications.menu
│   ├── usr/share/desktop-directories/spike-tools.directory
│   └── … branding, kdeglobals, session drop-ins
└── …
```

Full cookbook: `04-building-spike.md`.

## `docs/`

```
docs/
├── *.md                        → Product / policy specs (source of truth for behaviour)
├── agent-ops/                  → STATE, SESSION_LOG, DECISIONS, CONVENTIONS, …
├── dev-guide/                  → This tree (implementation maps)
└── user-guide/                 → End-user docs (stubs until desktop ships)
```

Recovery / migration map: `SPIKE-RECOVERY-TOOL-GENERAL.md`.

## Desktop category: Spike Tools

Apps that belong in the Spike tooling set use:

```
Categories=X-Spike-Tools;System;Utility;
```

Freedesktop menu: `includes.chroot/.../applications.menu` + `spike-tools.directory`.  
Spike Shell launcher: `Launcher.cpp` → `categoryFor()` maps `x-spike-tools` → **Spike Tools**.

## Versioning note

Component versions live in each package’s `CMakeLists.txt` / `pyproject.toml` (and often mirrored in `main.cpp` for Qt apps). ISO smoke should check the window title / package version after rebuild — see `agent-ops/STATE.md` for current shipping versions.

## Related

- Getting started: `01-getting-started.md`
- Building packages: `05-building-components.md`
- Rescue internals: `08-rescue-tool-internals.md`
- Installer map: `07-installer-internals.md`
- Agent ops status: `docs/agent-ops/STATE.md`
