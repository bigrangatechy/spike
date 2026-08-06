# live-build `config/`

| Path | Role |
| :-: | :-: |
| `package-lists/spike-live.list.chroot` | Stage 1 live + admin packages |
| `packages.chroot/` | Keep empty for Spike local debs (gpg local-repo path) |
| `includes.chroot/var/cache/spike-local/` | Local `spike-config_*.deb` (installed by hook) |
| `hooks/0500-spike-strip-telemetry.chroot` | Strip snap/cloud/telemetry |
| `hooks/0600-spike-config.chroot` | `dpkg -i` spike-config + seed state |
| `includes.chroot/` | Overlay files (hostname for Stage 1) |

Generated files from `lb config` (`config/bootstrap`, `config/chroot`, etc.) are gitignored.
Do not commit `.deb` files under `packages.chroot/` (rebuild on each ISO).
