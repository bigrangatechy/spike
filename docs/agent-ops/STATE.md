# Spike Agent Ops — Current State

**Last updated:** 2026-08-06  
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
| `scripts/build-iso.sh` | ✅ Packages spike-config + spike-shell + lb build |
| Stage 1 live ISO | ✅ Hardware boot + login |
| Stage 2 (`spike-config`) | ✅ Target detect OK (N4020 / ~4GB) |
| Stage 3 (Spike Shell) | 📝 0.0.13: defer PipeWire to session; full debug capture |
| Stage 4 (installer) | 🔲 Pre-alpha until installer E2E — that gate opens **Alpha** |

## In Progress

| Item | Notes |
| :-: | :-: |
| Stage 3 — Spike Shell | Rebuild; smoke TTY login (no audio spam) + spike-session + Network |

## Blocked / Waiting On Decision

| Item | Notes |
| :-: | :-: |
| Language / Keyboard / Mouse / Users / Date & Time / Accessibility | Spike custom pages (Plasma KCMs not shipped) — flesh UI next |
| SOF HDMI pcm7 probe errors | WirePlumber may still retry in-session; tune monitor later if spam returns |

## Recently Completed

| Date | Item |
| :-: | :-: |
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

1. Rebuild ISO + smoke Network tray (scan/connect/disconnect) and Settings → Network.  
2. Smoke Display/Sound/Power/Bluetooth/Printer KCMs.  
3. Flesh Plasma-coupled stubs (Language, Keyboard, Mouse, Users, Date & Time, Accessibility).  
4. Stage 4 (still pre-alpha): custom installer until it works E2E.  
5. Open **Alpha** when installer E2E is confirmed (BDFL gate).

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
