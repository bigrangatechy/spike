#!/usr/bin/env bash
# Generate Spike APT signing keypair (once per operator machine / CI secret store).
# Public key is committed under build/iso-build/config/spike-archives/spike.key
# Secret key stays gitignored under build/apt-signing/secret.gpg
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SIGN_DIR="${ROOT}/build/apt-signing"
SECRET="${SIGN_DIR}/secret.gpg"
PUB_ASC="${ROOT}/build/iso-build/config/spike-archives/spike.key"
NAME="${SPIKE_APT_KEY_NAME:-Spike Linux APT Archive}"
EMAIL="${SPIKE_APT_KEY_EMAIL:-apt-archive@bigrangatech.com}"

command -v gpg >/dev/null || {
  echo "error: gpg required" >&2
  exit 1
}

mkdir -p "$SIGN_DIR"
chmod 700 "$SIGN_DIR"

if [[ -f "$SECRET" && -f "$PUB_ASC" ]]; then
  echo "Signing material already present:"
  echo "  secret: $SECRET"
  echo "  public: $PUB_ASC"
  exit 0
fi

if [[ -f "$SECRET" && ! -f "$PUB_ASC" ]]; then
  echo "Exporting public key from existing secret…"
  export GNUPGHOME
  GNUPGHOME="$(mktemp -d)"
  trap 'rm -rf "$GNUPGHOME"' EXIT
  chmod 700 "$GNUPGHOME"
  gpg --batch --import "$SECRET" >/dev/null 2>&1
  gpg --batch --armor --export "$EMAIL" >"$PUB_ASC"
  echo "Wrote $PUB_ASC"
  exit 0
fi

echo "Generating new Spike APT signing key (${EMAIL})…"
export GNUPGHOME
GNUPGHOME="$(mktemp -d)"
trap 'rm -rf "$GNUPGHOME"' EXIT
chmod 700 "$GNUPGHOME"

gpg --batch --pinentry-mode loopback --passphrase '' --quick-generate-key \
  "${NAME} <${EMAIL}>" default default 0

gpg --batch --export-secret-keys --armor "$EMAIL" >"$SECRET"
chmod 600 "$SECRET"
gpg --batch --armor --export "$EMAIL" >"$PUB_ASC"

# Binary keyring for includes.chroot (also regenerated in build-iso / hooks).
gpg --batch --dearmor -o \
  "${ROOT}/build/iso-build/config/includes.chroot/usr/share/keyrings/spike-archive-keyring.gpg" \
  "$PUB_ASC"

echo "Wrote:"
echo "  $SECRET  (KEEP PRIVATE — gitignored)"
echo "  $PUB_ASC (commit this)"
echo "  includes.chroot/.../spike-archive-keyring.gpg"
