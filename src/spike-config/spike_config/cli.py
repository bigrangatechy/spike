"""Developer CLI for spike-config (see docs/CONFIGURATION.md)."""

from __future__ import annotations

import argparse
import json
import sys
from typing import Sequence

from spike_config import __version__, changelog, detect, generate, paths, state


def _build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="spike-config",
        description="Spike configuration engine (developer CLI)",
    )
    p.add_argument("--version", action="version", version=f"%(prog)s {__version__}")

    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument("--init-state", action="store_true", help="Seed default state store")
    g.add_argument("--generate-all", action="store_true", help="Regenerate all modules")
    g.add_argument("--generate", metavar="MODULE", help="Regenerate one module")
    g.add_argument("--detect", action="store_true", help="Run hardware detection")
    g.add_argument("--state", action="store_true", help="Dump state store JSON")
    g.add_argument(
        "--state-get",
        nargs=2,
        metavar=("MODULE", "KEY"),
        help="Get module.key from state",
    )
    g.add_argument(
        "--state-set",
        nargs=3,
        metavar=("MODULE", "KEY", "VALUE"),
        help="Set module.key and save state",
    )
    g.add_argument("--changelog", action="store_true", help="Show changelog JSON")
    g.add_argument("--rollback", metavar="ENTRY_ID", help="Revert a changelog entry")
    g.add_argument("--validate", action="store_true", help="Validate generated configs")
    g.add_argument(
        "--boot-count",
        metavar="ACTION",
        choices=["get", "reset", "increment"],
        help="Boot failure counter: get|reset|increment",
    )
    return p


def _boot_count_action(action: str) -> int:
    path = paths.boot_count_path()
    path.parent.mkdir(parents=True, exist_ok=True)
    current = 0
    if path.is_file():
        try:
            current = int(path.read_text(encoding="utf-8").strip() or "0")
        except ValueError:
            current = 0
    if action == "get":
        return current
    if action == "reset":
        current = 0
    elif action == "increment":
        current += 1
    path.write_text(str(current) + "\n", encoding="utf-8")
    try:
        st = state.load()
        st.setdefault("boot", {})["boot_failure_count"] = current
        state.save(st)
    except FileNotFoundError:
        pass
    return current


def main(argv: Sequence[str] | None = None) -> int:
    parser = _build_parser()
    args = parser.parse_args(list(argv) if argv is not None else None)

    try:
        if args.init_state:
            st = state.init_state()
            print(f"state initialized: {paths.state_path()}")
            print(f"variant={st.get('variant')}")
            return 0

        if args.detect:
            st = state.init_state()
            detect.detect(st)
            print("hardware detection updated state store")
            print(json.dumps(st.get("hardware", {}), indent=2))
            return 0

        if args.state:
            sys.stdout.write(state.dump(state.load()))
            return 0

        if args.state_get:
            module, key = args.state_get
            value = state.get_value(state.load(), module, key)
            print(json.dumps(value))
            return 0

        if args.state_set:
            module, key, raw = args.state_set
            st = state.load()
            old = state.set_value(st, module, key, raw)
            state.save(st)
            entry_id = changelog.append(
                module=module,
                setting=key,
                old_value=old,
                new_value=st[module][key],
                source="cli",
            )
            print(f"set {module}.{key}={st[module][key]!r} (was {old!r})")
            print(f"changelog_id={entry_id}")
            return 0

        if args.changelog:
            print(json.dumps(changelog.load(), indent=2))
            return 0

        if args.rollback:
            entry = changelog.find(args.rollback)
            st = state.load()
            module = entry["module"]
            setting = entry["setting"]
            if setting in ("generate", "generate-all"):
                print(
                    "cannot rollback a generate entry; use --state-set",
                    file=sys.stderr,
                )
                return 2
            before = state.get_value(st, module, setting)
            st[module][setting] = entry["old_value"]
            state.save(st)
            files: list[str] = []
            if module in generate.MODULES:
                files = generate.generate_module(
                    module, st, source="rollback", changelog_setting=setting
                )
            changelog.append(
                module=module,
                setting=setting,
                old_value=before,
                new_value=entry["old_value"],
                source="rollback",
                files_regenerated=files,
            )
            print(f"rolled back {module}.{setting} → {entry['old_value']!r}")
            return 0

        if args.generate_all:
            st = state.load()
            result = generate.generate_all(st)
            for mod, files in result.items():
                print(f"{mod}:")
                for f in files:
                    print(f"  {f}")
            return 0

        if args.generate:
            st = state.load()
            files = generate.generate_module(args.generate, st)
            for f in files:
                print(f)
            return 0

        if args.validate:
            st = state.load()
            ok = generate.validate_generated(st)
            print(f"validated {len(ok)} files")
            for f in ok:
                print(f"  ok {f}")
            return 0

        if args.boot_count:
            print(_boot_count_action(args.boot_count))
            return 0

    except (FileNotFoundError, KeyError, ValueError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    parser.error("no action")
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
