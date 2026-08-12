# Spike logo assets

**Source of truth:** SVG. Raster PNGs are derived for GRUB / Plymouth / icons.

## SVG (preferred)

| File | Use |
|------|-----|
| `spike-logo-full.svg` | **Interim master** — dog + “Spike” wordmark (docs, splash, website; use until emblem SVG exists) |
| `spike-logo-full-tagline.svg` | Full logo + tagline (*Let's make tech repairable again*) |
| `spike-emblem.svg` | Dog only — preferred later for GRUB / Plymouth / small icons |

Until `spike-emblem.svg` exists, keep the existing emblem **PNG** set for boot/icons (do not replace those with full-logo renders — the wordmark gets muddy at 16–96px).

## PNG rasters (keep for boot / icons)

| File | Size | Use |
|------|------|-----|
| `spike-emblem-16.png` … `512.png` | 16–512 | Freedesktop / panel / app icons |
| `spike-emblem-96.png` | 96 | Same as GRUB emblem |
| `../grub-theme/spike-emblem.png` | 96 | GRUB menu |
| `../plymouth/spike-*/logo.png` | 256 | Boot splash |
| `spike-emblem.png` | 1024 | Raster master (should be real PNG, not JPEG) |

Export emblem rasters from **`spike-emblem.svg`** when available. Until then, leave the
current `spike-emblem-*.png` / GRUB / Plymouth PNGs as-is; use `spike-logo-full.svg` for
places that want the wordmark (docs, larger UI).

Artwork license: CC-BY-SA 4.0 (see repo `LICENSE-CC-BY-SA-4.0`).
