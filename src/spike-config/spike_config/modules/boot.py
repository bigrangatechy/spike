"""Boot module → GRUB default + boot-count file."""

from __future__ import annotations

from typing import Any

from spike_config import generate as gen_mod, paths


def generate(state: dict[str, Any], changelog_id: str) -> list[str]:
    boot = state["boot"]
    ctx = gen_mod.base_context(state, changelog_id)
    params = boot.get("boot_parameters", [])
    # Theme assets ship via spike-branding. Prefer theme when theme.txt exists;
    # custom font is optional (GRUB falls back to Unifont named in theme.txt).
    theme_txt = paths.under_root("/boot/grub/themes/spike/theme.txt")
    font_pf2 = paths.under_root("/boot/grub/themes/spike/pf2/font.pf2")
    bg = paths.under_root("/boot/grub/themes/spike/background.png")
    if theme_txt.is_file():
        lines = [
            "GRUB_THEME=/boot/grub/themes/spike/theme.txt\n",
            "GRUB_GFXMODE=auto\n",
            "GRUB_GFXPAYLOAD_LINUX=keep\n",
            "GRUB_TERMINAL_OUTPUT=gfxterm\n",
        ]
        if bg.is_file():
            lines.append("GRUB_BACKGROUND=/boot/grub/themes/spike/background.png\n")
        if font_pf2.is_file():
            lines.append("GRUB_FONT=/boot/grub/themes/spike/pf2/font.pf2\n")
        theme_block = "".join(lines)
    else:
        theme_block = ""
    ctx.update(
        {
            "grub_timeout": boot["grub_timeout"],
            "grub_timeout_style": boot["grub_timeout_style"],
            "boot_parameters_joined": " ".join(params),
            "plymouth_theme": boot.get("plymouth_theme", "spike-minimal"),
            "grub_theme_block": theme_block,
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
