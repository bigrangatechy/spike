# Spike Agent Ops — Session Log

Append-only. Newest sessions at the **top**.

---

## 2026-08-06 — Stage 2 spike-config skeleton

**Done:** Created `src/spike-config/` — Python engine with state store, `{{var}}` templates, modules (memory, boot, network, multimedia, security/blacklist), developer CLI, and unit tests (`SPIKE_ROOT` sandbox). Spec: `CONFIGURATION.md`. Still pre-alpha; not packaged into the live ISO yet.

**Try:** `cd src/spike-config && python3 -m unittest tests.test_spike_config -v`

**Next:** deepen templates (UFW/sudo), real detect, Debian package + live-build install.

---

## 2026-08-06 — Stage 2 under pre-alpha (Alpha not opened)

**Decision:** Stage 2 (`spike-config`) proceeds under the **pre-alpha** label. Phase 2 (Alpha) in `ROADMAP.md` stays future until BDFL opens it.

**Why:** Avoid treating the first post-ISO slice as an alpha phase start; pre-alpha already covers early implementation.

---

## 2026-08-06 — Boot tagline under emblem

**Decision:** Boot splash (Plymouth Standard/Plus) and GRUB show tagline **"Let's make tech repairable again"** centered under the emblem (`tagline.png`, color `#a0a0b8`). Spec updated in `docs/BRANDING.md`; assets under `src/spike-branding/{plymouth,grub-theme}/`.

**Why:** Brand motto on every boot; baked PNG so GRUB does not need a custom `.pf2` font for the line.

---

## 2026-08-06 — Tier 1 firmware boot: hardware detect + login OK

**Finding:** Rebuilt ISO with Tier 1 firmware (~853 MB `spike-live.iso`). On test laptop: devices detected, login succeeded. Stage 1 live path validated end-to-end.

---

## 2026-08-06 — Tier 1 firmware set for live ISO

**Decision:** Stage 1 package list gains laptop-class split firmware (not full `linux-firmware` meta): Intel wireless/graphics, SOF, Realtek, Qualcomm/Atheros, Broadcom, MediaTek, Marvell wireless, AMD graphics. Datacenter blobs stay out.

**Why:** Hardware boot showed missing iwlwifi / i915 DMC / SOF on the test Gemini Lake laptop; product docs require Wi-Fi firmware on the ISO.

**Next:** `sudo ./scripts/build-iso.sh` then rewrite USB; expect ~100–200MB larger ISO.

---

## 2026-08-06 — Stage 1 live boot succeeded (hardware)

**Finding:** USB `writable/install-logs-2026-08-05.0` shows hostname **spike-live**, squashfs/overlay up, **getty@tty1** started, multi-user + graphical targets reached. Casper even created a log partition on free USB space.

**Noise (not boot-blockers):** missing Wi‑Fi/i915-DMC/SOF firmware (`linux-firmware-minimal`), casper-md5check (no md5 file), DNS/apt during live hooks, GPT size mismatch on oversized stick (harmless).

**User-facing:** likely looked “failed” (quiet splash / no desktop / firmware msgs). Stage 1 pass is text login, not Spike Shell.

**Logs copied to:** `build/iso-build/debug-logs/install-logs-2026-08-05.0/`

---

## 2026-08-06 — GRUB console only (no menu)

**Finding:** Shim reached GRUB, but menu missing. ISO had BIOS modules flat under `/boot/grub` and **zero** `/boot/grub/x86_64-efi/` (Kubuntu has ~300 EFI mods). Signed EFI GRUB could not load config.

**Done:** Remaster installs `x86_64-efi` modules, uses `gcdx64` as `grubx64.efi`, puts `grub.cfg` on the ESP.

**Next:** Rewrite USB with new `spike-live.iso`. At bare `grub>` (old stick), try:
`search -f /casper/filesystem.squashfs` / `set root=$root` / `configfile /boot/grub/grub.cfg`

---

## 2026-08-06 — Root cause: tiny ESP was FAT32, Kubuntu uses FAT12

**Finding:** USB write was fine (SanDisk showed GPT + 6M ESP after KDE writer). Diff vs Kubuntu ESP: **Spike = undersized FAT32** (mkfs cluster warning); **Kubuntu = FAT12**. Likely why firmware never listed the stick.

**Done:** `mkfs.vfat -F 12` for the appended ESP; remastered `spike-live.iso`.

**Next:** Rewrite with KDE ISO Image Writer again.

---

## 2026-08-05 — Align Spike ISO with Kubuntu for KDE ISO Image Writer

**Context:** Jessie writes Kubuntu successfully with KDE ISO Image Writer — writer + laptop USB boot are fine.

**Gap:** Spike El Torito EFI pointed at in-ISO `efi.img`; Kubuntu points at **appended ESP partition** (`--interval:appended_partition_2`).

**Done:** Remaster matches Kubuntu xorriso pattern (shim ESP + appended-partition El Torito + `/boot/grub/i386-pc/eltorito.img`).

**Next:** Rewrite USB with KDE ISO Image Writer using `build/iso-build/spike-live.iso` only.

