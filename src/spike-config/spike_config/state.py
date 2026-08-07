"""State store: /var/lib/spike/config/state.json"""

from __future__ import annotations

import json
import shutil
from copy import deepcopy
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from spike_config import paths


def utc_now() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def load_default() -> dict[str, Any]:
    with paths.default_state_path().open(encoding="utf-8") as fh:
        return json.load(fh)


def ensure_state_dir() -> None:
    paths.state_dir().mkdir(parents=True, exist_ok=True)


def load() -> dict[str, Any]:
    path = paths.state_path()
    if not path.is_file():
        raise FileNotFoundError(
            f"state store missing: {path} (run spike-config --init-state or --detect)"
        )
    with path.open(encoding="utf-8") as fh:
        data = json.load(fh)
    if not isinstance(data, dict) or "version" not in data:
        raise ValueError(f"invalid state store: {path}")
    return merge_missing_defaults(data)


def merge_missing_defaults(state: dict[str, Any]) -> dict[str, Any]:
    """Fill missing top-level modules / keys from default-state.json (upgrade path)."""
    defaults = load_default()
    changed = False
    for mod, default_section in defaults.items():
        if mod in ("version", "last_modified", "variant", "install_date"):
            continue
        if not isinstance(default_section, dict):
            continue
        if mod not in state or not isinstance(state.get(mod), dict):
            state[mod] = deepcopy(default_section)
            changed = True
            continue
        for key, val in default_section.items():
            if key not in state[mod]:
                state[mod][key] = deepcopy(val)
                changed = True
    if changed:
        try:
            save(state)
        except OSError:
            pass
    return state


def save(state: dict[str, Any]) -> None:
    ensure_state_dir()
    state = deepcopy(state)
    state["last_modified"] = utc_now()
    path = paths.state_path()
    tmp = path.with_suffix(".json.tmp")
    text = json.dumps(state, indent=2, sort_keys=False) + "\n"
    tmp.write_text(text, encoding="utf-8")
    tmp.replace(path)


def init_state(*, force: bool = False) -> dict[str, Any]:
    ensure_state_dir()
    path = paths.state_path()
    if path.is_file() and not force:
        return load()
    state = load_default()
    if not state.get("install_date"):
        state["install_date"] = utc_now()[:10]
    save(state)
    return state


def get_value(state: dict[str, Any], module: str, key: str) -> Any:
    if module not in state:
        raise KeyError(f"unknown module: {module}")
    section = state[module]
    if not isinstance(section, dict):
        raise KeyError(f"module {module} is not a mapping")
    if key not in section:
        raise KeyError(f"unknown key: {module}.{key}")
    return section[key]


def set_value(state: dict[str, Any], module: str, key: str, value: Any) -> Any:
    """Set module.key; returns previous value (None if the key is new)."""
    if module not in state:
        raise KeyError(f"unknown module: {module}")
    section = state[module]
    if not isinstance(section, dict):
        raise KeyError(f"module {module} is not a mapping")
    old = section.get(key)
    # Coerce JSON-ish strings from CLI.
    if isinstance(value, str):
        lowered = value.lower()
        if lowered == "true":
            value = True
        elif lowered == "false":
            value = False
        else:
            try:
                value = json.loads(value)
            except json.JSONDecodeError:
                pass
    section[key] = value
    return old


def dump(state: dict[str, Any]) -> str:
    return json.dumps(state, indent=2) + "\n"


def recover_from_defaults() -> dict[str, Any]:
    """Replace corrupt/missing state with defaults (caller should run detect)."""
    path = paths.state_path()
    if path.is_file():
        backup = path.with_suffix(".json.corrupt")
        shutil.copy2(path, backup)
    return init_state(force=True)
