# Spike Agent Ops — Session Log

Append-only. Newest sessions at the **top**.

---

## 2026-08-12 — plymouth-set-default-theme missing on install

Jessie: install WARN that command was missing. Expected on Ubuntu — theme selection is `update-alternatives` for `default.plymouth`, not Debian’s `plymouth-set-default-theme`. Installer **0.0.26** + branding **0.0.2** register/set spike-minimal at priority 200 before `update-initramfs`. Backup/restore E2E deferred to next smoke (test files forgotten this run).

---

## 2026-08-12 — live Fn volume/brightness still dead

KWin `registerShortcut` + kglobalacceld never reliably bind XF86 volume/brightness on Spike’s KWin-only session. Shell **0.0.51**: `EvdevMediaKeys` reads `KEY_VOLUME*` / `KEY_BRIGHTNESS*` from `/dev/input` (live user already in `input` group); KWin script kept only for Meta+L / Meta+Space.

---

## 2026-08-12 — live ISO GRUB had no theme

Quiet live boot menu was still plain white/black text: `spike-iso-hybridize.sh` rewrote `grub.cfg` without gfxterm/theme, and never copied `src/spike-branding/grub-theme/`. Hybridize now installs `boot/grub/themes/spike/` + unicode.pf2, enables gfxterm/gfxmenu, and embeds video/png modules in the BIOS core image. Re-run hybridize (full squashfs rebuild not required for GRUB-only).

---

## 2026-08-12 — greeter flash, partial Fn keys, no logos

Post-install smoke: greeter still blanks ~1s; some Fn keys work; **no** Plymouth/GRUB/desktop logos. Causes: (1) greeter raced `console-setup`/`setfont` on tty1; (2) `kglobalacceld` not a hard Depends; (3) branding never packaged — no themes on ISO. Shell **0.0.50** (After=console-setup, refresh timer, greeter emblem); pin `kglobalacceld`; new **spike-branding 0.0.1** + plymouth on ISO; installer **0.0.25** activates Plymouth/GRUB theme; config **0.0.13** enables GRUB_THEME when theme.txt exists.

---

## 2026-08-12 — restore skipped: backup wrote to /var/log

USB `install-logs-2026-08-12.0`: backup OK (9 files) but `destMount=/var/log` (casper writable bind); `SpikeBackup/` only survived under `install-logs-*/log/`. `install-all` had **no** `--restore-session` → `RESTORE_STATUS=skipped`. Installer **0.0.24**: mount `/run/spike-rescue/dest-writable`, never offer `/var/log`; drain `SESSION_PATH` + newest-session fallback for Layer 4.

---

## 2026-08-12 — backup scan: empty list despite successful probe

`backup-scan.log`: stderr reached `[100%] Scan complete` and auth mounted `/dev/nvme0n1p2` + `list-dirs …/home`, but stdout empty. Cause: `--list-systems` waited on `QEventLoop` after sync `scanFinished` (`quit()` before `exec()` → hang → installer 90s kill). Rescue **0.0.15** drops the loop for batch paths; installer **0.0.23** drops offscreen env.

---

## 2026-08-12 — quiet boot: logos never loaded (smoke note)

Jessie (prior quiet ISO): Plymouth/splash logos never appeared. Repo still has valid `plymouth/*/logo.png` (RGBA) + unused newer `spike-emblem-256.png` (RGB); scripts load `logo.png`. Investigate on next quiet smoke: theme selected in initramfs, `splash` on cmdline, asset install path — not fixed in this commit.

---

## 2026-08-12 — installer UX pass notes (queued)

Jessie: after install, “remove boot USB” confuses restore users; want keep-USB prompts, a calm static Finish/reboot UI when not debugging, and a dedicated **installed** debug mode (alongside live debug GRUB). Noted in `INSTALLER.md` UX pass backlog. Timing reminder: Layer 4 restore already runs during `install-all` (before Finish), not at first login — prompts should protect the USB through install completion.

---

## 2026-08-12 — installer backup scan missed installed system

USB logs: rescue *did* mount/probe `/dev/nvme0n1p2` during Step 7, but UI showed none. Fixes: rescue **0.0.14** skips entire live USB disk + tiny partitions; safer `--list-systems` output; `/etc/spike/installed` detection. Installer **0.0.22** runs scan offscreen, logs `/var/log/spike/backup-scan.log`, stricter `/dev/` parse.

---

## 2026-08-12 — Fn / media keyboard shortcuts

Volume ±/mute, brightness ±, media play/next/prev, Meta+L lock, Meta+Space launcher. KWin `spike-shortcuts` → D-Bus `org.spike.Shell.Shortcuts`; session starts `plasma-kglobalaccel`. Shell **0.0.49** + ISO `playerctl`. Greeter stay-visible and installer lazy backup scan still in tree for next ISO.

---

## 2026-08-12 — greeter flashes then blank (still accepts password)

Installed: login UI appears briefly then vanishes; typing password + Enter still logs in. Cause: getty@tty1 Conflicts stop racing with greeter paint — getty TTYReset clears fb after first frame. Shell **0.0.48**: `After=getty@tty1`, getty drop-in `TTYVTDisallocate=no`, prepare quiets fbcon, greeter keeps KD_GRAPHICS + delayed repaint.

---

## 2026-08-12 — installer: scan disks only when backup checked

Backup page no longer runs `--list-systems` on entry. Scan starts when “Back up files…” is checked (or Refresh with that checked), with a “can take a while” message. Installer **0.0.21**.

---

