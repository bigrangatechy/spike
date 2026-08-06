"""Hardware detection stub — real detectors land with the installer."""

from __future__ import annotations

import os
import re
from typing import Any

from spike_config import state as state_mod


def detect(state: dict[str, Any]) -> dict[str, Any]:
    """
    Best-effort local detect for development.

    Must run on the *target* machine (live boot / install), never in the
    ISO build chroot — that would bake the build host into state.json.

    Product path will call spike-installer detect modules; this Stage 2 stub
    fills hardware from /proc and /sys when available, else leaves defaults.
    """
    hw = state.setdefault("hardware", {})

    # CPU
    cpu = hw.setdefault("cpu", {})
    try:
        with open("/proc/cpuinfo", encoding="utf-8") as fh:
            text = fh.read()
        model = "unknown"
        bogomips = 0.0
        for line in text.splitlines():
            if line.startswith("model name") and model == "unknown":
                model = line.split(":", 1)[1].strip()
            if line.lower().startswith("bogomips") and bogomips == 0:
                bogomips = float(line.split(":", 1)[1].strip())
        # Count logical CPUs (lines like "processor : 0")
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
    except OSError:
        pass

    # RAM
    ram = hw.setdefault("ram", {})
    try:
        with open("/proc/meminfo", encoding="utf-8") as fh:
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
    except OSError:
        pass

    # Apply simple memory defaults from detected RAM.
    mem = state.setdefault("memory", {})
    total_kb = ram.get("total_kb") or 0
    if total_kb > 0:
        total_mb = total_kb // 1024
        variant = state.get("variant", "standard")
        if variant == "standard":
            mem["zram_size_mb"] = min(total_mb, 4096)
        else:
            mem["zram_size_mb"] = total_mb
        # Swappiness defaults by storage type (refined later by installer detect)
        storage_type = (hw.get("storage") or {}).get("type", "ssd")
        if storage_type == "hdd":
            mem["swappiness"] = 5
        elif storage_type in ("sd", "usb"):
            mem["swappiness"] = 10
        else:
            mem["swappiness"] = 15

    state_mod.save(state)
    return state
