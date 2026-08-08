# Spike common libraries

Shared code for Spike Rescue, Installer (Step 7 / Layer 4), and Spike Migration.

## SpikeBackup layout

```
SpikeBackup/
  <utc-stamp>/          # e.g. 20260808-113057
    <os-label>/         # Linux, Windows, macOS, …
      home/<user>/Documents/…
      home/<user>/Pictures/…
      …                 # or Users/<user>/… for Windows/macOS
      REPORT.txt
```

Legacy discovery also finds `install-logs-*/log/SpikeBackup/` (casper `/var/log` writes).

## Headers

- `SpikeBackupLayout.hpp` — find sessions, map restore paths into a target home

Installed to `/usr/include/spike/` by the `spike-rescue` package until a dedicated
`spike-common` package exists. Link the `.cpp` from consuming apps or install a
static library later.
