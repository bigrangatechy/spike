# Spike Dev Guide — Building Spike (ISO)

## Purpose

How Spike’s **single** hybrid live ISO is meant to be built with **live-build**. This is the build cookbook entry; it does not replace `ARCHITECTURE.md`, `INSTALLER.md`, or `VARIANT-DIFFERENCES.md`.

## Locked rules

1. **Engine:** live-build (`build/iso-build/`).  
2. **One ISO:** Do not build separate Standard and Plus ISOs. There is no `./scripts/build-iso.sh --variant standard|plus`.  
3. **Variant at install:** Standard vs Plus is applied by the custom installer + `spike-config` after copying the system (see `INSTALLER.md`, `VARIANT-DIFFERENCES.md`).  
4. **Installer binary:** Custom Qt `spike-installer` ships *on* the ISO; live-build does not use Calamares.

## High-level flow

```
Host (deps from 03-build-environment.md)
        │
        ▼
./scripts/build-iso.sh
        │
        ├── lb clean / lb config / lb build  (inside build/iso-build/)
        │
        ▼
Hybrid live ISO
        │
        ├── Boot → live Spike Shell (read-only)
        ├── "Install Spike" → spike-installer
        └── Spike Rescue → rescue tool
```

## Recipe layout

```
build/iso-build/
├── README.md
├── auto/                 → live-build auto config helpers
└── config/
    ├── package-lists/    → what gets into the squashfs
    ├── hooks/            → chroot/binary hooks (strip snap/telemetry, seed Flatpak, etc.)
    └── includes.*/       → files overlaid into the image (branding, spike packages)
```

Fill package lists and hooks as packages exist. Until then the tree is a **scaffold** — `build-iso.sh` may exit with a clear “recipe incomplete” message rather than producing a broken ISO.

## Wrapper script

```
./scripts/build-iso.sh           # attempt full build when recipe is ready
./scripts/build-iso.sh --check-deps
./scripts/build-iso.sh --help
```

Expected behavior (stub today, real later):

```
├── Verify host tools (live-build, debootstrap, …)
├── cd to build/iso-build/
├── lb clean (as appropriate)
├── lb config (from auto/ + config/)
├── lb build
└── Report path to the resulting ISO under build/iso-build/ (or a documented output dir)
```

Do **not** add Standard/Plus ISO variants to this script.

## What goes on the ISO (summary)

From `ARCHITECTURE.md` (sizes approximate):

```
├── Ubuntu Server **26.04 LTS** base (Snap/telemetry stripped in hooks)
├── KDE standalone apps + Spike Shell + spike-config + spike-installer
├── Flatpak runtimes pre-seeded for offline-friendly first apps
├── Firmware / VA-API / networking bits per HARDWARE.md / NETWORKING.md
├── Branding, Plymouth, GRUB theme
└── Live session config (autologin or live user as decided in BOOT-PROCESS.md)
```

Plus vs Standard differences that are **config** (governor, animations, ZRAM caps, etc.) are **not** separate package sets at build time unless a package is required on disk for both and merely enabled later. Prefer one squashfs; enable Plus behavior at install.

## Relation to install

The installer should prefer **unpacking the live squashfs** (or equivalent image contents) onto the target disk over a full network debootstrap, for speed and offline installs (`INSTALLER.md`). live-build’s job is to produce that squashfs + bootable wrapper.

## References

| Topic | Document |
| :-: | :-: |
| Host setup | `03-build-environment.md` |
| Installer / variant application | `07-installer-internals.md`, `INSTALLER.md` |
| Variant matrix | `VARIANT-DIFFERENCES.md` |
| ISO size / contents sketch | `ARCHITECTURE.md` |
| Operational decision | `docs/agent-ops/DECISIONS.md` |
