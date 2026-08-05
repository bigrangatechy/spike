"""Multimedia module → PipeWire conf drop-in."""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    mm = state["multimedia"]
    ctx = gen_mod.base_context(state, changelog_id)
    ctx.update({"sample_rate": mm["sample_rate"]})
    return [
        gen_mod.write_from_template(
            "pipewire-conf.tpl",
            "/etc/pipewire/pipewire.conf.d/99-spike.conf",
            ctx,
        )
    ]
