#!/usr/bin/env bash
# Build spike-shell as a local .deb for live-build injection.
# Spec: docs/DESKTOP.md | Stage 3 packaging (pre-alpha)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-shell"
OUT_DIR="${ROOT}/build/packages"
# Prefer SPIKE_SHELL_VERSION override; otherwise read CMake project VERSION.
if [[ -n "${SPIKE_SHELL_VERSION:-}" ]]; then
  VERSION="$SPIKE_SHELL_VERSION"
else
  VERSION="$(sed -n 's/^project(spike-shell VERSION \([0-9][0-9.]*\).*/\1/p' "${SRC}/CMakeLists.txt" | head -n1)"
fi
if [[ -z "$VERSION" ]]; then
  echo "error: could not determine version (set SPIKE_SHELL_VERSION or fix CMakeLists.txt)" >&2
  exit 1
fi
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
  /usr/bin/spike-seed-home
  /usr/bin/spike-fix-mozilla-home
  /usr/bin/spike-save-logs
  /usr/share/wayland-sessions/spike.desktop
  /etc/pam.d/spike-lock
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

# Prefer system LayerShellQt / KF6; fall back to extracted -dev under build/deps-extract.
CMAKE_ARGS=(-S "$SRC" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr)
EXTRACT_USR="${ROOT}/build/deps-extract/root/usr"
if [[ -d /usr/lib/x86_64-linux-gnu/cmake/LayerShellQt ]]; then
  :
elif [[ -d "${EXTRACT_USR}/lib/x86_64-linux-gnu/cmake/LayerShellQt" ]]; then
  CMAKE_ARGS+=("-DLayerShellQt_DIR=${EXTRACT_USR}/lib/x86_64-linux-gnu/cmake/LayerShellQt")
  CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=${EXTRACT_USR}")
else
  echo "error: LayerShellQt CMake package missing." >&2
  echo "  sudo apt install liblayershellqtinterface-dev" >&2
  echo "  (or extract it under build/deps-extract/ — see SESSION_LOG)" >&2
  exit 1
fi
if [[ ! -d /usr/lib/x86_64-linux-gnu/cmake/KF6KCMUtils ]] \
   && [[ -d "${EXTRACT_USR}/include/KF6/KCMUtils" ]]; then
  CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=${EXTRACT_USR}")
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
  "${DEST}/usr/share/doc/${PKG_NAME}" \
  "${DEST}/etc/pam.d"

install -m 755 "${BUILD}/spike-shell" "${DEST}/usr/bin/spike-shell"
install -m 755 "${BUILD}/spike-greeter" "${DEST}/usr/bin/spike-greeter"
install -m 755 "${SRC}/session/spike-session" "${DEST}/usr/bin/spike-session"
install -m 755 "${SRC}/session/spike-seed-home" "${DEST}/usr/bin/spike-seed-home"
install -m 755 "${SRC}/session/spike-fix-mozilla-home" "${DEST}/usr/bin/spike-fix-mozilla-home"
install -m 755 "${SRC}/session/spike-save-logs" "${DEST}/usr/bin/spike-save-logs"
install -m 644 "${SRC}/session/spike.desktop" "${DEST}/usr/share/wayland-sessions/spike.desktop"
mkdir -p "${DEST}/lib/systemd/system" "${DEST}/usr/lib/spike"
install -m 644 "${SRC}/session/spike-greeter.service" \
  "${DEST}/lib/systemd/system/spike-greeter.service"
install -m 755 "${SRC}/session/spike-greeter-prepare" \
  "${DEST}/usr/lib/spike/spike-greeter-prepare"
mkdir -p "${DEST}/lib/systemd/system/getty@tty1.service.d"
install -m 644 "${SRC}/session/getty@tty1.service.d/spike-greeter-coexist.conf" \
  "${DEST}/lib/systemd/system/getty@tty1.service.d/spike-greeter-coexist.conf"
mkdir -p "${DEST}/usr/share/spike/desktop" "${DEST}/usr/share/applications"
install -m 644 "${SRC}/session/spike-save-logs.desktop" \
  "${DEST}/usr/share/spike/desktop/spike-save-logs.desktop"
