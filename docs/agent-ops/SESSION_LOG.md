# Spike Agent Ops — Session Log

Append-only. Newest sessions at the **top**.

---

## 2026-08-06 — Settings pages + Volume/Battery applets + spike-icons (0.0.14)

**Done:**
- `spike-icons` inherit theme (`breeze-dark` → Breeze) + `QIcon::fromTheme` on panel/Settings nav
- Live: `breeze-icon-theme`, `pulseaudio-utils`, `upower`; polkit `timedate1` for live timezone/NTP
- Settings (real, not stubs): Date & Time (`timedatectl`), Keyboard / Mouse (`kcminputrc`), Keyboard Layout (XKB/`localectl`)
- Tray: Volume (pactl + scroll/mute) → Sound KCM; Battery (UPower, hidden if none) → Power KCM
- Still stubs: Language, Users, Accessibility, Notifications, VPN, …

**Package:** `spike-shell_0.0.14-1`.

**Try:** rebuild ISO → panel icons + volume/battery → Settings pages above → Session Shut down.

---

## 2026-08-06 — Pre-rebuild desktop polish (Breeze / menu / portal / SOF HDMI)

**Prior smoke (capture `spike-capture-20260806T115605Z`):** Login quiet (PipeWire deferred); `spike-session` OK; Wi‑Fi DHCP; Settings KCMs; **graceful shutdown**. Gaps: missing Breeze decorations, `applications.menu`, xdg-desktop-portal, SOF HDMI journal spam in-session.

**Landed (config + live packages, no plasma-workspace):**
- Packages: `breeze`, `plasma-integration`, `xdg-desktop-portal`, `xdg-desktop-portal-kde`
- `/etc/xdg/menus/applications.menu` (Spike-owned; no `plasma-workspace-data`)
- SpikeDark color scheme + `/etc/xdg/kwinrc` / `kdeglobals` (Breeze decorations, Spike purple/cyan tint)
- WirePlumber `51-spike-sof-hdmi-disable.conf` (sof-essx8336 HDMI only)
- `QT_QPA_PLATFORMTHEME=kde` via environment.d / profile.d / `spike-session`

**Out of scope this ISO:** custom Aurorae titlebars; printer KCM QML noise; Sound KCM Kirigami binding loops.

**Try:** rebuild → **Spike Live (debug logging)** → `spike-session` → check titlebars / menu / portal / quieter journal → Session menu Shut down → USB `spike-capture-*`.

---

## 2026-08-06 — Alpha gate locked: installer E2E

**BDFL:** Leave pre-alpha / enter Alpha only when the custom installer works end-to-end (live → install → reboot). Logged in `DECISIONS.md`; `STATE.md` / `ROADMAP.md` / `README.md` updated.

---

## 2026-08-06 — Login audio spam (SOF HDMI) + full debug capture

**Finding:** After login, PipeWire/WirePlumber auto-started and looped on broken SOF HDMI (`sof-essx8336` pcm7 / `ASoC error (-5)`), flooding the console (worse with `debug` cmdline). No `spike-session` log — desktop never started. Hard power-off was manual.

**Fix:**
- Debug/`spike.capture` boots: `spike-capture-logs --follow` (full journal) + full snapshot (journal/dmesg/audio).
- Defer PipeWire user units until `spike-session` (hook `0710-spike-audio-defer.chroot`).
- `spike-shell` 0.0.13 starts PipeWire from session script.

**Try:** rebuild → **Spike Live (debug logging)** → login (TTY should stay quiet) → `spike-session` → smoke Net/Settings → power off → USB `spike-capture-*` with `journal-follow.txt` + `audio/`.

---

## 2026-08-06 — Settings category audit + Network applet (0.0.12)

**Done:**
- Walked every DESKTOP.md Settings category; nav complete.
- Safe KCM packages on live list: `kscreen`, `plasma-pa`, `powerdevil`, `bluedevil`, `print-manager`, PipeWire stack.
- Plasma-coupled pages → Spike stubs (no `plasma-desktop` / `plasma-workspace`).
- Spike NetworkManager tray applet + Settings → Network (`NmClient` + `NetworkPanelWidget`); no `plasma-nm`.
- Docs aligned: `DESKTOP.md`, `DESIGN-DECISIONS.md`, `ARCHITECTURE.md`, `ROADMAP.md`, `NETWORKING.md`, `CONSTRAINTS.md`, `DECISIONS.md`, `STATE.md`.

