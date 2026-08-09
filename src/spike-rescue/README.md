# Spike Rescue — Layer 3 disaster recovery (docs/DISASTER-RECOVERY.md)

GUI tool for the **live ISO**. Mounts broken-system disks **read-only**, finds personal
files under known home folders, copies them to another USB under `SpikeBackup/` with
**SHA256** verification.

## Batch CLI (installer / migration)

```bash
spike-rescue --list-systems
# Fast: OS/users only (no personal-file inventory walk). Use GUI inventory/recover for counts.
spike-rescue --batch-recover --dest /run/media/…/writable [--system 0] [--exclude-disk /dev/nvme0n1]
# prints SESSION_PATH=…/SpikeBackup/<stamp>/<label>
# or SKIPPED=no-eligible-systems (exit 0) when only the wipe disk remains
spike-rescue --batch-restore --session /path/to/session --home /home/user
```

Version: **0.0.12**.

## Build

```bash
cd src/spike-rescue
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./build/spike-rescue
```

Needs: `cmake`, `qt6-base-dev`. RO mounts go through `/usr/lib/spike/spike-rescue-mount`
(passwordless for live user `spike` via `/etc/sudoers.d/spike-rescue`).

## Package

```bash
./scripts/package-spike-rescue.sh
```

Ships the GUI, the mount helper, and sudoers. Destinations include the live USB
`writable` partition when it has enough free space (no second stick required).
## Live ISO

`build-iso.sh` packages this `.deb` with spike-config/shell. Desktop entry:
**Rescue My Files**.

## License

GPLv2+
