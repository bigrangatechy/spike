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
sudo apt install live-build debootstrap squashfs-tools xorriso isolinux syslinux-common syslinux-utils \
  grub-pc-bin grub-efi-amd64-bin grub-efi-amd64-signed shim-signed mtools dosfstools rsync ca-certificates gnupg \
  build-essential cmake qt6-base-dev liblayershellqtinterface-dev
```

Exact package names can vary slightly by Ubuntu series; if `live-build` pulls additional recommends, keep them. Document any host-specific extras in `SESSION_LOG.md` when discovered.

`gnupg` must also land **inside** the bootstrap chroot. Spike sets `LB_BOOTSTRAP_INCLUDE=apt-utils,ca-certificates,gnupg` in `auto/config` (Ubuntu live-build 3.0 has no `--bootstrap-include` flag). Without it, apt archive setup fails with `env: 'gpg': No such file or directory` and no ISO is produced.

Host packages `cmake`, `qt6-base-dev`, and `liblayershellqtinterface-dev` are required to build `spike-shell` / `spike-rescue` `.deb`s during `build-iso.sh`.

Optional later:

```
├── qemu-system-x86 / virt-manager  → smoke-boot the ISO without USB
├── apt-cacher-ng or similar       → faster repeated builds
└── git, build-essential           → building Spike .debs before injection
```

## Repository paths

```
spike/
├── build/iso-build/              → live-build recipe (see README there)
├── build/packages/               → built Spike .debs (gitignored binaries)
├── scripts/build-iso.sh          → wrapper (packages local .debs, runs lb)
├── scripts/package-spike-config.sh → builds spike-config_*.deb
├── scripts/package-spike-shell.sh  → builds spike-shell_*.deb
├── scripts/package-spike-rescue.sh → builds spike-rescue_*.deb
└── docs/dev-guide/
    ├── 03-build-environment.md  → this file
    ├── 04-building-spike.md     → how to run a build
    └── 05-building-components.md → per-package .deb builds
```

## Verification

Before claiming the environment is ready:

1. `lb --version` (or `live-build --help`) succeeds.  
2. `debootstrap --version` succeeds.  
3. Enough free disk on the filesystem that will hold `build/iso-build/`.  
4. You can run `./scripts/build-iso.sh --check-deps` once that stub supports it.

## Out of scope

- Producing a finished ISO (see `04-building-spike.md`; recipe is scaffolded until Phase 3 fills it).  
- Installer, shell, or rescue development (see `07-installer-internals.md`, `06-spike-shell-architecture.md`, `08-rescue-tool-internals.md`).  
- CI wiring (see future `17-release-process.md` / GitLab CI stubs).
