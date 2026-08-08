# Spike Dev Guide — Appendix / Reference

## Purpose

Quick pointers that change often. Prefer linking to `STATE.md` over duplicating version numbers here for long.

## Component trees

| Tree | Spec | Dev-guide |
| :-: | :-: | :-: |
| `spike-shell` | `DESKTOP.md` | `06-spike-shell-architecture.md` |
| `spike-config` | `CONFIGURATION.md` | `09-spike-config-internals.md` |
| `spike-rescue` | `DISASTER-RECOVERY.md` | `08-rescue-tool-internals.md` |
| `spike-common` | SpikeBackup layout | `02-repo-structure.md`, rescue page |
| `spike-installer` | `INSTALLER.md` | `07-installer-internals.md` |
| `spike-migration` | `SPIKE-MIGRATION.md` | (scaffold; extend when coded) |

## Package scripts

```
./scripts/package-spike-shell.sh
./scripts/package-spike-config.sh
./scripts/package-spike-rescue.sh
sudo ./scripts/build-iso.sh
```

## Spike Tools category

```
Categories=X-Spike-Tools;…
```

Menu + directory under `build/iso-build/config/includes.chroot/`.

## Ops docs

- `docs/agent-ops/STATE.md` — current versions / next smoke
- `docs/agent-ops/SESSION_LOG.md` — what changed recently
- `docs/agent-ops/DECISIONS.md` — locked build/install decisions

## Related

- Full index: `docs/INDEX.md`
- Dev-guide index: `docs/dev-guide/INDEX.md`
