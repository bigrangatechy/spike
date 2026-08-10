# Package lists

- `spike-live.list.chroot` — Stage 1 live + installed-system squashfs packages
  (firmware, desktop runtime, default apps, recovery tools).

## Policy: ISO ships the full OS surface

Spike’s ISO **is** the OS image (live and post-install squashfs). Hardware
detection / modules only help if the matching firmware, daemons, and CLI
helpers are **already on the image**.

Rules:

1. **If Spike Shell, installer, rescue, or detect paths need a package, list it
   here.** Do not rely on `.deb` `Recommends:` alone — apt/live-build often
   omit Recommends.
2. **Firmware for Tier‑1 Wi‑Fi/BT/GPU stays on the ISO** (split
   `linux-firmware-*` packages + `linux-firmware-misc`). Users may have no
   Ethernet to download firmware later (`NETWORKING.md`).
3. **Verify in** `hooks/0720-spike-verify-includes.chroot` — fail the build if
   critical packages or binaries are missing.
4. Do **not** add `plasma-desktop` / `plasma-workspace` (fights Spike Shell).
5. Do not split Standard/Plus into separate list files for separate ISOs.

Mozilla APT for Firefox/Thunderbird: see `config/spike-archives/` (not Ubuntu
Snap stubs).
