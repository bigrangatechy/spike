#!/usr/bin/env bash
# Build spike-migration as a local .deb for live-build injection.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-migration"
OUT_DIR="${ROOT}/build/packages"
if [[ -n "${SPIKE_MIGRATION_VERSION:-}" ]]; then
  VERSION="$SPIKE_MIGRATION_VERSION"
else
  VERSION="$(sed -n 's/^project(spike-migration VERSION \([0-9][0-9.]*\).*/\1/p' "${SRC}/CMakeLists.txt" | head -n1)"
fi
if [[ -z "$VERSION" ]]; then
  echo "error: could not determine version" >&2
  exit 1
fi
REVISION="${SPIKE_MIGRATION_REVISION:-1}"
PKG_VER="${VERSION}-${REVISION}"
ARCH=amd64
PKG_NAME=spike-migration
DEB_NAME="${PKG_NAME}_${PKG_VER}_${ARCH}.deb"

OUT_DIR_OVERRIDE=""
case "${1:-}" in
  --help|-h)
    echo "Usage: ./scripts/package-spike-migration.sh [--out DIR]"
    exit 0
    ;;
  --out)
    OUT_DIR_OVERRIDE="${2:?}"
    shift 2 || true
    ;;
  "") ;;
  *)
    echo "unknown option: $1" >&2
    exit 1
    ;;
esac
[[ -n "$OUT_DIR_OVERRIDE" ]] && OUT_DIR="$OUT_DIR_OVERRIDE"

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

install -m 755 "${BUILD}/spike-migration" "${DEST}/usr/bin/spike-migration"
install -m 644 "${SRC}/data/spike-migration.desktop" \
  "${DEST}/usr/share/applications/spike-migration.desktop"
install -m 644 "${SRC}/data/spike-migration-desktop.desktop" \
  "${DEST}/usr/share/spike/live/spike-migration.desktop"
echo "spike-migration pre-alpha launcher" >"${DEST}/usr/share/doc/${PKG_NAME}/README"

cat >"${DEST}/usr/share/doc/${PKG_NAME}/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: spike-migration
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
Depends: libqt6widgets6 | libqt6widgets6t64, libqt6gui6 | libqt6gui6t64, libqt6core6t64 | libqt6core6
Recommends: spike-rescue, spike-installer
Description: Spike Migration — Move My Files
 Wizard shell (Mode A/B) over spike-rescue batch backup/restore (SPIKE-MIGRATION.md).
EOF

cat >"${DEST}/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
for skel in /etc/skel/Desktop /usr/share/spike/skel/Desktop; do
  if [ -d "$(dirname "$skel")" ] || mkdir -p "$skel" 2>/dev/null; then
    if [ -f /usr/share/spike/live/spike-migration.desktop ]; then
      mkdir -p "$skel"
      cp -f /usr/share/spike/live/spike-migration.desktop \
        "$skel/Move My Files.desktop" 2>/dev/null || true
      chmod 755 "$skel/Move My Files.desktop" 2>/dev/null || true
    fi
  fi
done
exit 0
EOF
chmod 755 "${DEST}/DEBIAN/postinst"

find "${DEST}" -type d -exec chmod 755 {} +
chmod 755 "${DEST}/usr/bin/spike-migration"

mkdir -p "$OUT_DIR"
dpkg-deb --root-owner-group --build "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
dpkg-deb -I "${OUT_DIR}/${DEB_NAME}"
rm -rf "$BUILD"
