# Spike Dev Guide — Installer Internals

## Purpose

Maps the product spec in `INSTALLER.md` to the intended **custom Qt Widgets** implementation. Explicitly: Spike does **not** use Calamares.

## Locked decisions

| Item | Choice |
| :-: | :-: |
| Installer | Custom Qt Widgets — `spike-installer` |
| Not used | Calamares |
| ISO | One live-build hybrid ISO; installer runs from the live session |
| Variant | Chosen/recommended at install time; applied via `spike-config` |

## Live → install handoff

```
Boot Spike ISO (live-build image)
        │
        ▼
Live Spike Shell (read-only root)
        │
        ├── Desktop affordance: "Install Spike"
        │         │
        │         ▼
        │   spike-installer (Qt Widgets wizard, 10 steps)
        │
        └── Spike Rescue (separate app: recover + restore; see DISASTER-RECOVERY.md,
            SPIKE-RECOVERY-TOOL-GENERAL.md, dev-guide/08-rescue-tool-internals.md)
```

The live session is for recovery and install only — not a permanent “try Spike” desktop product (see installer / desktop specs for live user constraints).

## Module map (from INSTALLER.md)

```
spike-installer/
├── ui/              → Qt Widgets screens (welcome … reboot)
├── detect/          → Hardware detection + SpikeBackup USB scan
├── partition/       → Automatic partitioning / wipe confirmation
├── backup/          → Step 7 optional USB backup before wipe
└── restore/         → Layer 4 post-reinstall restore into /home/<user>
```

**Status (0.0.4):** Qt wizard + privileged `spike-install-helper` can partition,
unsquashfs, configure account/hostname/variant, install GRUB (with `grub.cfg`
guaranteed — `update-grub` or minimal fallback), optional Layer 4 restore.
Wi‑Fi step uses nmcli (scan/connect/skip). Storage step requires typing **ERASE**.
Step 7 backup copy still stubbed. Alpha gate remains installer E2E smoke (`STATE.md`).

**GRUB note:** Live squashfs ships `grub-*-bin` without metapackages, so
`/etc/default/grub` and `/boot/grub` are often missing. Helper creates them,
runs `update-grub` (logged), and writes a minimal `grub.cfg` if that fails.
UEFI also installs the removable `EFI/BOOT` path.
Package: `./scripts/package-spike-installer.sh` → Desktop **Install Spike**.
Live Desktop also gets **Rescue My Files** and **Move My Files** (migration launcher).

### SpikeBackup (shared with Rescue / Migration)

Installer backup and restore **must** use the same layout as Spike Rescue:

```
SpikeBackup/<utc-stamp>/<os-label>/home/<user>/…
```

Implementation should link `src/spike-common/SpikeBackupLayout.*` (header also installed by the rescue package). Do not send the user out to Rescue for Step 7 or Layer 4 — those flows stay inside the installer wizard.

Rescue remains the live disaster tool (recover + restore). Migration (`spike-migration`) is the detailed new-user wizard. Map: `SPIKE-RECOVERY-TOOL-GENERAL.md`.

Authoritative UX and step list: **`INSTALLER.md`**. Do not invent extra installer questions (no dual boot, no encryption, no manual partitioner).

## System copy and variant application

Typical install path (implementation detail may evolve; behavior must match the spec):

1. Partition and format per `INSTALLER.md` / `HARDWARE.md`.  
2. Unpack live squashfs (or copy rootfs) onto the target — prefer this over network debootstrap for offline/speed.  
3. Configure users, hostname, locale, firmware selection from detection.  
4. **Apply variant:** write state / call `spike-config` so Standard or Plus differences take effect (ZRAM caps, governor, animations, Plymouth theme selection, etc.). Same packages on disk; Plus **enables** options.  
5. Install bootloader, finish, reboot.

### Privileged helper (**0.0.3**)

`InstallEngine` runs:

```
sudo -n /usr/lib/spike/spike-install-helper install-all \
  --disk … --user … --hostname … --confirm ERASE …
```

(CLI args — sudo `env_reset` strips `SPIKE_INSTALL_*`.) After copy, configure writes `/etc/spike/installed`, tty1 autologin for the new user, and a profile.d that starts `spike-session` without requiring `boot=casper`. Live-only installer sudoers and Desktop “Install Spike” icons are removed from the target.

Variant differences are enumerated in **`VARIANT-DIFFERENCES.md`** (14 items). If a difference cannot be expressed as config, escalate via docs-first change — do not silently split the ISO.

## Relation to live-build

live-build is responsible for:

```
├── Producing the live environment and squashfs
├── Including spike-installer, spike-shell, spike-config packages/files
└── Boot branding (GRUB/Plymouth as configured)
```

live-build is **not** responsible for:

```
├── The 10-step wizard UI
├── Calamares modules or Calamares branding
└── Building two ISOs for Standard vs Plus
```

See `04-building-spike.md` and `build/iso-build/README.md`.

## Testing notes (early)

- Run installer UI on a development host against mock detect backends when possible.  
- Full path requires a built ISO or chroot with squashfs — arrive after `build-iso.sh` produces images.  
- Hardware detection must match `HARDWARE.md` classifications.

## References

| Topic | Document |
| :-: | :-: |
| Full installer UX | `INSTALLER.md` |
| Variants | `VARIANT-DIFFERENCES.md` |
| Rescue | `DISASTER-RECOVERY.md`, `08-rescue-tool-internals.md` |
| SpikeBackup layout | `src/spike-common/`, `SPIKE-RECOVERY-TOOL-GENERAL.md` |
| Config engine | `CONFIGURATION.md` |
| ISO build | `04-building-spike.md` |