**Package:** `spike-shell_0.0.12-1`.

**Try after commit/push:** rebuild ISO → smoke tray Net popup (scan/connect) + Settings → Network; Display/Sound/Power KCMs.

---

## 2026-08-06 — Cursor works; session power + launcher stubs broken

**Finding:** Input/cursor OK on 0.0.6. Restart/Shut down used `systemctl` (needs root/polkit) and this Ubuntu `loginctl` has no power verbs. Launcher stubs pointed at missing apps (`xterm`/`dolphin`/`firefox`) and only activated on double-click.

**Fix (0.0.7):** Power via `busctl` → logind `PowerOff`/`Reboot`, then `sudo -n systemctl`. Launcher scans `.desktop` files, single-click launches, ISO adds `foot`.

**Try:** rebuild → Session menu power actions; Spike menu should list Foot (and any other .desktop apps).

---

## 2026-08-06 — PowerOff Access denied; launcher stubs

**Finding (USB session log):** Cursor OK on 0.0.6. `Call to PowerOff failed: Access denied` twice; auth log: `Polkit found to be unavailable` for `org.freedesktop.login1.power-off`. Launcher still stub apps (not on ISO).

**Fix (0.0.8):** Power via `sudo -n systemctl` first (wait for exit), then logind `busctl`. Ship `polkitd` + live allow rule. Launcher scans `.desktop` + single-click; ISO adds `foot` (from 0.0.7).

**Try:** rebuild → Restart/Shut down; Spike menu should show Foot / real .desktop apps.

---

## 2026-08-06 — "No input" was invisible cursor

**Finding:** Session 0.0.5 log shows KWin up, libinput opening the touchpad (`HTIX5288`), groups OK (video/input/render), system seatd OK. Then: `Failed to load cursor theme "default"` / `Unable to load any cursor theme`.

**Cause:** Pointer events work; no cursor theme on the live image → invisible cursor (feels like dead input).

**Fix (0.0.6):** Ship `breeze-cursor-theme`; `spike-session` sets `XCURSOR_THEME=breeze_cursors`.

**Try:** rebuild → `spike-session` → expect visible pointer + clicks on Spike button.

---

## 2026-08-06 — spike-session failed: seatd-launch vs system seatd

**Finding:** `session-latest.log` showed seat/session OK, then:
`Socket file found at socket path /run/seatd.sock, refusing to start` / `seatd exited prematurely`.
System `seatd.service` already running; `seatd-launch` tries to bind the same socket. Live user also not in `video` (seatd `-g video`).

**Fix (0.0.5):** Use system seatd when `/run/seatd.sock` exists (no seatd-launch). `spike-live-groups.service` adds live users to `video,input,render` before getty.

**Try:** rebuild → login on tty → `spike-session` → input should work. Log still at USB `…/log/spike/session-latest.log`.

---

## 2026-08-06 — Input still dead after root-guard; user re-ran without sudo

**Finding:** Root guard worked (sudo refused); user re-ran as `ubuntu`. Still no input. Follow log only showed the sudo attempt; no session log on USB (writable not user-writable at those paths).

**Cause (likely):** Starting KWin from a text VT without a DM — DRM works, libseat/logind often does not grant `/dev/input`.

**Fix (0.0.4):** Start via `seatd-launch`; ship `seatd`+`libseat1`; log to `/var/log/spike/` (casper → USB); sudo auto-re-exec as `SUDO_USER`.

**Try:** rebuild → `spike-session` (no sudo) → expect mouse/keyboard. Power-off OK; collect `/var/log/spike/session-latest.log` via USB install-logs.

---

## 2026-08-06 — Dead input again; follow log empty on USB

**Finding:** Captures `072454Z` (boot snapshot) + `072558Z` (--follow). Packages OK (shell 0.0.2, layer-shell-qt). Follow `journal-follow.txt` was **0 bytes** (stdio block buffer + hard reboot). No `spike-session.log`. Seat at boot had no sessions (expected pre-login).

