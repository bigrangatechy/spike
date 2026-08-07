"""Hardware detection from /proc and /sys (live / install target only)."""

from __future__ import annotations

import os
import re
import subprocess
from pathlib import Path
from typing import Any

from spike_config import state as state_mod

PCI_CLASS_VGA = "0x030000"
PCI_CLASS_3D = "0x030200"

PCI_VENDOR_NAMES = {
    "0x8086": "intel",
    "0x1002": "amd",
    "0x10de": "nvidia",
}

VA_API_BY_DRIVER = {
    "i915": "iHD",
    "xe": "iHD",
    "amdgpu": "radeonsi",
    "radeon": "radeonsi",
    "nouveau": "",
    "nvidia": "",
}


def _read_text(path: Path, default: str = "") -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace").strip()
    except OSError:
        return default


def _detect_cpu(cpu: dict[str, Any]) -> None:
    try:
        text = Path("/proc/cpuinfo").read_text(encoding="utf-8")
    except OSError:
        return
    model = "unknown"
    bogomips = 0.0
    for line in text.splitlines():
        if line.startswith("model name") and model == "unknown":
            model = line.split(":", 1)[1].strip()
        if line.lower().startswith("bogomips") and bogomips == 0:
            try:
                bogomips = float(line.split(":", 1)[1].strip())
            except ValueError:
                bogomips = 0.0
    cores = len(re.findall(r"^processor\s*:", text, flags=re.MULTILINE))
    if cores <= 0:
        cores = max(os.cpu_count() or 1, 1)
    cpu["model"] = model
    cpu["cores"] = cores
    cpu["bogomips_per_core"] = bogomips
    if bogomips and bogomips < 1500:
        cpu["classification"] = "low"
    elif bogomips:
        cpu["classification"] = "capable"


def _detect_ram(ram: dict[str, Any]) -> None:
    try:
        with Path("/proc/meminfo").open(encoding="utf-8") as fh:
            for line in fh:
                if line.startswith("MemTotal:"):
                    kb = int(line.split()[1])
                    ram["total_kb"] = kb
                    if kb < 3_000_000:
                        ram["classification"] = "low"
                    elif kb < 6_000_000:
                        ram["classification"] = "standard"
                    else:
                        ram["classification"] = "high"
                    break
    except (OSError, ValueError):
        return


def _pci_gpu_candidates(sys_pci: Path) -> list[Path]:
    out: list[Path] = []
    if not sys_pci.is_dir():
        return out
    for dev in sorted(sys_pci.iterdir()):
        class_id = _read_text(dev / "class").lower()
        if class_id in (PCI_CLASS_VGA, PCI_CLASS_3D):
            out.append(dev)
    return out


def _driver_name(dev: Path) -> str:
    link = dev / "driver"
    if link.is_symlink() or link.exists():
        try:
            return Path(os.path.realpath(link)).name
        except OSError:
            pass
    # Fallback: uevent DRIVER=
    for line in _read_text(dev / "uevent").splitlines():
        if line.startswith("DRIVER="):
            return line.split("=", 1)[1].strip()
    return "unknown"


def _lspci_model(slot: str) -> str:
    """Best-effort human model via lspci when available."""
    try:
        proc = subprocess.run(
            ["lspci", "-s", slot, "-nn"],
            capture_output=True,
            text=True,
            timeout=2,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired):
        return ""
    if proc.returncode != 0 or not proc.stdout.strip():
        return ""
    # e.g. "00:01.0 VGA compatible controller: Advanced Micro Devices, Inc. [AMD/ATI] Mullins [Radeon R4/R5 Graphics] [1002:9850]"
    line = proc.stdout.strip().splitlines()[0]
    if ": " in line:
        rest = line.split(": ", 1)[1]
        # Drop trailing [vendor:device] and optional (rev …)
        rest = re.sub(
            r"\s*\[[0-9a-fA-F]{4}:[0-9a-fA-F]{4}\]\s*(?:\(rev [^)]+\))?\s*$",
            "",
            rest,
        )
        return rest.strip()
    return ""