---

## 2026-08-05 — Research: USB never in boot list

**Compared to Kubuntu 26.04 ISO:** Spike hybrid GPT/El Torito layout already matched; main gap was **EFI chain** (unsigned GRUB vs **shim → grubx64**).

**Also:** many firmwares only reliably list removable media that present a **FAT32** volume with `\EFI\BOOT\BOOTX64.EFI` (Ubuntu iso2usb extract method). Hybrid GPT ISO9660 sticks can be invisible even when structurally valid.

**Done:**
- Remaster now embeds Canonical **shim-signed** + **grub-efi-amd64-signed** (ESP mirrors Kubuntu)
- Added `scripts/spike-usb-fat32.sh` (GPT + single FAT32 + extract)
- Documented control test: same stick/method with Kubuntu ISO

**Next for Jessie:**
1. Control: does Kubuntu appear on that laptop with the same USB/writer?
2. Prefer `sudo ./scripts/spike-usb-fat32.sh /dev/sdX`
3. Or dd the new shim-based `spike-live.iso`

---

## 2026-08-05 — USB still missing from boot list; keep only spike-live.iso

**Finding:** Remastered ISO still not listed after KDE ISO Image Writer. Deleted confusing leftovers (`binary.hybrid.iso`, `.bak`, `spike-stage1-test.iso`). Improved hybrid: **GPT + EFI System partition** + in-ISO `efi.img` El Torito. Prefer **raw `dd`** over GUI writers.

**Artifact:** `build/iso-build/spike-live.iso` only.

---

## 2026-08-05 — USB not in firmware boot menu (BIOS-only ISO)

**Finding:** KDE ISO Image Writer succeeded, but the other laptop showed no USB boot entry. Raw live-build ISO was **El Torito/BIOS only**, with **empty MBR** (classic `isohybrid` cannot patch GRUB). Modern UEFI firmwares ignore it.

**Done:**
- `scripts/spike-iso-hybridize.sh` remasters → `spike-live.iso` with GRUB2 MBR + EFI partition (`BOOTX64.EFI`)
- Wired into `build-iso.sh` after `lb build`
- Ready artifact: `build/iso-build/spike-live.iso` (605 MB)

**Next for Jessie:** rewrite USB with **spike-live.iso**, boot UEFI entry, disable Secure Boot if needed.

---

## 2026-08-05 — Hardware available for Stage 1 smoke-test

**Decision:** Prefer real USB boot on available hardware over QEMU for Stage 1 validation. QEMU remains optional fallback in `04-building-spike.md`.

---

## 2026-08-05 — Stage 1: ISO written but isohybrid missing

**Finding:** `genisoimage` produced **571 MB** `chroot/binary.hybrid.iso`. Post-step failed: `isohybrid: not found`. On Ubuntu, `isohybrid` is in **`syslinux-utils`**, not `syslinux` (live-build Check_package names the wrong package).

**Done:**
- Add `syslinux-utils` to package list
- `build-iso.sh` promotes leftover `chroot/*.iso` to recipe root
- Host dep check includes `isohybrid`

**Salvage (no full rebuild):**
```
sudo apt install syslinux-utils
cd build/iso-build
sudo mv chroot/binary.hybrid.iso ./
sudo isohybrid binary.hybrid.iso
ls -lh binary.hybrid.iso
```

**Or rebuild:** `sudo ./scripts/build-iso.sh`

---

## 2026-08-05 — Stage 1 build failed: obsolete syslinux Ubuntu themes

**Finding:** Past memtest. `lb_binary_syslinux` requires `syslinux-themes-ubuntu-oneiric` + `gfxboot-theme-ubuntu` (Oneiric-era). Not in resolute.

**Done:**
- `--bootloader grub2` + `--grub-splash none`
- Custom `config/binary_grub/grub.cfg` (no missing xbmc.tga splash)
- Keep `grub-pc-bin` / `grub2-common` / `genisoimage` / `syslinux` in the package list so ISO hybrid step can still run `grub-mkimage` + `isohybrid`

**Next:** `sudo ./scripts/build-iso.sh` — look for `*.iso` under `build/iso-build/`.

---

## 2026-08-05 — Stage 1 build failed: lb_binary_memtest vs Ubuntu memtest86+ layout

**Finding:** Chroot/squashfs succeeded. Binary stage died on `cp: cannot stat 'chroot/boot/.bin'`. Ubuntu 26.04 `memtest86+` installs `/boot/mt86+x64` (no `.bin`); live-build still looks for `memtest86+x64.bin`, leaves `_MEMTEST_BIN` empty, and copies `.bin`.

**Done:** `--memtest none` in `auto/config` (Stage 1 does not need a memtest menu entry).

**Next:** re-run `sudo ./scripts/build-iso.sh` (full clean optional; chroot may still be usable). Look for `*.iso` under `build/iso-build/`.

---

## 2026-08-05 — Stage 1 build failed: archive.ubuntu.com timeouts on -updates firmware

**Finding:** No ISO. Chroot apt used `archive.ubuntu.com` for **resolute-updates** (volatile mirror default). IPv6 to that host timed out while fetching `linux-firmware-*` split packages pulled by full `linux-firmware`.

