# Spike Dev Guide — Debugging

## Purpose

Practical debug paths for the live ISO and Spike packages. Expand as tooling grows.

## Live USB captures

Casper often persists logs on `LABEL=writable`:

```
install-logs-*/log/spike-capture-*/
  journal-follow.txt
  syslog.copy
  session-logs/
  …
```

Helpers:

```
./scripts/spike-collect-usb-logs.sh
# or mount writable and inspect manually
```

## Spike Rescue

- Window title shows version + `[pre-alpha debug]`.
- Recover: `SpikeBackup/.../REPORT.txt` on the destination (prefer partition root).
- Restore: `SpikeRestore-REPORT.txt` in the target home.
- Journal: `sudo` lines for `/usr/lib/spike/spike-rescue-mount`.

See `08-rescue-tool-internals.md`.

## Shell / session

- Session log paths under capture `session-logs/`.
- Launcher category bugs: check `.desktop` `Categories=` and `Launcher.cpp` `categoryFor()`.

## QEMU

Optional smoke without USB — see `04-building-spike.md` (when host has qemu). Prefer hardware smoke for Rescue mounts and writable partition behaviour.

## Related

- Build: `04-building-spike.md`
- Testing (future): `11-testing.md`
