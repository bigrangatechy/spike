# Installer Layer 4 — restore after fresh install

Placeholder for post-install restore from `SpikeBackup/` into `/home/<newuser>/`.

Use `spike::buildRestoreMappings(sessionPath, targetHome)` then SHA256-verified
copy (same semantics as Spike Rescue restore mode).

Detection: scan connected USB / writable for `SpikeBackup/` (and legacy
`install-logs-*/log/SpikeBackup/`). Prefer newest stamp; let the user confirm.
