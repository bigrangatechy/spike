# Spike Dev Guide — Spike Shell Architecture

## Purpose

Implementation sketch of **spike-shell** — the live/installed desktop shell (not Plasma as a full DE). Product UI requirements: `DESKTOP.md`. Update this page as modules land or IPC changes.

## Status

Shipping on the live ISO (version in `src/spike-shell/CMakeLists.txt` / `STATE.md`). Alpha: expect polish gaps vs full SECURITY.md lock chrome.

## High-level processes

```
tty / display manager
        │
        ▼
spike-session (wayland session entry)
        │
        ├── plasma-kglobalaccel (Fn / media global shortcuts)
        ├── compositor / layer-shell stack (as packaged)
        └── spike-shell
                  ├── Panel (applets: network, volume, battery, …)
                  ├── Launcher (Kickoff-style; scans *.desktop)
                  ├── ShellShortcuts (KWin spike-shortcuts → D-Bus)
                  ├── Settings host (KCMs + custom pages via org.spike.Config)
                  ├── FirstRunWizard (installed only; see firstrun/)
                  ├── LockController + SpikeLockScreen (PAM)
                  └── Power / audio / network helpers (SleepInhibit, BrightnessClient)
```

## Source layout

```
src/spike-shell/src/
├── main.cpp
├── firstrun/          → FirstRunWizard + FirstRunHooks (placeholders)
├── lock/              → SpikeLockScreen, PamAuth, LockController
├── panel/
├── launcher/          → Launcher.cpp (categoryFor, desktop scan)
├── settings/
├── network/
├── audio/
├── power/             → BatteryClient, BrightnessClient, SleepInhibit
└── shortcuts/         → ShellShortcuts + ShortcutsAdaptor (Fn/media keys)
```

### Global shortcuts (0.0.51+)

`spike-session` starts `plasma-kglobalaccel` for Meta shortcuts. Shell **0.0.51+** also
listens on `/dev/input` (`EvdevMediaKeys`) for `KEY_VOLUME*` / `KEY_BRIGHTNESS*` / media
keys — XF86 binds via KWin `registerShortcut` are unreliable outside full Plasma.
Requires the session user in group `input` (live: `spike-live-groups`; install: helper).
KWin script `spike-shortcuts` keeps Meta+L / Meta+Space → `org.spike.Shell.Shortcuts`.
Actions use `VolumeClient` / `BrightnessClient` / `playerctl` / `LockController` / launcher.

### First-run wizard (0.0.31+)

On installed systems (`/etc/spike/installed`) without `~/.config/spike/first_run_completed`,
`main.cpp` shows `FirstRunWizard` after the panel is up:

Welcome → timezone → Wi‑Fi → tour placeholder → accessibility offer → import files →
notices → Get started.

Drop-in hooks: `firstrun::runDesktopTour`, `verifyFlatpakRuntimes`, `checkSecurityUpdates`,
`offerAccessibilityWizard`, `collectPostInstallNotices` (reads
`/var/lib/spike/installer-notifications/*.txt`).

### Lock / sleep inhibit (0.0.32+)

- **Manually block sleep and screen locking** — battery popup + Settings → Power; holds
  logind `Inhibit("sleep:idle", …, "block")` (`SleepInhibit`).
- **Lock Screen** — Session menu / Meta+L (global) / `PrepareForSleep` / session `Lock` →
  `SpikeLockScreen` (PAM service `spike-lock`, packaged under `/etc/pam.d/`).
- Manual Suspend asks for confirmation when the inhibit switch is on.

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
