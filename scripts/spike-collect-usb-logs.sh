#!/usr/bin/env bash
# Copy Spike live USB diagnostic captures into the repo for agent review.
# Usage: sudo ./scripts/spike-collect-usb-logs.sh [/dev/sdX4 or mountpoint]
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEST_ROOT="${ROOT}/build/iso-build/debug-logs"
mkdir -p "$DEST_ROOT"

SRC="${1:-}"
MNT=""
cleanup() {
  if [[ -n "${MNT}" && -d "${MNT}" && "${MNT}" == /tmp/spike-writable-* ]]; then
    umount "${MNT}" 2>/dev/null || true
    rmdir "${MNT}" 2>/dev/null || true
  fi
}
trap cleanup EXIT

if [[ -z "$SRC" ]]; then
  if findmnt -n -S LABEL=writable >/dev/null 2>&1; then
    SRC="$(findmnt -n -o TARGET -S LABEL=writable)"
  elif [[ -b /dev/disk/by-label/writable ]]; then
    SRC=/dev/disk/by-label/writable
  else
    echo "error: pass mountpoint or block device (LABEL=writable not found)" >&2
    echo "  sudo ./scripts/spike-collect-usb-logs.sh /run/media/\$USER/writable" >&2
    exit 1
  fi
fi

if [[ -b "$SRC" ]]; then
  MNT="$(mktemp -d /tmp/spike-writable-XXXXXX)"
  mount "$SRC" "$MNT"
  SRC="$MNT"
fi

[[ -d "$SRC" ]] || { echo "error: not a directory: $SRC" >&2; exit 1; }

echo "Collecting from $SRC → $DEST_ROOT"
copied=0
# Top-level captures + casper install-logs (which may nest spike-capture under log/)
for d in \
  "$SRC"/spike-capture-* \
  "$SRC"/install-logs-* \
  "$SRC"/install-logs-*/log/spike-capture-*
do
  [[ -e "$d" ]] || continue
  name="$(basename "$d")"
  echo "  $name"
  rm -rf "${DEST_ROOT}/${name}"
  cp -a "$d" "${DEST_ROOT}/"
  if [[ -n "${SUDO_UID:-}" ]]; then
    chown -R "${SUDO_UID}:${SUDO_GID:-${SUDO_UID}}" "${DEST_ROOT}/${name}" || true
  fi
  copied=1
done

if [[ "$copied" -eq 0 ]]; then
  echo "warning: no spike-capture-* or install-logs-* found under $SRC" >&2
  ls -la "$SRC" >&2 || true
  exit 2
fi

echo "Done. Review under ${DEST_ROOT}/"
ls -la "$DEST_ROOT"