## 2026-08-11 — Spike APT update plumbing (host deferred)

Publish path ready before package-hosting hardware: `generate-spike-apt-key.sh` + `publish-spike-apt-repo.sh` → `build/apt-repo/`. ISO ships `spike.sources` (**Enabled: no**) + keyring + `unattended-upgrades`. Installer **0.0.20** seeds APT source on target; shell **0.0.47** Settings → Updates Apply wires Ubuntu security auto-install. Flip `Enabled: yes` + upload repo when `packages.bigrangatech.com` is live (`docs/UPDATES.md`).

---

## 2026-08-11 — greeter vs quiet/splash (Plymouth)

Likely what looked like “boot GUI login” earlier was sleep→SpikeLockScreen. Quiet ISO boots hold fb/DRM in Plymouth — greeter must wait. Shell **0.0.46**: `spike-greeter-prepare` (`plymouth quit --wait`, chvt 1, wait for fb/dri); unit After=`plymouth-quit-wait`; TTYVTDisallocate=no. Keep installer **0.0.19** greeter+getty fallback. Smoke on **quiet** (not only debug) boot.

---

## 2026-08-11 — restore greeter with getty fallback

Jessie: GUI greeter/lock worked on prior ISO; black-after-GRUB was from **disabling getty** when enabling spike-greeter. Keep greeter enabled; leave getty enabled as OnFailure fallback (unit already Restart=on-failure + OnFailure=getty@tty1). Installer **0.0.19** + shell **0.0.45**. Backup hang fixes from **0.0.18** / rescue **0.0.13** unchanged.

---

## 2026-08-11 — backup mount hang + black screen after GRUB

Smoke: Step 7 backup still hangs on disk mount; post-install = black after GRUB (spike-greeter + getty disabled). Fixes: rescue **0.0.13** — `--batch-recover` skips inventory-on-scan; mount `timeout 15` + ext `noload`; `--partition` for install path. Installer **0.0.18** — skip second blocking list-systems when partition known; 90s scan kill; default **text getty** again (greeter packaged, not enabled). Shell **0.0.44** — greeter unit OnFailure→getty; Users Apply keeps getty. Rebuild → skip or finish backup faster; expect text login then desktop.

---

## 2026-08-10 — ISO ships full desktop/hardware runtime

Policy: squashfs must include everything Spike Shell + detect need — do not lean on `.deb` Recommends. ISO list gains bluez/rfkill/wmctrl/brightnessctl/gvfs-backends/smartmontools/nm-connection-editor/xdg-utils/linux-firmware-misc; shell **0.0.43** promotes those helpers to Depends; `0720-spike-verify-includes` fails the build if udisks2/firmware/greeter/etc. missing. Rebuild with installer **0.0.17** + shell **0.0.43**.

---

## 2026-08-10 — USB hotplug + Storage + graphical boot login

Installed smoke: USB shows in `lsusb` but Discover/tray ignore it (no notify); Settings Storage thin; boot asks for login but only text agetty (graphical lock works after sleep).

Fixes: ISO adds **udisks2**, **gvfs**, **exfatprogs**. Shell **0.0.42** — Removable Devices via UDisks2 ObjectManager + auto-mount + Notify; Settings → Storage Mount/Eject/Open; **spike-greeter** (linuxfb on tty1) when auto-login off. Installer **0.0.17** enables greeter / disables getty@tty1 (or reverse for auto-login). Rebuild → smoke plug USB + cold boot login UI.

---

## 2026-08-10 — installed-system logs → spare USB

Drop heavy live audio dump from default `spike-capture-logs` (still on `--full`/debug boots). Live capture units already removed on install. Shell **0.0.41** `spike-save-logs` + Desktop **Copy Spike Logs to USB** + `/var/log/spike/night-light.log` on Apply. Installer **0.0.16** seeds Desktop icon, copies `install-from-live.log` onto target, drops live `spike-capture-logs` binary. Host: `spike-collect-usb-logs.sh` picks `spike-logs-*`. Rebuild → smoke → plug 64GB stick → copy logs.

---

## 2026-08-10 — installer backup/NM/autologin + night light + Sleep

Installed smoke: backup step never saw OS; NM “no connections to copy” despite Wi‑Fi; night light still dead; logout restarted session (auto-login); need Sleep; want login prompt by default.

Fixes: installer **0.0.14** — Step 7 backs up OS **on** the wipe disk (stop excluding target; exclude live USB only); rebuild Wi‑Fi keyfiles (nmcli `export` is VPN-only); optional auto-login checkbox **default off** (tty1 password login → `spike-session`). Shell **0.0.39** — Night Light writes `Mode=0` (Constant) + preview retries; session menu **Sleep**; Users Apply toggles getty autologin. Rebuild ISO.

---

## 2026-08-10 — ISO build: packagekit-tools missing

`lb` failed: `E: Unable to locate package packagekit-tools`. On Ubuntu resolute, `pkcon`/`packagekit-tools` are gone; PackageKit ships `pkgcli` in `packagekit`. Removed `packagekit-tools` from `spike-live.list.chroot`; shell **0.0.38** session refresh tries `pkcon` then `pkgcli refresh`. Rebuild ISO.

---

## 2026-08-10 — lock→sleep→broken greeter + night light

Smoke: mouse/power live OK (cpupower message noisy); night light silent fail; idle lock then immediate sleep → “screen locker is broken” on wake. Fixes in shell **0.0.37**: disable kscreenlocker Autolock/LockOnResume; Spike LockScreen.qml into plasma *shell* path (greeter was hitting outdated Plasma QML); Night Light via kwinrc `[NightColor]` + `preview()`; quieter Power Apply text; unlock-session on Spike unlock.

