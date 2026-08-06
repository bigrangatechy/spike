#!/usr/bin/env bash
# Remaster a Spike live-build ISO into a USB-bootable BIOS + UEFI hybrid.
#
# Why: Ubuntu's live-build 3.x builds grub_eltorito (El Torito / optical) but
# classic `isohybrid` only patches isolinux. Without this step, KDE ISO Writer
# / dd produce a stick modern firmware never lists (no MBR, no EFI).
#
# Usage:
#   ./scripts/spike-iso-hybridize.sh [input.iso] [output.iso]
# Defaults: build/iso-build/binary.hybrid.iso → build/iso-build/spike-live.iso
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RECIPE="${ROOT}/build/iso-build"
IN_ISO="${1:-}"
OUT_ISO="${2:-${RECIPE}/spike-live.iso}"
if [[ -z "${IN_ISO}" ]]; then
  if [[ -f "${RECIPE}/binary.hybrid.iso" ]]; then
    IN_ISO="${RECIPE}/binary.hybrid.iso"
  elif [[ -f "${RECIPE}/spike-live.iso" ]]; then
    IN_ISO="${RECIPE}/spike-live.iso"
  else
    IN_ISO="${RECIPE}/binary.hybrid.iso"
  fi
fi
WORK="$(mktemp -d "${TMPDIR:-/tmp}/spike-iso-XXXXXX")"
cleanup() { rm -rf "${WORK}"; }
trap cleanup EXIT

need() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "error: missing tool: $1" >&2
    exit 1
  }
}

need xorriso
need grub-mkimage
need mkfs.vfat
need mcopy
need mmd

[[ -f "${IN_ISO}" ]] || {
  echo "error: input ISO not found: ${IN_ISO}" >&2
  exit 1
}

for f in /usr/lib/grub/i386-pc/boot_hybrid.img /usr/lib/grub/x86_64-efi; do
  [[ -e "$f" ]] || {
    echo "error: missing $f — install grub-pc-bin grub-efi-amd64-bin" >&2
    exit 1
  }
done

echo "Input:  ${IN_ISO}"
echo "Output: ${OUT_ISO}"
echo "Work:   ${WORK}"

TREE="${WORK}/tree"
EFI_IMG="${WORK}/efi.img"
EMBED="${WORK}/grub-embed.cfg"
mkdir -p "${TREE}"

echo "Extracting ISO tree..."
xorriso -osirrox on -indev "${IN_ISO}" -extract / "${TREE}" >/dev/null
# Extracted tree often preserves root ownership / mode 444 from the ISO.
chmod -R u+rwX "${TREE}" 2>/dev/null || true

# Clean Stage-1 GRUB menu (live-build sed can scramble template + entries).
KERNEL="$(basename "$(echo "${TREE}"/casper/vmlinuz-* | awk '{print $1}')")"
INITRD="$(basename "$(echo "${TREE}"/casper/initrd.img-* | awk '{print $1}')")"
if [[ ! -f "${TREE}/casper/${KERNEL}" || ! -f "${TREE}/casper/${INITRD}" ]]; then
  echo "error: could not find casper kernel/initrd under ${TREE}/casper" >&2
  ls -la "${TREE}/casper" >&2 || true
  exit 1
fi

mkdir -p "${TREE}/boot/grub" "${TREE}/EFI/BOOT"

# live-build dumps BIOS *.mod flat into /boot/grub — wrong for EFI GRUB (needs
# /boot/grub/x86_64-efi/). Clear the flat dump and install Ubuntu-like layout.
find "${TREE}/boot/grub" -maxdepth 1 -type f \( -name '*.mod' -o -name '*.lst' -o -name '*.img' -o -name '*.o' -o -name 'efiemu*' \) -delete 2>/dev/null || true
mkdir -p "${TREE}/boot/grub/x86_64-efi" "${TREE}/boot/grub/i386-pc"
cp -a /usr/lib/grub/x86_64-efi/. "${TREE}/boot/grub/x86_64-efi/"
cp -a /usr/lib/grub/i386-pc/. "${TREE}/boot/grub/i386-pc/"

cat > "${TREE}/boot/grub/grub.cfg" <<EOF
set default=0
set timeout=8
set color_normal=white/black
set color_highlight=black/light-gray

menuentry "Spike Live" {
	linux	/casper/${KERNEL} boot=casper hostname=spike-live quiet splash ---
	initrd	/casper/${INITRD}
}

menuentry "Spike Live (debug logging)" {
	linux	/casper/${KERNEL} boot=casper hostname=spike-live debug ---
	initrd	/casper/${INITRD}
}

menuentry "Spike Live (safe graphics)" {
	linux	/casper/${KERNEL} boot=casper hostname=spike-live nomodeset quiet splash ---
	initrd	/casper/${INITRD}
}
EOF

# Early config for EFI: lives next to grubx64.efi on the ESP so shim→grub
# never drops to a bare console when prefix search fails.
cat > "${WORK}/esp-grub.cfg" <<EOF
set timeout=8
search --no-floppy --set=root --file /casper/filesystem.squashfs
set prefix=(\$root)/boot/grub

menuentry "Spike Live" {
	linux	/casper/${KERNEL} boot=casper hostname=spike-live quiet splash ---
	initrd	/casper/${INITRD}
}

menuentry "Spike Live (debug logging)" {
	linux	/casper/${KERNEL} boot=casper hostname=spike-live debug ---
	initrd	/casper/${INITRD}
}

