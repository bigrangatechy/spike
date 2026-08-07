#!/usr/bin/env python3
# Patch a casper multi-layer initrd during Spike ISO remaster:
#   1) Rewrite conf/uuid.conf to match ISO9660 volume UUID (blkid)
#   2) Ensure overlay.ko is reachable from the main layer + preload it
#      (casper panics with "/cow format specified as 'overlay' and no support
#      found" when modprobe overlay fails even though the live medium was found)
#
# Initrd layout (Ubuntu): uncompressed newc archive(s) [microcode…, modules…]
# then one compressed newc trailer [scripts, conf, kmod, …].
"""Usage: spike-patch-initrd-uuid.py <initrd> <uuid>"""

from __future__ import annotations

import gzip
import lzma
import os
import re
import shutil
import stat
import subprocess
import sys
import tempfile
from pathlib import Path

SPIKE_OVERLAY_INIT_TOP = """#!/bin/sh
# Spike: load overlay early so casper's setup_overlay does not panic.
PREREQ=\"\"
prereqs() { echo \"$PREREQ\"; }
case $1 in
prereqs) prereqs; exit 0 ;;
esac

if grep -qw overlay /proc/filesystems 2>/dev/null; then
	exit 0
fi
modprobe overlay 2>/dev/null || modprobe -b overlay 2>/dev/null || true
if grep -qw overlay /proc/filesystems 2>/dev/null; then
	exit 0
fi
for f in /lib/modules/*/kernel/fs/overlayfs/overlay.ko \\
	/lib/modules/*/kernel/fs/overlayfs/overlay.ko.zst \\
	/usr/lib/modules/*/kernel/fs/overlayfs/overlay.ko \\
	/usr/lib/modules/*/kernel/fs/overlayfs/overlay.ko.zst; do
	[ -e \"$f\" ] || continue
	insmod \"$f\" 2>/dev/null && break
done
exit 0
"""

# Indented to match setup_overlay() body in scripts/casper.
CASPER_OVERLAY_LOAD = """\
    # Spike: resilient overlay load (modprobe -b alone has failed on live USB).
    if ! grep -qw overlay /proc/filesystems 2>/dev/null; then
        modprobe \"${MP_QUIET}\" overlay || modprobe \"${MP_QUIET}\" -b overlay || true
    fi
    if ! grep -qw overlay /proc/filesystems 2>/dev/null; then
        for _spike_ov in /lib/modules/$(uname -r)/kernel/fs/overlayfs/overlay.ko \\
            /lib/modules/$(uname -r)/kernel/fs/overlayfs/overlay.ko.zst \\
            /usr/lib/modules/$(uname -r)/kernel/fs/overlayfs/overlay.ko \\
            /usr/lib/modules/$(uname -r)/kernel/fs/overlayfs/overlay.ko.zst; do
            [ -e \"${_spike_ov}\" ] || continue
            insmod \"${_spike_ov}\" || true
            break
        done
    fi
    grep -qw overlay /proc/filesystems || panic \"/cow format specified as 'overlay' and no support found\"
"""


def newc_uncompressed_end(data: bytes) -> int:
    """Offset of the first byte after consecutive uncompressed newc archives."""
    pos = 0
    n = len(data)
    while pos + 110 <= n and data[pos : pos + 6] in (b"070701", b"070702"):
        while True:
            if pos + 110 > n or data[pos : pos + 6] not in (b"070701", b"070702"):
                return pos
            namesize = int(data[pos + 94 : pos + 102], 16)
            filesize = int(data[pos + 54 : pos + 62], 16)
            header_end = pos + 110
            name_end = header_end + namesize
            name_pad = (4 - (name_end % 4)) % 4
            data_start = name_end + name_pad
            data_end = data_start + filesize
            data_pad = (4 - (data_end % 4)) % 4
            name = data[header_end : header_end + namesize - 1]
            pos = data_end + data_pad
            if name == b"TRAILER!!!":
                while pos < n and data[pos] == 0:
                    pos += 1
                break
    return pos


def iter_uncompressed_newc(data: bytes) -> list[bytes]:
    """Return each uncompressed newc archive blob (before compressed trailer)."""
    pos = 0
    n = len(data)
    archives: list[bytes] = []
    while pos + 110 <= n and data[pos : pos + 6] in (b"070701", b"070702"):
        start = pos
        while True:
            if pos + 110 > n or data[pos : pos + 6] not in (b"070701", b"070702"):
                return archives
            namesize = int(data[pos + 94 : pos + 102], 16)
            filesize = int(data[pos + 54 : pos + 62], 16)
            header_end = pos + 110
            name_end = header_end + namesize
            name_pad = (4 - (name_end % 4)) % 4
            data_start = name_end + name_pad
            data_end = data_start + filesize
            data_pad = (4 - (data_end % 4)) % 4
            name = data[header_end : header_end + namesize - 1]
            pos = data_end + data_pad
            if name == b"TRAILER!!!":
                while pos < n and data[pos] == 0:
                    pos += 1
                break
        archives.append(data[start:pos])
    return archives


def decompress(blob: bytes) -> tuple[bytes, str]:
    if blob.startswith(b"\x1f\x8b\x08"):
        return gzip.decompress(blob), "gzip"
    if blob.startswith(b"\xfd7zXZ\x00"):
        return lzma.decompress(blob), "xz"
    if blob.startswith(b"\x28\xb5\x2f\xfd"):
        proc = subprocess.run(["zstd", "-d", "-c"], input=blob, capture_output=True)
        if proc.returncode == 0:
            return proc.stdout, "zstd"
        raise RuntimeError(f"zstd decompress failed: {proc.stderr[:200]!r}")
    raise RuntimeError(f"unknown initrd compression magic={blob[:8]!r}")