install -m 644 "${SRC}/session/spike-save-logs.desktop" \
  "${DEST}/usr/share/applications/spike-save-logs.desktop"
install -m 644 "${SRC}/pam/spike-lock" "${DEST}/etc/pam.d/spike-lock"
# Minimal kscreenlocker LNF + plasma shell lockscreen (greeter looks at shell first)
mkdir -p "${DEST}/usr/share/plasma/look-and-feel/org.kde.breeze.desktop/contents/lockscreen"
mkdir -p "${DEST}/usr/share/plasma/shells/org.kde.plasma.desktop/contents/lockscreen"
LOCK_QML="${ROOT}/build/iso-build/config/includes.chroot/usr/share/plasma/look-and-feel/org.kde.breeze.desktop/contents/lockscreen/LockScreen.qml"
if [[ -f "$LOCK_QML" ]]; then
  install -m 644 "$LOCK_QML" \
    "${DEST}/usr/share/plasma/look-and-feel/org.kde.breeze.desktop/contents/lockscreen/LockScreen.qml"
  install -m 644 "$LOCK_QML" \
    "${DEST}/usr/share/plasma/shells/org.kde.plasma.desktop/contents/lockscreen/LockScreen.qml"
fi
mkdir -p "${DEST}/etc/xdg" "${DEST}/usr/lib/udev/rules.d"
if [[ -f "${ROOT}/build/iso-build/config/includes.chroot/etc/xdg/kscreenlockerrc" ]]; then
  install -m 644 "${ROOT}/build/iso-build/config/includes.chroot/etc/xdg/kscreenlockerrc" \
    "${DEST}/etc/xdg/kscreenlockerrc"
fi
install -m 644 "${SRC}/udev/90-spike-backlight.rules" \
  "${DEST}/usr/lib/udev/rules.d/90-spike-backlight.rules"
# KWin script → panel task list (Wayland window icons)
mkdir -p "${DEST}/usr/share/kwin/scripts"
cp -a "${SRC}/kwin-scripts/spike-tasklist" "${DEST}/usr/share/kwin/scripts/"
cp -a "${SRC}/kwin-scripts/spike-shortcuts" "${DEST}/usr/share/kwin/scripts/"
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
Depends: libqt6widgets6 | libqt6widgets6t64, libqt6gui6 | libqt6gui6t64, libqt6core6t64 | libqt6core6, libqt6dbus6 | libqt6dbus6t64, qt6-wayland, liblayershellqtinterface6, layer-shell-qt, libkf6kcmutils6, libkf6kcmutilscore6, libkf6coreaddons6, libpam0g, udisks2, gvfs, kwin-wayland, xwayland, dbus-user-session, seatd, libseat1, breeze-cursor-theme, breeze-icon-theme, qt6-svg-plugins, libkf6kcmutils-bin, pulseaudio-utils, upower, bluez, rfkill, wmctrl, brightnessctl, brightness-udev, playerctl, kglobalacceld, xdg-utils
Recommends: spike-config, gvfs-backends, smartmontools, nm-connection-editor
Description: Spike Linux desktop shell (Qt6 Widgets)
 Bottom panel, Kickoff launcher, Network/Volume/Battery tray applets, Settings
 (custom pages + in-window KCMs), Spike lock screen, spike-greeter login,
 session menu under standalone KWin. Stage 3 Alpha — see docs/DESKTOP.md.
 Runtime Depends ship on the ISO (see package-lists/README.md); do not thin
 this list without updating spike-live.list.chroot + the verify hook.
EOF

find "${DEST}" -type d -exec chmod 755 {} +
chmod 755 "${DEST}/usr/bin/spike-shell" "${DEST}/usr/bin/spike-greeter" \
  "${DEST}/usr/bin/spike-session" \
  "${DEST}/usr/bin/spike-seed-home" "${DEST}/usr/bin/spike-fix-mozilla-home" \
  "${DEST}/usr/bin/spike-save-logs" "${DEST}/usr/lib/spike/spike-greeter-prepare"

mkdir -p "$OUT_DIR"
dpkg-deb --root-owner-group --build "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
dpkg-deb -I "${OUT_DIR}/${DEB_NAME}"
rm -rf "$BUILD"