def _detect_gpu(gpu: dict[str, Any], *, sys_pci: Path | None = None) -> None:
    sys_pci = sys_pci or Path("/sys/bus/pci/devices")
    cands = _pci_gpu_candidates(sys_pci)
    if not cands:
        gpu["vendor"] = "unknown"
        gpu["model"] = "unknown"
        gpu["driver"] = "unknown"
        gpu["va_api_driver"] = ""
        return

    vendors: list[str] = []
    primary = cands[0]
    # Prefer VGA class over 3D-only (NVIDIA discrete often 3D)
    for dev in cands:
        if _read_text(dev / "class").lower() == PCI_CLASS_VGA:
            primary = dev
            break

    for dev in cands:
        vid = _read_text(dev / "vendor").lower()
        name = PCI_VENDOR_NAMES.get(vid, "unknown")
        if name not in vendors:
            vendors.append(name)

    if len(vendors) > 1 and "nvidia" in vendors:
        vendor = "hybrid"
    elif vendors:
        vendor = vendors[0]
    else:
        vendor = "unknown"

    driver = _driver_name(primary)
    slot = primary.name  # 0000:00:01.0
    # lspci wants bus:slot.func without domain sometimes; try both
    model = _lspci_model(slot)
    if not model:
        short = slot.split(":", 1)[-1] if ":" in slot else slot
        model = _lspci_model(short)
    if not model:
        vid = _read_text(primary / "vendor")
        did = _read_text(primary / "device")
        model = f"{vendor} gpu ({vid}:{did})" if vendor != "unknown" else f"pci {vid}:{did}"

    gpu["vendor"] = vendor
    gpu["model"] = model
    gpu["driver"] = driver
    gpu["va_api_driver"] = VA_API_BY_DRIVER.get(driver, "")


def _is_virtual_block(name: str) -> bool:
    return bool(re.match(r"^(loop|ram|zram|dm-|md|sr|fd|nbd)", name))


def _is_partition(name: str) -> bool:
    return bool(
        re.match(r"^sd[a-z]+\d+$", name)
        or re.match(r"^vd[a-z]+\d+$", name)
        or re.match(r"^nvme\d+n\d+p\d+$", name)
        or re.match(r"^mmcblk\d+p\d+$", name)
    )


