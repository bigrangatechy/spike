# Spike Agent Ops — Current State

**Last updated:** 2026-08-05  
**Phase label:** Pre-alpha (Stage 1 prototyping: live ISO)  
**Note:** “Pre-alpha” remains the project label until Spike is ready for a beta release.

## Summary

| Area | Status |
| :-: | :-: |
| Top-level product specs | ✅ Largely complete |
| Root `LICENSE` + `LICENSE-CC-BY-SA-4.0` | ✅ Present (also under `licences/`) |
| `TRANSLATIONS.md` | 🔲 Not started (not a build blocker) |
| `USER-GUIDE.md` + `user-guide/` | 🔲 Stubs — fill as the desktop ships |
| `dev-guide/` | 📝 03/04/07 drafted; 04 updated for Stage 1 |
| agent-ops core | ✅ Written |
| ISO / build tooling | ✅ live-build; **Stage 1 recipe ready** (`build/iso-build/.recipe-ready`) |
| Installer stack | ✅ Custom Qt (not built yet) |
| `scripts/build-iso.sh` | ✅ Implements lb clean/config/build |
| Stage 1 live ISO artifact | ✅ Tier 1 firmware ISO: devices detected + login on hardware |
| Stage 2+ (spike-config, shell, installer) | 🔲 Not started |

## In Progress

| Item | Notes |
| :-: | :-: |
| Stage 1 — first `lb build` | Recipe + wrapper landed; run `sudo ./scripts/build-iso.sh` on a 26.04 host |

## Blocked / Waiting On Decision

_None._

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-05 | Stage 1 recipe: auto/config (resolute), package list, strip hook, `.recipe-ready` |
| 2026-08-05 | `build-iso.sh` real lb clean/config/build path |
| 2026-08-05 | Confirmed base pin: Ubuntu Server **26.04 LTS** (stripped) |
| 2026-08-05 | Locked live-build + one ISO + custom Qt installer |

## Next Suggested Work

1. Stage 1 closed on hardware — next: Stage 2 `spike-config` skeleton (or more firmware only if a chip class is still missing).
2. Stage 2: `src/spike-config/` skeleton.  
3. Stage 3: minimal Spike Shell.  
4. Stage 4: installer unpack squashfs → ext4.

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
