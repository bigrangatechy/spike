"""Hardware detection stub — real detectors land with the installer."""

from __future__ import annotations

from typing import Any

from spike_config import state as state_mod


def detect(state: dict[str, Any]) -> dict[str, Any]:
    """
    Best-effort local detect for development.

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
            if line.startswith("bogomips") and bogomips == 0:
                bogomips = float(line.split(":", 1)[1].strip())
        cores = text.count("processor\t")
        cpu["model"] = model
        cpu["cores"] = max(cores, 1)
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

    # Apply simple memory defaults from RAM when still at seed zeros.
    mem = state.setdefault("memory", {})
    total_kb = ram.get("total_kb") or 0
    if total_kb > 0:
        total_mb = total_kb // 1024
        variant = state.get("variant", "standard")
        if variant == "standard":
            mem["zram_size_mb"] = min(total_mb, 4096)
        else:
            mem["zram_size_mb"] = total_mb

    state_mod.save(state)
    return state