def compress(payload: bytes, kind: str) -> bytes:
    if kind == "gzip":
        return gzip.compress(payload, compresslevel=9)
    if kind == "xz":
        return lzma.compress(payload)
    if kind == "zstd":
        proc = subprocess.run(
            ["zstd", "-19", "-c"], input=payload, capture_output=True, check=True
        )
        return proc.stdout
    raise RuntimeError(f"unhandled compression kind={kind!r}")


def cpio_extract(cpio_blob: bytes, dest: Path) -> None:
    dest.mkdir(parents=True, exist_ok=True)
    subprocess.run(
        ["cpio", "-id", "--quiet", "--no-absolute-filenames", "--no-preserve-owner"],
        cwd=dest,
        input=cpio_blob,
        check=True,
        stderr=subprocess.DEVNULL,
    )


def cpio_pack(root: Path) -> bytes:
    listing = subprocess.check_output(["find", ".", "-print0"], cwd=root)
    return subprocess.check_output(
        ["cpio", "-o", "-H", "newc", "--quiet", "--null"],
        cwd=root,
        input=listing,
    )


def find_overlay_ko(early_archives: list[bytes]) -> tuple[str, bytes] | None:
    """Return (relative path, bytes) for overlay.ko* from the modules archive."""
    for blob in early_archives:
        with tempfile.TemporaryDirectory(prefix="spike-mods-") as tmp:
            root = Path(tmp)
            try:
                cpio_extract(blob, root)
            except subprocess.CalledProcessError:
                continue
            hits = list(root.rglob("overlay.ko*"))
            if not hits:
                continue
            ko = hits[0]
            rel = str(ko.relative_to(root))
            return rel, ko.read_bytes()
    return None


def patch_casper_overlay_load(casper: Path) -> bool:
    """Replace the single modprobe-overlay panic line with a resilient block."""
    text = casper.read_text(encoding="utf-8", errors="replace")
    pattern = re.compile(
        r'^[ \t]*modprobe "\$\{MP_QUIET\}" -b overlay \|\| '
        r'panic "/cow format specified as \'overlay\' and no support found"[ \t]*$',
        re.MULTILINE,
    )
    new_text, n = pattern.subn(CASPER_OVERLAY_LOAD.rstrip("\n"), text, count=1)
    if n != 1:
        return False
    casper.write_text(new_text, encoding="utf-8")
    return True


def patch_initrd(path: Path, new_uuid: str) -> None:
    data = path.read_bytes()
    split = newc_uncompressed_end(data)
    if split >= len(data):
        raise RuntimeError(f"{path}: no compressed trailer after newc archives")
    early = data[:split]
    archives = iter_uncompressed_newc(early)
    overlay = find_overlay_ko(archives)
    main, kind = decompress(data[split:])

    with tempfile.TemporaryDirectory(prefix="spike-initrd-") as tmp:
        root = Path(tmp) / "root"
        cpio_extract(main, root)

        uuid_path = root / "conf" / "uuid.conf"
        if not uuid_path.is_file():
            raise RuntimeError(f"{path}: conf/uuid.conf missing in initrd")
        uuid_path.write_text(new_uuid.rstrip() + "\n", encoding="utf-8")

        # Preload list for initramfs-tools init.
        modules_conf = root / "conf" / "modules"
        existing = ""
        if modules_conf.is_file():
            existing = modules_conf.read_text(encoding="utf-8", errors="replace")
        if re.search(r"(?m)^overlay\s*$", existing) is None:
            modules_conf.write_text(
                existing.rstrip() + ("\n" if existing.strip() else "") + "overlay\n",
                encoding="utf-8",
            )

        # Early init-top hook (belt and suspenders with conf/modules).
        init_top = root / "scripts" / "init-top" / "00-spike-overlay"
        init_top.parent.mkdir(parents=True, exist_ok=True)
        init_top.write_text(SPIKE_OVERLAY_INIT_TOP, encoding="utf-8")
        init_top.chmod(init_top.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

        # Copy overlay.ko into the main layer so it survives even if the
        # modules archive is slow/partial on low-RAM boxes.
        if overlay is not None:
            rel, blob = overlay
            dest = root / rel
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(blob)
            print(f"  embedded {rel} ({len(blob)} bytes) into main initrd layer")
        else:
            print("  warning: overlay.ko* not found in early initrd archives", file=sys.stderr)

        casper = root / "scripts" / "casper"
        if casper.is_file():
            if patch_casper_overlay_load(casper):
                print("  patched scripts/casper overlay load")
            else:
                print(
                    "  warning: could not patch scripts/casper overlay line",
                    file=sys.stderr,
                )

        new_cpio = cpio_pack(root)

    out = early + compress(new_cpio, kind)
    tmp_out = path.with_suffix(path.suffix + ".spike-tmp")
    tmp_out.write_bytes(out)
    tmp_out.replace(path)


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__.strip(), file=sys.stderr)
        return 2
    initrd = Path(sys.argv[1])
    new_uuid = sys.argv[2].strip()
    if not initrd.is_file():
        print(f"error: initrd not found: {initrd}", file=sys.stderr)
        return 1
    if not new_uuid:
        print("error: empty uuid", file=sys.stderr)
        return 1
    patch_initrd(initrd, new_uuid)
    print(f"Patched {initrd}: uuid={new_uuid} + overlay preload")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
