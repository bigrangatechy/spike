# Installer Layer 4 — restore after fresh install

After wipe+copy, restores a chosen `SpikeBackup/<stamp>/<os>/` session into
`/home/<newuser>/` using the same flatten rules as Spike Rescue
(`spike::buildRestoreMappings` / `home/<olduser>/Documents/…` → `Documents/…`).

Implementation (`spike-install-helper` `cmd_restore`):

1. Prefer `spike-rescue --batch-restore --session … --home /mnt/spike/home/<user>`
2. Fallback: shell flatten of category paths
3. `chown -R` the new user

Detection: wizard scans USB / `LABEL=writable` via `BackupScanner` + spike-common.
