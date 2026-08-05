# Spike Configuration Engine (spike-config)

Stage 2 skeleton — pre-alpha. Spec: `docs/CONFIGURATION.md`.

## What this is

`spike-config` is Spike’s **on-demand** configuration engine (not a daemon). It:

1. Reads `/var/lib/spike/config/state.json`
2. Fills templates under `/usr/lib/spike/config/templates/`
3. Writes managed files under `/etc/` (and related paths)
4. Appends to `/var/lib/spike/config/changelog.json`

## Layout

```
src/spike-config/
├── README.md
├── pyproject.toml
├── data/default-state.json     → seed state (schema version 1)
├── templates/*.tpl             → installed to /usr/lib/spike/config/templates/
├── spike_config/               → Python package
│   ├── cli.py                  → developer CLI
│   ├── state.py                → state store I/O
│   ├── templates.py            → {{variable}} substitution
│   ├── generate.py             → module registry + generate-all
│   ├── changelog.py
│   ├── paths.py                → path roots (overridable for tests)
│   ├── detect.py               → hardware detect stub
│   └── modules/                → memory, boot, network, multimedia, security
└── tests/
```

## Developer usage

```bash
cd src/spike-config
python3 -m spike_config --help

# Safe local run (no root, no /etc writes):
export SPIKE_ROOT=/tmp/spike-config-test
export SPIKE_STATE_DIR=/tmp/spike-config-test/var/lib/spike/config
mkdir -p "$SPIKE_STATE_DIR"
python3 -m spike_config --init-state
python3 -m spike_config --generate-all
python3 -m spike_config --state
```

## Implemented (skeleton)

| Flag | Status |
|------|--------|
| `--init-state` | ✅ Seed default state |
| `--generate-all` / `--generate <module>` | ✅ memory, boot, network, multimedia, security |
| `--state` / `--state-get` / `--state-set` | ✅ |
| `--changelog` | ✅ |
| `--validate` | ✅ leftover `{{}}` + required keys |
| `--detect` | ⬜ stub (writes placeholder hardware; real detect later with installer) |
| `--rollback` | ✅ basic (revert one changelog entry) |
| `--boot-count` | ✅ get / reset / increment |
| DBus API | ⬜ not in Stage 2 |

## Install paths (product)

| Path | Role |
|------|------|
| `/usr/bin/spike-config` | CLI entry (packaging TBD) |
| `/usr/lib/spike/config/templates/` | Templates |
| `/var/lib/spike/config/state.json` | State store |
| `/var/lib/spike/config/changelog.json` | Change log |

License: GPLv2+ (code). Docs/branding remain CC-BY-SA 4.0.
