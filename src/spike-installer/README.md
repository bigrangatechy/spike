# Spike Installer

Custom Qt Widgets installer per `docs/INSTALLER.md`. **Not Calamares.**

## Status (0.0.11)

| Area | State |
| --- | --- |
| 10-step wizard UI | ✅ Collects answers |
| Languages (11) | ✅ INSTALLER.md launch set |
| Timezones | ✅ Full `zone.tab` list (filterable) |
| Keyboard | ✅ XKB layouts + suggestion + `/etc/default/keyboard` |
| First-boot markers | ✅ `/var/lib/spike/first-boot` + `installer-notifications/` |
| Type **ERASE** confirm | ✅ Storage step |
| Privileged `spike-install-helper` | ✅ partition / squashfs / user / grub (CLI args) |
| GRUB after install | ✅ sanitize theme paths; `update-grub` or minimal cfg; UEFI `--removable` |
| Local (non-casper) boot | ✅ regenerate initramfs + `boot=local` cmdline |
| Module blacklist | ✅ `--detect` → KERNEL.md policy → `spike-blacklist.conf` before initramfs |
| Installed session groups | ✅ `video,input,render` + enable `seatd` (fixes black screen) |
| Wi‑Fi page | ✅ nmcli scan / connect / skip |
| Step 7 backup | ✅ async system scan + picker + `--exclude-disk`; SKIPPED honesty |
| Layer 4 restore | ✅ `RESTORE_STATUS=ok\|failed\|skipped` on Finish |
| Installed session path | ✅ tty1 autologin + `/etc/spike/installed` → `spike-session` |

Alpha gate: live → **real** install → reboot to installed desktop (+ first-run wizard with shell **0.0.31**).

## Build

```
./scripts/package-spike-installer.sh
# → build/packages/spike-installer_0.0.11-1_amd64.deb
```

Live Desktop: **Install Spike**. Helper: `/usr/lib/spike/spike-install-helper` (sudoers).
Requires `spike-rescue` **≥0.0.12** on the live image for Step 7 / preferred Layer 4 restore.

## Smoke (spare disk)

1. Rebuild ISO with installer **0.0.11** + shell **0.0.31** + rescue **0.0.12** + config **0.0.11**.
2. Variant → Continue stays responsive; Step 7 optional.
3. Type **ERASE** → Install Spike now.
4. Confirm `install.log`: first-boot marker, blacklist, keyboard, `update-initramfs`, groups/`seatd`.
5. Remove USB, reboot → desktop → **first-run wizard** once → Get started.
6. Second login: no wizard.

## Related

- Spec: `docs/INSTALLER.md`
- Internals: `docs/dev-guide/07-installer-internals.md`
