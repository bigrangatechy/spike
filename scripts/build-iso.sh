#!/usr/bin/env bash
# Spike ISO build wrapper — live-build, one hybrid ISO.
# Spec: docs/dev-guide/04-building-spike.md
# Decision: docs/agent-ops/DECISIONS.md (live-build + one ISO + custom installer)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RECIPE="${ROOT}/build/iso-build"
# Marker file: create when the live-build recipe is complete enough to run lb build.
READY_MARKER="${RECIPE}/.recipe-ready"

usage() {
  cat <<'EOF'
Usage: ./scripts/build-iso.sh [option]

  (default)       Build the single Spike hybrid live ISO (when recipe is ready)
  --check-deps    Verify host tools for live-build
  --help          Show this help

Spike builds ONE ISO. There is no --variant standard|plus flag.
Standard vs Plus is applied at install time by spike-installer + spike-config.
EOF
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
  if [[ "$missing" -ne 0 ]]; then
    echo "Install host packages — see docs/dev-guide/03-build-environment.md" >&2
    return 1
  fi
  echo "Dependency check passed."
}

build_iso() {
  if [[ ! -d "$RECIPE" ]]; then
    echo "Recipe directory missing: $RECIPE" >&2
    exit 1
  fi

  check_deps

  if [[ ! -f "$READY_MARKER" ]]; then
    cat >&2 <<EOF
build-iso.sh: live-build recipe is not marked ready yet.

  Recipe path: $RECIPE
  When package-lists/hooks are filled, create:
    $READY_MARKER

  Until then this stub will not run lb build (avoids a broken ISO).
  Docs: docs/dev-guide/04-building-spike.md
EOF
    exit 2
  fi

  echo "Building Spike ISO (single artifact) in ${RECIPE} ..."
  cd "$RECIPE"
  # Real sequence once auto/config exists:
  #   lb clean
  #   lb config
  #   lb build
  echo "TODO: invoke lb clean / lb config / lb build when auto/* is implemented." >&2
  exit 3
}

main() {
  case "${1:-}" in
    --help|-h) usage ;;
    --check-deps) check_deps ;;
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
