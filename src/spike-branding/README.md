# Spike branding

Assets for Plymouth splash, GRUB, and desktop/greeter logos.

## Package

```
./scripts/package-spike-branding.sh
# → build/packages/spike-branding_0.0.1-1_all.deb
```

Installs:

| Path | Contents |
| --- | --- |
| `/usr/share/plymouth/themes/spike-minimal/` | Standard splash |
| `/usr/share/plymouth/themes/spike-full/` | Plus splash |
| `/usr/share/spike/grub/` + `/boot/grub/themes/spike/` | GRUB theme |
| `/usr/share/spike/branding/logo/` | Emblem PNG sizes + SVG masters |

`postinst` sets `plymouth-set-default-theme spike-minimal` and runs `update-initramfs -u` when available.

## Layout

See `logo/README.md`, `docs/BRANDING.md`.
