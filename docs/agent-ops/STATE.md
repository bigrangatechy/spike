# Spike Agent Ops — Current State

**Last updated:** 2026-08-06  
**Phase label:** Pre-alpha (Stage 1 done; Stage 2 skeleton in progress)  
**Note:** “Pre-alpha” remains the project label until Spike is ready for a beta release. Early implementation stages (1–2+) stay under that label for now — **Phase 2 (Alpha)** in `ROADMAP.md` is not declared open yet.

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
| Stage 2 (`spike-config`) — still pre-alpha | 📝 Skeleton: CLI + templates + modules (not packaged into ISO yet) |
| Stage 3+ (shell, installer) | 🔲 Not started |

## In Progress

| Item | Notes |
| :-: | :-: |
| Stage 2 — `src/spike-config/` | Working Python engine: state, templates, generate (memory/boot/network/multimedia/security), CLI, tests |

## Blocked / Waiting On Decision

_None._

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-06 | Stage 2 skeleton: `src/spike-config/` (CLI, templates, modules, tests) |
| 2026-08-06 | Stage 2 kept under **pre-alpha** for now (Alpha phase not opened yet) |
| 2026-08-06 | Boot tagline: "Let's make tech repairable again" |
| 2026-08-05 | Stage 1 recipe: auto/config (resolute), package list, strip hook, `.recipe-ready` |
| 2026-08-05 | `build-iso.sh` real lb clean/config/build path |
| 2026-08-05 | Confirmed base pin: Ubuntu Server **26.04 LTS** (stripped) |
| 2026-08-05 | Locked live-build + one ISO + custom Qt installer |

## Next Suggested Work

1. Stage 2 deepen: UFW/sudo templates, real detect hooks, Debian package + ISO install.  
2. Stage 3: minimal Spike Shell.  
3. Stage 4: installer unpack squashfs → ext4.  
4. Declare **Phase 2 (Alpha)** later when core stack is far enough — not gated on Stage 2 alone.

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.