---

## 2026-08-10 — Discover offline + task icons + System Monitor

Installed smoke (after home/LO/lock fixes): FF/TB/LO OK; Discover opens but “can’t connect” despite Wi‑Fi (browsers work). Panel lacked open-app icons; need System Monitor. Fixes: shell **0.0.36** KWin `spike-tasklist` → panel task icons; session refreshes Flatpak appstream + `pkcon`; NM connectivity check; ISO adds `plasma-systemmonitor` + `packagekit-tools`. Rebuild to verify.

---

## 2026-08-10 — installed desktop: home seed, Discover, LO, lock

Installed smoke: empty home (no Documents/…), Discover AppStream error, LibreOffice silent fail (“user installation could not be completed”), KWin “screen locker is broken” (missing breeze LNF lockscreen QML). Fixes: shell **0.0.35** `spike-seed-home` + Spike LockScreen.qml; installer **0.0.13** seeds XDG/app dirs at configure; ISO packages Flatpak/AppStream + hook `0740`; hide powerdevil autostart. **On current install:** run `xdg-user-dirs-update`; `mkdir -p ~/.config/libreoffice/4/user`; `sudo apt install flatpak plasma-discover-backend-flatpak appstream && sudo flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo && sudo appstreamcli refresh --force`; copy LockScreen.qml from next package or rebuild ISO.

---

## 2026-08-10 — Firefox/Thunderbird profile cannot be loaded

Mozilla `.deb`s hit Ubuntu AppArmor / broken `profiles.ini`. Fix: unconfined AppArmor stubs in `includes.chroot/etc/apparmor.d/{firefox,thunderbird}`, hook `0730-spike-mozilla-apparmor.chroot` removes confined `usr.bin.*` leftovers, shell **0.0.34** `spike-fix-mozilla-home` resets inaccessible dirs / broken profiles.ini at session start. **Current live workaround:** `rm -rf ~/.mozilla ~/.thunderbird ~/.cache/mozilla` then reopen (or rebuild ISO).

---

## 2026-08-10 — live apply (power/input) + Wi‑Fi handoff

Smoke: brightness OK; apps present; power profiles not live on installed; Wi‑Fi from installer missing after install. Fixes: spike-config **0.0.12** applies CPU governor + Wi‑Fi powersave on Power SetSetting (root DBus); shell **0.0.33** applies mouse/touchpad/keyboard live via KWin `InputDevice` D-Bus and re-applies on session start; installer **0.0.12** copies NetworkManager connections into the installed system (`copy_network_connections`). Rebuild to verify.

---

## 2026-08-09 — Future: lightweight replacements for default apps

BDFL: Alpha ships Firefox / Thunderbird / VLC / LibreOffice; later Spike will aim for lighter replacements that keep the same roles. Logged in DECISIONS + DESIGN-DECISIONS; not an Alpha blocker.

---

## 2026-08-09 — seed default apps (Mozilla .deb + VLC + LibreOffice)

Ubuntu `firefox`/`thunderbird` apt packages are Snap stubs — forbidden. Wired **packages.mozilla.org** (`mozilla` + `thunderbird-deb` suites) via `config/spike-archives/` (staged to live-build `config/archives/` by `build-iso.sh`) and `includes.chroot/etc/apt/` (sources + pin + key). Live list: `firefox`, `thunderbird`, `vlc`, `libreoffice` (+ qt6/kf6/plasma). Strip hook drops Snap stubs if present; verify hook fails build if FF/TB are Snap. Rebuild to confirm launcher entries.

---

## 2026-08-09 — shell 0.0.32: lock, block sleep/locking, brightness live

SpikeLockScreen (PAM `/etc/pam.d/spike-lock`) from Session menu / PrepareForSleep / session Lock; Super+L best-effort. Plasma-equivalent **Manually block sleep and screen locking** in battery popup + Settings → Power (`SleepInhibit` logind `sleep:idle` block). Manual Suspend confirms when inhibit is on. Brightness applet: sysfs → logind `SetBrightness` → brightnessctl (was silent no-op without write perms). Package `spike-shell_0.0.32-1_amd64.deb`.

---

## 2026-08-09 — Thunderbird locked as default email

BDFL: preinstalled email client is **Thunderbird** (prefer `.deb`). FAQ/STATE/AGENTS/SECURITY/DECISIONS updated; ready to seed with Firefox + VLC.

---

## 2026-08-09 — AppImage stance clarified

Steer away from AppImage (trust / sideload model). Not a moral ban — if ever first-class, must install/remove via GUI as easily as `.deb` or Flatpak (Discover or Spike tools). Logged in DECISIONS + DESIGN-DECISIONS.

---

## 2026-08-09 — Apps policy: Flatpak + .deb (no Snap)

BDFL: drop Flatpak-only. Discover supports **Flatpak and `.deb`**; Spike system stack stays `.deb`; Snap still forbidden; AppImage not a Spike install path. Specs updated: DESIGN-DECISIONS, ARCHITECTURE, AGENTS, CONTRIBUTING, FAQ, CONSTRAINTS, STATE default-apps (prefer deb Firefox/VLC/email on Tier‑1).

---

## 2026-08-09 — Alpha opened; docs + default-apps track

