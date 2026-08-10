# Spike Dev Guide — Debugging

## Purpose

Practical debug paths for the live ISO and Spike packages. Expand as tooling grows.

## Installed system → spare USB (preferred)

After smoke on the installed desktop, plug any writable USB (e.g. a second SanDisk) and use the Desktop icon **Copy Spike Logs to USB** (or run `spike-save-logs`).

That writes `spike-logs-<UTC>/` with:

- `session-logs/` — `spike-session` + install.log
- `config/kwinrc` + Night Light D-Bus
- `network/nmcli.txt` (no Wi‑Fi passwords)
- `journal-filtered.txt` — shell / KWin / NM / installer this boot
- package versions + `spike-config --state`

On the build PC:

```
sudo ./scripts/spike-collect-usb-logs.sh /run/media/$USER/<stick-name>
# → build/iso-build/debug-logs/spike-logs-*
```

Tell the agent the folder name + what you saw on screen.

## Live USB captures

Casper often persists logs on `LABEL=writable`:

```
install-logs-*/log/spike-capture-*/
  journal-follow.txt
  session-logs/
  …
```

Debug GRUB entry (`debug` / `spike.capture`) still arms full journal follow. Default snapshots skip the heavy SOF audio dump unless `--full`.

Helpers:

```
./scripts/spike-collect-usb-logs.sh
# or mount writable and inspect manually
```

Live capture units are **removed on install** — use `spike-save-logs` on the installed system instead.

## Spike Rescue

- Window title shows version + `[pre-alpha debug]`.
- Recover: `SpikeBackup/.../REPORT.txt` on the destination (prefer partition root).
- Restore: `SpikeRestore-REPORT.txt` in the target home.
- Journal: `sudo` lines for `/usr/lib/spike/spike-rescue-mount`.

See `08-rescue-tool-internals.md`.

## Shell / session

- Always: `/var/log/spike/session-latest.log` (and `$XDG_RUNTIME_DIR/spike-session.log`).
- Copy to USB: Desktop **Copy Spike Logs to USB** / `spike-save-logs`.
- Launcher category bugs: check `.desktop` `Categories=` and `Launcher.cpp` `categoryFor()`.

## QEMU

Optional smoke without USB — see `04-building-spike.md` (when host has qemu). Prefer hardware smoke for Rescue mounts and writable partition behaviour.

## Related

- Build: `04-building-spike.md`
- Testing (future): `11-testing.md`
