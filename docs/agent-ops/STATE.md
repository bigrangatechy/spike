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
| `dev-guide/` | 🔲 Stubs — fill as implementation proceeds |
| agent-ops core (RULES, CONSTRAINTS, GLOSSARY, CONVENTIONS, PROTOCOLS) | ✅ Written |
| agent-ops remaining static helpers | 🔲 Empty stubs |
| ISO / build tooling choice | ⏳ Open — BDFL undecided (live-build vs custom, etc.) |
| Installer stack choice | ⏳ Open — Calamares vs custom (product flow is specified) |
| Source / ISO / CI scaffolding | 🔲 Empty stubs only |
| Prototyping (base ISO, spike-config, shell, installer, rescue) | 🔲 Not started |

## In Progress

_None._

## Blocked / Waiting On Decision

| Item | Waiting on | Notes |
| :-: | :-: | :-: |
| First ISO build path | BDFL | Build system not chosen yet; product specs do not depend on it |
| Installer implementation tech | BDFL | `INSTALLER.md` defines UX/flow; packaging tool TBD |

## Recently Completed

| Date | Item |
| :-: | :-: |
| 2026-08-05 | GitLab Markdown cleanup (fences, escapes, headings, `.org`→`.com`) |
| 2026-08-05 | Root CoC → pointer to `docs/CODE_OF_CONDUCT.md` |
| 2026-08-05 | Root `LICENSE` and `LICENSE-CC-BY-SA-4.0` added (copies of `licences/`) |
| 2026-08-05 | Initialized `STATE.md`, `DECISIONS.md`, `SESSION_LOG.md` |

## Next Suggested Work

1. Choose ISO build approach when ready (no rush — document the decision in `DECISIONS.md` + thin `dev-guide` pages).  
2. Keep this file, `DECISIONS.md`, and `SESSION_LOG.md` current every session.  
3. Refresh stale status lists in `AGENTS.md` / `INDEX.md` when docs change (done this session for known drift).  
4. Begin prototyping against existing specs when BDFL starts Phase 3 work.

## How To Update This File

- At **session start:** read this file.  
- At **session end:** update Summary / In Progress / Recently Completed.  
- Move resolved blockers to Recently Completed; add new ones under Blocked.
