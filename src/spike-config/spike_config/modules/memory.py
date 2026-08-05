"""Memory module → sysctl, zram-generator, udev, earlyoom."""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod


def _ctx(state: dict[str, Any], changelog_id: str) -> dict[str, object]:
    mem = state["memory"]
    prefer = "|".join(mem.get("earlyoom_prefer", []))
    protect = "|".join(mem.get("earlyoom_protected", []))
    ctx = gen_mod.base_context(state, changelog_id)
    ctx.update(
        {
            "swappiness": mem["swappiness"],
            "vfs_cache_pressure": mem["vfs_cache_pressure"],
            "zram_size_mb": mem["zram_size_mb"],
            "zram_compression": mem["zram_compression"],
            "zram_priority": mem["zram_priority"],
            "earlyoom_threshold": mem["earlyoom_threshold"],
            "earlyoom_prefer": prefer,
            "earlyoom_protected": protect,
        }
    )
    return ctx


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    ctx = _ctx(state, changelog_id)
    files = [
        gen_mod.write_from_template(
            "sysctl-memory.tpl", "/etc/sysctl.d/99-spike-memory.conf", ctx
        ),
        gen_mod.write_from_template(
            "earlyoom.tpl", "/etc/default/earlyoom", ctx
        ),
    ]
    if state["memory"].get("zram_enabled", True):
        files.append(
            gen_mod.write_from_template(
                "zram-generator.tpl", "/etc/systemd/zram-generator.conf", ctx
            )
        )
        files.append(
            gen_mod.write_from_template(
                "zram-udev.tpl", "/etc/udev/rules.d/99-spike-zram.rules", ctx
            )
        )
    return files
