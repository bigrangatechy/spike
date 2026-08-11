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

  (default)       Clean old ISOs, package .debs, build hybrid live ISO
  --check-deps    Verify host tools for live-build
  --clean-only    Full clean (chroot, caches, stages, all *.iso artifacts)
  --config-only   Run lb config (via auto/config) without building
  --help          Show this help

Requires root (or passwordless sudo) for chroot/loop mounts.

Old ISO/image artifacts are removed during clean and again immediately
before lb build so generation cannot collide with leftovers.

spike-config, spike-shell, and spike-rescue are packaged and copied to includes.chroot (dpkg -i in a hook).
Do not put Spike .debs in packages.chroot — that triggers in-chroot gpg.

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
# Spike Shell needs Qt6 — also require cmake/qt6-base-dev on the *build host*
# for scripts/package-spike-shell.sh (see 03-build-environment.md).
  for cmd in lb debootstrap mksquashfs xorriso gpg cmake; do
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
    echo "    grub-pc-bin grub-efi-amd64-bin grub-efi-amd64-signed shim-signed mtools dosfstools rsync ca-certificates gnupg" >&2
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

remove_iso_artifacts() {
  # Wipe prior ISO/image outputs under the recipe.
  # Reasons:
  # 1. live-build / xorriso / isohybrid can conflict or reuse stale names
  #    when an old binary*.iso or spike-live.iso is still present.
  # 2. Post-build discovery must not pick up yesterday's artifact.
  local reason="${1:-cleanup}"
  echo "Removing old ISO/image artifacts (${reason})..."
  local artifact
  local removed=0
  while IFS= read -r -d '' artifact; do
    echo "  rm ${artifact#"${RECIPE}/"}"
    rm -f "$artifact"
    removed=1
  done < <(
    find "$RECIPE" -maxdepth 3 \( \
      -name '*.iso' -o \
      -name '*.img' -o \
      -name '*.zsync' -o \
      -name '*.zsync.gz' \
    \) -type f -print0 2>/dev/null
  )
  # Named leftovers (in case find maxdepth misses a path)
  rm -f \
    "${RECIPE}"/binary*.iso \
    "${RECIPE}"/binary*.img \
    "${RECIPE}"/binary*.tar.gz \
    "${RECIPE}"/binary*.zsync* \
    "${RECIPE}"/live-image*.iso \
    "${RECIPE}"/*.hybrid.iso \
    "${RECIPE}"/spike-live.iso \
    "${RECIPE}"/chroot/*.iso \
    "${RECIPE}"/chroot/*.hybrid.iso
  if [[ "$removed" -eq 0 ]]; then
    echo "  (none found)"
  fi
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

  remove_iso_artifacts "clean"

  # Never leave local debs in packages.chroot — triggers in-chroot gpg signing.
  find "${RECIPE}/config/packages.chroot" -maxdepth 1 -type f -name '*.deb' -delete 2>/dev/null || true

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
    spike-build-console.log \
    .lock \
    chroot.packages.live chroot.packages.install chroot.headers \
    binary.sh binary.contents binary.packages md5sum.txt

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

# Stage Mozilla APT sources into live-build config/archives/ (needed before
# package-lists install). Tracked copies live in config/spike-archives/.
stage_mozilla_archives() {
  local src="${RECIPE}/config/spike-archives"
  local dest="${RECIPE}/config/archives"
  if [[ ! -f "${src}/mozilla.list" || ! -f "${src}/mozilla.key" || ! -f "${src}/mozilla.pref" ]]; then
    echo "error: missing Mozilla archive files under ${src}" >&2
    exit 4
  fi
  mkdir -p "$dest"
  # Replace lb's empty nobody-owned archives dir when needed.
  if [[ ! -w "$dest" ]]; then
    rm -rf "$dest"
    mkdir -p "$dest"
  fi
  cp -f "${src}/mozilla.list" "${dest}/mozilla.list"
  cp -f "${src}/mozilla.pref" "${dest}/mozilla.pref"
  # live-build copies *.key into trusted.gpg.d as binary-friendly .gpg
  if command -v gpg >/dev/null 2>&1; then
    gpg --batch --yes --dearmor -o "${dest}/mozilla.key" "${src}/mozilla.key"
  else
    cp -f "${src}/mozilla.key" "${dest}/mozilla.key"
  fi
  echo "Staged Mozilla APT archives → config/archives/"
}

# Spike APT keyring + sources for the squashfs (Enabled: no until host is online).
# Do NOT copy spike.list into config/archives/ while the HTTPS host is down —
# live-build would apt-update against a dead mirror and fail.
stage_spike_archives() {
  local src="${RECIPE}/config/spike-archives"
  local keyring_dir="${RECIPE}/config/includes.chroot/usr/share/keyrings"
  local sources_dir="${RECIPE}/config/includes.chroot/etc/apt/sources.list.d"
  local prefs_dir="${RECIPE}/config/includes.chroot/etc/apt/preferences.d"
  if [[ ! -f "${src}/spike.key" ]]; then
    echo "error: missing ${src}/spike.key (run ./scripts/generate-spike-apt-key.sh)" >&2
    exit 4
  fi
  if [[ ! -f "${sources_dir}/spike.sources" ]]; then
    echo "error: missing ${sources_dir}/spike.sources" >&2
    exit 4
  fi
  mkdir -p "$keyring_dir" "$prefs_dir"
  if command -v gpg >/dev/null 2>&1; then
    gpg --batch --yes --dearmor -o "${keyring_dir}/spike-archive-keyring.gpg" "${src}/spike.key"
  else
    echo "error: gpg required to dearmor spike.key" >&2
    exit 4
  fi
  if [[ -f "${src}/spike.pref" ]]; then
    cp -f "${src}/spike.pref" "${prefs_dir}/spike.pref"
  fi
  # Optional: enable Spike archive during ISO build when host is known-good.
  if [[ "${SPIKE_APT_ENABLE:-0}" = "1" ]]; then
    local dest="${RECIPE}/config/archives"
    mkdir -p "$dest"
    if [[ -f "${src}/spike.list" ]] && grep -qE '^[[:space:]]*deb ' "${src}/spike.list"; then
      cp -f "${src}/spike.list" "${dest}/spike.list"
      cp -f "${src}/spike.pref" "${dest}/spike.pref" 2>/dev/null || true
      gpg --batch --yes --dearmor -o "${dest}/spike.key" "${src}/spike.key"
      echo "Staged Spike APT into config/archives/ (SPIKE_APT_ENABLE=1)"
    else
      echo "WARN: SPIKE_APT_ENABLE=1 but spike.list has no active deb line" >&2
    fi
  else
    echo "Spike APT sources on image (Enabled: no) — set SPIKE_APT_ENABLE=1 when host is live"
  fi
}

inject_local_debs() {
  # Build Spike .debs and stage via includes.chroot + hook (dpkg -i).
  # Do NOT use config/packages.chroot/: live-build signs that local repo with
  # gpg inside the bootstrap chroot (fails with env: 'gpg': No such file).
  local pkg_dir="${ROOT}/build/packages"
  local inc_dir="${RECIPE}/config/includes.chroot/var/cache/spike-local"
  local pkg_chroot="${RECIPE}/config/packages.chroot"
  mkdir -p "$pkg_dir" "$inc_dir"

  stage_mozilla_archives
  stage_spike_archives

  echo "Packaging spike-config ..."
  "${ROOT}/scripts/package-spike-config.sh" --out "$pkg_dir"
  echo "Packaging spike-shell ..."
  "${ROOT}/scripts/package-spike-shell.sh" --out "$pkg_dir"
  echo "Packaging spike-rescue ..."
  "${ROOT}/scripts/package-spike-rescue.sh" --out "$pkg_dir"
  echo "Packaging spike-installer ..."
  "${ROOT}/scripts/package-spike-installer.sh" --out "$pkg_dir"
  echo "Packaging spike-migration ..."
  "${ROOT}/scripts/package-spike-migration.sh" --out "$pkg_dir"

  # Clear any prior packages.chroot debs so archives won't invoke gpg.
  if [[ -d "$pkg_chroot" ]]; then
    find "$pkg_chroot" -maxdepth 1 -type f -name '*.deb' -delete 2>/dev/null || true
  fi

  find "$inc_dir" -maxdepth 1 -type f -name 'spike-*.deb' -delete 2>/dev/null || true

  stage_newest() {
    local pattern="$1"
    local newest=""
    newest="$(ls -1 "${pkg_dir}"/${pattern} 2>/dev/null | sort -V | tail -n1 || true)"
    if [[ -n "$newest" && -f "$newest" ]]; then
      cp -f "$newest" "$inc_dir/"
      echo "Staged $(basename "$newest") → includes.chroot/var/cache/spike-local/"
      return 0
    fi
    return 1
  }

  if ! stage_newest 'spike-config_*.deb'; then
    echo "error: spike-config .deb missing after package step" >&2
    exit 4
  fi
  if ! stage_newest 'spike-shell_*.deb'; then
    echo "error: spike-shell .deb missing after package step" >&2
    exit 4
  fi
  if ! stage_newest 'spike-rescue_*.deb'; then
    echo "error: spike-rescue .deb missing after package step" >&2
    exit 4
  fi
  if ! stage_newest 'spike-installer_*.deb'; then
    echo "error: spike-installer .deb missing after package step" >&2
    exit 4
  fi
  if ! stage_newest 'spike-migration_*.deb'; then
    echo "error: spike-migration .deb missing after package step" >&2
    exit 4
  fi
}

build_iso() {
  need_root "$@"
  check_deps
  require_recipe_ready

  echo "Building Spike ISO (single artifact) in ${RECIPE} ..."
  spike_clean
  inject_local_debs

  cd "$RECIPE"
  if [[ -x ./auto/config ]]; then
    ./auto/config
  else
    lb config
  fi

  # Extra guard: no stale ISOs right before generation (avoids lb/xorriso clashes).
  remove_iso_artifacts "pre-lb-build"

  # live-build sometimes cleans up and exits 0 after a mid-chroot failure
  # (e.g. missing gpg in bootstrap). Treat a missing binary image as hard fail.
  set +e
  lb build 2>&1 | tee -a "${RECIPE}/spike-build-console.log"
  local lb_rc=${PIPESTATUS[0]}
  set -e

  echo "Looking for ISO artifacts (lb exit=${lb_rc})..."
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
    echo "error: no .iso produced (lb exit=${lb_rc})." >&2
    echo "Last errors from build.log:" >&2
    if command -v rg >/dev/null 2>&1; then
      rg -n "^(E:|error:|env:)" "${RECIPE}/build.log" 2>/dev/null | tail -n 20 >&2 || true
    else
      grep -nE '^(E:|error:|env:)' "${RECIPE}/build.log" 2>/dev/null | tail -n 20 >&2 || true
    fi
    if [[ ! -s "${RECIPE}/build.log" ]] || ! grep -qE '^(E:|error:|env:)' "${RECIPE}/build.log" 2>/dev/null; then
      tail -n 40 "${RECIPE}/build.log" >&2 || true
    fi
    if grep -q "env: 'gpg': No such file" "${RECIPE}/build.log" 2>/dev/null; then
      echo "hint: local packages.chroot .debs make live-build run gpg in chroot." >&2
      echo "      Spike now installs spike-config via includes.chroot + dpkg -i." >&2
      echo "      Remove any *.deb under config/packages.chroot/ and rebuild clean." >&2
    fi
    exit 3
  fi
  if [[ "$lb_rc" -ne 0 ]]; then
    echo "warning: lb build exited ${lb_rc} but an ISO was found; continuing remaster" >&2
  fi

  # Drop previous ship artifact so remaster cannot collide with an old spike-live.iso.
  rm -f "${RECIPE}/spike-live.iso"
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
