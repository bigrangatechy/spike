# Spike Agent Ops — Session Log

Append-only. Newest sessions at the **top**.

---

## 2026-08-05 — Lock live-build + one ISO + custom installer

**Participants:** BDFL + agent (Cursor)

**Done:**
- Decision: ISO via **live-build**; **one** hybrid live ISO; Standard/Plus at **install time**; installer = **custom Qt** (not Calamares)
- Base confirmed: **Ubuntu Server 26.04 LTS** (stripped), development window before 28.04
- Logged in `DECISIONS.md`; updated `STATE.md`
- Fixed stale dual-ISO / Calamares wording in ARCHITECTURE, CONTRIBUTING, INDEX, AGENTS, DESIGN-DECISIONS
- Drafted `dev-guide/03-build-environment.md`, `04-building-spike.md`, `07-installer-internals.md`
- Scaffolded `build/iso-build/` and stub `scripts/build-iso.sh`

**Open:**
- Working ISO not yet produced (scaffold only)
- Remaining `dev-guide/` / `user-guide` / agent-ops stubs

**Next:**
- Prototype: fill live-build recipe and produce first bootable image when BDFL starts Phase 3

---

## 2026-08-05 — Docs cleanup + license root + agent-ops init

**Participants:** BDFL + agent (Cursor)

**Done:**
- Fixed GitLab Markdown rendering across docs (fence backticks, escapes, root README headings/fences, domain `.org`→`.com` for BigRangaTech only)
- Matrix room normalized to `#spike-linux:matrix.org`
- Docs license corrected to CC-BY-SA 4.0 where it wrongly said GPLv2+
- Root CoC reduced to pointer → `docs/CODE_OF_CONDUCT.md`
- Copied license texts to root: `LICENSE`, `LICENSE-CC-BY-SA-4.0` (kept `licences/`)
- Initialized `STATE.md`, `DECISIONS.md`, this `SESSION_LOG.md`
- Began refreshing stale status in AGENTS/INDEX/READMEs

**Open:**
- ISO build tooling / installer implementation stack undecided _(resolved in later session same day)_
- `TRANSLATIONS.md`, user-guide, most dev-guide, remaining agent-ops stubs still empty (intentional for now)

**Next:**
- Keep STATE / DECISIONS / SESSION_LOG current
- Finish status-doc sync this session
- Prototyping when BDFL starts; choose build tooling when ready
