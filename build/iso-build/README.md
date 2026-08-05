# Spike ISO build recipe (live-build) — Stage 1

## Rule: one ISO

Spike ships **one** hybrid live ISO. Standard vs Plus is applied at **install time** by `spike-installer` + `spike-config`. Do not add parallel Standard/Plus live-build projects here.

## Base

**Ubuntu Server 26.04 LTS** (resolute), stripped per `ARCHITECTURE.md` / privacy-security specs.

## Engine

- **Tool:** live-build (`--mode ubuntu`)
- **Wrapper:** `./scripts/build-iso.sh` from repo root
- **Docs:** `docs/dev-guide/03-build-environment.md`, `docs/dev-guide/04-building-spike.md`

## Layout

```
iso-build/
├── README.md
├── .recipe-ready              → present when build-iso.sh may run lb build
├── auto/
│   ├── config                 → lb config (resolute, iso-hybrid, casper)
│   ├── clean
│   └── build
└── config/
    ├── package-lists/
    │   └── spike-live.list.chroot
    ├── hooks/
    │   └── 0500-spike-strip-telemetry.chroot
    └── includes.chroot/
        └── etc/hostname
```

## Build

```
# One-time host deps (interactive sudo on your machine)
sudo apt install live-build debootstrap squashfs-tools xorriso isolinux syslinux-common \
  grub-pc-bin grub-efi-amd64-bin mtools dosfstools rsync ca-certificates

./scripts/build-iso.sh --check-deps
sudo ./scripts/build-iso.sh
```

Output ISO name depends on live-build version (often `live-image-amd64.hybrid.iso` or similar) in this directory after a successful build. See `build.log` if present.

### If the build looks “stuck”

Debootstrap prints `I: Retrieving <package>` with **no progress bar**. A slow or stalled mirror can sit on one package for a long time.

Signs of a stall (not progress):
- Same `I: Retrieving …` line for many minutes
- A file under `chroot/var/cache/apt/archives/partial/` that stops growing

This recipe defaults to **`http://au.archive.ubuntu.com/ubuntu/`** (faster from Australia than `archive.ubuntu.com`). Override if needed:

```
SPIKE_UBUNTU_MIRROR=http://archive.ubuntu.com/ubuntu/ sudo ./scripts/build-iso.sh
```

Clean a failed/partial bootstrap, then rebuild:

```
cd ~/Documents/Gitlab/spike
sudo ./scripts/build-iso.sh --clean-only   # full wipe: chroot, .build, cache, logs
sudo ./scripts/build-iso.sh
```

Note: plain `lb clean` (without our wrapper) only removes some stages and **keeps** `cache/` — that can make the next build restore a broken bootstrap. Always use `./scripts/build-iso.sh --clean-only`.

Watch live progress in another terminal:

```
tail -f build/iso-build/chroot/debootstrap/debootstrap.log
# or
watch -n2 'ls -la build/iso-build/chroot/var/cache/apt/archives/partial/ | tail'
```

### Verification checklist

1. `./scripts/build-iso.sh --check-deps` — all tools `ok`  
2. `sudo ./scripts/build-iso.sh` — completes without error; `*.iso` appears here  
3. QEMU boot — see `docs/dev-guide/04-building-spike.md`  

Note: the Cursor agent cannot enter your sudo password; run steps 2–3 locally.

## Stage 1 status

Recipe is marked ready for a first bootable stripped live image. Desktop shell, installer, and Flatpak pre-seed are **out of scope** until later stages.
