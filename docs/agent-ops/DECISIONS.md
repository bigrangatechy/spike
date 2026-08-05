# Spike Agent Ops — Decision Log

Operational decisions that affect how work proceeds. Product/architecture rationale remains in `docs/DESIGN-DECISIONS.md`. Log new entries at the **top** (newest first).

**Last updated:** 2026-08-05

---

## 2026-08-05 — Base: Ubuntu Server 26.04 LTS (Resolute), stripped

**Decision:** Spike’s base is **Ubuntu Server 26.04 LTS** (“Resolute Raccoon”), not Desktop. Strip Snap, telemetry, cloud-init, and related components per `ARCHITECTURE.md` / `PRIVACY.md` / `SECURITY.md`. live-build recipes and build hosts should target **26.04** archives.

**Why (BDFL):**
- Locks the base for the current development window; next Ubuntu LTS (28.04) is far enough out to develop without rushing a rebase.
- Server is a light, supportable substrate (security updates, archive, kernel) so Spike effort stays on **Spike Shell** and the **custom installer** — not reinventing a distro core.
- Unused kernel modules are disabled carefully at **install time** from detected hardware (see `KERNEL.md`) without breaking usability; that policy is installer/spike-config work, not a custom kernel.

**Product authority:** `DESIGN-DECISIONS.md` (Server 26.04, no custom kernel), `KERNEL.md` (blacklist generation), `INSTALLER.md` (install-time detection + blacklist step).

**Refs:** `DESIGN-DECISIONS.md`, `KERNEL.md`, `ARCHITECTURE.md`, `INSTALLER.md`, `docs/dev-guide/03-build-environment.md`, `docs/dev-guide/04-building-spike.md`, `docs/dev-guide/07-installer-internals.md`

---

## 2026-08-05 — ISO engine: live-build; installer: custom Qt; one ISO

**Decision:**
- **ISO builder:** [live-build](https://live-team.pages.debian.net/live-manual/), wrapped by `scripts/build-iso.sh`, recipe under `build/iso-build/`.
- **Artifact:** **One** hybrid live ISO (not separate Standard/Plus downloads).
- **Variant:** Applied at **install time** via the custom installer + `spike-config` (Plus enables features; same squashfs/codebase).
- **Installer:** **Custom Qt Widgets** (`spike-installer/` per `INSTALLER.md`) — **not Calamares**.

**Why:** Matches Ubuntu/Debian live-ISO practice (casper-style live session + squashfs unpack). Product already specified one ISO in `VARIANT-DIFFERENCES.md` / `DESIGN-DECISIONS.md`. Installer UX is Spike-specific (10-step beginner flow + Rescue); Calamares would fight that.

**Supersedes:** “ISO build tooling still open” entry below (struck as resolved).

**Refs:** `docs/dev-guide/03-build-environment.md`, `docs/dev-guide/04-building-spike.md`, `docs/dev-guide/07-installer-internals.md`, `build/iso-build/`, `INSTALLER.md`, `VARIANT-DIFFERENCES.md`

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

## 2026-08-05 — ISO build tooling still open — RESOLVED

**Status:** Superseded 2026-08-05 by “ISO engine: live-build; installer: custom Qt; one ISO”.

**Historical note:** Previously deferred live-build vs custom and Calamares vs custom. Now locked: live-build + custom Qt installer + one ISO.

---

## Founding product decisions (pointer)

Major product/architecture choices (Ubuntu Server LTS base, Wayland/KWin shell, Flatpak-only apps, ZRAM, one ISO / two variants, no telemetry, etc.) are recorded in:

→ **`docs/DESIGN-DECISIONS.md`** (authoritative “why”)

Do not duplicate them here unless an *operational* follow-up changes how agents/humans work.