menuentry "Spike Live (safe graphics)" {
	linux	/casper/${KERNEL} boot=casper hostname=spike-live nomodeset quiet splash ---
	initrd	/casper/${INITRD}
}
EOF

echo "Building EFI boot chain (Ubuntu-style shim → grub)..."
SHIM_SRC=""
for s in \
  /usr/lib/shim/shimx64.efi.signed.latest \
  /usr/lib/shim/shimx64.efi.signed \
  /usr/lib/shim/shimx64.efi
do
  [[ -f "$s" ]] && SHIM_SRC="$s" && break
done
# gcdx64 is the ISO/CD-oriented signed image (includes iso9660); prefer it as
# grubx64.efi so the ESP menu can read the ISO9660 partition.
GRUB_SIGNED=""
for g in \
  /usr/lib/grub/x86_64-efi-signed/gcdx64.efi.signed \
  /usr/lib/grub/x86_64-efi-signed/grubx64.efi.signed
do
  [[ -f "$g" ]] && GRUB_SIGNED="$g" && break
done
if [[ -z "${SHIM_SRC}" || -z "${GRUB_SIGNED}" ]]; then
  echo "error: need shim-signed + grub-efi-amd64-signed on the build host" >&2
  exit 1
fi
mkdir -p "${TREE}/EFI/boot"
cp -f "${SHIM_SRC}" "${TREE}/EFI/boot/bootx64.efi"
cp -f "${GRUB_SIGNED}" "${TREE}/EFI/boot/grubx64.efi"
cp -f "${WORK}/esp-grub.cfg" "${TREE}/EFI/boot/grub.cfg"
[[ -f /usr/lib/shim/mmx64.efi ]] && cp -f /usr/lib/shim/mmx64.efi "${TREE}/EFI/boot/mmx64.efi"
mkdir -p "${TREE}/EFI/BOOT"
cp -f "${TREE}/EFI/boot/"* "${TREE}/EFI/BOOT/" 2>/dev/null || true

# FAT12 ESP like Kubuntu (~6MiB).
dd if=/dev/zero of="${EFI_IMG}" bs=1M count=6 status=none
mkfs.vfat -F 12 -n ESP "${EFI_IMG}" >/dev/null
mmd -i "${EFI_IMG}" ::/EFI ::/EFI/boot
mcopy -i "${EFI_IMG}" "${TREE}/EFI/boot/bootx64.efi" ::/EFI/boot/bootx64.efi
mcopy -i "${EFI_IMG}" "${TREE}/EFI/boot/grubx64.efi" ::/EFI/boot/grubx64.efi
mcopy -i "${EFI_IMG}" "${TREE}/EFI/boot/grub.cfg" ::/EFI/boot/grub.cfg
[[ -f "${TREE}/EFI/boot/mmx64.efi" ]] && mcopy -i "${EFI_IMG}" "${TREE}/EFI/boot/mmx64.efi" ::/EFI/boot/mmx64.efi

echo "Building BIOS eltorito.img (Ubuntu path)..."
mkdir -p "${TREE}/boot/grub/i386-pc"
CORE="${WORK}/core.img"
grub-mkimage -O i386-pc -o "${CORE}" -p /boot/grub \
  biosdisk iso9660 normal configfile \
  search search_label search_fs_file search_fs_uuid \
  fat part_msdos part_gpt echo test true gzio
cat /usr/lib/grub/i386-pc/cdboot.img "${CORE}" > "${TREE}/boot/grub/i386-pc/eltorito.img"
cp -f "${TREE}/boot/grub/i386-pc/eltorito.img" "${TREE}/boot/grub/grub_eltorito"
ELTORITO="boot/grub/i386-pc/eltorito.img"

VOLID="Spike Live"
VOLID="${VOLID:0:32}"

echo "Writing hybrid BIOS+UEFI ISO (${VOLID}) — Kubuntu-matching xorriso flags..."
TMP_OUT="${OUT_ISO}.tmp"
rm -f "${TMP_OUT}" "${OUT_ISO}"
set +e
xorriso -as mkisofs \
  -r -J -joliet-long \
  -V "${VOLID}" \
  -o "${TMP_OUT}" \
  --grub2-mbr /usr/lib/grub/i386-pc/boot_hybrid.img \
  --protective-msdos-label \
  -partition_offset 16 \
  --mbr-force-bootable \
  -append_partition 2 0xef "${EFI_IMG}" \
  -appended_part_as_gpt \
  -iso_mbr_part_type a2a0d0ebe5b9334487c068b6b72699c7 \
  -b "${ELTORITO}" \
  -c boot.catalog \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  --grub2-boot-info \
  -eltorito-alt-boot \
  -e '--interval:appended_partition_2:all::' \
  -no-emul-boot \
  "${TREE}"
xorrc=$?
set -e
if [[ ! -f "${TMP_OUT}" ]]; then
  echo "error: xorriso failed to write ${TMP_OUT} (exit ${xorrc})" >&2
  exit "${xorrc}"
fi
if [[ "${xorrc}" -ne 0 && "${xorrc}" -ne 32 ]]; then
  echo "error: xorriso failed (exit ${xorrc})" >&2
  rm -f "${TMP_OUT}"
  exit "${xorrc}"
fi
mv -f "${TMP_OUT}" "${OUT_ISO}"

echo
ls -lh "${OUT_ISO}"
echo
echo "Write with KDE ISO Image Writer (same as Kubuntu), using: ${OUT_ISO}"
echo "Or: sudo dd if=${OUT_ISO} of=/dev/sdX bs=4M status=progress oflag=sync"
