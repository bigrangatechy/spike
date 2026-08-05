# live-build `config/`

Placeholder tree for the Spike live-build configuration.

| Path | Role |
| :-: | :-: |
| `package-lists/` | Package lists installed into the chroot/squashfs |
| `hooks/` | Hooks to strip Snap/telemetry, seed Flatpak, install Spike packages |
| `includes.chroot/` | Files copied into the chroot (themes, defaults, installer desktop entry) |

Fill these when implementing the first real ISO. Keep **one** shared package set; do not fork lists into `standard` vs `plus` ISOs.
