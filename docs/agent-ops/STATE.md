# Spike Agent Ops — Current State

**Last updated:** 2026-08-12  
**Phase label:** **Alpha** (installer E2E met — live → install → reboot to installed desktop)  
**Note:** Pre-alpha gate from `DECISIONS.md` is closed. Alpha focus: Tier‑1 desktop completeness (preinstalled apps, first-run polish, hardware matrix). Beta is still later. See `ROADMAP.md` Phase 2.

## Summary

| Area | Status |
| :-: | :-: |
| Top-level product specs | ✅ Largely complete |
| Root `LICENSE` + `LICENSE-CC-BY-SA-4.0` | ✅ Present (also under `licences/`) |
| `TRANSLATIONS.md` | 🔲 Not started (not a build blocker) |
| `USER-GUIDE.md` + `user-guide/` | 🔲 Stubs — fill as the desktop ships |
| `dev-guide/` | 📝 Core filled (01–09, 12, 19); remaining stubs point at product docs |
| agent-ops core | ✅ Written |
| ISO / build tooling | ✅ live-build; hybrid remaster + debug capture |
| Installer stack | ✅ **0.0.26** + shell **0.0.51** + rescue **0.0.15** + branding **0.0.2** + config **0.0.13** |
| `scripts/build-iso.sh` | ✅ Packages config + **branding** + shell + rescue + installer + migration + lb build |
| Stage 1 live ISO | ✅ Hardware boot + login |
| Stage 2 (`spike-config`) | ✅ **0.0.13** GRUB theme when theme.txt present (font optional) |
| Stage 3 (Spike Shell) | 📝 **0.0.51** Fn volume/brightness via evdev; greeter stay-visible; kglobalacceld for Meta keys |
| Stage 4 (installer) | ✅ **0.0.25** — Plymouth/GRUB theme activate; writable backup dest |
| Preinstalled apps | 📝 Seeded: Mozilla FF/TB `.deb` + VLC + LibreOffice; Discover Flatpak/AppStream |

## In Progress

| Item | Notes |
| :-: | :-: |
| Rebuild smoke | shell **0.0.51** Fn keys; branding **0.0.2** Plymouth via alternatives; backup+restore with test files |
| Spike APT host | Waiting on hosting hardware — plumbing shipped (`UPDATES.md`, Enabled: no) |
| Stage 3 — first-run polish | Wizard shipped **0.0.31**; tour/a11y/Flatpak/update hooks still placeholders |

## Default apps (Alpha — planned)

**Package policy (2026-08-09):** Discover supports **Flatpak and `.deb`**. Spike system software is always `.deb`. Snap forbidden. See `DESIGN-DECISIONS.md`.

Spec sources: `AGENTS.md`, `DESKTOP.md` (VLC, Discover, Internet/Media categories).

| Role | Spec intent | On ISO today | Alpha plan |
| :-: | :-: | :-: | :-: |
| Browser | Firefox + Spike prefs | 📝 next ISO — Mozilla `.deb` | Seeded via `packages.mozilla.org` (not Ubuntu Snap stub) |
| Media player | VLC in Media category | 📝 next ISO — Ubuntu `.deb` | Seeded |
| Email | **Thunderbird** (locked) | 📝 next ISO — Mozilla `.deb` | Suite `thunderbird-deb` on packages.mozilla.org |
| Office | LibreOffice | 📝 next ISO — Ubuntu `.deb` + qt6/kf6/plasma | Seeded (size accepted) |
| Files / editor / terminal | Dolphin, Kate, Konsole | ✅ `.deb` | Keep |
| Software center | Discover | ✅ `.deb` | Keep; Flatpak + apt backends |
| Spike Tools | Install / Rescue / Move My Files | ✅ `.deb` | Keep |

Implementation: `spike-live.list.chroot` + `config/spike-archives/` (staged by `build-iso.sh`) + `includes.chroot/etc/apt/` for Mozilla sources/pinning.

**Future:** BDFL intends lighter-weight replacements for these defaults later (same roles; not Alpha-blocking). See `DECISIONS.md`.

## Blocked / Waiting On Decision