BDFL: installer E2E met (installed desktop + module blacklist OK) → **Alpha** (`DECISIONS.md`). Docs: STATE/ROADMAP/README/CHANGELOG/INDEX flipped off pre-alpha gate. Next Alpha focus documented in STATE: preinstalled software (Firefox Flatpak, media/VLC, email candidate Thunderbird, LibreOffice later). First-run wizard already in shell **0.0.31**; rebuild still needed to smoke it on a fresh install.

---

## 2026-08-09 — post-install first-run wizard (shell 0.0.31 / installer 0.0.11)

Smoke before this change: erase install reached installed desktop; KERNEL.md module blacklist looked correct. Gap: no first-boot UX. Shell **0.0.31**: FirstRunWizard (Welcome → TZ → Wi‑Fi → Tour placeholder → Accessibility offer → Import → Notices → Done); hooks in `firstrun/FirstRunHooks` for Flatpak/updates/tour drop-ins; gate on `/etc/spike/installed` + `~/.config/spike/first_run_completed`. Installer **0.0.11**: writes `/var/lib/spike/first-boot` + `installer-notifications/`. Packages ready; rebuild to verify wizard on next install.

---

## 2026-08-09 — installer freeze on Variant → Backup (0.0.10 / 0.0.12)

Smoke `install-logs-2026-08-09.1`: Continue from Spike variant blocked GUI on `spike-rescue --list-systems` (`waitForFinished` 120s) while scan did `find-files` on NVMe home (Documents/…). Both killed SIGTERM ~76s later. Fix: rescue **0.0.12** `--list-systems` skips inventory walk; installer **0.0.10** async list-systems on backup page. Rebuild both packages into ISO.

---

## 2026-08-09 — installer 0.0.9: languages, timezones, keyboard

Installer welcome lists all 11 INSTALLER.md languages. Timezone page loads full `zone.tab` (~400 IANA zones, filterable) plus XKB layouts from `evdev.lst`, with language/timezone suggestions and a type-to-test field. Helper writes `/etc/default/keyboard`, runs `locale-gen` / `setupcon`. Package: `spike-installer_0.0.9-1`.

---

## 2026-08-09 — Spike Tools + install-time blacklist (0.0.8 / 0.0.11 / 0.0.3)

spike-config **0.0.11**: `blacklist.py` KERNEL.md policy from PCI presence → `security.module_blacklist`; detect() applies it; tests for empty PCI / MegaRAID vendor. Installer helper runs `--detect` then `--generate-all` before initramfs; logs blacklist entry count. Installer **0.0.8**: Step 7 system picker + resolve eligible after `--exclude-disk`; SKIPPED clears auto-restore; `RESTORE_STATUS=ok|failed|skipped` on Finish. Migration **0.0.3**: async wizard shell Welcome→Scan→Select→Dest→Progress→Done (+ Mode B import). Packages ready; no ISO rebuild in this change.

---

## 2026-08-09 — installer 0.0.7 + shell 0.0.30: black screen + live Apply

Smoke after **0.0.6**: install.log OK (`update-initramfs`, `update-grub`), then installed boot = **hung black screen**. Root cause: live `spike-live-groups` (video/input/render) is stripped on target, but squashfs user lacks those groups → kwin DRM/seat fails. Fix: helper always `usermod -aG video,input,render,…`, enables `seatd`, warns in profile.d. Live Apply: Panel/tray Apply saved state but panel did not refresh (dbus-python `StateChanged` `v` typing); shell **0.0.30** `ConfigClient::setSetting` local-echoes `stateChanged`; config **0.0.10** emits `variant_level=1` strings. Packages: installer **0.0.7**, shell **0.0.30**, config **0.0.10**. Rebuild → erase install → expect wallpaper+panel.

---

## 2026-08-09 — installer 0.0.6: casper initrd → local boot

Smoke (`install-logs-2026-08-09.0`): GRUB menu OK, but selecting Spike → `stdin: invalid argument` → `(initramfs) unable to find a medium containing a live file system`. Root cause: live initrd has `conf.d/default-boot-to-casper.conf` (`BOOT=casper`); minimal `grub.cfg` had `root=UUID` only. Also `update-grub` failed on missing `GRUB_FONT` theme path from spike-config. Fix: helper regenerates initramfs with `BOOT=local`, disables casper hook, adds `boot=local` cmdline, sanitizes theme paths; config **0.0.9** omits theme lines when assets missing. Step 7 hang: rescue **0.0.11** `--exclude-disk` (installer passes wipe target) → `SKIPPED` if nothing else. Packages: installer **0.0.6**, rescue **0.0.11**, config **0.0.9**. Rebuild ISO → erase install → reboot (USB out).

---

## 2026-08-09 — Spike Tools + installer 0.0.5 for full data-path smoke

Rescue **0.0.10**: `--list-systems`, `--batch-recover`, `--batch-restore` (shared with installer/migration). Installer **0.0.5**: Step 7 runs batch-recover before wipe (abort on failure); Layer 4 flatten via batch-restore + shell fallback. Migration **0.0.2**: thin Mode A/B UI over the same CLI. Packages: `spike-rescue_0.0.10-1`, `spike-installer_0.0.5-1`, `spike-migration_0.0.2-1`. Rebuild ISO → smoke backup→ERASE→restore→reboot.

---

## 2026-08-09 — installer 0.0.4: GRUB cfg + Wi‑Fi page

Smoke (`install-logs-2026-08-09.1` / `install.log`): install to `/dev/nvme0n1` finished “OK” but `WARN: update-grub failed` → post-reboot **grub>** (no `grub.cfg`). Root cause: squashfs has `grub-*-bin` only → no `/etc/default/grub`, no `/boot/grub`. Helper now creates defaults, logs grub-install/update-grub, writes minimal `grub.cfg` if needed, UEFI `--removable` fallback; strips live casper units from target. Wizard Wi‑Fi page: nmcli scan/connect/skip. Package: `spike-installer_0.0.4-1`.

