# Installer Step 7 — data backup

Optional USB backup **before** disk wipe. Uses `spike-rescue --batch-recover`:

```
SpikeBackup/<utc-stamp>/<os-label>/home/<user>/…
```

Wizard: check “Back up…”, select destination (prefer Spike USB `writable`), then
Install runs recover first and **aborts wipe** if backup fails.

If “restore after install” is also checked and no prior session is selected,
InstallEngine uses the new `SESSION_PATH=` from batch-recover for Layer 4.
