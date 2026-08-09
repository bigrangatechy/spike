"""Power module → systemd-logind drop-in + live CPU/Wi‑Fi apply."""

from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Any

from spike_config import generate as gen_mod
from spike_config import paths

# logind action tokens
_ACTIONS = {"suspend", "hibernate", "hybrid-sleep", "suspend-then-hibernate", "poweroff", "lock", "ignore"}

_PROFILE_GOVERNOR = {
    "adaptive": "powersave",
    "performance": "performance",
    "battery_saver": "powersave",
    "critical": "powersave",
}


def _logind_action(value: str, default: str = "suspend") -> str:
    v = (value or default).strip().lower().replace("_", "-")
    if v == "shutdown":
        v = "poweroff"
    if v not in _ACTIONS:
        return default
    return v


def _effective_governor(power: dict[str, Any]) -> str:
    explicit = str(power.get("cpu_governor", "") or "").strip().lower()
    profile = str(power.get("profile", "adaptive") or "adaptive").strip().lower()
    if explicit in {"powersave", "schedutil", "performance", "ondemand", "conservative"}:
        return explicit
    return _PROFILE_GOVERNOR.get(profile, "powersave")


def _wifi_powersave_desired(power: dict[str, Any]) -> bool | None:
    """True/False to force; None = leave unchanged (adaptive / unknown)."""
    mode = str(power.get("wifi_power_saving", "adaptive") or "adaptive").strip().lower()
    profile = str(power.get("profile", "adaptive") or "adaptive").strip().lower()
    if mode == "on":
        return True
    if mode == "off":
        return False
    if profile in {"battery_saver", "critical"}:
        return True
    if profile == "performance":
        return False
    return None


def apply_runtime(power: dict[str, Any]) -> list[str]:
    """
    Apply CPU governor + Wi‑Fi powersave immediately.
    Called from generate(); spike-config-dbus runs as root so sysfs writes work
    on the installed system without sudo from the shell.
    """
    notes: list[str] = []
    root = Path(paths.spike_root())
    # Under SPIKE_ROOT tests, skip mutating host sysfs.
    if str(root) not in ("/", ""):
        return notes

    gov = _effective_governor(power)
    applied = 0
    cpu_root = Path("/sys/devices/system/cpu")
    if cpu_root.is_dir():
        for gov_path in sorted(cpu_root.glob("cpu[0-9]*/cpufreq/scaling_governor")):
            try:
                avail_path = gov_path.parent / "scaling_available_governors"
                available = avail_path.read_text(encoding="utf-8").split() if avail_path.is_file() else []
                use = gov if (not available or gov in available) else None
                if use is None and available:
                    # Fallbacks for locked-down firmware
                    for candidate in ("powersave", "schedutil", "ondemand", "performance"):
                        if candidate in available:
                            use = candidate
                            break
                if not use:
                    continue
                gov_path.write_text(f"{use}\n", encoding="utf-8")
                applied += 1
            except OSError:
                continue
    if applied:
        notes.append(f"cpu_governor={gov} ({applied} CPUs)")
    else:
        # Best-effort cpupower if sysfs unavailable
        try:
            proc = subprocess.run(
                ["cpupower", "frequency-set", "-g", gov],
                capture_output=True,
                text=True,
                timeout=8,
                check=False,
            )
            if proc.returncode == 0:
                notes.append(f"cpu_governor={gov} (cpupower)")
        except (OSError, subprocess.TimeoutExpired):
            pass

    wifi = _wifi_powersave_desired(power)
    if wifi is not None:
        ifaces: list[str] = []
        try:
            proc = subprocess.run(
                ["iw", "dev"],
                capture_output=True,
                text=True,
                timeout=5,
                check=False,
            )
            for line in (proc.stdout or "").splitlines():
                stripped = line.strip()
                if stripped.startswith("Interface "):
                    parts = stripped.split()
                    if len(parts) >= 2:
                        ifaces.append(parts[1])
        except (OSError, subprocess.TimeoutExpired):
            ifaces = []
        ok = 0
        for iface in ifaces:
            try:
                proc = subprocess.run(
                    ["iw", "dev", iface, "set", "power_save", "on" if wifi else "off"],
                    capture_output=True,
                    text=True,
                    timeout=5,
                    check=False,
                )
                if proc.returncode == 0:
                    ok += 1
            except (OSError, subprocess.TimeoutExpired):
                continue
        if ok:
            notes.append(f"wifi_powersave={'on' if wifi else 'off'} ({ok} ifaces)")

    try:
        out = Path("/var/lib/spike")
        out.mkdir(parents=True, exist_ok=True)
        (out / "power-last-apply.txt").write_text(
            "profile={}\ngovernor={}\nwifi_power_saving={}\nnotes={}\n".format(
                power.get("profile", ""),
                gov,
                power.get("wifi_power_saving", ""),
                "; ".join(notes) if notes else "(none)",
            ),
            encoding="utf-8",
        )
    except OSError:
        pass

    return notes


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    power = state.get("power") or {}
    lid = _logind_action(str(power.get("lid_close_action", "suspend")))
    lid_ac = _logind_action(str(power.get("lid_close_action_ac", lid)), lid)
    power_btn = _logind_action(str(power.get("power_button_action", "suspend")))

    blank = int(power.get("screen_blank_minutes", 15) or 15)
    if blank < 0:
        blank = 0
    allow_suspend = bool(power.get("allow_suspend", True))
    if blank <= 0 or not allow_suspend:
        idle_action = "ignore"
        idle_sec = "0"
    else:
        idle_action = "suspend"
        idle_sec = f"{blank}min"

    ctx = gen_mod.base_context(state, changelog_id)
    ctx.update(
        {
            "lid_close_action": lid,
            "lid_close_action_ac": lid_ac,
            "power_button_action": power_btn,
            "idle_action": idle_action,
            "idle_action_sec": idle_sec,
        }
    )
    files = [
        gen_mod.write_from_template(
            "logind-spike-power.tpl",
            "/etc/systemd/logind.conf.d/99-spike-power.conf",
            ctx,
        ),
    ]
    apply_runtime(power)
    return files