---

## 2026-08-09 — default wallpaper Coastal-Run (stretch)

Temporary desktop default: `Coastal-Run.png` from Pixel Archive Dark → `/usr/share/spike/wallpapers/` (includes.chroot; also `spike-default.png`). spike-config **0.0.8** default-state path; shell **0.0.29** paints stretch-to-fit (`IgnoreAspectRatio`). Verify hook checks wallpaper present.

---

## 2026-08-09 — ISO build: drop conflicting grub metapackages

`lb` failed: `grub-pc` Conflicts `grub-efi-amd64` (both were added for installer). Keep `grub-pc-bin` + `grub-efi-amd64-bin` + `efibootmgr` + `grub2-common` only — helper already passes `--target=`. Re-run `build-iso.sh`.

---

## 2026-08-09 — installer smoke fail: bare install-all (ISO had 0.0.2)

Capture `spike-capture-20260809T073708Z` (SanDisk writable / `install-logs-2026-08-09.0`). Sudoers OK (`spike` ran helper as root). Command was only `spike-install-helper install-all` — no `--disk`/`--confirm`/… — session closed in ~5ms; no `install.log`. Smoked ISO built **14:41** with installer **0.0.2** (env `SPIKE_INSTALL_*`); **0.0.3** deb stamped **15:06** after that ISO. Shell on image **0.0.27**. Rebuild with installer **0.0.3** + shell **0.0.28** before next spare-disk attempt.

---

## 2026-08-09 — shell 0.0.28: desktop icons actually drawn

Smoke: Desktop shortcuts “weren’t there” — ISO hook did copy `~/Desktop/*.desktop`, but Spike Shell only painted wallpaper (`DesktopBackground`) with no icon layer / pointer input. Added `DesktopIcons` (scan + seed from `/usr/share/spike/live`, click to launch); background layer `Bottom` + `OnDemand` input. Package: `spike-shell_0.0.28-1`.

---

## 2026-08-09 — installer 0.0.3 ready for spare-disk smoke

Fixed blockers before install cycle: (1) `InstallEngine` passes `install-all` CLI args (sudo was stripping `SPIKE_INSTALL_*`); (2) live package list adds `grub-efi-amd64`/`efibootmgr`/`grub-pc` for UEFI grub-install in squashfs; (3) configure writes `/etc/spike/installed` + tty1 autologin for new user + profile.d that starts `spike-session` without `boot=casper`; drops live installer sudoers/Desktop icons on target. Step 7 backup still stub — skip. Package: `spike-installer_0.0.3-1`.

---

## 2026-08-09 — shell 0.0.27 full tray + Settings PANEL

Finished existing applets (Volume 150%/middle-mute, Battery ETA, Clock calendar, Session Lock+confirms, Network VPN row). Added Notifications (in-process fdo daemon), Brightness, Removable, Updates, Night Light, Bluetooth, Airplane, Keyboard Layout, Window list. Settings → **PANEL**: Panel / Tray Applets / Night Light. config **0.0.7** desktop defaults. Packages: `spike-shell_0.0.27`, `spike-config_0.0.7`. Fold into next ISO rebuild.

---

## 2026-08-09 — Power Apply logind restart flood (shell 0.0.26)

Smoke capture `spike-capture-20260808T143121Z`: config **0.0.6** OK (Appearance Apply works; power state saved to `performance`). Apply then ran `systemctl restart systemd-logind` → session tear-down / journal flood (polkit + logind stop at 14:32:38). Removed mid-session logind restart; drop-in applies on reboot; governor still tried live via cpupower.

---

## 2026-08-09 — spike-config 0.0.6: dbus.Variant crash on Apply

Smoke: Settings → Power (and any Apply) failed with `module 'dbus' has no attribute 'Variant'`. dbus-python 1.4 dropped `dbus.Variant`; Apply always `SetSetting`s every field (even unchanged), and each call emitted `StateChanged` via `dbus.Variant(...)`. Fixed unwrap + emit plain values for signature `v`. Package: `spike-config_0.0.6-1_all.deb`. Live ISO still on 0.0.5 until rebuild/hot-install.

---

## 2026-08-09 — installer 0.0.2 engine + Desktop icons (migration 0.0.1)

Privileged `spike-install-helper` (partition GPT/MBR, unsquashfs, fstab, user/hostname/variant, grub, optional Layer 4 restore). Wizard requires type **ERASE** + Install Spike now. Packages + sudoers. Live Desktop: **Install Spike**, **Rescue My Files**, **Move My Files** (migration launcher → Rescue/Installer until full wizard). build-iso + 0600/0720 hooks stage migration. Step 7 backup copy still stubbed. Needs spare-disk E2E before claiming Alpha.

---

## 2026-08-08 — Settings finish (shell 0.0.25); installer paused

Replaced Advanced JSON dumps with **Updates** / **Kernel Modules** / **Storage** forms. Human **About** card + collapsible raw state. **Appearance** wallpaper picker + live accent/font via QSS/palette; desktop **DesktopBackground** layer-shell wallpaper. Diagnostics sectioned + Copy; Software Sources universe/multiverse enable; VPN Import; Accessibility magnifier + HC chrome live. Installer engines stay parked until Settings smoke. Package: `spike-shell_0.0.25-1`.

