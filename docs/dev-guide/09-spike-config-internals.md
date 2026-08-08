# Spike Dev Guide — spike-config Internals

## Purpose

Map `spike-config` (variant + hardware profile engine) to code under `src/spike-config/`. Product rules: `CONFIGURATION.md`, `VARIANT-DIFFERENCES.md`, `MEMORY.md`.

## Status

Shipping on the live ISO. Expand this page as modules and `state.json` schema stabilize.

## Layout (today)

```
src/spike-config/
├── spike_config/          → Python package entry
├── modules/               → apply / detect modules
├── boot/ memory/ network/ multimedia/ security/ dbus/ data/ templates/ tests/
└── pyproject.toml
```

Package: `./scripts/package-spike-config.sh` → `build/packages/spike-config_*.deb`.

## Principles

- One ISO; Standard vs Plus is **config**, not a second image (`04-building-spike.md`).
- Installer / Settings call into spike-config rather than duplicating policy.
- Detection must match `HARDWARE.md` classifications.

## Related

- Repo map: `02-repo-structure.md`
- Installer handoff: `07-installer-internals.md`
- Agent ops: `STATE.md`
