# Spike Agent Ops — Current State

**Last updated:** 2026-08-09  
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
| Installer stack | ✅ **0.0.11** E2E smoke green; first-boot markers; shell **0.0.32** lock/inhibit |
| `scripts/build-iso.sh` | ✅ Packages config + shell + rescue + installer + **migration** + lb build |
| Stage 1 live ISO | ✅ Hardware boot + login |
| Stage 2 (`spike-config`) | ✅ **0.0.11** (KERNEL.md module blacklist from detect) |
| Stage 3 (Spike Shell) | 📝 **0.0.32** lock + sleep inhibit + brightness live + Rescue **0.0.12** + migration **0.0.3** |
| Stage 4 (installer) | ✅ **0.0.11** — E2E install + blacklist + locale; first-run on next ISO |
| Preinstalled apps | 🔲 Alpha next — Firefox / media / email (see below) |

## In Progress

| Item | Notes |
| :-: | :-: |
| Alpha — preinstalled software | Seed browser, media player, email (Flatpak per AGENTS.md); see Default apps |
| Stage 3 — first-run polish | Wizard shipped **0.0.31**; tour/a11y/Flatpak/update hooks still placeholders |
| Rebuild smoke | Shell **0.0.32**: lock, block-sleep switch, brightness via logind; first-run still needs ISO smoke |

## Default apps (Alpha — planned)

**Package policy (2026-08-09):** Discover supports **Flatpak and `.deb`**. Spike system software is always `.deb`. Snap forbidden. See `DESIGN-DECISIONS.md`.

Spec sources: `AGENTS.md`, `DESKTOP.md` (VLC, Discover, Internet/Media categories).

| Role | Spec intent | On ISO today | Alpha plan |
| :-: | :-: | :-: | :-: |
| Browser | Firefox + Spike prefs | ❌ | Prefer **`.deb`** `firefox` on Tier‑1; Flatpak still available in Discover |
| Media player | VLC in Media category | ❌ | Prefer **`.deb`** `vlc` (or Flatpak if deb too heavy) |
| Email | **Thunderbird** (locked) | ❌ | Prefer **`.deb`** `thunderbird` |
| Office | LibreOffice | ❌ | Flatpak or `.deb`; seed after browser works on 4GB |
| Files / editor / terminal | Dolphin, Kate, Konsole | ✅ `.deb` | Keep |
| Software center | Discover | ✅ `.deb` | Keep; Flatpak + apt backends |
| Spike Tools | Install / Rescue / Move My Files | ✅ `.deb` | Keep |

Implementation sketch: add `firefox`, `vlc`, `thunderbird` to `spike-live.list.chroot` (and/or install-time seed); Flatpak runtimes still preseed per variant for third-party apps.

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

1. **Preinstalled apps:** add Firefox + VLC + **Thunderbird** as **`.deb`** where possible; keep Flatpak runtimes for Discover third-party apps.  
2. Rebuild ISO with shell **0.0.32** + installer **0.0.11** → smoke lock, brightness, block-sleep, first-run.  
3. Fill first-run placeholder hooks (tour / Flatpak verify / update check) as apps land.  
4. spike-migration inventory/conflict UI (not Alpha-blocking).  

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
