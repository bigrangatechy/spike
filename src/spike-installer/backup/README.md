# Installer Step 7 — data backup

Placeholder for the optional USB backup during install.

Implementation will reuse `spike::discoverAllBackupSessions` / copy helpers from
`src/spike-common/SpikeBackupLayout.*` and the privileged mount helper patterns
from `spike-rescue` (or a future shared `spike-backup` binary helper).

Writes:

```
SpikeBackup/<utc-stamp>/<os-label>/home/<user>/…
```
