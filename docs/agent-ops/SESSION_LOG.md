# Spike Agent Ops — Session Log

Append-only. Newest sessions at the **top**.

---

## 2026-08-05 — Stage 1 build failed: missing packages (not a success yet)

**Finding:** No ISO produced. `lb build` died on:
- `wireless-tools` — removed from resolute (use `iw`)
- `nouveau-firmware` — pulled by `--firmware-chroot true` but not in archive

**Done:** Dropped obsolete packages; set `--firmware-chroot false` / `--firmware-binary false` for Stage 1; list `linux-firmware` explicitly instead.

**Next:** BDFL clean + rebuild:
```
sudo ./scripts/build-iso.sh --clean-only
sudo ./scripts/build-iso.sh
```
Success = `*.iso` under `build/iso-build/`.

---

## 2026-08-05 — Stage 1 build stall on dbus-session-bus-common

**Participants:** BDFL + agent

**Finding:** Not a package bug — debootstrap download-only hung mid-fetch of `dbus-session-bus-common` (partial ~49KB of ~54KB) from `archive.ubuntu.com`. AU mirror is substantially faster.

**Done:** Switched `auto/config` default mirrors to `au.archive.ubuntu.com`; documented stall diagnosis + clean/retry in `build/iso-build/README.md` and `04-building-spike.md`.

**Next:** BDFL `sudo ./scripts/build-iso.sh --clean-only` then rebuild.

---

## 2026-08-05 — Stage 1 live-build recipe + build-iso.sh

**Participants:** BDFL + agent (Cursor)

**Done:**
- Filled `build/iso-build/`: `auto/config` (ubuntu/resolute, iso-hybrid, casper), `spike-live.list.chroot`, strip-telemetry chroot hook, `.recipe-ready`
- Implemented `scripts/build-iso.sh` (check-deps, clean, config, build; rejects `--variant`)
- Updated `docs/dev-guide/04-building-spike.md` with Stage 1 flow + QEMU smoke-test
- Updated `STATE.md`

**Open / verify:**
- Host still needs interactive: `sudo apt install live-build debootstrap …` then `sudo ./scripts/build-iso.sh` (agent cannot supply sudo password)
- Script smoke: `--check-deps` reports missing lb/debootstrap until installed; `--variant` correctly rejected; shell syntax OK on auto/config + hook + wrapper
- Confirm ISO boots in QEMU after local build

**Next:**
- BDFL: install deps + first `lb build` + QEMU
- Then Stage 2 (`spike-config`)

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