| Item | Notes |
| :-: | :-: |
| Layer 2 GRUB recovery screen | Still deferred |
| spike-migration full wizard | Wizard shell **0.0.3**; inventory/conflict/old-disk Mode B later |
| Notification daemon / history tray | Prefs + fdo Notifications + tray (**0.0.27**); retention polish later |
| Magnifier / high-contrast theme apply | Magnifier + HC chrome live; full app themes later |
| APT edit / PPA / NVIDIA driver UX | Sources page lists + launches tools; in-page edit later |
| Custom Spike SVG icons / Aurorae | Follow-up branding polish |
| N4020 sof-essx8336 silence | Sink present; speakers still dead — separate track |

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-12 | installer **0.0.26** + branding **0.0.2**: Plymouth via update-alternatives (no plymouth-set-default-theme) |
| 2026-08-12 | shell **0.0.51**: Fn volume/brightness via `/dev/input` evdev (not KWin XF86 binds) |
| 2026-08-12 | shell **0.0.50** + branding **0.0.1** + installer **0.0.25** + config **0.0.13**: greeter/Fn/logos |
| 2026-08-12 | installer **0.0.24**: SpikeBackup to writable partition root (not `/var/log`); Layer 4 SESSION_PATH drain |
| 2026-08-12 | rescue **0.0.15** + installer **0.0.23**: fix list-systems QEventLoop hang (empty OS list) |
| 2026-08-12 | rescue **0.0.14** + installer **0.0.22**: backup scan finds installed OS reliably |
| 2026-08-12 | shell **0.0.49**: Fn/media keys via KWin spike-shortcuts + kglobalacceld |
| 2026-08-12 | shell **0.0.48**: greeter no longer wiped by getty TTYReset race |
| 2026-08-12 | installer **0.0.21**: Step 7 disk scan only when backup checkbox checked |
| 2026-08-10 | shell **0.0.41** + installer **0.0.16**: USB save-logs; night-light.log; install-from-live.log on target |
| 2026-08-10 | installer **0.0.14** + shell **0.0.39**: Step 7 backup on wipe disk; NM keyfiles; auto-login opt-in; night light; Sleep |
| 2026-08-10 | ISO: drop `packagekit-tools` (not on resolute); shell **0.0.38** uses `pkgcli refresh` |
| 2026-08-10 | shell **0.0.37**: disable idle kscreenlocker; Spike greeter QML on shell path; night light kwinrc+preview |
| 2026-08-10 | shell **0.0.36**: panel task icons (KWin), Discover Flatpak/PackageKit refresh, plasma-systemmonitor |
| 2026-08-10 | shell **0.0.35** + installer **0.0.13**: home seed, Discover/AppStream/Flatpak, LO profile dirs, Spike lockscreen QML |
| 2026-08-10 | shell **0.0.34**: Mozilla Firefox/Thunderbird AppArmor + profile-home fix |
| 2026-08-10 | config **0.0.12** + shell **0.0.33** + installer **0.0.12**: power/input live apply; Wi‑Fi NM handoff |
| 2026-08-09 | Default apps seeded: Mozilla FF/TB `.deb` + VLC + LibreOffice (package list + APT archives) |
| 2026-08-09 | shell **0.0.32**: Spike lock + block sleep/locking + brightness logind live |
| 2026-08-09 | Default email: **Thunderbird**; apps policy Flatpak+`.deb`; Alpha opened |
| 2026-08-09 | shell **0.0.31** + installer **0.0.11**: post-install first-run wizard |
| 2026-08-09 | installer **0.0.10** + rescue **0.0.12**: fix variant→backup freeze |
| 2026-08-09 | installer **0.0.9**: full timezone/language/keyboard lists |
| 2026-08-09 | installer **0.0.8** + config **0.0.11** + migration **0.0.3**: blacklist; Step 7; Move wizard |
| 2026-08-09 | installer **0.0.7** + shell **0.0.30** + config **0.0.10**: DRM groups/seatd; live Apply |
| 2026-08-09 | installer **0.0.6** + rescue **0.0.11** + config **0.0.9**: non-casper boot; exclude wipe disk |
| 2026-08-06 | BDFL: pre-alpha → Alpha only when installer works E2E (**met**) |

## Next Suggested Work

1. Rebuild ISO → smoke: Plymouth splash (no WARN); Fn keys; backup+restore with test files on the wipe disk.  
2. **Installer UX pass (queued):** keep-USB/restore prompts; static Finish/reboot UI (non-debug); installed-system debug mode — see `INSTALLER.md` § UX pass backlog.  
3. Confirm first-run wizard + brightness / block-sleep.  
4. spike-migration inventory/conflict UI (not Alpha-blocking).  
5. Desktop panel Spike emblem (greeter logo shipped; panel still start-here-kde).  

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
