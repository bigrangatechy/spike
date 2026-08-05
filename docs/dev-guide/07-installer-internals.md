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
        └── Spike Rescue (separate entry; see DISASTER-RECOVERY.md)
```

The live session is for recovery and install only — not a permanent “try Spike” desktop product (see installer / desktop specs for live user constraints).

## Module map (from INSTALLER.md)

```
spike-installer/
├── ui/              → Qt Widgets screens (welcome … reboot)
├── detect/          → Hardware detection
│   ├── cpu.cpp
│   ├── storage.cpp
│   ├── gpu.cpp
│   ├── network.cpp
│   ├── bluetooth.cpp
│   └── modem.cpp
├── partition/       → Automatic partitioning / wipe confirmation
├── backup/          → Optional USB backup before wipe
└── restore/         → Post-reinstall data restore
```

Authoritative UX and step list: **`INSTALLER.md`**. Do not invent extra installer questions (no dual boot, no encryption, no manual partitioner).

## System copy and variant application

Typical install path (implementation detail may evolve; behavior must match the spec):

1. Partition and format per `INSTALLER.md` / `HARDWARE.md`.  
2. Unpack live squashfs (or copy rootfs) onto the target — prefer this over network debootstrap for offline/speed.  
3. Configure users, hostname, locale, firmware selection from detection.  
4. **Apply variant:** write state / call `spike-config` so Standard or Plus differences take effect (ZRAM caps, governor, animations, Plymouth theme selection, etc.). Same packages on disk; Plus **enables** options.  
5. Install bootloader, finish, reboot.

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
| Rescue | `DISASTER-RECOVERY.md` |
| Config engine | `CONFIGURATION.md` |
| ISO build | `04-building-spike.md` |
