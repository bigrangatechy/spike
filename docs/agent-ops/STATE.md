# Spike Agent Ops — Current State

**Last updated:** 2026-08-05  
**Phase label:** Pre-alpha (documentation nearly complete; prototyping not started)  
**Note:** “Pre-alpha” remains the project label until Spike is ready for a beta release. Coding may begin under that label.

## Summary

| Area | Status |
| :-: | :-: |
| Top-level product specs | ✅ Largely complete |
| Root `LICENSE` + `LICENSE-CC-BY-SA-4.0` | ✅ Present (also under `licences/`) |
| `TRANSLATIONS.md` | 🔲 Not started (not a build blocker) |
| `USER-GUIDE.md` + `user-guide/` | 🔲 Stubs — fill as the desktop ships |
| `dev-guide/` | 📝 03/04/07 drafted (build + installer); rest stubs |
| agent-ops core (RULES, CONSTRAINTS, GLOSSARY, CONVENTIONS, PROTOCOLS) | ✅ Written |
| agent-ops remaining static helpers | 🔲 Empty stubs |
| ISO / build tooling | ✅ **live-build** (one hybrid ISO) on **Ubuntu Server 26.04 LTS** base |
| Installer stack | ✅ **Custom Qt Widgets** (`spike-installer`) — not Calamares |
| `build/iso-build/` + `scripts/build-iso.sh` | ✅ Scaffold/stub present (no working ISO yet) |
| Prototyping (base ISO, spike-config, shell, installer, rescue) | 🔲 Not started |

## In Progress

_None._

## Blocked / Waiting On Decision

_None for ISO/installer stack._

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-05 | Confirmed base pin: Ubuntu Server **26.04 LTS** (stripped) for development window |
| 2026-08-05 | Locked live-build + one ISO + custom Qt installer; thin dev-guide + build scaffold |
| 2026-08-05 | GitLab Markdown cleanup (fences, escapes, headings, `.org`→`.com`) |
| 2026-08-05 | Root CoC → pointer to `docs/CODE_OF_CONDUCT.md` |
| 2026-08-05 | Root `LICENSE` and `LICENSE-CC-BY-SA-4.0` added (copies of `licences/`) |
| 2026-08-05 | Initialized `STATE.md`, `DECISIONS.md`, `SESSION_LOG.md` |

## Next Suggested Work

1. Keep this file, `DECISIONS.md`, and `SESSION_LOG.md` current every session.  
2. When prototyping starts: flesh out live-build recipe under `build/iso-build/` until `build-iso.sh` produces a bootable image.  
3. Implement `spike-installer` / `spike-config` / Spike Shell against existing specs.  
4. Fill remaining `dev-guide/` stubs as subsystems are built.

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.  
- Move resolved blockers to Recently Completed; add new ones under Blocked.
