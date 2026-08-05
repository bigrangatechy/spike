"""Append-only changelog at /var/lib/spike/config/changelog.json"""

from __future__ import annotations

import json
import uuid
from copy import deepcopy
from pathlib import Path
from typing import Any

from spike_config import paths, state as state_mod

MAX_ENTRIES = 5000


def _empty() -> list[dict[str, Any]]:
    return []


def load() -> list[dict[str, Any]]:
    path = paths.changelog_path()
    if not path.is_file():
        return _empty()
    with path.open(encoding="utf-8") as fh:
        data = json.load(fh)
    if not isinstance(data, list):
        raise ValueError(f"invalid changelog: {path}")
    return data


def save(entries: list[dict[str, Any]]) -> None:
    paths.state_dir().mkdir(parents=True, exist_ok=True)
    if len(entries) > MAX_ENTRIES:
        entries = entries[-MAX_ENTRIES:]
    path = paths.changelog_path()
    tmp = path.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(entries, indent=2) + "\n", encoding="utf-8")
    tmp.replace(path)


def append(
    *,
    module: str,
    setting: str,
    old_value: Any,
    new_value: Any,
    source: str,
    files_regenerated: list[str] | None = None,
    services_reloaded: list[str] | None = None,
    post_action: str | None = None,
) -> str:
    entry_id = str(uuid.uuid4())
    entry: dict[str, Any] = {
        "id": entry_id,
        "timestamp": state_mod.utc_now(),
        "module": module,
        "setting": setting,
        "old_value": deepcopy(old_value),
        "new_value": deepcopy(new_value),
        "source": source,
        "files_regenerated": list(files_regenerated or []),
        "services_reloaded": list(services_reloaded or []),
    }
    if post_action:
        entry["post_action"] = post_action
    entries = load()
    entries.append(entry)
    save(entries)
    return entry_id


def find(entry_id: str) -> dict[str, Any]:
    for entry in load():
        if entry.get("id") == entry_id:
            return entry
    raise KeyError(f"changelog entry not found: {entry_id}")
