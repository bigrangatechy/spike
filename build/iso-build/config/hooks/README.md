# Hooks

- `0500-spike-strip-telemetry.chroot` — purge snap/cloud/telemetry packages if present; drop Snap-stub firefox/thunderbird if they landed.
- `0600-spike-config.chroot` — `dpkg -i` spike-config; seed **default** state only (no `--detect` in chroot).
- `0700-spike-capture.chroot` — enable `spike-detect.service` (target boot) + `spike-capture-logs.service` (debug cmdline).
- `0720-spike-verify-includes.chroot` — fail build if critical includes missing or default apps are Snap stubs.
- `0730-spike-mozilla-apparmor.chroot` — unconfined AppArmor stubs for Mozilla Firefox/Thunderbird `.deb`s.
- `0740-spike-discover-home.chroot` — Flatpak/Flathub + AppStream refresh, skel XDG dirs, hide powerdevil autostart, breeze LNF for lockscreen.
- `0750-spike-kscreenlocker.chroot` — Spike LockScreen.qml into plasma shell path; disable idle autolock (suspend race).

Soft-fail on missing packages where noted in each hook.

Third-party APT: `config/spike-archives/` (Mozilla) is staged into `config/archives/` by `scripts/build-iso.sh` before `lb build`.
