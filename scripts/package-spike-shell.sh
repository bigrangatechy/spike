#!/usr/bin/env bash
# Build spike-shell as a local .deb for live-build injection.
# Spec: docs/DESKTOP.md | Stage 3 packaging (pre-alpha)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-shell"
OUT_DIR="${ROOT}/build/packages"
VERSION="${SPIKE_SHELL_VERSION:-0.0.9}"
REVISION="${SPIKE_SHELL_REVISION:-1}"
PKG_VER="${VERSION}-${REVISION}"
ARCH=amd64
PKG_NAME=spike-shell
DEB_NAME="${PKG_NAME}_${PKG_VER}_${ARCH}.deb"

usage() {
  cat <<EOF
Usage: ./scripts/package-spike-shell.sh [--out DIR]

Builds ${DEB_NAME} from src/spike-shell/ (Qt6 Widgets, Architecture: amd64).

Install paths:
  /usr/bin/spike-shell
  /usr/bin/spike-session
  /usr/share/wayland-sessions/spike.desktop
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
  echo "error: dpkg-deb required (apt install dpkg-dev)" >&2
  exit 1
}
command -v cmake >/dev/null || {
  echo "error: cmake required" >&2
  exit 1
}

[[ -f "${SRC}/CMakeLists.txt" ]] || {
  echo "error: incomplete source tree at ${SRC}" >&2
  exit 1
}

BUILD="${SRC}/build-pkg"
rm -rf "$BUILD"

# Prefer system LayerShellQt; fall back to extracted -dev under build/deps-extract.
CMAKE_ARGS=(-S "$SRC" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr)
if [[ ! -d /usr/lib/x86_64-linux-gnu/cmake/LayerShellQt ]]; then
  EXTRACT="${ROOT}/build/deps-extract/root/usr"
  if [[ -d "${EXTRACT}/lib/x86_64-linux-gnu/cmake/LayerShellQt" ]]; then
    CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=${EXTRACT}")
  else
    echo "error: LayerShellQt CMake package missing." >&2
    echo "  sudo apt install liblayershellqtinterface-dev" >&2
    echo "  (or extract it under build/deps-extract/ — see SESSION_LOG)" >&2
    exit 1
  fi
fi

cmake "${CMAKE_ARGS[@]}"
cmake --build "$BUILD" -j"$(nproc)"

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

DEST="${STAGE}/${PKG_NAME}"
mkdir -p \
  "${DEST}/DEBIAN" \
  "${DEST}/usr/bin" \
  "${DEST}/usr/share/wayland-sessions" \
  "${DEST}/usr/share/doc/${PKG_NAME}"

install -m 755 "${BUILD}/spike-shell" "${DEST}/usr/bin/spike-shell"
install -m 755 "${SRC}/session/spike-session" "${DEST}/usr/bin/spike-session"
install -m 644 "${SRC}/session/spike.desktop" "${DEST}/usr/share/wayland-sessions/spike.desktop"
cp "${SRC}/README.md" "${DEST}/usr/share/doc/${PKG_NAME}/README"

cat >"${DEST}/usr/share/doc/${PKG_NAME}/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: spike-shell
Source: https://git.bigrangatech.com/Ranga/spike

Files: *
Copyright: BigRangaTech
License: GPL-2.0-or-later
EOF

cat >"${DEST}/DEBIAN/control" <<EOF
Package: ${PKG_NAME}
Version: ${PKG_VER}
Section: x11
Priority: optional
Architecture: ${ARCH}
Maintainer: BigRangaTech <spike@bigrangatech.com>
Depends: libqt6widgets6 | libqt6widgets6t64, libqt6gui6 | libqt6gui6t64, libqt6core6t64 | libqt6core6, libqt6dbus6 | libqt6dbus6t64, qt6-wayland, liblayershellqtinterface6, layer-shell-qt
Recommends: kwin-wayland, xwayland, dbus-user-session, seatd, libseat1, breeze-cursor-theme, spike-config
Description: Spike Linux desktop shell (Qt6 Widgets)
 Bottom panel, Kickoff-style launcher, Settings (hybrid KCM + custom),
 and session menu under standalone KWin. Stage 3 pre-alpha — see docs/DESKTOP.md.
EOF

find "${DEST}" -type d -exec chmod 755 {} +
chmod 755 "${DEST}/usr/bin/spike-shell" "${DEST}/usr/bin/spike-session"

mkdir -p "$OUT_DIR"
dpkg-deb --root-owner-group --build "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
dpkg-deb -I "${OUT_DIR}/${DEB_NAME}"
rm -rf "$BUILD"