**Fix:** Line-buffered follow + periodic sync; `spike-session` tees to USB `spike-session-latest.log`; refuses root / no-seat; shutdown finalize unit. Shell → 0.0.3.

**Try:** rebuild → `--follow` → `spike-session` as live user (not sudo) → power off if stuck → collect USB (`spike-session-latest.log` + `journal-follow.txt`).

---

## 2026-08-06 — Slim debug capture for shell/session

**Done:** `spike-capture-logs` no longer dumps full syslog/dmesg/udev/nmcli/lsmod. Keeps spike-config, package check, seat/session, filtered journal, and copies `spike-session.log` when present. Re-run `sudo spike-capture-logs` after testing the shell so session logs land on USB.

---

## 2026-08-06 — spike-session: centred panel + dead input

**Finding:** USB capture `spike-capture-20260806T065815Z` is early boot only (no kwin/shell lines). User report: panel appeared mid-screen; mouse/keyboard dead under `spike-session`.

**Cause:** (1) Wayland ignores client `setGeometry` — KWin centres the xdg-shell stub. (2) Session likely nested / not taking DRM seat → compositor up, seat input dead.

**Fix:** Panel uses LayerShellQt bottom anchors + exclusive zone. `spike-session` clears parent displays, starts `kwin_wayland --drm --xwayland`, wraps `dbus-run-session`, logs to `$XDG_RUNTIME_DIR/spike-session.log`. ISO adds `layer-shell-qt` / `liblayershellqtinterface6`. Shell bumped to 0.0.2.

**Try:** rebuild ISO → from a text VT (Ctrl+Alt+F3), log in, run `spike-session`. Panel should sit on the bottom edge; input should work. If not, grab `$XDG_RUNTIME_DIR/spike-session.log`.

---

## 2026-08-06 — Spike Shell white text + ISO packaging

**Done:** Panel/launcher/menus use **white text on dark** (`spike.qss` + `applyDarkPalette` in `main.cpp`). New `scripts/package-spike-shell.sh` → `spike-shell_0.0.1-1_amd64.deb`. `build-iso.sh` packages + stages both config and shell; hook installs both; package list adds Qt6 Widgets, `qt6-wayland`, `kwin-wayland`, `xwayland`. Host needs `cmake` + `qt6-base-dev`.

**Try:** `sudo ./scripts/build-iso.sh` → USB → live: `spike-shell` (panel text must be readable). Optional: `spike-session` under KWin.

**Next:** live smoke; then layer-shell anchoring / app scan.

---

## 2026-08-06 — Stage 3 Spike Shell skeleton

**Done:** Created `src/spike-shell/` — Qt6 Widgets panel (32px), Spike launcher stub, clock, session menu (`loginctl`/`systemctl`), `spike.qss`, session stub (`spike-session` + `spike.desktop`). CMake build succeeds (`./build/spike-shell`). Still pre-alpha; no layer-shell / ISO packaging yet.

**Try:** `cd src/spike-shell && cmake -S . -B build && cmake --build build && ./build/spike-shell`

**Next:** layer-shell anchoring, real app scan, package into live ISO.

---

## 2026-08-06 — Pre-alpha through shell/installer; Alpha by feel

**Decision:** Spike Shell (+ more stack) stays **pre-alpha**. Open Alpha later by BDFL feel — practical cue: installer works end-to-end. Phase names are labels, not code gates.

---

## 2026-08-06 — Detect on target confirmed (Celeron N4020 / ~4GB)

**Finding:** After rebuild, debug capture `spike-capture-20260806T061802Z` shows correct hardware: **Celeron N4020**, **2 cores**, **~3.7GB RAM** (`total_kb: 3818872`), zram ~3729MB. No longer the build-host Xeon.

**Note:** Deb on image still reported `0.0.1-1` (hook/service fix was enough); bump to 0.0.2 on next package if desired. User also ran `sudo spike-config --generate memory` on live.

**Artifacts:** `build/iso-build/debug-logs/spike-capture-20260806T061802Z/`

---

## 2026-08-06 — Detect baked build-host hardware into live ISO (bug)

**Finding:** Capture showed Xeon E3-1505M / 16GB because `0600-spike-config.chroot` ran `spike-config --detect` inside the **build chroot**. User’s target is dual-core / 4GB.