**Done:**
- Point volatile/updates mirrors at `au.archive.ubuntu.com` (same as bootstrap)
- Switch package list to `linux-firmware-minimal` (satisfies kernel Depends without huge firmware set)
- Add chroot apt `Acquire::ForceIPv4 true`

**Next:**
```
sudo ./scripts/build-iso.sh --clean-only
sudo ./scripts/build-iso.sh
```

---

## 2026-08-05 — Stage 1 build failed: missing packages (not a success yet)

**Finding:** No ISO produced. `lb build` died on:
- `wireless-tools` — removed from resolute (use `iw`)
- `nouveau-firmware` — pulled by `--firmware-chroot true` but not in archive

**Done:** Dropped obsolete packages; set `--firmware-chroot false` / `--firmware-binary false` for Stage 1; list `linux-firmware` explicitly instead.

**Next:** BDFL clean + rebuild:
```
sudo ./scripts/build-iso.sh --clean-only
sudo ./scripts/build-iso.sh
```
Success = `*.iso` under `build/iso-build/`.

---

## 2026-08-05 — Stage 1 build stall on dbus-session-bus-common

**Participants:** BDFL + agent

**Finding:** Not a package bug — debootstrap download-only hung mid-fetch of `dbus-session-bus-common` (partial ~49KB of ~54KB) from `archive.ubuntu.com`. AU mirror is substantially faster.

**Done:** Switched `auto/config` default mirrors to `au.archive.ubuntu.com`; documented stall diagnosis + clean/retry in `build/iso-build/README.md` and `04-building-spike.md`.

**Next:** BDFL `sudo ./scripts/build-iso.sh --clean-only` then rebuild.

---

## 2026-08-05 — Stage 1 live-build recipe + build-iso.sh

**Participants:** BDFL + agent (Cursor)

**Done:**
- Filled `build/iso-build/`: `auto/config` (ubuntu/resolute, iso-hybrid, casper), `spike-live.list.chroot`, strip-telemetry chroot hook, `.recipe-ready`
- Implemented `scripts/build-iso.sh` (check-deps, clean, config, build; rejects `--variant`)
- Updated `docs/dev-guide/04-building-spike.md` with Stage 1 flow + QEMU smoke-test
- Updated `STATE.md`

**Open / verify:**
- Host still needs interactive: `sudo apt install live-build debootstrap …` then `sudo ./scripts/build-iso.sh` (agent cannot supply sudo password)
- Script smoke: `--check-deps` reports missing lb/debootstrap until installed; `--variant` correctly rejected; shell syntax OK on auto/config + hook + wrapper
- Confirm ISO boots in QEMU after local build

**Next:**
- BDFL: install deps + first `lb build` + QEMU
- Then Stage 2 (`spike-config`)

---

## 2026-08-05 — Lock live-build + one ISO + custom installer

**Participants:** BDFL + agent (Cursor)

**Done:**
- Decision: ISO via **live-build**; **one** hybrid live ISO; Standard/Plus at **install time**; installer = **custom Qt** (not Calamares)
- Base confirmed: **Ubuntu Server 26.04 LTS** (stripped), development window before 28.04
- Logged in `DECISIONS.md`; updated `STATE.md`
- Fixed stale dual-ISO / Calamares wording in ARCHITECTURE, CONTRIBUTING, INDEX, AGENTS, DESIGN-DECISIONS
- Drafted `dev-guide/03-build-environment.md`, `04-building-spike.md`, `07-installer-internals.md`
- Scaffolded `build/iso-build/` and stub `scripts/build-iso.sh`

**Open:**
- Working ISO not yet produced (scaffold only)
- Remaining `dev-guide/` / `user-guide` / agent-ops stubs

**Next:**
- Prototype: fill live-build recipe and produce first bootable image when BDFL starts Phase 3

---

## 2026-08-05 — Docs cleanup + license root + agent-ops init

**Participants:** BDFL + agent (Cursor)

**Done:**
- Fixed GitLab Markdown rendering across docs (fence backticks, escapes, root README headings/fences, domain `.org`→`.com` for BigRangaTech only)
- Matrix room normalized to `#spike-linux:matrix.org`
- Docs license corrected to CC-BY-SA 4.0 where it wrongly said GPLv2+
- Root CoC reduced to pointer → `docs/CODE_OF_CONDUCT.md`
- Copied license texts to root: `LICENSE`, `LICENSE-CC-BY-SA-4.0` (kept `licences/`)
- Initialized `STATE.md`, `DECISIONS.md`, this `SESSION_LOG.md`
- Began refreshing stale status in AGENTS/INDEX/READMEs

**Open:**
- ISO build tooling / installer implementation stack undecided _(resolved in later session same day)_
- `TRANSLATIONS.md`, user-guide, most dev-guide, remaining agent-ops stubs still empty (intentional for now)

**Next:**
- Keep STATE / DECISIONS / SESSION_LOG current
- Finish status-doc sync this session
- Prototyping when BDFL starts; choose build tooling when ready
