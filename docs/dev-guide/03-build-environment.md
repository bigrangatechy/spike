# Spike Dev Guide — Build Environment

## Purpose

What you need on a build host before producing a Spike ISO with **live-build**. Product specs remain in the top-level docs; this page is only host setup.

## Decisions (locked)

| Item | Choice |
| :-: | :-: |
| ISO engine | **live-build** |
| Artifacts | **One** hybrid live ISO |
| Installer | Custom Qt (`spike-installer`) — not part of this page |

See `docs/agent-ops/DECISIONS.md` and `04-building-spike.md`.

## Recommended host

- **OS:** Prefer **Ubuntu 26.04 LTS** on the build host (same series as Spike’s base — see `DESIGN-DECISIONS.md`). Building on 26.04 reduces archive skew.
- **Architecture:** x86_64 only (Spike is x86_64-only).
- **Privileges:** live-build typically needs root (or equivalent) for chroot and loop mounts. Prefer a dedicated build machine or CI runner — not a daily laptop if you can avoid it.
- **Disk:** Plan for **tens of GB** free (bootstrap cache, chroots, squashfs, ISO). Exact numbers grow with Flatpak pre-seeds (~3GB ISO target in `ARCHITECTURE.md` implies larger work trees).
- **Network:** Stable access to Ubuntu archives (and later any Spike package repo).

## Packages (minimum)

Install at least:

```
sudo apt update
sudo apt install live-build debootstrap squashfs-tools xorriso isolinux syslinux-common \
  grub-pc-bin grub-efi-amd64-bin mtools dosfstools rsync ca-certificates
```

Exact package names can vary slightly by Ubuntu series; if `live-build` pulls additional recommends, keep them. Document any host-specific extras in `SESSION_LOG.md` when discovered.

Optional later:

```
├── qemu-system-x86 / virt-manager  → smoke-boot the ISO without USB
├── apt-cacher-ng or similar       → faster repeated builds
└── git, build-essential           → building Spike .debs before injection
```

## Repository paths

```
spike/
├── build/iso-build/     → live-build recipe (see README there)
├── scripts/build-iso.sh → wrapper (checks deps, runs lb)
└── docs/dev-guide/
    ├── 03-build-environment.md  → this file
    └── 04-building-spike.md     → how to run a build
```

## Verification

Before claiming the environment is ready:

1. `lb --version` (or `live-build --help`) succeeds.  
2. `debootstrap --version` succeeds.  
3. Enough free disk on the filesystem that will hold `build/iso-build/`.  
4. You can run `./scripts/build-iso.sh --check-deps` once that stub supports it.

## Out of scope

- Producing a finished ISO (see `04-building-spike.md`; recipe is scaffolded until Phase 3 fills it).  
- Installer or shell development (see `07-installer-internals.md`, `DESKTOP.md`).  
- CI wiring (see future `17-release-process.md` / GitLab CI stubs).
