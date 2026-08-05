"""Filesystem roots for spike-config (overridable for tests / packaging)."""

from __future__ import annotations

import os
from pathlib import Path

_PKG_ROOT = Path(__file__).resolve().parent.parent


def spike_root() -> Path:
    """Prefix for generated system files (default `/`)."""
    return Path(os.environ.get("SPIKE_ROOT", "/"))


def state_dir() -> Path:
    override = os.environ.get("SPIKE_STATE_DIR")
    if override:
        return Path(override)
    return spike_root() / "var" / "lib" / "spike" / "config"


def state_path() -> Path:
    return state_dir() / "state.json"


def changelog_path() -> Path:
    return state_dir() / "changelog.json"


def template_dir() -> Path:
    override = os.environ.get("SPIKE_TEMPLATE_DIR")
    if override:
        return Path(override)
    # Prefer package-adjacent templates when developing from source.
    src_templates = _PKG_ROOT / "templates"
    if src_templates.is_dir():
        return src_templates
    return Path("/usr/lib/spike/config/templates")


def default_state_path() -> Path:
    override = os.environ.get("SPIKE_DEFAULT_STATE")
    if override:
        return Path(override)
    candidate = _PKG_ROOT / "data" / "default-state.json"
    if candidate.is_file():
        return candidate
    return Path("/usr/lib/spike/config/default-state.json")


def boot_count_path() -> Path:
    return spike_root() / "boot" / ".spike" / "boot-count"


def under_root(absolute: str | Path) -> Path:
    """Map an absolute product path (e.g. /etc/...) under SPIKE_ROOT."""
    path = Path(absolute)
    if not path.is_absolute():
        raise ValueError(f"expected absolute path, got {absolute!r}")
    return spike_root() / path.relative_to("/")
