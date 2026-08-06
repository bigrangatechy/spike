# Spike Agent Ops — Current State

**Last updated:** 2026-08-06  
**Phase label:** Pre-alpha (Stage 2 detect OK; shell/installer still pre-alpha)  
**Note:** “Pre-alpha” stays until BDFL opens Alpha **by feel** — target cue: installer works end-to-end. Shell and further stack work remain under pre-alpha for now.

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
| Stage 3 (Spike Shell) | 📝 Skeleton packaged for ISO; white-on-dark theme; layer-shell later |
| Stage 4 (installer) | 🔲 Later — still **pre-alpha** until BDFL opens Alpha |

## In Progress

| Item | Notes |
| :-: | :-: |
| Stage 3 — Spike Shell | On ISO path; deepen layer-shell / app scan after live smoke |

## Blocked / Waiting On Decision

_None._ Alpha timing is intentional “by feel,” not blocked.

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-06 | spike-shell white-on-dark QSS + `.deb` wired into live ISO |
| 2026-08-06 | Stage 3 Spike Shell skeleton (`src/spike-shell/`, builds) |
| 2026-08-06 | Pre-alpha kept through shell/installer; Alpha by feel (installer cue) |
| 2026-08-06 | Target detect confirmed: Celeron N4020 / ~4GB |
| 2026-08-06 | Detect no longer baked from build host; capture + debug GRUB |
| 2026-08-06 | spike-config packaged into live ISO |
| 2026-08-05 | Stage 1 live ISO path (firmware, UEFI hybrid) |

## Next Suggested Work

1. Rebuild ISO + smoke `spike-shell` on live (white panel text; optional `spike-session` / KWin).  
2. Stage 3 deepen: layer-shell panel, `.desktop` scanning.  
3. Stage 4 (pre-alpha): installer squashfs → ext4.  
4. Open **Alpha** only when installer feels ready (BDFL).

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