**Fix:** Hook only seeds default state. New `spike-detect.service` runs `--detect` on the target at boot (before capture). Package bumped to 0.0.2.

**Retry:** `sudo ./scripts/build-iso.sh` → debug boot → confirm capture CPU/RAM match the laptop.

---

## 2026-08-06 — Stage 2 live debug boot: spike-config OK

**Finding:** Rebuilt ISO with debug GRUB entry. Boot cmdline had `debug`; `spike-capture-logs.service` succeeded. Capture landed under casper `install-logs-…/log/spike-capture-…` (wrote to `/var/log` first; recovered via log-persistence).

**Verified:** `spike-config 0.0.1-1` installed; state seeded (`variant=standard`); detect filled CPU/RAM (Xeon E3-1505M, ~16GB). Multi-user/getty reached. Wi‑Fi iface present (`wlan0` down/unmanaged in capture — expected if not connected yet).

**Artifacts:** `build/iso-build/debug-logs/spike-capture-20260806T055843Z/`

**Next:** Stage 2 deepen (better network detect, UFW templates) or Stage 3 Spike Shell.

---

## 2026-08-06 — Live debug logging (rebuild ISO)

**Done:** GRUB entry **Spike Live (debug logging)** (`debug`, no quiet/splash). On that cmdline, `spike-capture-logs.service` dumps journal/dmesg/`spike-config --state` onto casper `writable` / LABEL=writable. Host collector: `sudo ./scripts/spike-collect-usb-logs.sh`.

**Also:** Copied today's `install-logs-2026-08-06.0` into `build/iso-build/debug-logs/` — boot reached multi-user + graphical; iwlwifi firmware loaded; no spike-config errors in casper syslog (noise: DNS during live hooks, GPT size mismatch on oversized stick).

**Retry:** `sudo ./scripts/build-iso.sh` → write USB → boot **debug logging** entry → remount stick → collect logs.

---

## 2026-08-06 — gpg failure was packages.chroot local-repo signing

**Finding:** `env: 'gpg': No such file` comes from `lb_chroot_archives` signing **local packages.chroot** debs — not from apt mirrors. `LB_BOOTSTRAP_INCLUDE` was also wiped to `""` when `lb config` re-read existing `config/bootstrap`.

**Fix:**
- Stage `spike-config_*.deb` under `includes.chroot/var/cache/spike-local/` and install with hook `0600` (`dpkg -i`).
- Clear any `packages.chroot/*.deb` before build.
- Force `LB_BOOTSTRAP_INCLUDE=…,gnupg` in `config/bootstrap` after `lb config`.

**Retry:** `sudo ./scripts/build-iso.sh`

---

## 2026-08-06 — lb config: no --bootstrap-include on Ubuntu live-build 3.0

**Finding:** `lb config: unrecognized option '--bootstrap-include'` on live-build `3.0~a57`. Variable `LB_BOOTSTRAP_INCLUDE` is still honored via `config/bootstrap`.

**Fix:** `auto/config` exports `LB_BOOTSTRAP_INCLUDE=apt-utils,ca-certificates,gnupg` instead of the CLI flag.

---

## 2026-08-06 — ISO build abort: missing gpg in bootstrap chroot

**Finding:** `lb build` stopped during `lb_chroot_archives` with `env: 'gpg': No such file or directory` / `E: GPG exited with error status 127`. No ISO was produced. live-build still exited in a way that looked “finished” to the wrapper.

**Fix:** `auto/config` now `--bootstrap-include apt-utils,ca-certificates,gnupg`. `build-iso.sh` fails hard when no `.iso` exists and prints matching `build.log` errors.

**Retry:** `sudo ./scripts/build-iso.sh` (full clean + rebuild).

---

## 2026-08-06 — spike-config packaged into live ISO

**Done:** `scripts/package-spike-config.sh` builds `spike-config_0.0.1-1_all.deb`; `build-iso.sh` stages it into `config/packages.chroot/` before `lb build`; hook `0600-spike-config.chroot` seeds state + `--detect`.

**Also:** Pre-alpha Secure Boot is optional (disable in firmware OK). Remaster still prefers shim when present.

**Try:** `./scripts/package-spike-config.sh` then `sudo ./scripts/build-iso.sh`

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
