# Spike Agent Ops — Decision Log

Operational decisions that affect how work proceeds. Product/architecture rationale remains in `docs/DESIGN-DECISIONS.md`. Log new entries at the **top** (newest first).

**Last updated:** 2026-08-06

---

## 2026-08-06 — Pre-alpha ends when the installer works (enter Alpha)

**Decision (BDFL):** Leave **pre-alpha** and open **Alpha** only when the **custom Spike installer works end-to-end** (live USB → install to disk → reboot into an installed Spike desktop). Until that gate is met, keep the **pre-alpha** label even while shell, ISO, and other stack work continues.

**What “installer works” means (minimum):**
- Installer runs from the live session
- Partitions / copies the system as specified in `INSTALLER.md`
- Reboot reaches an installed session (not only live)
- BDFL confirms the gate is met (no calendar date)

**Does not mean:** Alpha starts when the shell boots, when Stage N finishes, or when docs are “100%”. Shell/ISO work stays under pre-alpha until this gate.

**Supersedes:** Soft “Alpha by feel” wording (installer was already the cue). Clarifies vs the older “pre-alpha until beta” note — that referred to the long docs-first era; the **phase label flip** is now **pre-alpha → Alpha** at installer E2E.

**Refs:** `STATE.md`, `ROADMAP.md` (Phase 1 exit / Phase 2 entry), `INSTALLER.md`, prior “Pre-alpha stays open through shell/installer” entry.

---

## 2026-08-06 — Settings Network is Spike custom; KCM allowlist only

**Decision:**
- **Network GUI:** Spike Shell tray applet + **Settings → Network** (NetworkManager D-Bus / `nmcli` helper). Do **not** ship `plasma-nm` (QtWebEngine ~200MB).
- **KCM host:** Keep in-window KF6 `KCModuleLoader` for standalone providers only: `kscreen`, `plasma-pa`, `powerdevil`, `bluedevil`, `print-manager` (+ PipeWire for Sound).
- **Forbidden on ISO:** `plasma-desktop`, `plasma-workspace` (plasmashell autostart fights Spike Shell).
- **Plasma-coupled pages** (Language, Hardware Keyboard, Mouse/Touchpad, Users, Date & Time, Accessibility): Spike custom stubs / pages — do not pull those KCMs via Plasma packages.

**Why:** Matches `DESIGN-DECISIONS.md` custom network applet; Tier-1 RAM budget; no second desktop shell.

**Refs:** `DESKTOP.md`, `NETWORKING.md`, `ARCHITECTURE.md`, `CONSTRAINTS.md`, `STATE.md`, `src/spike-shell/src/network/`.

---

## 2026-08-06 — Pre-alpha stays open through shell/installer; Alpha by feel

**Decision:** Keep implementing **Spike Shell** and other early stack pieces under the **pre-alpha** label. Do **not** open **Phase 2 (Alpha)** on a calendar or Stage counter.

**Status:** Superseded 2026-08-06 by “Pre-alpha ends when the installer works (enter Alpha)” — installer E2E is now the explicit gate (not vague “feel” alone).

**Refs:** Newer entry above; `STATE.md`, `ROADMAP.md`.

---

## 2026-08-06 — Secure Boot optional in pre-alpha

**Decision:** During **pre-alpha**, Secure Boot is **not required**. Testers may disable Secure Boot in firmware. Spike still ships Ubuntu-style **shim → grub** on the hybrid ISO when available (works with SB on machines that trust Canonical/Microsoft keys), but we do **not** enroll custom MOK keys, sign Spike packages, or treat SB-on as a release gate until later.

**Why:** Pre-alpha focus is boot + `spike-config` + desktop path. Custom SB signing is Alpha/Beta polish. Disabling SB is an acceptable workaround for firmware that rejects unsigned/third-party bits.

**Refs:** `BOOT-PROCESS.md` (Secure Boot section), `04-building-spike.md`, Stage 2 packaging.

---

## 2026-08-06 — Stage 2 stays under pre-alpha

**Decision:** **Stage 2** (`spike-config` skeleton and related early work) remains part of **pre-alpha**. Do **not** open **Phase 2 (Alpha)** in `ROADMAP.md` just because Stage 1 closed or Stage 2 starts.

**Why:** Alpha is a longer core-build phase (shell, installer, Tier 1 desktop). Pre-alpha already allows implementation once specs are enough; keeping Stage 2 under that label avoids implying a public alpha calendar or milestone shift.

**Refs:** `STATE.md`, `ROADMAP.md` (Phase 1 / Phase 2), earlier “Pre-alpha label meaning” entry.

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

**Status:** Partially superseded 2026-08-06. **Phase label** flip is now **pre-alpha → Alpha** when the **installer works E2E** (see top entry). Pre-alpha still does not forbid writing code. “Until beta” was the old docs-era shorthand and must not be read as skipping Alpha.

**Refs:** `docs/ROADMAP.md`, newer installer-gate entry, `docs/AGENTS.md`

---

## 2026-08-05 — ISO build tooling still open — RESOLVED

**Status:** Superseded 2026-08-05 by “ISO engine: live-build; installer: custom Qt; one ISO”.

**Historical note:** Previously deferred live-build vs custom and Calamares vs custom. Now locked: live-build + custom Qt installer + one ISO.

---

## Founding product decisions (pointer)

Major product/architecture choices (Ubuntu Server LTS base, Wayland/KWin shell, Flatpak-only apps, ZRAM, one ISO / two variants, no telemetry, etc.) are recorded in:

→ **`docs/DESIGN-DECISIONS.md`** (authoritative “why”)

Do not duplicate them here unless an *operational* follow-up changes how agents/humans work.
