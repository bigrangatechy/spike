# Spike Dev Guide — Getting Started

## Purpose

Onboard a developer (or agent) who already has the repo cloned. For product intent, start with `PHILOSOPHY.md` and `AGENTS.md`; this page is the shortest path to a working edit → package → ISO loop.

## Prerequisites

1. Read `docs/agent-ops/STATE.md` and the latest entries in `SESSION_LOG.md`.
2. Host setup: `03-build-environment.md` (Ubuntu 26.04 recommended, live-build + Qt6 + cmake).
3. Follow `docs/agent-ops/CONVENTIONS.md` and `AGENTS.md` (GUI-first for user tools, docs-first for behaviour changes).

## What to build first

| Goal | Start here |
| :-: | :-: |
| Change the live desktop shell | `src/spike-shell/` → `./scripts/package-spike-shell.sh` |
| Change Rescue / Restore | `src/spike-rescue/` + `src/spike-common/` → `./scripts/package-spike-rescue.sh` |
| Change variant / detect | `src/spike-config/` → `./scripts/package-spike-config.sh` |
| Full live image | `./scripts/build-iso.sh` (see `04-building-spike.md`) |
| Installer | 📝 **0.0.1** wizard on ISO — `INSTALLER.md`, `07-installer-internals.md` (wipe engine not enabled) |
| Migration | Spec + scaffold — `SPIKE-MIGRATION.md`; `src/spike-migration/` |

## Minimal loop (component .deb)

```
# Example: rescue
./scripts/package-spike-rescue.sh
ls build/packages/spike-rescue_*.deb

# Example: shell
./scripts/package-spike-shell.sh
```

`build-iso.sh` injects the newest matching debs from `build/packages/` into the live image (see hook `0600` and `04-building-spike.md`).

## Smoke on hardware

1. Write the hybrid ISO to USB (`spike-iso-hybridize.sh` / project USB docs).
2. Boot live → open **Rescue My Files** (desktop) or launcher **Spike Tools**.
3. For writable-partition backups: confirm `SpikeBackup/` at the **root** of `LABEL=writable` (not under `install-logs-*/log/` unless testing legacy restore).
4. Pull logs: `./scripts/spike-collect-usb-logs.sh` or mount `writable` and read `install-logs-*/log/spike-capture-*` plus any `REPORT.txt`.

## Where docs live

```
Product behaviour     → docs/*.md (DISASTER-RECOVERY, DESKTOP, INSTALLER, …)
Day-to-day ops        → docs/agent-ops/
How we implement      → docs/dev-guide/ (this tree)
```

When behaviour changes, update the **product** doc first, then the matching **dev-guide** internals page.

## Related

- Repo map: `02-repo-structure.md`
- ISO build: `04-building-spike.md`
- Package builds: `05-building-components.md`
