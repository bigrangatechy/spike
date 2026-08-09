"""Atomic write + module generation orchestration."""

from __future__ import annotations

from pathlib import Path
from typing import Any, Callable

from spike_config import changelog, paths, state as state_mod, templates
from spike_config.modules import boot, memory, multimedia, network, power, security

ModuleFn = Callable[[dict[str, Any], str], list[str]]

MODULES: dict[str, ModuleFn] = {
    "memory": memory.generate,
    "boot": boot.generate,
    "network": network.generate,
    "multimedia": multimedia.generate,
    "power": power.generate,
    "security": security.generate,
}


def atomic_write(target: Path, content: str) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    tmp = target.with_name(target.name + ".tmp")
    tmp.write_text(content, encoding="utf-8")
    tmp.replace(target)


def write_from_template(
    template_name: str,
    dest_absolute: str,
    context: dict[str, object],
) -> str:
    """Render template and write under SPIKE_ROOT. Returns product path string."""
    tpl = paths.template_dir() / template_name
    if not tpl.is_file():
        raise FileNotFoundError(f"template missing: {tpl}")
    text = templates.render_file(tpl, context)
    templates.validate_no_placeholders(text)
    out = paths.under_root(dest_absolute)
    atomic_write(out, text if text.endswith("\n") else text + "\n")
    return dest_absolute


def base_context(state: dict[str, Any], changelog_id: str) -> dict[str, object]:
    return {
        "timestamp": state_mod.utc_now(),
        "changelog_id": changelog_id,
        "variant": state.get("variant", "standard"),
    }


def generate_module(
    name: str,
    state: dict[str, Any],
    *,
    source: str = "cli",
    changelog_setting: str = "generate",
) -> list[str]:
    if name not in MODULES:
        raise KeyError(f"unknown module: {name} (known: {', '.join(sorted(MODULES))})")
    entry_id = changelog.append(
        module=name,
        setting=changelog_setting,
        old_value=None,
        new_value="regenerated",
        source=source,
        files_regenerated=[],
    )
    files = MODULES[name](state, entry_id)
    # Update last changelog entry with files list.
    entries = changelog.load()
    for entry in reversed(entries):
        if entry.get("id") == entry_id:
            entry["files_regenerated"] = files
            break
    changelog.save(entries)
    return files


def generate_all(state: dict[str, Any], *, source: str = "cli") -> dict[str, list[str]]:
    result: dict[str, list[str]] = {}
    for name in sorted(MODULES):
        result[name] = generate_module(name, state, source=source, changelog_setting="generate-all")
    return result


def validate_generated(state: dict[str, Any]) -> list[str]:
    """Regenerate to a temp check: ensure templates resolve. Returns list of OK paths."""
    # Dry validation: render without relying on prior writes.
    entry_id = "validate"
    ok: list[str] = []
    for name, fn in MODULES.items():
        # Modules write files; for validate we call them under SPIKE_ROOT (caller sets test root).
        files = fn(state, entry_id)
        for product_path in files:
            text = paths.under_root(product_path).read_text(encoding="utf-8")
            templates.validate_no_placeholders(text)
            ok.append(product_path)
    return ok
