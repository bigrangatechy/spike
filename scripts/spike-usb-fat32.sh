#!/usr/bin/env bash
# Prepare a UEFI-friendly USB by FAT32 extraction (not hybrid dd).
#
# Research (2026-08-05): Some laptop firmwares never list hybrid GPT/ISO9660
# USB sticks in the boot menu, even when the ISO layout matches Kubuntu. The
# most compatible removable-media path is:
#   single FAT32 partition (esp/boot flag) + \EFI\BOOT\BOOTX64.EFI
# See: Ubuntu Installation/iso2usb "UEFI only" extract method.
#
# Usage (DESTRUCTIVE to the whole USB device):
#   sudo ./scripts/spike-usb-fat32.sh /dev/sdX [spike-live.iso]
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEV="${1:-}"
ISO="${2:-${ROOT}/build/iso-build/spike-live.iso}"

if [[ "${EUID}" -ne 0 ]]; then
  echo "error: must run as root (needs wipe/format of ${DEV:-USB})" >&2
  exit 1
fi
if [[ -z "${DEV}" || ! -b "${DEV}" ]]; then
  echo "usage: sudo $0 /dev/sdX [iso]" >&2
  echo "WARNING: this erases the entire device." >&2
  lsblk -d -o NAME,SIZE,MODEL,TRAN | sed 's/^/  /'
  exit 1
fi
if [[ ! -f "${ISO}" ]]; then
  echo "error: ISO not found: ${ISO}" >&2
  exit 1
fi

# Refuse clearly-wrong targets
case "${DEV}" in
  /dev/sda|/dev/nvme0n1|/dev/nvme1n1|/dev/mmcblk0)
    echo "error: refusing likely internal disk ${DEV}" >&2
    exit 1
    ;;
esac

echo "Device: ${DEV}"
lsblk "${DEV}"
echo "ISO:    ${ISO}"
read -r -p "Type YES to erase ${DEV} and write Spike FAT32 USB: " ans
[[ "${ans}" == "YES" ]] || { echo "aborted"; exit 1; }

umount "${DEV}"* 2>/dev/null || true

echo "Partitioning GPT + FAT32 ESP..."
wipefs -a "${DEV}"
parted -s "${DEV}" mklabel gpt
parted -s "${DEV}" mkpart ESP fat32 1MiB 100%
parted -s "${DEV}" set 1 esp on
parted -s "${DEV}" set 1 boot on
sleep 1
partprobe "${DEV}" || true

PART=""
for p in "${DEV}1" "${DEV}p1"; do
  [[ -b "$p" ]] && PART="$p" && break
done
[[ -n "${PART}" ]] || { echo "error: partition not found after parted"; exit 1; }

mkfs.vfat -F 32 -n SPIKELIVE "${PART}"

MNT="$(mktemp -d /tmp/spike-usb-XXXXXX)"
cleanup() { umount "${MNT}" 2>/dev/null || true; rmdir "${MNT}" 2>/dev/null || true; }
trap cleanup EXIT
mount "${PART}" "${MNT}"

echo "Extracting ISO contents to FAT32..."
# 7z handles Rock Ridge better than plain mount+cp for some trees; fall back to xorriso.
if command -v 7z >/dev/null 2>&1; then
  7z x -y -o"${MNT}" "${ISO}" >/dev/null
else
  xorriso -osirrox on -indev "${ISO}" -extract / "${MNT}" >/dev/null
  chmod -R u+rwX "${MNT}" 2>/dev/null || true
fi

# Ensure removable-media UEFI path exists (shim preferred).
mkdir -p "${MNT}/EFI/BOOT" "${MNT}/EFI/boot"
if [[ -f "${MNT}/EFI/boot/bootx64.efi" ]]; then
  cp -f "${MNT}/EFI/boot/"* "${MNT}/EFI/BOOT/" 2>/dev/null || true
elif [[ -f "${MNT}/EFI/BOOT/BOOTX64.EFI" ]]; then
  cp -f "${MNT}/EFI/BOOT/"* "${MNT}/EFI/boot/" 2>/dev/null || true
elif [[ -f /usr/lib/shim/shimx64.efi.signed.latest && -f /usr/lib/grub/x86_64-efi-signed/grubx64.efi.signed ]]; then
  cp -f /usr/lib/shim/shimx64.efi.signed.latest "${MNT}/EFI/BOOT/BOOTX64.EFI"
  cp -f /usr/lib/grub/x86_64-efi-signed/grubx64.efi.signed "${MNT}/EFI/BOOT/grubx64.efi"
  cp -f /usr/lib/shim/mmx64.efi "${MNT}/EFI/BOOT/mmx64.efi" 2>/dev/null || true
  cp -f "${MNT}/EFI/BOOT/"* "${MNT}/EFI/boot/"
else
  echo "error: no EFI bootloader found on ISO or host" >&2
  exit 1
fi

sync
umount "${MNT}"
trap - EXIT
rmdir "${MNT}" 2>/dev/null || true

echo
echo "Done. Verify:"
echo "  lsblk -f ${DEV}"
echo "  # should show vfat SPIKELIVE; mount and check EFI/BOOT/BOOTX64.EFI"
echo "Boot menu: pick UEFI: SPIKELIVE / USB. If Secure Boot complains, enroll MOK or disable it."
