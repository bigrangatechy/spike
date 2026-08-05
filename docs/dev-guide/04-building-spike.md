# Spike Dev Guide — Building Spike (ISO)

## Purpose

How Spike’s **single** hybrid live ISO is built with **live-build**. This is the build cookbook entry; it does not replace `ARCHITECTURE.md`, `INSTALLER.md`, or `VARIANT-DIFFERENCES.md`.

## Locked rules

1. **Engine:** live-build (`build/iso-build/`), `--mode ubuntu`, distribution **resolute** (26.04).  
2. **One ISO:** Do not build separate Standard and Plus ISOs. There is no `./scripts/build-iso.sh --variant standard|plus`.  
3. **Variant at install:** Standard vs Plus is applied by the custom installer + `spike-config` after copying the system (see `INSTALLER.md`, `VARIANT-DIFFERENCES.md`).  
4. **Installer binary:** Custom Qt `spike-installer` ships *on* the ISO later; live-build does not use Calamares.  
5. **Installed disk FS:** **ext4**. **squashfs** is only the compressed root on the ISO/live media.

## High-level flow

```
Host (Ubuntu 26.04 recommended; deps from 03-build-environment.md)
        │
        ▼
sudo ./scripts/build-iso.sh
        │
        ├── lb clean
        ├── auto/config  → lb config (resolute, iso-hybrid, casper)
        ├── lb build
        │
        ▼
Hybrid live ISO (*.iso in build/iso-build/)
        │
        ├── Stage 1: boot → live shell / casper environment
        ├── Later: Spike Shell + "Install Spike" + Rescue
        └── Installer unpacks squashfs → ext4 on target disk
```

## Recipe layout

```
build/iso-build/
├── .recipe-ready                         → allows build-iso.sh to run lb build
├── README.md
├── auto/
│   ├── config                            → lb config flags
│   ├── clean
│   └── build
└── config/
    ├── package-lists/spike-live.list.chroot
    ├── hooks/0500-spike-strip-telemetry.chroot
    └── includes.chroot/etc/hostname
```

## Wrapper script

```
sudo ./scripts/build-iso.sh --check-deps
sudo ./scripts/build-iso.sh --config-only
sudo ./scripts/build-iso.sh              # clean + config + build
sudo ./scripts/build-iso.sh --clean-only
./scripts/build-iso.sh --help
```

Must run as **root** for chroot and loop devices (`sudo`).

Do **not** add Standard/Plus ISO variants to this script.

## If the build looks stuck

Debootstrap shows `I: Retrieving <package>` with no progress bar. A slow/stalled mirror (especially `archive.ubuntu.com` from AU) can sit on one `.deb` for a long time. Check whether `chroot/var/cache/apt/archives/partial/` is still growing.

Default mirror is **`au.archive.ubuntu.com`**. Full clean and retry (plain `lb clean` is not enough — it keeps `cache/`):

```
sudo ./scripts/build-iso.sh --clean-only
sudo ./scripts/build-iso.sh
tail -f build/iso-build/chroot/debootstrap/debootstrap.log
```

## Stage 1 contents

Current recipe aims for a **bootable stripped live image**:

```
├── Ubuntu 26.04 bootstrap (Server-oriented archive)
├── casper live boot + linux-generic
├── NetworkManager + basic admin tools
├── Strip hook: snapd / cloud-init / telemetry packages if present
└── Not yet: Spike Shell, installer, Flatpak seed, full branding
```

Full product ISO contents (shell, Flatpak, firmware set) land in later stages; see `ARCHITECTURE.md` for the long-term size sketch.

## Smoke-test with QEMU

After a successful `sudo ./scripts/build-iso.sh`, find the ISO in `build/iso-build/` (name varies by live-build; often `*.hybrid.iso` or `live-image-amd64.hybrid.iso`).

```
# Example — adjust ISO filename to match build output
ISO=build/iso-build/live-image-amd64.hybrid.iso

qemu-system-x86_64 \
  -enable-kvm \
  -m 2048 \
  -smp 2 \
  -cdrom "$ISO" \
  -boot d \
  -display gtk
```

Without KVM (slower):

```
qemu-system-x86_64 -m 2048 -smp 2 -cdrom "$ISO" -boot d -display gtk
```

**Stage 1 pass criteria:** ISO boots to a live environment (casper). A graphical Spike Shell is **not** required yet.

Install QEMU on the host if needed: `sudo apt install qemu-system-x86`.

## Relation to install

The installer should prefer **unpacking the live squashfs** onto the target **ext4** filesystem over a full network debootstrap (`INSTALLER.md`). live-build’s job is to produce that squashfs + bootable wrapper.

## References

| Topic | Document |
| :-: | :-: |
| Host setup | `03-build-environment.md` |
| Installer / variant application | `07-installer-internals.md`, `INSTALLER.md` |
| Variant matrix | `VARIANT-DIFFERENCES.md` |
| ISO size / contents sketch | `ARCHITECTURE.md` |
| Operational decision | `docs/agent-ops/DECISIONS.md` |