---

## 2026-08-08 — Settings → Power (shell 0.0.24 + config 0.0.5)

Replaced powerdevil KCM host with Spike custom **Power** page: profile, idle/screen blank, suspend/hibernate flags, lid (AC/battery), power button, CPU governor, Wi‑Fi powersave prefs. Saves via `org.spike.Config` power module; generates `/etc/systemd/logind.conf.d/99-spike-power.conf`; Apply tries live `cpupower` + `systemctl restart systemd-logind`. Session menu gains **Suspend**. Dimming / charge limits still later per POWER-MANAGEMENT.md. Rebuild smoke should include Power + Suspend alongside Rescue/Installer.

---

## 2026-08-08 — spike-installer 0.0.1 started (wizard before rebuild)

Started the installer ahead of the next ISO rebuild (after Rescue restore smoke). Qt 10-step wizard collects language/timezone/user/hostname/variant/optional backup + SpikeBackup session pick (spike-common); storage confirm is dry-run — **does not wipe**. Packaged via `package-spike-installer.sh`; `build-iso.sh` + hooks install Desktop **Install Spike**; verify hook requires `/usr/bin/spike-installer`. Docs: installer README, `07-installer-internals`, repo map, building-components, STATE. Next engines: Wi‑Fi, Step 7 copy, wipe/squashfs/bootloader, Layer 4 restore.

---

## 2026-08-08 — docs catch-up (status + product refs)

Aligned ROADMAP internal deps, AGENTS Phase 3 / Layer 3, CHANGELOG Unreleased, GLOSSARY, INSTALLER/DISASTER-RECOVERY/ARCHITECTURE/BOOT-PROCESS/MIGRATION-GUIDE/SPIKE-MIGRATION/INDEX with current shipping: Rescue recover+restore, spike-common, Spike Tools, installer/migration scaffolds. Session earlier: living `dev-guide/` fill.

---

## 2026-08-08 — dev-guide: repo map + rescue/shell docs

Filled living `docs/dev-guide/` pages so implementation can track the tree as it grows: `01-getting-started`, `02-repo-structure`, `05-building-components`, `06-spike-shell-architecture`, `08-rescue-tool-internals`, `09-spike-config-internals`, `12-debugging`, `19-appendix`, plus `dev-guide/INDEX.md`. Updated `07-installer-internals` / `03-build-environment` for spike-rescue + SpikeBackup/`spike-common`. Remaining 10–11/13–18 are short stubs pointing at product docs. `docs/INDEX.md` status markers refreshed.

---

## 2026-08-08 — recover/restore everywhere + Spike Tools

**Rescue 0.0.9:** Prefer RW mount of `LABEL=writable` at `/run/spike-rescue/dest-writable` so `SpikeBackup/` is at partition root (not under casper `/var/log` → `install-logs-*/log/`). Entry screen: Rescue my files | Restore from backup. Shared `src/spike-common/SpikeBackupLayout` finds sessions (incl. legacy), maps into target home. Helper allows `/home/*` dests and SpikeBackup/media sources for restore.

**Shell 0.0.23:** Launcher category **Spike Tools** (`X-Spike-Tools`). Menu + `spike-tools.directory` in includes.chroot. Rescue `.desktop` uses that category.

**Scaffold:** `src/spike-installer/{backup,restore}/` + `src/spike-migration/` for later in-installer / detailed migration on the same layout. Docs aligned (DISASTER-RECOVERY, SPIKE-MIGRATION, INSTALLER, SPIKE-RECOVERY-TOOL-GENERAL).

---

## 2026-08-08 — spike-rescue 0.0.7: pre-alpha debug mode

Prioritize debug over polish: window title shows version + `[pre-alpha debug]`; select/inventory/dest show paths/devices/mounts; Done screen dumps full debug log; every copy writes `REPORT.txt` beside the backup (scan summary, dest device, failure kinds, helper detail). Easy to strip later when the tool is finished.

---

## 2026-08-08 — spike-rescue 0.0.6: honest copy error kinds

Completion report now splits **source read** vs **destination write** vs **verify**, and lists `path — kind: helper detail` (e.g. `destination not allowed`) instead of dumping write failures into “Could not read”.

---

## 2026-08-08 — spike-rescue 0.0.5: /var/log writable dest

**Root cause (capture `…T102649Z`):** Casper mounts live USB `LABEL=writable` at **`/var/log`**. Rescue correctly offered it as dest and tried `copy-file` → `/var/log/SpikeBackup/…/Untitled 1.odt`, but the helper **rejected** `/var/log` (not in allow list) → reported as “Could not read”. Fix: allow `/var/log` when LABEL=writable; `mkdir-dest`; prefer `/var/log` in ensureLiveUsbWritableDest.

---

## 2026-08-08 — rescue 0.0.4 (700/600 reads) + Dolphin dark chrome

**Rescue:** Test doc on installed OS unreadable — mounts as root but inventory/copy as `spike` misses `700` homes / `600` files. Helper gains `list-dirs` / `find-files` / `sha256` / `copy-file`; engine uses them under `/run/spike-rescue/`.

**Dolphin:** White window + white text — SpikeDark scheme name alone; plasma-integration falls back to BreezeLight backgrounds. Inlined Colors:* into `/etc/xdg/kdeglobals` + skel; `QT_QUICK_CONTROLS_STYLE=org.kde.desktop`; shell **0.0.22**.

SanDisk writable had install-logs/capture only (no SpikeBackup from this host mount).

---

## 2026-08-08 — spike-rescue 0.0.3: save to live USB writable

