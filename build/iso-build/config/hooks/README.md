# Hooks

- `0500-spike-strip-telemetry.chroot` — purge snap/cloud/telemetry packages if present.
- `0600-spike-config.chroot` — `dpkg -i` spike-config; seed **default** state only (no `--detect` in chroot).
- `0700-spike-capture.chroot` — enable `spike-detect.service` (target boot) + `spike-capture-logs.service` (debug cmdline).

Soft-fail on missing packages where noted in each hook.
