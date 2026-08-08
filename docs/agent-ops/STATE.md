# Spike Agent Ops — Current State

**Last updated:** 2026-08-08  
**Phase label:** Pre-alpha (implementation underway; Alpha gate not met yet)  
**Note:** Stay in **pre-alpha** until the **installer works end-to-end** (live → install → reboot to installed desktop). That is the BDFL gate to open **Alpha** — not a calendar date, not “shell boots,” not Stage counter. See `DECISIONS.md`.

## Summary

| Area | Status |
| :-: | :-: |
| Top-level product specs | ✅ Largely complete |
| Root `LICENSE` + `LICENSE-CC-BY-SA-4.0` | ✅ Present (also under `licences/`) |
| `TRANSLATIONS.md` | 🔲 Not started (not a build blocker) |
| `USER-GUIDE.md` + `user-guide/` | 🔲 Stubs — fill as the desktop ships |
| `dev-guide/` | 📝 03/04/07 drafted; 04 updated for Stage 1 |
| agent-ops core | ✅ Written |
| ISO / build tooling | ✅ live-build; hybrid remaster + debug capture |
| Installer stack | ✅ Custom Qt (not built yet) — Alpha cue when this works |
| `scripts/build-iso.sh` | ✅ Packages spike-config + spike-shell + spike-rescue + lb build |
| Stage 1 live ISO | ✅ Hardware boot + login |
| Stage 2 (`spike-config`) | ✅ Target detect OK (N4020 / ~4GB) |
| Stage 3 (Spike Shell) | 📝 **0.0.22** + Rescue **0.0.7** (debug REPORT + honest errors) |
| Stage 4 (installer) | 🔲 Pre-alpha until installer E2E — that gate opens **Alpha** |

## In Progress

| Item | Notes |
| :-: | :-: |
| Stage 3 — desktop smoke | Rebuild ISO: shell **0.0.22** + rescue **0.0.7** |

## Blocked / Waiting On Decision

| Item | Notes |
| :-: | :-: |
| Layer 2 GRUB recovery screen / Layer 4 installer restore | Rescue GUI MVP only for now |
| spike-migration (Move My Files) | Specced (`SPIKE-MIGRATION.md`); implement after rescue stable |
| Notification daemon / history tray | Prefs UI done; daemon not shipped |
| Magnifier / high-contrast theme apply | Accessibility prefs started; visuals later |
| APT edit / PPA / NVIDIA driver UX | Sources page lists + launches tools; in-page edit later |
| Custom Spike SVG icons | Inherit-only `spike-icons` for now (BRANDING overrides later) |
| Custom Spike Aurorae decorations | After Breeze works; BRANDING.md pixel match is follow-up |
| Plasma-ish / BRANDING desktop polish | Expected gap: custom QSS shell ≠ Plasma recolour; wallpaper, fonts, Aurorae, icons after smoke |
| Printer KCM QML / Sound Kirigami spam | Upstream noise; note only unless one-line fix |
| N4020 sof-essx8336 silence | Sink present; speakers still dead — separate from canberra |

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-08 | rescue **0.0.4** + shell **0.0.22**: 700/600 file reads; Dolphin dark kdeglobals |
| 2026-08-08 | spike-rescue **0.0.3**: dest = Spike USB `writable` partition |
| 2026-08-08 | Docs: SPIKE-MIGRATION.md + SPIKE-RECOVERY-TOOL-GENERAL.md (spec only) |
| 2026-08-08 | spike-rescue **0.0.2**: passwordless RO helper + Fedora btrfs home subvols |
| 2026-08-08 | spike-rescue 0.0.1: Layer 3 Rescue My Files (RO copy + SHA256) |
| 2026-08-08 | spike-shell 0.0.20 + config 0.0.4: Notifications / Accessibility / Software Sources started |
| 2026-08-08 | spike-shell 0.0.19: Appearance live panel; Memory/Boot forms; Language/Users/VPN pages |
| 2026-08-08 | A4 audio OK (`libcanberra-pulse`); detect 0.0.3 HW fill; version single-source |
| 2026-08-07 | spike-shell 0.0.18: live user spike + tty1 autologin → spike-session; drop sof HDMI UCM; non-blocking portals |
| 2026-08-06 | spike-shell 0.0.17: portals Spike:KDE, audio/pipewire groups, BT SPA, PW 44100 live drop-in, SOF scoped, volume/wpctl, launcher icons, verify-includes hook |
| 2026-08-06 | spike-shell 0.0.16: Libinput kcminputrc + honest Mouse/Keyboard Apply |
| 2026-08-06 | spike-shell 0.0.15: non-empty WP/SpikeDark, qt6-svg-plugins, pulse wait |
| 2026-08-06 | spike-shell 0.0.14: Date/Time, Keyboard, Mouse, Layout pages; Volume + Battery applets; spike-icons |
| 2026-08-06 | Pre-rebuild polish: breeze + plasma-integration + portal-kde + applications.menu + SpikeDark + SOF HDMI WP rule |
| 2026-08-06 | Smoke: session + Network/Settings + **graceful shutdown** (capture 20260806T115605Z) |
| 2026-08-06 | BDFL: pre-alpha → Alpha only when installer works E2E |
| 2026-08-06 | Full debug capture follow + defer PipeWire until spike-session (SOF spam) |
| 2026-08-06 | Docs updated for Network custom UI + Settings KCM allowlist |
| 2026-08-06 | NetworkManager tray applet + Settings Network page (D-Bus + nmcli, no plasma-nm) |
| 2026-08-06 | Settings category audit: every DESKTOP.md page present; Appearance wired to Config |
| 2026-08-06 | Live packages: kscreen, plasma-pa, powerdevil, bluedevil, print-manager, PipeWire |
| 2026-08-06 | In-window KCM host (`KCModuleLoader`) with Apply/Reset/Defaults |
| 2026-08-06 | `org.spike.Config` D-Bus service + Settings skeleton + Kickoff-style launcher |
| 2026-08-06 | Live smoke: shutdown OK; apps OK; htop needed Terminal=true wrap |
| 2026-08-06 | Align live package list with KDE standalone apps (CONSTRAINTS.md) |
| 2026-08-06 | Session input fixed (seatd/groups/cursors); launcher `.desktop` scan; power via sudo/polkit |
| 2026-08-06 | spike-shell white-on-dark QSS + `.deb` wired into live ISO |
| 2026-08-06 | Stage 3 Spike Shell skeleton (`src/spike-shell/`, builds) |
| 2026-08-06 | Pre-alpha kept through shell/installer; Alpha by feel (installer cue) |
| 2026-08-06 | Target detect confirmed: Celeron N4020 / ~4GB |
| 2026-08-06 | Detect no longer baked from build host; capture + debug GRUB |
| 2026-08-06 | spike-config packaged into live ISO |
| 2026-08-05 | Stage 1 live ISO path (firmware, UEFI hybrid) |

## Next Suggested Work

1. Rebuild ISO + smoke: Rescue My Files + Settings/OSK; A4 audio.  
2. Next: installer (Alpha gate) or Layer 2/4 recovery polish.  
3. Stage 4 (still pre-alpha): custom installer until it works E2E.  
4. Open **Alpha** when installer E2E is confirmed (BDFL gate).

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
