#!/usr/bin/env bash
# Build spike-config as a local .deb for live-build injection.
# Spec: docs/CONFIGURATION.md | Stage 2 packaging (pre-alpha)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${ROOT}/src/spike-config"
OUT_DIR="${ROOT}/build/packages"
VERSION="${SPIKE_CONFIG_VERSION:-0.0.2}"
REVISION="${SPIKE_CONFIG_REVISION:-1}"
PKG_VER="${VERSION}-${REVISION}"
ARCH=all
PKG_NAME=spike-config
DEB_NAME="${PKG_NAME}_${PKG_VER}_${ARCH}.deb"

usage() {
  cat <<EOF
Usage: ./scripts/package-spike-config.sh [--out DIR]

Builds ${DEB_NAME} from src/spike-config/ (pure Python, Architecture: all).

Install paths (product):
  /usr/bin/spike-config
  /usr/lib/python3/dist-packages/spike_config/
  /usr/lib/spike/config/templates/
  /usr/lib/spike/config/default-state.json
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

[[ -d "${SRC}/spike_config" && -d "${SRC}/templates" ]] || {
  echo "error: incomplete source tree at ${SRC}" >&2
  exit 1
}

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

DEST="${STAGE}/${PKG_NAME}"
mkdir -p \
  "${DEST}/DEBIAN" \
  "${DEST}/usr/bin" \
  "${DEST}/usr/lib/python3/dist-packages" \
  "${DEST}/usr/lib/spike/config/templates" \
  "${DEST}/usr/share/doc/${PKG_NAME}"

# Python package
cp -a "${SRC}/spike_config" "${DEST}/usr/lib/python3/dist-packages/"
# Drop tests/cache if any
find "${DEST}/usr/lib/python3/dist-packages/spike_config" -type d -name '__pycache__' -exec rm -rf {} + 2>/dev/null || true

# Templates + default state (product paths)
cp -a "${SRC}/templates/." "${DEST}/usr/lib/spike/config/templates/"
cp -a "${SRC}/data/default-state.json" "${DEST}/usr/lib/spike/config/default-state.json"

# CLI wrapper
cat >"${DEST}/usr/bin/spike-config" <<'EOF'
#!/usr/bin/python3
"""Spike configuration engine (developer / install-time CLI)."""
from spike_config.cli import main

if __name__ == "__main__":
    raise SystemExit(main())
EOF
chmod 755 "${DEST}/usr/bin/spike-config"

# Docs
cp "${SRC}/README.md" "${DEST}/usr/share/doc/${PKG_NAME}/README"
cat >"${DEST}/usr/share/doc/${PKG_NAME}/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: spike-config
Source: https://git.bigrangatech.com/Ranga/spike

Files: *
Copyright: BigRangaTech
License: GPL-2.0-or-later
EOF

# Maintainer scripts
cat >"${DEST}/DEBIAN/control" <<EOF
Package: ${PKG_NAME}
Version: ${PKG_VER}
Section: admin
Priority: optional
Architecture: ${ARCH}
Maintainer: BigRangaTech <spike@bigrangatech.com>
Depends: python3 (>= 3.10)
Description: Spike Linux configuration engine
 Generates system configuration from a JSON state store and templates.
 Invoked on-demand by the installer, Settings (later), or the developer CLI.
 See docs/CONFIGURATION.md in the Spike source tree.
EOF

cat >"${DEST}/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
case "$1" in
  configure)
    mkdir -p /var/lib/spike/config
    chmod 755 /var/lib/spike/config
    # Seed state on first install if missing (live ISO / fresh root).
    if [ ! -f /var/lib/spike/config/state.json ] && command -v spike-config >/dev/null 2>&1; then
      spike-config --init-state || true
    fi
    ;;
esac
exit 0
EOF
chmod 755 "${DEST}/DEBIAN/postinst"

# Permissions
find "${DEST}" -type d -exec chmod 755 {} +
find "${DEST}/usr/lib" -type f -exec chmod 644 {} +
chmod 755 "${DEST}/usr/bin/spike-config"

mkdir -p "$OUT_DIR"
dpkg-deb --root-owner-group --build "$DEST" "${OUT_DIR}/${DEB_NAME}"
echo "Built ${OUT_DIR}/${DEB_NAME}"
dpkg-deb -I "${OUT_DIR}/${DEB_NAME}"
dpkg-deb -c "${OUT_DIR}/${DEB_NAME}" | head -40
