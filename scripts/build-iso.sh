#!/usr/bin/env bash
# Spike ISO build wrapper — live-build, one hybrid ISO (Ubuntu 26.04).
# Spec: docs/dev-guide/04-building-spike.md
# Decision: docs/agent-ops/DECISIONS.md
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RECIPE="${ROOT}/build/iso-build"
READY_MARKER="${RECIPE}/.recipe-ready"

usage() {
  cat <<'EOF'
Usage: ./scripts/build-iso.sh [option]

  (default)       Build the single Spike hybrid live ISO
  --check-deps    Verify host tools for live-build
  --clean-only    Full clean (chroot, stages, cache, logs, ISOs)
  --config-only   Run lb config (via auto/config) without building
  --help          Show this help

Requires root (or passwordless sudo) for chroot/loop mounts.

Spike builds ONE ISO. There is no --variant standard|plus flag.
Standard vs Plus is applied at install time by spike-installer + spike-config.
EOF
}

need_root() {
  if [[ "${EUID}" -ne 0 ]]; then
    echo "error: this step must run as root (try: sudo $0 ${*:-})" >&2
    exit 1
  fi
}

check_deps() {
  local missing=0
  local cmd
  for cmd in lb debootstrap mksquashfs xorriso; do
    if ! command -v "$cmd" >/dev/null 2>&1; then
      echo "missing: $cmd" >&2
      missing=1
    else
      echo "ok: $cmd ($("$cmd" --version 2>/dev/null | head -n1 || echo present))"
    fi
  done
  if ! command -v isohybrid >/dev/null 2>&1; then
    echo "missing: isohybrid (package: syslinux-utils) — needed for iso-hybrid" >&2
    missing=1
  else
    echo "ok: isohybrid"
  fi
  for pkgfile in \
    /usr/lib/shim/shimx64.efi.signed.latest \
    /usr/lib/grub/x86_64-efi-signed/grubx64.efi.signed
  do
    if [[ ! -f "$pkgfile" ]]; then
      echo "missing: $pkgfile (packages: shim-signed grub-efi-amd64-signed)" >&2
      missing=1
    else
      echo "ok: $pkgfile"
    fi
  done
  if [[ "$missing" -ne 0 ]]; then
    echo "Install host packages — see docs/dev-guide/03-build-environment.md" >&2
    echo "  sudo apt install live-build debootstrap squashfs-tools xorriso isolinux syslinux-common syslinux-utils \\" >&2
    echo "    grub-pc-bin grub-efi-amd64-bin grub-efi-amd64-signed shim-signed mtools dosfstools rsync ca-certificates" >&2
    return 1
  fi
  if [[ ! -x "${RECIPE}/auto/config" ]]; then
    echo "missing executable: ${RECIPE}/auto/config" >&2
    return 1
  fi
  echo "Dependency check passed."
}

require_recipe_ready() {
  if [[ ! -f "$READY_MARKER" ]]; then
    cat >&2 <<EOF
build-iso.sh: recipe not marked ready (${READY_MARKER} missing).
See build/iso-build/README.md
EOF
    exit 2
  fi
}

umount_chroot() {
  # Best-effort teardown if a previous build left mounts behind.
  if [[ ! -d chroot ]]; then
    return 0
  fi
  local m
  for m in \
    chroot/dev/pts chroot/dev/shm chroot/dev \
    chroot/proc/sys/fs/binfmt_misc chroot/proc \
    chroot/sys chroot/run \
    chroot/var/lib/dpkg chroot/root/config \
    chroot/binary.tmp chroot/dev.tmp/pts
  do
    umount -lf "${m}" >/dev/null 2>&1 || true
  done
}

spike_clean() {
  # Ubuntu live-build: plain `lb clean` → --all only (keeps cache + .build stages).
  # After a failed bootstrap that is not enough — next build restores a dirty cache.
  echo "Cleaning Spike live-build tree in ${RECIPE} ..."
  cd "$RECIPE"
  umount_chroot

  # Bypass auto/clean recursion; purge caches and wipe stage markers.
  lb clean noauto --all || true
  lb clean noauto --cache || true
  lb clean noauto --stage || true

  umount_chroot
  rm -rf \
    chroot chroot.tmp \
    binary binary.tmp \
    cache \
    .build \
    local \
    tftpboot \
    source
  rm -f \
    build.log \
    .lock \
    chroot.packages.live chroot.packages.install chroot.headers \
    binary*.iso binary*.img binary*.tar.gz binary*.zsync* \
    binary.sh binary.contents binary.packages md5sum.txt \
    live-image*.iso *.hybrid.iso spike-live.iso

  # Keep versioned recipe: auto/, config/package-lists|hooks|includes*, READMEs, .recipe-ready
  echo "Clean finished. Remaining top-level:"
  ls -la
}

run_clean() {
  need_root "$@"
  check_deps
  spike_clean
}

run_config() {
  need_root "$@"
  check_deps
  require_recipe_ready
  cd "$RECIPE"
  # Prefer auto/config so flags stay in version control
  if [[ -x ./auto/config ]]; then
    ./auto/config
  else
    lb config
  fi
}

build_iso() {
  need_root "$@"
  check_deps
  require_recipe_ready

  echo "Building Spike ISO (single artifact) in ${RECIPE} ..."
  spike_clean

  cd "$RECIPE"
  if [[ -x ./auto/config ]]; then
    ./auto/config
  else
    lb config
  fi
  lb build

  echo "Build finished. Looking for ISO artifacts..."
  # live-build may leave the ISO under chroot/ if a post-genisoimage step fails
  # (e.g. missing isohybrid). Promote those to the recipe root.
  local stuck
  for stuck in "${RECIPE}/chroot/"*.iso "${RECIPE}/chroot/"*.hybrid.iso; do
    if [[ -f "$stuck" ]]; then
      echo "Found ISO left in chroot/; moving to ${RECIPE}/"
      mv -f "$stuck" "${RECIPE}/"
    fi
  done

  local raw=""
  if [[ -f "${RECIPE}/binary.hybrid.iso" ]]; then
    raw="${RECIPE}/binary.hybrid.iso"
  else
    raw="$(find "$RECIPE" -maxdepth 1 -type f -name '*.iso' ! -name 'spike-live.iso' | head -n1 || true)"
  fi
  if [[ -z "$raw" || ! -f "$raw" ]]; then
    echo "warning: no .iso found in ${RECIPE}; check build.log / lb output" >&2
    exit 3
  fi

  echo "Remastering USB-bootable BIOS+UEFI hybrid → spike-live.iso"
  "${ROOT}/scripts/spike-iso-hybridize.sh" "$raw" "${RECIPE}/spike-live.iso"
  ls -lh "${RECIPE}/spike-live.iso"
  echo "Smoke-test: write spike-live.iso to USB (see docs/dev-guide/04-building-spike.md)"
}

main() {
  case "${1:-}" in
    --help|-h) usage ;;
    --check-deps) check_deps ;;
    --clean-only) run_clean ;;
    --config-only) run_config ;;
    --variant)
      echo "error: --variant is not supported (one ISO only)." >&2
      echo "Standard/Plus is selected at install time." >&2
      exit 1
      ;;
    "") build_iso ;;
    *)
      echo "unknown option: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
}

main "$@"
