# live-build `config/`

| Path | Role |
| :-: | :-: |
| `package-lists/spike-live.list.chroot` | Stage 1 live + admin packages |
| `hooks/0500-spike-strip-telemetry.chroot` | Strip snap/cloud/telemetry |
| `includes.chroot/` | Overlay files (hostname for Stage 1) |

Generated files from `lb config` (`config/bootstrap`, `config/chroot`, etc.) are gitignored.