Smoke: OS detect worked. Dest UI still pushed a second stick; Spike stick’s unmounted `LABEL=writable` (~27GB on 28GB SanDisk) was not offered. 0.0.3 RW-mounts that partition via helper `mount-rw` (writable-only) and lists **This Spike USB (writable)**. 2TB HDD unchanged (user off-limits; only appears if mounted under /run/media like any other volume).

---

## 2026-08-08 — spike-migration docs (spec)

Filled `SPIKE-RECOVERY-TOOL-GENERAL.md` (Rescue vs Migration vs installer map) and wrote `SPIKE-MIGRATION.md` (Move My Files: pre-install live + post-install import, shared `SpikeBackup/`). Cross-linked INDEX, AGENTS, MIGRATION-GUIDE, DISASTER-RECOVERY, INSTALLER, ROADMAP. **No app code** — implementation after rescue is stable.

---

## 2026-08-08 — Spike Rescue 0.0.2: detect systems (sudo + btrfs)

**Symptom:** Rescue My Files opened but found **no installed systems** on Fedora (N4020) or Kubuntu (A4).

**Causes:** (1) `sudo -n mount` failed on the live image (no passwordless RO path) and failures were skipped silently; (2) Fedora btrfs often has `/` and `/home` as separate subvols — empty `/home` on the root subvol caused systems to be dropped.

**Fix:** Constrained helper `/usr/lib/spike/spike-rescue-mount` + `/etc/sudoers.d/spike-rescue` for user `spike`; btrfs `subvol=root/@/@root` + separate `home/@home` mount; keep systems without users; scan summary shown when empty; LVM `vgchange -ay` in prepare. Rebuild ISO to smoke.

---

## 2026-08-08 — Spike Rescue Layer 3 MVP (0.0.1)

New Qt6 Widgets app `src/spike-rescue/` per DISASTER-RECOVERY.md: RO disk scan, home inventory, copy to `SpikeBackup/` with SHA256, live desktop **Rescue My Files**. Packaged via `package-spike-rescue.sh`; wired into `build-iso.sh` + hook `0600`. Live list: `ntfs-3g` / `hfsprogs`. Out of scope still: Layer 2 GRUB recovery UI, Layer 4 installer restore.

---

## 2026-08-08 — Live a11y packages (OSK / Orca)

Settings Accessibility OSK button failed on smoke — `maliit-keyboard` / `onboard` / `orca` were not in `spike-live.list.chroot`. Added per ACCESSIBILITY.md (+ onboard fallback). spike-shell **0.0.21** prefers maliit on Wayland (starts server then keyboard).

---

## 2026-08-08 — Notifications / Accessibility / Software Sources started

**spike-shell 0.0.20 + spike-config 0.0.4:** Real Settings pages (not placeholders). Notifications: DND/sound/retention → privacy (+ test Notify if a daemon owns the bus). Accessibility: sticky/slow/bounce/mouse keys via setxkbmap + Orca/OSK launchers; high contrast saved. Software Sources: read-only APT list + software-properties / drivers / update buttons. State merge fills new modules/keys on load. Still later: notification daemon, magnifier, APT edit/PPA UI.

---

## 2026-08-08 — Settings mix (fewer stubs) + casper UUID/overlay

**spike-shell 0.0.19:** Appearance panel height/position/auto-hide live via `ConfigClient` `StateChanged` + layer-shell. Memory + Boot Apply forms (SetSetting/generate). Language (`localectl`), thin Users (password + auto_login), VPN (nmcli list/up/down). Still stubs: Notifications, Accessibility, Software Sources.

**Also this day:** Casper UUID remaster + overlay initrd fix; A4 audio OK after `libcanberra-pulse`; N4020 still silent on sof-essx8336; hardware detect 0.0.3 confirmed on both machines; `__version__` single-sourced for spike-config.

---

## 2026-08-08 — Casper UUID sync on remaster (initramfs live-media miss)

**Symptom:** USB stuck at `initramfs$`; writable empty — casper never found the live medium. Remaster left ISO9660 blkid UUID (volume date) ≠ initrd `conf/uuid.conf` / `.disk/casper-uuid*` (`uuidgen`).

**Fix (option 3):** `spike-iso-hybridize.sh` now picks `--modification-date=YYYYMMDDhhmmsscc`, rewrites `.disk/casper-uuid*` + patches initrd via `scripts/spike-patch-initrd-uuid.py`, and verifies `blkid` == casper UUID. No full squashfs rebuild required.

**Follow-up:** After UUID sync, boot reached overlay setup and panicked: `/cow format specified as 'overlay' and no support found` (modprobe overlay failed despite `overlay.ko.zst` in the modules archive). Patcher now also embeds overlay into the main initrd layer, adds `conf/modules` + `init-top/00-spike-overlay`, and makes casper’s overlay load resilient (`modprobe` → `insmod`).

**Smoke:** N4020 + AMD A4 both reach spike-session as `spike`. Audio: treat **N4020 / sof-essx8336 as a bad audio reference** for now; use **AMD A4 (CX20751/2 analog-stereo)** as the audio reference machine. Hardware detect previously only filled CPU/RAM (GPU left as Intel stub defaults) — extended to gpu/storage/network in spike-config 0.0.3.

**Audio (A4):** Settings Test “No such driver” was missing `libcanberra-pulse` (plasma-pa Recommends only; live list had `libcanberra-alsa`). Added to `spike-live.list.chroot`. After rebuild: **A4 analog playback confirmed working** — keep as audio reference. N4020 retest: Test button OK but **still silent** (sof-essx8336 stereo-fallback sink present; hardware/UCM path, not canberra).

