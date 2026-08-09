"""Install-time kernel module blacklist policy (docs/KERNEL.md).

Only blacklist modules for ABSENT soldered / non-hotplug categories.
Never blacklist USB, network, audio, Bluetooth, webcam, or GPU drivers.
"""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

# (module_name, category, optional PCI vendor ids that mean "present — do not blacklist")
BlacklistCandidate = tuple[str, str, tuple[str, ...]]

# KERNEL.md examples — enterprise / exotic / absent on Spike target laptops.
CANDIDATES: list[BlacklistCandidate] = [
    # Enterprise RAID
    ("megaraid_sas", "enterprise_raid", ("0x1000",)),  # LSI/Broadcom MegaRAID
    ("mptsas", "enterprise_raid", ("0x1000",)),
    ("hpsa", "enterprise_raid", ("0x103c", "0x1590")),  # HP Smart Array
    ("aacraid", "enterprise_raid", ("0x9005",)),  # Adaptec
    # Infiniband
    ("ib_core", "infiniband", ("0x15b3",)),  # Mellanox
    ("ib_uverbs", "infiniband", ("0x15b3",)),
    ("mlx5_ib", "infiniband", ("0x15b3",)),
    # Fiber Channel
    ("qla2xxx", "fibre_channel", ("0x1077",)),  # QLogic
    ("lpfc", "fibre_channel", ("0x10df",)),  # Emulex
    ("bfa", "fibre_channel", ("0x1657",)),  # Brocade
    # TV tuners
    ("cx88", "tv_tuner", ()),
    ("saa7134", "tv_tuner", ()),
    ("em28xx", "tv_tuner", ()),
    # Exotic / industrial
    ("wmi_bmof", "exotic_sensor", ()),
    ("acpi_power_meter", "exotic_sensor", ()),
    ("industrialio", "industrial_io", ()),
    ("adc1x8s102", "industrial_io", ()),
    ("hamradio", "amateur_radio", ()),
    # Old parallel port
    ("parport", "legacy_parallel", ()),
    ("parport_pc", "legacy_parallel", ()),
]

TV_TUNER_DRIVER_NAMES = frozenset(
    {"cx88xx", "cx8800", "cx8802", "saa7134", "em28xx", "em28xx_dvb"}
)

NEVER_BLACKLIST = frozenset(
    {
        "usbcore",
        "uhci_hcd",
        "ehci_hcd",
        "xhci_hcd",
        "btusb",
        "btintel",
        "btrtl",
        "iwlwifi",
        "ath9k",
        "ath10k_pci",
        "rtl8xxxu",
        "brcmfmac",
        "snd_hda_intel",
        "snd_soc_core",
        "uvcvideo",
        "rtsx_pci",
        "rtsx_usb",
        "hid",
        "usbhid",
        "i915",
        "amdgpu",
        "radeon",
        "nouveau",
        "nvidia",
        "nvidia_drm",
    }
)


def _read_text(path: Path, default: str = "") -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace").strip()
    except OSError:
        return default


def _pci_vendors(sys_pci: Path) -> set[str]:
    vendors: set[str] = set()
    if not sys_pci.is_dir():
        return vendors
    for dev in sys_pci.iterdir():
        if not dev.is_dir():
            continue
        vid = _read_text(dev / "vendor").lower()
        if vid:
            vendors.add(vid)
    return vendors


def _pci_bound_drivers(sys_pci: Path) -> set[str]:
    drivers: set[str] = set()
    if not sys_pci.is_dir():
        return drivers
    for dev in sys_pci.iterdir():
        link = dev / "driver"
        if not link.exists() and not link.is_symlink():
            continue
        try:
            drivers.add(link.resolve().name)
        except OSError:
            try:
                drivers.add(Path(os.readlink(link)).name)
            except OSError:
                pass
    return drivers


def compute_module_blacklist(*, sys_pci: Path | None = None) -> list[str]:
    """Return sorted module names to blacklist for the current (or fake) PCI tree."""
    sys_pci = sys_pci or Path("/sys/bus/pci/devices")
    vendors = _pci_vendors(sys_pci)
    drivers = _pci_bound_drivers(sys_pci)
    out: list[str] = []

    for module, category, vendor_ids in CANDIDATES:
        if module in NEVER_BLACKLIST:
            continue
        if category == "tv_tuner":
            if drivers & TV_TUNER_DRIVER_NAMES:
                continue
            out.append(module)
            continue
        if vendor_ids and any(v in vendors for v in vendor_ids):
            continue
        out.append(module)

    return sorted({m for m in out if m not in NEVER_BLACKLIST})


def apply_blacklist_to_state(state: dict[str, Any], *, sys_pci: Path | None = None) -> list[str]:
    """Write security.module_blacklist into state; return the list."""
    modules = compute_module_blacklist(sys_pci=sys_pci)
    sec = state.setdefault("security", {})
    sec["module_blacklist"] = modules
    return modules
