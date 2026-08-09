"""Power module → systemd-logind drop-in (lid / power button / idle)."""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod

# logind action tokens
_ACTIONS = {"suspend", "hibernate", "hybrid-sleep", "suspend-then-hibernate", "poweroff", "lock", "ignore"}


def _logind_action(value: str, default: str = "suspend") -> str:
    v = (value or default).strip().lower().replace("_", "-")
    if v == "shutdown":
        v = "poweroff"
    if v not in _ACTIONS:
        return default
    return v


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
    return [
        gen_mod.write_from_template(
            "logind-spike-power.tpl",
            "/etc/systemd/logind.conf.d/99-spike-power.conf",
            ctx,
        ),
    ]
