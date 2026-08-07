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
│   ├── detect.py               → /proc+/sys hardware detect (cpu/ram/gpu/storage/net)
│   └── modules/                → memory, boot, network, multimedia, security
└── tests/
```

## Packaging (live ISO)

```bash
./scripts/package-spike-config.sh
# → build/packages/spike-config_0.0.1-1_all.deb

sudo ./scripts/build-iso.sh
# rebuilds the .deb, stages under includes.chroot/var/cache/spike-local/,
# hook runs dpkg -i (not packages.chroot — that path needs gpg in chroot)
```

Install paths in the `.deb`:

| Path | Role |
|------|------|
| `/usr/bin/spike-config` | CLI |
| `/usr/lib/python3/dist-packages/spike_config/` | Python package |
| `/usr/lib/spike/config/templates/` | Templates |
| `/usr/lib/spike/config/default-state.json` | Seed state |
| `/var/lib/spike/config/state.json` | Runtime state (created by postinst / hook) |

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
| `--detect` | Fill hardware from `/proc` + `/sys` (cpu/ram/gpu/storage/network) |
| `--rollback` | ✅ basic (revert one changelog entry) |
| `--boot-count` | ✅ get / reset / increment |
| `.deb` + ISO inject | ✅ `package-spike-config.sh` / live hook |
| DBus API (`org.spike.Config`) | ✅ system bus + activation (`spike-config-dbus`) |

License: GPLv2+ (code). Docs/branding remain CC-BY-SA 4.0.
