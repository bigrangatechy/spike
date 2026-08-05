"""Security module (Stage 2: module blacklist only; UFW/sudo later)."""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    sec = state["security"]
    blacklist = sec.get("module_blacklist") or []
    if blacklist:
        lines = "\n".join(f"blacklist {mod}" for mod in blacklist)
    else:
        lines = "# (no modules blacklisted)"
    ctx = gen_mod.base_context(state, changelog_id)
    ctx["module_blacklist_lines"] = lines
    return [
        gen_mod.write_from_template(
            "spike-blacklist.tpl",
            "/etc/modprobe.d/spike-blacklist.conf",
            ctx,
        )
    ]
