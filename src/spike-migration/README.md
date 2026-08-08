# Spike Migration — Move My Files

Dedicated, detailed wizard for users switching **to** Spike.

Spec: `docs/SPIKE-MIGRATION.md`

## Status

Scaffold only. Implementation will link `src/spike-common/` for SpikeBackup layout
and verified copy, with richer UX than Spike Rescue (education, multi-user,
conflict policy).

## Modes

- **Pre-install (live):** guided copy from old OS → USB `SpikeBackup/`
- **Post-install:** import from USB `SpikeBackup/` or old disk → `$HOME`

Installer Step 7 / Layer 4 remain in `spike-installer` (in-flow). Rescue remains
the disaster recover+restore tool.

## Desktop

`Categories=X-Spike-Tools;…` so the app appears under **Spike Tools**.
