#!/usr/bin/env bash
# Build spike-rescue as a local .deb for live-build injection.
# Spec: docs/DISASTER-RECOVERY.md Layer 3
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-rescue"
OUT_DIR="${ROOT}/build/packages"
if [[ -n "${SPIKE_RESCUE_VERSION:-}" ]]; then
  VERSION="$SPIKE_RESCUE_VERSION"
else
  VERSION="$(sed -n 's/^project(spike-rescue VERSION \([0-9][0-9.]*\).*/\1/p' "${SRC}/CMakeLists.txt" | head -n1)"
fi
if [[ -z "$VERSION" ]]; then
  echo "error: could not determine version (set SPIKE_RESCUE_VERSION or fix CMakeLists.txt)" >&2
  exit 1
fi
REVISION="${SPIKE_RESCUE_REVISION:-1}"
PKG_VER="${VERSION}-${REVISION}"
ARCH=amd64
PKG_NAME=spike-rescue
DEB_NAME="${PKG_NAME}_${PKG_VER}_${ARCH}.deb"

usage() {
  cat <<EOF
Usage: ./scripts/package-spike-rescue.sh [--out DIR]

Builds ${DEB_NAME} from src/spike-rescue/ (Qt6 Widgets).

Install paths:
  /usr/bin/spike-rescue
  /usr/share/applications/spike-rescue.desktop
  /usr/share/spike/live/spike-rescue.desktop
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
command -v cmake >/dev/null || {
  echo "error: cmake required" >&2
  exit 1
}

BUILD="${SRC}/build-pkg"
rm -rf "$BUILD"
cmake -S "$SRC" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$BUILD" -j"$(nproc)"

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

DEST="${STAGE}/${PKG_NAME}"
mkdir -p \
  "${DEST}/DEBIAN" \
  "${DEST}/usr/bin" \
  "${DEST}/usr/share/applications" \
  "${DEST}/usr/share/spike/live" \
  "${DEST}/usr/share/doc/${PKG_NAME}"

install -m 755 "${BUILD}/spike-rescue" "${DEST}/usr/bin/spike-rescue"
install -m 644 "${SRC}/data/spike-rescue.desktop" \
  "${DEST}/usr/share/applications/spike-rescue.desktop"
install -m 644 "${SRC}/data/spike-rescue-desktop.desktop" \
  "${DEST}/usr/share/spike/live/spike-rescue.desktop"
cp "${SRC}/README.md" "${DEST}/usr/share/doc/${PKG_NAME}/README"

cat >"${DEST}/usr/share/doc/${PKG_NAME}/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: spike-rescue
Source: https://git.bigrangatech.com/Ranga/spike

Files: *
Copyright: BigRangaTech
License: GPL-2.0-or-later
EOF

cat >"${DEST}/DEBIAN/control" <<EOF
Package: ${PKG_NAME}
Version: ${PKG_VER}
Section: utils
Priority: optional
Architecture: ${ARCH}
Maintainer: BigRangaTech <spike@bigrangatech.com>
Depends: libqt6widgets6 | libqt6widgets6t64, libqt6gui6 | libqt6gui6t64, libqt6core6t64 | libqt6core6, util-linux, sudo
Recommends: ntfs-3g, hfsprogs
Description: Spike Rescue — recover personal files from a live USB
 GUI tool (DISASTER-RECOVERY.md Layer 3): read-only mount broken disks,
 copy home Documents/Pictures/… to another USB with SHA256 verification.
EOF

# Live desktop shortcut helper (optional postinst copies for skel)
cat >"${DEST}/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
# Seed Desktop shortcut for live user skel if present.
for skel in /etc/skel/Desktop /usr/share/spike/skel/Desktop; do
  if [ -d "$(dirname "$skel")" ] || mkdir -p "$skel" 2>/dev/null; then
    if [ -f /usr/share/spike/live/spike-rescue.desktop ]; then
      mkdir -p "$skel"
      cp -f /usr/share/spike/live/spike-rescue.desktop "$skel/" 2>/dev/null || true
      chmod 755 "$skel/spike-rescue.desktop" 2>/dev/null || true
    fi
  fi
done
exit 0
EOF
chmod 755 "${DEST}/DEBIAN/postinst"

find "${DEST}" -type d -exec chmod 755 {} +
chmod 755 "${DEST}/usr/bin/spike-rescue"

mkdir -p "$OUT_DIR"
dpkg-deb --root-owner-group --build "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
dpkg-deb -I "${OUT_DIR}/${DEB_NAME}"
rm -rf "$BUILD"
