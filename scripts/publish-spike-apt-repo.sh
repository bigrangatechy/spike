#!/usr/bin/env bash
# Build a signed APT repository tree from Spike .debs in build/packages/.
# Spec plan: Spike APT updates (spike-*.deb only — not a full OS OTA).
#
# Usage:
#   ./scripts/generate-spike-apt-key.sh          # once — public key → spike-archives/
#   ./scripts/package-spike-*.sh                  # build debs
#   ./scripts/publish-spike-apt-repo.sh           # → build/apt-repo/
#   # Upload build/apt-repo/ to SPIKE_APT_REPO_URL when hosting is online.
#
# Env:
#   SPIKE_APT_REPO_DIR   output tree (default: build/apt-repo)
#   SPIKE_APT_SIGN_KEY   secret keyring file (default: build/apt-signing/secret.gpg)
#   SPIKE_APT_CODENAME   suite name (default: spike)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PKG_DIR="${ROOT}/build/packages"
OUT="${SPIKE_APT_REPO_DIR:-${ROOT}/build/apt-repo}"
CODENAME="${SPIKE_APT_CODENAME:-spike}"
COMPONENT=main
ARCH=amd64
SECRET="${SPIKE_APT_SIGN_KEY:-${ROOT}/build/apt-signing/secret.gpg}"
PUB_ASC="${ROOT}/build/iso-build/config/spike-archives/spike.key"

usage() {
  cat <<EOF
Usage: ./scripts/publish-spike-apt-repo.sh [--out DIR]

Assembles pool/ + dists/${CODENAME}/${COMPONENT}/binary-${ARCH}/ from
build/packages/spike-*.deb and signs InRelease with the Spike packaging key.

Prerequisites:
  ./scripts/generate-spike-apt-key.sh
  apt-ftparchive (apt-utils), gpg

Upload the resulting tree to your HTTPS host (SPIKE_APT_REPO_URL), then set
Enabled: yes in /etc/apt/sources.list.d/spike.sources on images.
EOF
}

case "${1:-}" in
  --help|-h) usage; exit 0 ;;
  --out)
    OUT="${2:?}"
    shift 2 || true
    ;;
  "") ;;
  *)
    echo "unknown option: $1" >&2
    usage >&2
    exit 1
    ;;
esac

command -v gpg >/dev/null || {
  echo "error: gpg required" >&2
  exit 1
}

have_ftparchive=0
have_scanpackages=0
command -v apt-ftparchive >/dev/null && have_ftparchive=1
command -v dpkg-scanpackages >/dev/null && have_scanpackages=1
if [[ "$have_ftparchive" -eq 0 && "$have_scanpackages" -eq 0 ]]; then
  echo "error: need apt-ftparchive (apt-utils) or dpkg-scanpackages (dpkg-dev)" >&2
  exit 1
fi

[[ -f "$SECRET" ]] || {
  echo "error: signing key missing: $SECRET" >&2
  echo "  run: ./scripts/generate-spike-apt-key.sh" >&2
  exit 1
}
[[ -f "$PUB_ASC" ]] || {
  echo "error: public key missing: $PUB_ASC" >&2
  echo "  run: ./scripts/generate-spike-apt-key.sh" >&2
  exit 1
}

