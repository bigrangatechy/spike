"""Template engine: {{variable}} substitution from a flat context dict."""

from __future__ import annotations

import re
from pathlib import Path

_PLACEHOLDER = re.compile(r"\{\{\s*([a-zA-Z0-9_.]+)\s*\}\}")


def render(template_text: str, context: dict[str, object]) -> str:
    missing: list[str] = []

    def repl(match: re.Match[str]) -> str:
        name = match.group(1)
        if name not in context:
            missing.append(name)
            return match.group(0)
        value = context[name]
        if value is None:
            return ""
        if isinstance(value, bool):
            return "true" if value else "false"
        return str(value)

    out = _PLACEHOLDER.sub(repl, template_text)
    if missing:
        uniq = ", ".join(sorted(set(missing)))
        raise KeyError(f"unresolved template variables: {uniq}")
    leftover = _PLACEHOLDER.findall(out)
    if leftover:
        raise ValueError(f"leftover placeholders after render: {leftover}")
    return out


def render_file(template_path: Path, context: dict[str, object]) -> str:
    text = template_path.read_text(encoding="utf-8")
    return render(text, context)


def validate_no_placeholders(text: str) -> None:
    left = _PLACEHOLDER.findall(text)
    if left:
        raise ValueError(f"unsubstituted placeholders: {left}")
