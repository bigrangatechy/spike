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

**Status (0.0.12):** Qt wizard + privileged `spike-install-helper` can partition,
unsquashfs, configure account/hostname/variant, set **locale + timezone + keyboard**,
**copy NetworkManager Wi‑Fi/Ethernet connections** from the live session into the
installed system, write **`/var/lib/spike/first-boot`** + `installer-notifications/` for the shell first-run
wizard, run **spike-config --detect** (KERNEL.md module blacklist), regenerate a **local**
(non-casper) initramfs, enforce DRM/seat groups + enable `seatd`, Step 7 backup with
**async** system scan, SKIPPED honesty / `--exclude-disk`, Layer 4 with `RESTORE_STATUS=` on
Finish. Wi‑Fi step uses nmcli (system-wide permissions preferred for handoff). Storage requires typing **ERASE**. Installer E2E
gate is **met** (Alpha); remaining work is polish (`STATE.md`).

**Step 7 note:** Entering the backup page used to call `spike-rescue --list-systems` with
`waitForFinished` on the GUI thread while rescue walked personal-file trees — that hung
the Variant→Backup transition. Installer now scans asynchronously; rescue **0.0.12**
`--list-systems` skips the inventory `find-files` walk (OS/users only).

**GRUB / boot note:** Live squashfs ships `grub-*-bin` without metapackages and a
casper-oriented initrd (`BOOT=casper` baked into conf.d). Helper sanitizes
`/etc/default/grub` (strip missing theme font paths), runs `update-initramfs` with
`BOOT=local`, passes `boot=local` on the kernel cmdline, and refuses a `grub.cfg`
that still contains `boot=casper`. UEFI also installs the removable `EFI/BOOT` path.

**Installed session note:** Live `spike-live-groups` is removed from the target (correct),
so the helper must `usermod -aG video,input,render,…` and `systemctl enable seatd` —
otherwise kwin DRM fails and the installed boot looks like a hung black screen.

**Module blacklist note:** `cmd_configure` runs `spike-config --detect` (fills
`security.module_blacklist`) then `--generate-all` before `prepare_local_initramfs`,
so the blacklist is present when initrd is rebuilt.

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
