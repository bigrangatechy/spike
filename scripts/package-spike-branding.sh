#!/usr/bin/env bash
# Build spike-branding as a local .deb for live-build injection.
# Spec: docs/BRANDING.md — Plymouth, GRUB theme, logo masters.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-branding"
OUT_DIR="${ROOT}/build/packages"
VERSION="${SPIKE_BRANDING_VERSION:-0.0.1}"
REVISION="${SPIKE_BRANDING_REVISION:-1}"
PKG_VER="${VERSION}-${REVISION}"
ARCH=all
PKG_NAME=spike-branding
DEB_NAME="${PKG_NAME}_${PKG_VER}_${ARCH}.deb"

usage() {
  cat <<EOF
Usage: ./scripts/package-spike-branding.sh [--out DIR]

Builds ${DEB_NAME} from src/spike-branding/.

Install paths:
  /usr/share/plymouth/themes/spike-{minimal,full}/
  /usr/share/spike/grub/  (+ /boot/grub/themes/spike/)
  /usr/share/spike/branding/logo/
EOF
}

OUT_DIR_OVERRIDE=""
case "${1:-}" in
  --help|-h) usage; exit 0 ;;
  --out)
    OUT_DIR_OVERRIDE="${2:?}"
    shift 2 || true
    ;;
  "") ;;
  *)
    echo "unknown option: $1" >&2
    usage >&2
    exit 1
    ;;
esac
[[ -n "$OUT_DIR_OVERRIDE" ]] && OUT_DIR="$OUT_DIR_OVERRIDE"

command -v dpkg-deb >/dev/null || {
  echo "error: dpkg-deb required" >&2
  exit 1
}

[[ -d "$SRC/plymouth/spike-minimal" ]] || {
  echo "error: missing $SRC/plymouth/spike-minimal" >&2
  exit 1
}

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

DEST="${STAGE}/${PKG_NAME}"
mkdir -p \
  "${DEST}/DEBIAN" \
  "${DEST}/usr/share/plymouth/themes" \
  "${DEST}/usr/share/spike/grub" \
  "${DEST}/boot/grub/themes/spike" \
  "${DEST}/usr/share/spike/branding/logo" \
  "${DEST}/usr/share/doc/${PKG_NAME}"

cp -a "${SRC}/plymouth/spike-minimal" "${DEST}/usr/share/plymouth/themes/"
cp -a "${SRC}/plymouth/spike-full" "${DEST}/usr/share/plymouth/themes/"
# Prefer the newer emblem as Plymouth logo.png when present.
if [[ -f "${SRC}/plymouth/spike-minimal/spike-emblem-256.png" ]]; then
  cp -f "${SRC}/plymouth/spike-minimal/spike-emblem-256.png" \
    "${DEST}/usr/share/plymouth/themes/spike-minimal/logo.png"
  cp -f "${SRC}/plymouth/spike-minimal/spike-emblem-256.png" \
    "${DEST}/usr/share/plymouth/themes/spike-full/logo.png"
fi

cp -a "${SRC}/grub-theme/." "${DEST}/usr/share/spike/grub/"
cp -a "${SRC}/grub-theme/." "${DEST}/boot/grub/themes/spike/"
# Prefer the 96px RGBA emblem for GRUB if present.
if [[ -f "${SRC}/grub-theme/spike-emblem-96.png" ]]; then
  cp -f "${SRC}/grub-theme/spike-emblem-96.png" \
    "${DEST}/usr/share/spike/grub/spike-emblem.png"
  cp -f "${SRC}/grub-theme/spike-emblem-96.png" \
    "${DEST}/boot/grub/themes/spike/spike-emblem.png"
fi

# Logo sizes for greeter / desktop (skip mislabeled JPEG master if present).
shopt -s nullglob
for f in "${SRC}/logo"/spike-emblem-*.png "${SRC}/logo"/spike-logo-*.svg; do
  cp -f "$f" "${DEST}/usr/share/spike/branding/logo/"
done
shopt -u nullglob
# Real PNG master preferred; skip JFIF-named .png
if file "${SRC}/logo/spike-emblem.png" 2>/dev/null | grep -qi 'PNG image'; then
  cp -f "${SRC}/logo/spike-emblem.png" "${DEST}/usr/share/spike/branding/logo/"
elif [[ -f "${SRC}/logo/spike-emblem-256.png" ]]; then
  cp -f "${SRC}/logo/spike-emblem-256.png" \
    "${DEST}/usr/share/spike/branding/logo/spike-emblem.png"
fi

cp "${SRC}/logo/README.md" "${DEST}/usr/share/doc/${PKG_NAME}/README" 2>/dev/null || \
  echo "Spike branding assets" >"${DEST}/usr/share/doc/${PKG_NAME}/README"

cat >"${DEST}/usr/share/doc/${PKG_NAME}/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: spike-branding
Source: https://git.bigrangatech.com/Ranga/spike

Files: *
Copyright: BigRangaTech
License: GPL-2.0-or-later
EOF

cat >"${DEST}/DEBIAN/control" <<EOF
Package: ${PKG_NAME}
Version: ${PKG_VER}
Section: graphics
Priority: optional
Architecture: ${ARCH}
Maintainer: BigRangaTech <spike@bigrangatech.com>
Depends: plymouth
Recommends: plymouth-themes
Description: Spike branding — Plymouth, GRUB theme, logos
 Ships spike-minimal / spike-full Plymouth themes, GRUB theme assets,
 and emblem PNGs under /usr/share/spike/branding/logo/.
EOF

cat >"${DEST}/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
# Default to Standard splash; Plus systems may switch via spike-config later.
if command -v plymouth-set-default-theme >/dev/null 2>&1; then
  if [ -d /usr/share/plymouth/themes/spike-minimal ]; then
    plymouth-set-default-theme spike-minimal >/dev/null 2>&1 || true
  fi
fi
# Embed theme in initramfs when tools exist (ISO chroot + installed).
if command -v update-initramfs >/dev/null 2>&1; then
  update-initramfs -u >/dev/null 2>&1 || true
fi
# Keep /boot theme in sync with /usr/share when boot is mounted.
if [ -d /usr/share/spike/grub ] && [ -d /boot/grub ]; then
  mkdir -p /boot/grub/themes/spike
  cp -a /usr/share/spike/grub/. /boot/grub/themes/spike/ 2>/dev/null || true
fi
exit 0
EOF
chmod 755 "${DEST}/DEBIAN/postinst"

mkdir -p "$OUT_DIR"
dpkg-deb --build --root-owner-group "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
