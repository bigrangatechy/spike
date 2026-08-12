# Spike Dev Guide — Building Components

## Purpose

How to build individual Spike packages (`.deb`s) without always running a full ISO build. The ISO wrapper (`build-iso.sh`) consumes these artifacts from `build/packages/`.

## Output directory

All `package-spike-*.sh` scripts write to:

```
build/packages/<name>_<version>-<revision>_amd64.deb
```

Binaries there are gitignored; commit **source** and bump versions in each project’s metadata.

## Packages

### spike-branding

```
./scripts/package-spike-branding.sh
```

- Source: `src/spike-branding/`
- Version: `SPIKE_BRANDING_VERSION` (default `0.0.1`)
- Depends: `plymouth`
- Installs: Plymouth themes, GRUB theme, `/usr/share/spike/branding/logo/`

### spike-shell

```
./scripts/package-spike-shell.sh
```

- Source: `src/spike-shell/`
- Version: `project(spike-shell VERSION …)` in `CMakeLists.txt`
- Needs: cmake, Qt6 Widgets (+ LayerShell / other deps as listed in `03-build-environment.md`)
- Installs: `/usr/bin/spike-shell`, session helpers, QSS/assets per packaging script

### spike-config

```
./scripts/package-spike-config.sh
```

- Source: `src/spike-config/` (Python)
- Version: `pyproject.toml` / packaging script
- Used on live and installed systems for variant + hardware profiles

### spike-rescue

```
./scripts/package-spike-rescue.sh
```

- Source: `src/spike-rescue/` (+ compiles `src/spike-common/SpikeBackupLayout.cpp`)
- Version: `CMakeLists.txt` **and** `src/main.cpp` `setApplicationVersion` (keep in sync)
- Installs:
  - `/usr/bin/spike-rescue`
  - `/usr/lib/spike/spike-rescue-mount` (helper)
  - `/etc/sudoers.d/spike-rescue`
  - `/usr/share/applications/spike-rescue.desktop`
  - `/usr/share/spike/live/spike-rescue.desktop` (Desktop template)
  - `/usr/include/spike/SpikeBackupLayout.hpp` (for future installer/migration)

Optional override:

```
SPIKE_RESCUE_VERSION=0.0.9 ./scripts/package-spike-rescue.sh
```

### spike-installer

```
./scripts/package-spike-installer.sh
```

- Source: `src/spike-installer/` (+ `src/spike-common/`)
- Version: `CMakeLists.txt` / `main.cpp` (keep in sync)
- Installs: `/usr/bin/spike-installer`, applications + live Desktop template
- **0.0.1:** wizard only — does not wipe disks

### Not packaged yet

| Tree | Notes |
| :-: | :-: |
| `spike-migration/` | Scaffold — desktop file present, `NoDisplay=true` |
| `spike-common/` | Built into rescue + installer today; may become its own `.deb` later |

## After packaging

1. Rebuild ISO (`04-building-spike.md`) **or** for quick iteration on an existing stick, install the `.deb` inside a running live session only if you understand overlay limitations (prefer ISO rebuild for rescue helper/sudoers changes).
2. Update `docs/agent-ops/STATE.md` with the new version when you ship a smoke-worthy bump.
3. For Rescue/Shell UI changes, note the smoke checklist in `01-getting-started.md`.
4. **Spike APT repo** (when package host is online) — see `docs/UPDATES.md`:

```
./scripts/generate-spike-apt-key.sh   # once
./scripts/publish-spike-apt-repo.sh   # → build/apt-repo/
# upload build/apt-repo/ to https://packages.bigrangatech.com/spike
# set Enabled: yes in spike.sources (or SPIKE_APT_ENABLE=1)
```

## Related

- Host deps: `03-build-environment.md`
- Full ISO: `04-building-spike.md`
- Updates model: `../UPDATES.md`
- Rescue internals: `08-rescue-tool-internals.md`
