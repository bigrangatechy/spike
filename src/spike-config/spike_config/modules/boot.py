"""Boot module → GRUB default + boot-count file."""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod, paths


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    boot = state["boot"]
    ctx = gen_mod.base_context(state, changelog_id)
    params = boot.get("boot_parameters", [])
    ctx.update(
        {
            "grub_timeout": boot["grub_timeout"],
            "grub_timeout_style": boot["grub_timeout_style"],
            "boot_parameters_joined": " ".join(params),
            "plymouth_theme": boot.get("plymouth_theme", "spike-minimal"),
        }
    )
    files = [
        gen_mod.write_from_template("grub-default.tpl", "/etc/default/grub", ctx),
    ]
    # Ensure boot-count file exists (product path /boot/.spike/boot-count).
    count_path = paths.boot_count_path()
    count_path.parent.mkdir(parents=True, exist_ok=True)
    if not count_path.is_file():
        count_path.write_text(str(boot.get("boot_failure_count", 0)) + "\n", encoding="utf-8")
    files.append("/boot/.spike/boot-count")
    return files