**Detect (captures `…T153052Z` A4 / `…T153719Z` N4020):** Full hardware fill on both (pkg `0.0.3-1`; CLI string still prints `0.0.2`). A4: AMD Mullins/`radeon`/`radeonsi`, HDD `/dev/sdb` 931G, ath10k wifi+eth+bt. N4020: UHD 600/`i915`/`iHD`, NVMe 476G, iwlwifi+bt no eth. No Intel stub leftovers on A4.

---

## 2026-08-07 — Build fail: HiFi verify false positive + missing SectionVerb

**Finding:** `0720-spike-verify-includes` failed on “still includes HDMI” because a *comment* contained `Include.hdmi`. Worse: that comment edit had also dropped `SectionVerb {` from the shipped HiFi.conf (UCM would be invalid).

**Fix:** Restore HiFi.conf from upstream minus the HDMI include; verify hook matches only active `Include.hdmi` lines and requires `SectionVerb`.

**Try:** re-run `./scripts/build-iso.sh`.

---


**Smoke (`spike-capture-20260806T135426Z`, 0.0.17):** Session “failed a lot” then started — `spike-session` blocked ~76s on `systemctl --user start` portals *before* Wayland (KDE portal ABRT, parent portal timeout). Pulse only `auto_null`; ~80 SOF ASoC −5 on HDMI pcm6/7. Packages/SVG/groups OK.

**Root causes:**
1. Portal start before KWin (blocking).
2. UCM `HiFi.conf` `Include.hdmi` → ACP probes broken HDMI devices → spam + no analog sink.

**Fix:** Drop HDMI include from shipped sof-essx8336 HiFi; broaden WP disable OR-rules; portals only from shell after Wayland (`--no-block`); wait/log for `alsa_output` sink; disable KWin overview plugin; verify-hook guards UCM.

**Live login:** casper user **`spike`** (not ubuntu); getty@tty1 autologin; `profile.d` execs `spike-session` on tty1. Ctrl+Alt+F2 for a text console.

**Package:** `spike-shell_0.0.18-1`.

**Try:** rebuild → boot should land on desktop as spike; Speakers sink (not null); quiet SOF; no ~76s hang.

---

## 2026-08-06 — Pre-rebuild hardening (0.0.17)

More before ISO rebuild (beyond 0.0.15/0.0.16 smoke fixes):

**Shell / diagnostics**
- Volume: pactl retries + `wpctl` fallback; live pkg `rtkit`
- Launcher: `.desktop` / favorites use `QIcon::fromTheme`
- Clock click → Settings Date & Time; HTIX-style touchpad detection
- Capture: audio probes via `runuser` as session user (`XDG_RUNTIME_DIR`)
- Appearance / Memory status wording honest (saved ≠ live panel apply)

**Live session / audio / portals**
- `XDG_CURRENT_DESKTOP=Spike:KDE` so `kde.portal` matches; start portal units from session + shell
- Live user groups: +`audio` +`pipewire`
- `libspa-0.2-bluetooth`; PipeWire live drop-in 44100Hz (`99-spike.conf`)
- SOF HDMI WP rule scoped to Essx8336 card (not all HDMI)
- Hook `0720-spike-verify-includes.chroot` fails build if critical includes empty / SVG engine missing

**Package:** `spike-shell_0.0.17-1`.

**Try:** rebuild → icons + launcher icons; Volume/`pactl`; quieter SOF; Mouse Apply honesty; clock→Date&Time; portals without ServiceUnknown.

---


**Previous ISO (`spike-capture-20260806T130212Z`, before 099c3c1 rebuild):**
- Worked: Breeze titlebars; session start; PipeWire units start; graceful path available.
- Looked OK but broken: panel “icons” were text (no `qt6-svg-plugins`); Mouse/Keyboard Apply only wrote flat `kcminputrc` keys KWin ignores → no live feel change.
- Broken: SOF HDMI mute file was empty → pcm6/7 spam; Sound/Volume testing unreliable on that image.
- Already fixed for next ISO: non-empty WP/SpikeDark, `qt6-svg-plugins`, pulse wait (0.0.15 / `099c3c1`).

**Now (0.0.16):** Mouse/Keyboard write Plasma-style `[Libinput][vid][pid]` sections, call KWin `reconfigure`, and status text says restart-session when live apply is uncertain.

---

## 2026-08-06 — Fix empty WP/SpikeDark includes; icons need qt6-svg-plugins (0.0.15)

**Smoke (`spike-capture-20260806T130212Z`):** Breeze titlebars OK. Panel stayed text (no Wi‑Fi/etc icons). Audio felt broken; journal still flooded with sof-essx8336 HDMI pcm6/7 ASoC -5.

**Root causes:**
1. `51-spike-sof-hdmi-disable.conf` and `SpikeDark.colors` were **0-byte** on the ISO — WirePlumber logged `Ignoring empty configuration file`.
2. Breeze icons are SVG; live image lacked **`qt6-svg-plugins`** (`libqsvgicon.so`), so `QIcon::fromTheme` could not paint.

**Fix:** Rewrite those includes; add `qt6-svg-plugins` (+ `alsa-utils`); shell 0.0.15 uses `breeze-dark` + SVG-aware theme setup; `spike-session` waits/logs `pipewire-pulse` + `pactl`.

**Try:** rebuild → icons on panel; quieter SOF HDMI; Volume/`pactl` works after session start.

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
