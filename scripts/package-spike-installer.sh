#!/usr/bin/env bash
# Build spike-installer as a local .deb for live-build injection.
# Spec: docs/INSTALLER.md (wizard UI; wipe engine still stubbed in 0.0.1)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-installer"
OUT_DIR="${ROOT}/build/packages"
if [[ -n "${SPIKE_INSTALLER_VERSION:-}" ]]; then
  VERSION="$SPIKE_INSTALLER_VERSION"
else
  VERSION="$(sed -n 's/^project(spike-installer VERSION \([0-9][0-9.]*\).*/\1/p' "${SRC}/CMakeLists.txt" | head -n1)"
fi
if [[ -z "$VERSION" ]]; then
  echo "error: could not determine version (set SPIKE_INSTALLER_VERSION or fix CMakeLists.txt)" >&2
  exit 1
fi
REVISION="${SPIKE_INSTALLER_REVISION:-1}"
PKG_VER="${VERSION}-${REVISION}"
ARCH=amd64
PKG_NAME=spike-installer
DEB_NAME="${PKG_NAME}_${PKG_VER}_${ARCH}.deb"

usage() {
  cat <<EOF
Usage: ./scripts/package-spike-installer.sh [--out DIR]

Builds ${DEB_NAME} from src/spike-installer/ (Qt6 Widgets).

Install paths:
  /usr/bin/spike-installer
  /usr/share/applications/spike-installer.desktop
  /usr/share/spike/live/spike-installer.desktop
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

install -m 755 "${BUILD}/spike-installer" "${DEST}/usr/bin/spike-installer"
install -m 644 "${SRC}/data/spike-installer.desktop" \
  "${DEST}/usr/share/applications/spike-installer.desktop"
install -m 644 "${SRC}/data/spike-installer-desktop.desktop" \
  "${DEST}/usr/share/spike/live/spike-installer.desktop"
cp "${SRC}/README.md" "${DEST}/usr/share/doc/${PKG_NAME}/README"

cat >"${DEST}/usr/share/doc/${PKG_NAME}/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: spike-installer
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
Depends: libqt6widgets6 | libqt6widgets6t64, libqt6gui6 | libqt6gui6t64, libqt6core6t64 | libqt6core6, util-linux
Recommends: spike-config
Description: Spike Installer — install Spike Linux from the live USB
 Pre-alpha Qt wizard (INSTALLER.md): collects language, account, variant,
 optional SpikeBackup restore choice. Disk wipe / system copy not yet enabled.
EOF

cat >"${DEST}/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
for skel in /etc/skel/Desktop /usr/share/spike/skel/Desktop; do
  if [ -d "$(dirname "$skel")" ] || mkdir -p "$skel" 2>/dev/null; then
    if [ -f /usr/share/spike/live/spike-installer.desktop ]; then
      mkdir -p "$skel"
      cp -f /usr/share/spike/live/spike-installer.desktop \
        "$skel/Install Spike.desktop" 2>/dev/null || true
      chmod 755 "$skel/Install Spike.desktop" 2>/dev/null || true
    fi
  fi
done
exit 0
EOF
chmod 755 "${DEST}/DEBIAN/postinst"

find "${DEST}" -type d -exec chmod 755 {} +
chmod 755 "${DEST}/usr/bin/spike-installer"

mkdir -p "$OUT_DIR"
dpkg-deb --root-owner-group --build "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
dpkg-deb -I "${OUT_DIR}/${DEB_NAME}"
rm -rf "$BUILD"
