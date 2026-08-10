# Spike Agent Ops — Current State

**Last updated:** 2026-08-10  
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
| Installer stack | ✅ **0.0.17** + shell **0.0.43** (ISO runtime Depends + greeter + USB) |
| `scripts/build-iso.sh` | ✅ Packages config + shell + rescue + installer + **migration** + lb build |
| Stage 1 live ISO | ✅ Hardware boot + login |
| Stage 2 (`spike-config`) | ✅ **0.0.12** power profile live apply (governor + Wi‑Fi powersave) |
| Stage 3 (Spike Shell) | 📝 **0.0.43** full runtime Depends + UDisks/greeter; Rescue **0.0.12** |
| Stage 4 (installer) | ✅ **0.0.17** — greeter when auto-login off; wipe-disk backup; Wi‑Fi keyfiles |
| Preinstalled apps | 📝 Seeded: Mozilla FF/TB `.deb` + VLC + LibreOffice; Discover Flatpak/AppStream |

## In Progress

| Item | Notes |
| :-: | :-: |
| Rebuild smoke | installer **0.0.17** + shell **0.0.43**: verify hook + USB + cold-boot greeter |
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

1. Rebuild ISO → smoke fixes + **Copy Spike Logs to USB** → collect `spike-logs-*` on host.  
2. Confirm first-run wizard + brightness / block-sleep.  
3. Fill first-run placeholder hooks as apps land.  
4. spike-migration inventory/conflict UI (not Alpha-blocking).  

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
