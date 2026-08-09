# Spike Installer

Custom Qt Widgets installer per `docs/INSTALLER.md`. **Not Calamares.**

## Status (0.0.4)

| Area | State |
| --- | --- |
| 10-step wizard UI | ✅ Collects answers |
| Type **ERASE** confirm | ✅ Storage step |
| Privileged `spike-install-helper` | ✅ partition / squashfs / user / grub (CLI args) |
| GRUB after install | ✅ `/etc/default/grub` + `/boot/grub`; `update-grub` or minimal `grub.cfg`; UEFI `--removable` fallback |
| Wi‑Fi page | ✅ nmcli scan / connect / skip (Ethernet OK) |
| Installed session path | ✅ tty1 autologin + `/etc/spike/installed` → `spike-session` |
| Layer 4 restore into new home | ✅ Optional if session selected |
| Step 7 backup copy engine | 🔲 Checkbox only — skip for smoke |

Alpha gate: live → **real** install → reboot to installed desktop.

## Build

```
./scripts/package-spike-installer.sh
# → build/packages/spike-installer_0.0.4-1_amd64.deb
```

Live Desktop: **Install Spike**. Helper: `/usr/lib/spike/spike-install-helper` (sudoers).

Live ISO must include `grub-efi-amd64-bin` + `grub-pc-bin` + `efibootmgr` (see `spike-live.list.chroot`) so chrooted `grub-install --target=…` works. Do not pull both `grub-pc` and `grub-efi-amd64` metapackages — they conflict. Squashfs often lacks `/etc/default/grub` and `/boot/grub` without those metapackages — the helper creates them.

## Smoke (spare disk)

1. Rebuild ISO with **0.0.4** + shell **0.0.29** + config **0.0.8**.
2. Boot live; open **Install Spike**.
3. Pick a **spare** disk only (not the USB). Skip Step 7 backup.
4. Type **ERASE** → Install Spike now.
5. Confirm install.log has `update-grub OK` or `Wrote minimal /boot/grub/grub.cfg` (not a silent WARN-only path).
6. Remove USB, reboot from installed disk → GRUB menu → tty1 autologin → Spike Shell.

## Related

- Spec: `docs/INSTALLER.md`
- Internals: `docs/dev-guide/07-installer-internals.md`
