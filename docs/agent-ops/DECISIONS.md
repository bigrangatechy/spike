# Spike Agent Ops — Decision Log

Operational decisions that affect how work proceeds. Product/architecture rationale remains in `docs/DESIGN-DECISIONS.md`. Log new entries at the **top** (newest first).

**Last updated:** 2026-08-05

---

## 2026-08-05 — Root Code of Conduct is a pointer

**Decision:** Root `CODE_OF_CONDUCT.md` is a short pointer to canonical `docs/CODE_OF_CONDUCT.md`.

**Why:** Keep GitLab’s expected root CoC path, avoid duplicate full text and broken relative links from root.

**Refs:** `CODE_OF_CONDUCT.md`, `docs/CODE_OF_CONDUCT.md`

---

## 2026-08-05 — Documentation license is CC-BY-SA 4.0

**Decision:** Code = GPLv2+; documentation and branding/artwork = CC-BY-SA 4.0. Correct any docs that said documentation was GPLv2+.

**Why:** Align with `AGENTS.md` / `DESIGN-DECISIONS.md`; README had drifted.

**Refs:** `LICENSE`, `LICENSE-CC-BY-SA-4.0`, `licences/`

---

## 2026-08-05 — License texts at repo root

**Decision:** Place full license texts at repository root as `LICENSE` (GPLv2+) and `LICENSE-CC-BY-SA-4.0`, while keeping organized copies under `licences/`.

**Why:** Conventional root discovery for tools/humans; folder retained for organization.

**Refs:** `LICENSE`, `LICENSE-CC-BY-SA-4.0`, `licences/GPL-2.0-or-later.md`, `licences/CC BY-SA 4.0.md`

---

## 2026-08-05 — Matrix room name

**Decision:** Official Matrix room is `#spike-linux:matrix.org` (not `#spike:matrix.org`).

**Refs:** `docs/SUPPORT.md`, `docs/FAQ.md`, `docs/GOVERNANCE.md`

---

## 2026-08-05 — BigRangaTech domains use .com

**Decision:** All BigRangaTech emails/domains use `.com` (e.g. `bigrangatech.com`). Third-party `.org` hosts unchanged.

**Refs:** agent-ops email examples; public URLs already mostly `.com`

---

## 2026-08-05 — Pre-alpha label meaning

**Decision:** The project stays labeled **pre-alpha** until it is ready for a **beta** release. Pre-alpha is the release label, not a hard gate that forbids starting implementation once specs are sufficient.

**Why:** Clarified by BDFL. User/dev guide stubs are filled as the desktop is built.

**Implication:** Formal “all 70 docs before any code” checklist in `ROADMAP.md` should be treated as aspirational structure, not a stop-work order; update ROADMAP when BDFL wants the written exit criteria to match this.

**Refs:** `docs/ROADMAP.md`, `docs/AGENTS.md`, this file

---

## 2026-08-05 — ISO build tooling still open

**Decision:** *No decision yet.* live-build vs custom (and Calamares vs custom installer implementation) remain TBD.

**Why:** BDFL not sure what is best yet. Product behavior is specified; packaging choice deferred.

**Do not assume** a build stack in code or docs until this entry is replaced with a concrete choice.

**Refs:** `docs/CONTRIBUTING.md`, `docs/INDEX.md` (dev-guide stubs), `STATE.md`

---

## Founding product decisions (pointer)

Major product/architecture choices (Ubuntu Server LTS base, Wayland/KWin shell, Flatpak-only apps, ZRAM, one ISO / two variants, no telemetry, etc.) are recorded in:

→ **`docs/DESIGN-DECISIONS.md`** (authoritative “why”)

Do not duplicate them here unless an *operational* follow-up changes how agents/humans work.
