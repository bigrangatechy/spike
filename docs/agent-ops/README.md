# Spike — Agent Operations Reference

## What Is This Folder?

This folder contains the operational reference for AI agents working on the Spike Linux distribution. It's the **procedural layer** that sits alongside the technical specifications.

### Specs vs. Operations

| **Layer** | **Purpose** | **Stability** | **Location** |
| :-: | :-: | :-: | :-: |
| **Specifications** | What to build, technical details | Stable, updated as discoveries are made | Root docs (README.md, MEMORY.md, etc.) |
| **Operations** | How to build, workflows, rules | Evolving, updated as processes are refined | This folder (`agent-ops/`) |

> **Rule:** If the spec and the operations conflict, the spec wins. The spec is the source of truth; this folder is the method.

## Who Reads This?

- **AI agents** — The primary audience. An agent starts here before doing any work. 

- **Human contributors** — To understand the development process, not just the technical specs. 

- **Future maintainers** — To onboard new humans or agents years from now. 

## Reading Order

New agents (and humans) should read in this order:

```
`1. README.md          → This file (orientation)`

`2. RULES.md           → Core workflow rules (documentation-first mandate)`

`3. CONSTRAINTS.md     → Hard technical constraints (never do X)`

`4. GLOSSARY.md        → Project terminology`

`5. STATE.md           → Current progress (what's done, in progress, blocked)`

`6. SESSION\_LOG.md     → Recent sessions (what happened last time)`

`7. DECISIONS.md       → Past decisions (what was settled, why)`

`8. CONVENTIONS.md     → Code style, naming, commit format`

`9. PROTOCOLS.md       → Step-by-step procedures for common situations`

`10. PATTERNS.md       → Approved implementation patterns`

`... → Then task-specific files as needed`
```

After initial orientation, read only the files relevant to your current task. Don't read everything every time.

## File Structure

```
`agent-ops/`

`├── README.md              → Entry point (this file)`

`├── RULES.md               → Workflow rules`

`├── CONSTRAINTS.md         → Hard technical constraints`

`├── PREFERENCES.md         → Soft preferences`

`├── CONVENTIONS.md         → Code style, naming, commits`

`├── PROTOCOLS.md           → Step-by-step procedures`

`├── PATTERNS.md            → Approved implementation patterns`

`├── TEMPLATES.md           → Fill-in structures for docs/specs`

`├── GLOSSARY.md            → Project terminology`

`├── GOTCHAS.md             → Known traps, easy mistakes`

`├── SELF-CHECK.md          → Pre-completion checklist`

`├── QUICK-REF.md           → One-page cheat sheet`

`├── FAQ.md                 → Common questions with answers`

`├── TROUBLESHOOTING.md     → What to try when stuck`

`├── SECURITY-CHECKLIST.md  → Privacy/security review items`

`├── MISTAKES.md            → Catalog of mistakes (grows over time)`

`├── CHANGELOG.md           → Changelog for agent-ops itself`

`│`

`├── STATE.md               → Dynamic: progress tracker`

`├── DECISIONS.md           → Dynamic: decision log`

`├── SESSION\_LOG.md         → Dynamic: session history`

`│`

`└── adapters/`

`    ├── .cursorrules       → Cursor IDE adapter`

`    ├── CLAUDE-CODE.md     → Future: Claude Code adapter`

`    └── AIDER.md           → Future: Aider adapter`
```

### Static vs. Dynamic Files

| **Type** | **Updates When** | **Examples** |
| :-: | :-: | :-: |
| **Static** | Only when process changes | RULES.md, CONSTRAINTS.md, CONVENTIONS.md |
| **Dynamic** | Updated every session | STATE.md, DECISIONS.md, SESSION\_LOG.md |

Static files are rarely touched. Dynamic files are the **running record** of what's happening.

## Key Principles

1. **Specification First.** Never implement a feature without a spec section. If it doesn't exist, draft it first (see `PROTOCOLS.md`).

2. **Approvals Matter.** Drafts must be approved before implementation. No silent changes.

3. **State Always Updates.** Check `STATE.md` at task start. Update `STATE.md` at task end.

4. **No Silent Deviations.** If you deviate from a spec (even slightly), log it in `SESSION\_LOG.md` and notify the human.

5. **Tool-Agnostic Core.** This folder is not tied to any specific tool. `adapters/` contains tool-specific wrappers.

6. **Living Documentation.** `GOTCHAS.md`, `MISTAKES.md`, and `DECISIONS.md` grow organically. Add to them after every incident or decision.

7. **Pre-Completion Check.** Before claiming any task is done, run `SELF-CHECK.md`.

## Relationship to Main Docs

```
`Main Specs (ROOT/)         agent-ops/`

`├── INDEX.md               │`

`├── AGENTS.md              │    ┌─────────────────┐`

`├── MEMORY.md              │    │ OPERATIONAL     │`

`├── CONFIGURATION.md       │───▶│ REFERENCE       │`

`├── DESIGN-DECISIONS.md    │    │ (this folder)   │`

`└── ...                    │    └─────────────────┘`
```

Main specs define **what**. `agent-ops/` defines **how**.

Main specs are authored by humans. `agent-ops/` is used by both humans and agents.

When implementing a feature:

1. Read the relevant spec(s) in root (`MEMORY.md` for memory module, etc.) 

2. Read `agent-ops/CONSTRAINTS.md` for hard limits 

3. Read `agent-ops/PROTOCOLS.md` for step-by-step procedure 

4. Read `agent-ops/PATTERNS.md` for approved code patterns 

5. Implement 

6. Run `agent-ops/SELF-CHECK.md` 

7. Update `agent-ops/STATE.md` and `agent-ops/SESSION\_LOG.md` 

## Quick Links

- [Workflow Rules](https://lumo.proton.me/RULES.md) — The documentation-first mandate 

- [Hard Constraints](https://lumo.proton.me/CONSTRAINTS.md) — Things that must never happen 

- [Code Conventions](https://lumo.proton.me/CONVENTIONS.md) — Style, naming, commits 

- [Procedures](https://lumo.proton.me/PROTOCOLS.md) — What to do when X happens 

- [Implementation Patterns](https://lumo.proton.me/PATTERNS.md) — How to write Y code 

- [Current State](https://lumo.proton.me/STATE.md) — What's being worked on now 

- [Decision Log](https://lumo.proton.me/DECISIONS.md) — What was decided and why 

## For Humans

If you're a human contributor (developer, maintainer):

- Read `RULES.md` and `CONSTRAINTS.md` to understand the project's non-negotiables 

- Check `STATE.md` to see what's in progress 

- Update `DECISIONS.md` whenever you make a decision that affects future work 

- Add to `GOTCHAS.md` if you discover a trap worth documenting 

- Add to `MISTAKES.md` if you spot an error worth learning from 

## For Agents

If you're an AI agent:

1. Start by reading this file (you're already doing that) 

2. Read `RULES.md` next — it's the most important file 

3. Check `STATE.md` to find your current task 

4. Read the spec(s) relevant to your task 

5. Follow the procedure in `PROTOCOLS.md` 

6. Use the patterns in `PATTERNS.md` 

7. Run `SELF-CHECK.md` before finishing 

8. Update `STATE.md` and `SESSION\_LOG.md` after completion 

## Version History

| **Date** | **Change** | **Author** |
| :-: | :-: | :-: |
| 2026-07-XX | Initial creation | BDFL |

(Use `CHANGELOG.md` for ongoing version history.)

🐕 BigRangaTech

