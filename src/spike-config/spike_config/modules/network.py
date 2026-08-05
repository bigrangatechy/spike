"""Network module → NetworkManager.conf"""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    net = state["network"]
    ctx = gen_mod.base_context(state, changelog_id)
    ctx.update(
        {
            "dhcp_client": net["dhcp_client"],
            "dns_resolver": net["dns_resolver"],
        }
    )
    return [
        gen_mod.write_from_template(
            "networkmanager-conf.tpl",
            "/etc/NetworkManager/NetworkManager.conf",
            ctx,
        )
    ]
