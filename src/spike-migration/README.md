# Spike Migration — Move My Files

Dedicated tool for users switching **to** Spike.

Spec: `docs/SPIKE-MIGRATION.md`

## Status (0.0.3)

Wizard shell (async `QProcess`) over `spike-rescue` batch CLI:

| Page | Mode A (before install) | Mode B (into this Spike) |
| --- | --- | --- |
| Welcome | Spec copy → Continue | Branch: Import into this Spike |
| Scan | `--list-systems` | Session picker |
| Select / Dest | Pick system + USB/writable root | — |
| Progress | `--batch-recover` | `--batch-restore` |
| Done | SESSION_PATH / Open Install Spike | Close |

Inventory size preview, exclude folders, conflict rename, and old-disk remount Mode B from the full spec are **not** in this build.

## Desktop

`Categories=X-Spike-Tools;…` → **Spike Tools**.