shopt -s nullglob
debs=("${PKG_DIR}"/spike-*.deb)
if [[ ${#debs[@]} -eq 0 ]]; then
  echo "error: no spike-*.deb under ${PKG_DIR}" >&2
  echo "  run package-spike-*.sh first" >&2
  exit 1
fi

rm -rf "$OUT"
POOL="${OUT}/pool/${COMPONENT}"
DIST_BIN="${OUT}/dists/${CODENAME}/${COMPONENT}/binary-${ARCH}"
mkdir -p "$POOL" "$DIST_BIN"

# Keep only the newest version of each package name (build/packages accumulates history).
declare -A newest_path newest_ver
for deb in "${debs[@]}"; do
  base="$(basename "$deb" .deb)"
  # name_version-revision_arch
  name="${base%%_*}"
  rest="${base#*_}"
  ver="${rest%%_*}"
  if [[ -z "${newest_ver[$name]:-}" ]] || dpkg --compare-versions "$ver" gt "${newest_ver[$name]}"; then
    newest_ver[$name]="$ver"
    newest_path[$name]="$deb"
  fi
done

copied=0
for name in "${!newest_path[@]}"; do
  deb="${newest_path[$name]}"
  cp -f "$deb" "$POOL/"
  echo "  pool: $(basename "$deb")"
  copied=$((copied + 1))
done

(
  cd "$OUT"
  if [[ "$have_ftparchive" -eq 1 ]]; then
    apt-ftparchive packages "pool/${COMPONENT}" >"dists/${CODENAME}/${COMPONENT}/binary-${ARCH}/Packages"
  else
    dpkg-scanpackages "pool/${COMPONENT}" /dev/null >"dists/${CODENAME}/${COMPONENT}/binary-${ARCH}/Packages"
  fi
  gzip -9 -k -f "dists/${CODENAME}/${COMPONENT}/binary-${ARCH}/Packages"
)

if [[ "$have_ftparchive" -eq 1 ]]; then
  cat >"${OUT}/apt-release.conf" <<EOF
APT::FTPArchive::Release::Origin "Spike";
APT::FTPArchive::Release::Label "Spike";
APT::FTPArchive::Release::Suite "${CODENAME}";
APT::FTPArchive::Release::Codename "${CODENAME}";
APT::FTPArchive::Release::Architectures "${ARCH}";
APT::FTPArchive::Release::Components "${COMPONENT}";
APT::FTPArchive::Release::Description "Spike Linux component packages";
EOF
  (
    cd "$OUT"
    apt-ftparchive -c apt-release.conf release "dists/${CODENAME}" >"dists/${CODENAME}/Release"
    rm -f apt-release.conf
  )
else
  # Minimal Release without apt-ftparchive.
  rel="${OUT}/dists/${CODENAME}/Release"
  {
    echo "Origin: Spike"
    echo "Label: Spike"
    echo "Suite: ${CODENAME}"
    echo "Codename: ${CODENAME}"
    echo "Architectures: ${ARCH}"
    echo "Components: ${COMPONENT}"
    echo "Description: Spike Linux component packages"
    echo "Date: $(date -Ru)"
    echo "MD5Sum:"
    (
      cd "${OUT}/dists/${CODENAME}"
      for f in ${COMPONENT}/binary-${ARCH}/Packages ${COMPONENT}/binary-${ARCH}/Packages.gz; do
        [[ -f "$f" ]] || continue
        printf " %s %8s %s\n" "$(md5sum "$f" | awk '{print $1}')" "$(wc -c <"$f" | tr -d ' ')" "$f"
      done
    )
    echo "SHA256:"
    (
      cd "${OUT}/dists/${CODENAME}"
      for f in ${COMPONENT}/binary-${ARCH}/Packages ${COMPONENT}/binary-${ARCH}/Packages.gz; do
        [[ -f "$f" ]] || continue
        printf " %s %8s %s\n" "$(sha256sum "$f" | awk '{print $1}')" "$(wc -c <"$f" | tr -d ' ')" "$f"
      done
    )
  } >"$rel"
fi

export GNUPGHOME
GNUPGHOME="$(mktemp -d)"
trap 'rm -rf "$GNUPGHOME"' EXIT
chmod 700 "$GNUPGHOME"
gpg --batch --import "$SECRET" >/dev/null 2>&1

(
  cd "${OUT}/dists/${CODENAME}"
  gpg --batch --yes --pinentry-mode loopback --armor --detach-sign -o Release.gpg Release
  gpg --batch --yes --pinentry-mode loopback --clearsign -o InRelease Release
)

# Convenience: copy public key beside the tree for operators.
cp -f "$PUB_ASC" "${OUT}/spike-archive-keyring.asc"

cat >"${OUT}/README.txt" <<EOF
Spike APT repository tree
=========================
Suite: ${CODENAME}
Component: ${COMPONENT}
Arch: ${ARCH}

Upload this directory's contents to your HTTPS document root so that:
  \${SPIKE_APT_REPO_URL}/dists/${CODENAME}/InRelease
  \${SPIKE_APT_REPO_URL}/pool/${COMPONENT}/…

Default SPIKE_APT_REPO_URL (when hosting is ready):
  https://packages.bigrangatech.com/spike

Then set Enabled: yes in /etc/apt/sources.list.d/spike.sources (ISO + installs).
EOF

echo "Built APT repo at ${OUT}"
echo "Packages: ${copied}"
ls -la "${OUT}/dists/${CODENAME}/"