def _detect_storage(storage: dict[str, Any], *, sys_block: Path | None = None) -> None:
    sys_block = sys_block or Path("/sys/block")
    if not sys_block.is_dir():
        return

    best: tuple[int, Path] | None = None  # size sectors, path
    for entry in sorted(sys_block.iterdir()):
        name = entry.name
        if _is_virtual_block(name) or _is_partition(name):
            continue
        if not name.startswith(("sd", "vd", "xvd", "mmcblk", "nvme")):
            continue
        if _read_text(entry / "removable", "0") == "1":
            continue
        try:
            sectors = int(_read_text(entry / "size", "0") or "0")
        except ValueError:
            continue
        if sectors <= 0:
            continue
        if best is None or sectors > best[0]:
            best = (sectors, entry)

    if best is None:
        storage["device"] = ""
        storage["type"] = "unknown"
        storage["size_gb"] = 0
        storage["rotational"] = False
        return

    sectors, entry = best
    name = entry.name
    rotational = _read_text(entry / "queue" / "rotational", "0") == "1"
    size_gb = int((sectors * 512) // (1024**3))

    if name.startswith("nvme"):
        stype = "nvme"
    elif name.startswith("mmcblk"):
        stype = "emmc"
    elif rotational:
        stype = "hdd"
    else:
        stype = "ssd"

    storage["device"] = f"/dev/{name}"
    storage["type"] = stype
    storage["size_gb"] = size_gb
    storage["rotational"] = rotational


def _is_virtual_netif(name: str, net_dir: Path) -> bool:
    if name in ("lo",):
        return True
    if name.startswith(
        ("docker", "br-", "veth", "virbr", "tun", "tap", "wg", "tailscale", "zt")
    ):
        return True
    # Skip interfaces without a physical device link when clearly virtual
    if (net_dir / "device").exists():
        return False
    # bridge/bond without device
    if (net_dir / "bridge").exists() or (net_dir / "bonding").exists():
        return True
    return False


def _detect_network(network: dict[str, Any], *, sys_net: Path | None = None) -> None:
    sys_net = sys_net or Path("/sys/class/net")
    has_wifi = False
    has_ethernet = False
    wifi_driver = ""

    if sys_net.is_dir():
        for entry in sorted(sys_net.iterdir()):
            name = entry.name
            if _is_virtual_netif(name, entry):
                continue
            wireless = (entry / "wireless").exists() or (entry / "phy80211").exists()
            if wireless:
                has_wifi = True
                if not wifi_driver:
                    # device/driver under netdev
                    for cand in (entry / "device" / "driver", entry / "device"):
                        if cand.is_symlink() or (cand / "driver").exists():
                            link = cand if cand.name == "driver" else cand / "driver"
                            if link.exists():
                                try:
                                    wifi_driver = Path(os.path.realpath(link)).name
                                except OSError:
                                    wifi_driver = ""
                            break
                continue
            # type 1 = ARPHRD_ETHER
            if _read_text(entry / "type", "") == "1":
                has_ethernet = True

    has_bluetooth = Path("/sys/class/bluetooth").is_dir() and any(
        Path("/sys/class/bluetooth").iterdir()
    )
    # Cellular / WWAN: usb_wwan, cdc_mbim, qmi_wwan netdevs or usb drivers
    has_modem = False
    if sys_net.is_dir():
        for entry in sys_net.iterdir():
            drv = ""
            link = entry / "device" / "driver"
            if link.exists():
                try:
                    drv = Path(os.path.realpath(link)).name
                except OSError:
                    drv = ""
            if drv in ("cdc_mbim", "qmi_wwan", "cdc_ncm", "huawei_cdc_ncm", "usb_wwan"):
                has_modem = True
                break

    network["has_wifi"] = has_wifi
    network["wifi_driver"] = wifi_driver
    network["has_ethernet"] = has_ethernet
    network["has_bluetooth"] = has_bluetooth
    network["has_modem"] = has_modem


def _apply_memory_defaults(state: dict[str, Any], hw: dict[str, Any]) -> None:
    mem = state.setdefault("memory", {})
    ram = hw.get("ram") or {}
    total_kb = ram.get("total_kb") or 0
    if total_kb <= 0:
        return
    total_mb = total_kb // 1024
    variant = state.get("variant", "standard")
    if variant == "standard":
        mem["zram_size_mb"] = min(total_mb, 4096)
    else:
        mem["zram_size_mb"] = total_mb
    storage_type = (hw.get("storage") or {}).get("type", "ssd")
    if storage_type == "hdd":
        mem["swappiness"] = 5
    elif storage_type in ("sd", "usb", "emmc"):
        mem["swappiness"] = 10
    else:
        mem["swappiness"] = 15


def detect(state: dict[str, Any]) -> dict[str, Any]:
    """
    Best-effort local detect for live/install targets.

    Must run on the *target* machine, never in the ISO build chroot —
    that would bake the build host into state.json.

    Fills hardware.cpu / ram / gpu / storage / network from /proc and /sys.
    """
    hw = state.setdefault("hardware", {})

    _detect_cpu(hw.setdefault("cpu", {}))
    _detect_ram(hw.setdefault("ram", {}))
    _detect_gpu(hw.setdefault("gpu", {}))
    _detect_storage(hw.setdefault("storage", {}))
    _detect_network(hw.setdefault("network", {}))
    _apply_memory_defaults(state, hw)

    state_mod.save(state)
    return state
