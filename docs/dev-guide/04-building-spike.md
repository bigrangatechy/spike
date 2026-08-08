# Spike Dev Guide — Building Spike (ISO)

## Purpose

How Spike’s **single** hybrid live ISO is built with **live-build**. This is the build cookbook entry; it does not replace `ARCHITECTURE.md`, `INSTALLER.md`, or `VARIANT-DIFFERENCES.md`.

## Locked rules

1. **Engine:** live-build (`build/iso-build/`), `--mode ubuntu`, distribution **resolute** (26.04).  
2. **One ISO:** Do not build separate Standard and Plus ISOs. There is no `./scripts/build-iso.sh --variant standard|plus`.  
3. **Variant at install:** Standard vs Plus is applied by the custom installer + `spike-config` after copying the system (see `INSTALLER.md`, `VARIANT-DIFFERENCES.md`).  
4. **Installer binary:** Custom Qt `spike-installer` **0.0.1** ships on the ISO (wizard UI; wipe/copy not enabled yet). live-build does not use Calamares.  
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
        ├── Spike Shell + Install Spike + Rescue My Files
        └── Installer (when engines land) unpacks squashfs → ext4 on target disk
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

## Stage 1–2 contents

Current recipe aims for a **bootable stripped live image** plus `spike-config`:

```
├── Ubuntu 26.04 bootstrap (Server-oriented archive)
├── casper live boot + linux-generic
├── NetworkManager + basic admin tools
├── Strip hook: snapd / cloud-init / telemetry packages if present
├── Local package: spike-config (.deb via packages.chroot)
└── Not yet: Spike Shell, installer GUI, Flatpak seed, full branding
```

Full product ISO contents (shell, Flatpak, firmware set) land in later stages; see `ARCHITECTURE.md` for the long-term size sketch.

### Local Spike packages

```
./scripts/package-spike-config.sh          # → build/packages/*.deb
sudo ./scripts/build-iso.sh                # stages deb via includes.chroot + dpkg -i hook
```

`build-iso.sh` rebuilds `spike-config` and copies it to `config/includes.chroot/var/cache/spike-local/`. It deliberately does **not** use `packages.chroot/` (that path makes live-build run `gpg` in the bootstrap chroot and fails).

## Smoke-test (prefer real hardware)

Only keep/use **`build/iso-build/spike-live.iso`**.

### Research notes (why USB may not appear)

Compared to **Kubuntu 26.04** (local reference ISO), Spike’s hybrid GPT layout is now the same shape (protective MBR + ISO9660 + trailing EFI System partition + El Torito BIOS/UEFI). Remaining differences that matter on real laptops:

1. **Secure Boot chain** — Kubuntu ships **shim → grubx64** (Canonical-signed). Unsigned `BOOTX64.EFI` (raw GRUB) is often rejected; some firmwares hide the stick entirely. Spike now uses the host’s `shim-signed` + `grub-efi-amd64-signed` like Ubuntu.
2. **Pre-alpha:** Secure Boot may be **disabled** in firmware for testing (see `agent-ops/DECISIONS.md`). Shim is still embedded when available.
3. **Writer method** — Prefer **raw `dd`**, or the FAT32 extract method below. GUI writers sometimes rewrite partitions and drop the hybrid GPT/ESP.
4. **Control test** — On the **same laptop + same USB**, write Kubuntu with the same method. If Kubuntu appears and Spike does not, it’s still an image issue. If **neither** appears, it’s firmware/USB-boot settings (or a dead stick/port).

### Method A — KDE ISO Image Writer (same as Kubuntu)

Use **`build/iso-build/spike-live.iso`** only. The remaster now matches Kubuntu’s hybrid flags (appended EFI partition + shim), so the same writer workflow should apply.

### Method B — FAT32 extract (most compatible for picky UEFI)

Single FAT32 partition with `\EFI\BOOT\BOOTX64.EFI` — what many firmwares look for on removable media:

```
sudo ./scripts/spike-usb-fat32.sh /dev/sdX build/iso-build/spike-live.iso
```

Boot tips: firmware boot menu → **UEFI USB**; **pre-alpha: disable Secure Boot if the stick is rejected**; try a USB 2.0 port.

GRUB entries on the remastered ISO:

| Entry | Notes |
| :-: | :-: |
| Spike Live | Normal (`quiet splash`) |
| Spike Live (debug logging) | No splash; kernel `debug`; auto-runs slim `spike-capture-logs` onto USB `writable` |
| Spike Live (safe graphics) | `nomodeset` |

Debug capture (Stage 3) keeps **spike-config**, **shell/session/seat**, filtered journal (`kwin`/`spike-shell`/`libinput`/…), and `$XDG_RUNTIME_DIR/spike-session.log` when present. It does **not** dump full syslog/dmesg/udev.

To log **spike-session startup** (finalize optional — hard power-off is OK):

```
# Live boots: autologin as **spike** on tty1 and starts spike-session.
# Optional debug capture before/during session (Ctrl+Alt+F2):
sudo spike-capture-logs --follow
# Manual session if needed (as spike, not sudo):
spike-session
# if input dies: power off; stick keeps journal-follow + spike-session-latest.log
# optional if you can switch VT: sudo spike-capture-logs --finalize
```

`spike-session` refuses root and requires a logind seat. It tees a line-buffered log to USB `spike-session-latest.log`.

Then on the build host with the stick remounted:

```
sudo ./scripts/spike-collect-usb-logs.sh
# → build/iso-build/debug-logs/spike-capture-*
```

Remaster ISO only: `./scripts/spike-iso-hybridize.sh`

That remaster **syncs the casper UUID** to the ISO9660 volume date (`blkid` UUID == `.disk/casper-uuid*` == initrd `conf/uuid.conf`). Without that, casper can drop to `initramfs$` (“Unable to find a medium…”) after hybridize. Override the date with `SOURCE_DATE_EPOCH` for reproducible builds.

### Optional — QEMU

```
ISO=build/iso-build/spike-live.iso
qemu-system-x86_64 -enable-kvm -m 2048 -smp 2 -cdrom "$ISO" -boot d -display gtk
```

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
