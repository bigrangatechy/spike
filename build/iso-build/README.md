# Spike ISO build recipe (live-build)

## Rule: one ISO

Spike ships **one** hybrid live ISO. Standard vs Plus is applied at **install time** by `spike-installer` + `spike-config`. Do not add parallel Standard/Plus live-build projects here.

## Base

**Ubuntu Server 26.04 LTS** (Resolute), stripped per `ARCHITECTURE.md` / privacy-security specs. live-build should bootstrap from 26.04 archives.

## Engine

- **Tool:** [live-build](https://live-team.pages.debian.net/live-manual/)
- **Wrapper:** `../../scripts/build-iso.sh` (from repo root: `./scripts/build-iso.sh`)
- **Docs:** `docs/dev-guide/03-build-environment.md`, `docs/dev-guide/04-building-spike.md`

## Layout

```
iso-build/
├── README.md                 → this file
├── auto/
│   └── README.md             → how auto/* will drive lb config
└── config/
    ├── README.md
    ├── package-lists/        → .list.chroot stubs (fill when packaging)
    ├── hooks/                → chroot/binary hook placeholders
    └── includes.chroot/      → overlay files (branding, spike bits)
```

## Status

Scaffold only. Package lists and hooks are empty placeholders until Spike packages and strip/seed scripts exist. `build-iso.sh` will refuse a full `lb build` until the recipe is marked ready (see script).

## Related dirs (repo)

```
build/live-environment/   → reserved for live-session notes/assets (optional)
build/package-configs/    → reserved for .deb packaging configs
build/signing/            → reserved for ISO/signing keys workflow
```
