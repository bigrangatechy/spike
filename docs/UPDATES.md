# Spike Updates

How Spike receives software updates after install. Spec alignment:
`BOOT-PROCESS.md`, `DESIGN-DECISIONS.md`, `PRIVACY.md`.

## Model

| Channel | What | How |
| --- | --- | --- |
| Ubuntu mirrors | Base OS + security | `apt` / `unattended-upgrades` (Settings → Updates) |
| Spike APT | `spike-shell`, `spike-config`, `spike-rescue`, `spike-installer`, `spike-migration` | Signed repo at `https://packages.bigrangatech.com/spike` |
| Mozilla APT | Firefox / Thunderbird `.deb`s | `packages.mozilla.org` (already on ISO) |
| Flathub | Optional Flatpak apps | Discover |

There is **no** custom OTA client. Discover + the panel Update Notifier + `apt` are the UI.

Spike component upgrades are **not** silent: `unattended-upgrades` blacklists `spike-*` so shell/session packages install only when the user upgrades via Discover/`apt`.

## Spike APT (operator)

### One-time key

```bash
./scripts/generate-spike-apt-key.sh
# Public:  build/iso-build/config/spike-archives/spike.key  (commit)
# Secret:  build/apt-signing/secret.gpg                   (gitignored)
```

### Publish a tree

```bash
./scripts/package-spike-shell.sh
./scripts/package-spike-config.sh
# …rescue / installer / migration as needed
./scripts/publish-spike-apt-repo.sh
# → build/apt-repo/  (dists/ + pool/ + InRelease)
```

Upload `build/apt-repo/` to the HTTPS document root so clients can fetch:

`https://packages.bigrangatech.com/spike/dists/spike/InRelease`

### Enable on images (when the host answers)

1. Set `Enabled: yes` in [`spike.sources`](../build/iso-build/config/includes.chroot/etc/apt/sources.list.d/spike.sources).
2. Optional live-build: `SPIKE_APT_ENABLE=1 sudo ./scripts/build-iso.sh` (stages into `config/archives/`).
3. Installs: `SPIKE_APT_ENABLE=1` for the helper, or edit `spike.sources` on the target.

Until the host is online, keep **`Enabled: no`** so `apt update` does not fail on a dead mirror.

## User Settings

**Settings → Updates → Automatically install security updates** writes:

- `/etc/apt/apt.conf.d/20auto-upgrades`
- `/etc/apt/apt.conf.d/51spike-unattended-upgrades` (Ubuntu `-security` origins; Spike packages blacklisted)

Requires passwordless `sudo -n` (same pattern as other Spike Settings Apply actions).

## Privacy

`PRIVACY.md` already allows apt package-list refresh to Ubuntu (and by extension configured APT hosts). Spike APT only serves Spike component `.deb`s — no telemetry.
