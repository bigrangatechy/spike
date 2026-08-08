# Spike Installer (scaffold)

Custom Qt installer — not implemented yet. Alpha gate: live → install → reboot.

## Backup / restore modules

These directories will call the shared SpikeBackup engine (`src/spike-common/`):

| Path | Role |
| --- | --- |
| `backup/` | Step 7 optional backup of the *target* drive → `SpikeBackup/<stamp>/…` |
| `restore/` | Layer 4 “Fresh install and restore my data” after install, into `/home/<newuser>/` |
| `detect/` | Existing Spike install + USB `SpikeBackup/` scan |
| `ui/` | Wizard pages (backup/restore stay **inside** the installer) |

Do not send the user out to Spike Rescue for install-time backup/restore. Rescue remains
the disaster live tool; Migration is the detailed new-user wizard.

Include:

```cpp
#include <spike/SpikeBackupLayout.hpp>
// or relative: #include "../../spike-common/SpikeBackupLayout.hpp"
```

Layout must match Rescue (see `docs/DISASTER-RECOVERY.md`, `docs/INSTALLER.md`).
