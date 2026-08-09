"""D-Bus API for spike-config — org.spike.Config (docs/CONFIGURATION.md).

On-demand: activated by D-Bus when Settings (or another client) calls the bus name.
Not a long-lived daemon beyond the call lifetime (systemd Type=dbus holds it while
the name is owned).
"""

from __future__ import annotations

import json
import sys
from typing import Any

import dbus
import dbus.mainloop.glib
import dbus.service
from gi.repository import GLib

from spike_config import changelog, detect, generate, state

BUS_NAME = "org.spike.Config"
OBJECT_PATH = "/org/spike/Config"
IFACE = "org.spike.Config"


def _json_dumps(value: Any) -> str:
    return json.dumps(value, indent=2, sort_keys=False)


def _variant_to_python(value: Any) -> Any:
    """Unwrap dbus types into plain Python for the state store.

    dbus-python 1.4 no longer exports ``dbus.Variant``; method args with
    signature ``v`` arrive already unwrapped (or as ordinary dbus.* types).
    """
    # Older dbus-python had dbus.Variant with a private payload — handle both.
    variant_cls = getattr(dbus, "Variant", None)
    if variant_cls is not None and isinstance(value, variant_cls):
        inner = getattr(value, "_value", None)
        if inner is None:
            inner = getattr(value, "variant", value)
        return _variant_to_python(inner)
    if isinstance(value, (dbus.String, str)):
        text = str(value)
        # Allow JSON payloads in a string variant (handy from qdbus/busctl).
        try:
            return json.loads(text)
        except json.JSONDecodeError:
            return text
    if isinstance(value, (dbus.Boolean, bool)):
        return bool(value)
    if isinstance(value, (dbus.Int16, dbus.Int32, dbus.Int64, dbus.UInt16, dbus.UInt32, dbus.UInt64, int)):
        return int(value)
    if isinstance(value, (dbus.Double, float)):
        return float(value)
    if isinstance(value, (dbus.Array, list, tuple)):
        return [_variant_to_python(v) for v in value]
    if isinstance(value, (dbus.Dictionary, dict)):
        return {str(k): _variant_to_python(v) for k, v in value.items()}
    return value


def _signal_variant(value: Any) -> Any:
    """Value suitable for a D-Bus 'v' signal argument (dbus-python 1.4+)."""
    # Prefer plain strings so Qt QDBusVariant clients get a stable type.
    if value is None:
        return ""
    return str(value)


class ConfigService(dbus.service.Object):
    def __init__(self, bus: dbus.Bus) -> None:
        dbus.service.Object.__init__(self, bus, OBJECT_PATH)

    @dbus.service.method(IFACE, in_signature="", out_signature="s")
    def GetState(self) -> str:
        return _json_dumps(state.load())

    @dbus.service.method(IFACE, in_signature="s", out_signature="s")
    def GetModuleState(self, module: str) -> str:
        st = state.load()
        if module not in st:
            raise dbus.DBusException(
                f"unknown module: {module}",
                name="org.spike.Config.Error.UnknownModule",
            )
        return _json_dumps(st[module])

    @dbus.service.method(IFACE, in_signature="ssv", out_signature="b")
    def SetSetting(self, module: str, key: str, value: Any) -> bool:
        st = state.load()
        python_value = _variant_to_python(value)
        old = state.set_value(st, module, key, python_value)
        state.save(st)
        new = st[module][key]
        changelog.append(
            module=module,
            setting=key,
            old_value=old,
            new_value=new,
            source="dbus",
        )
        self.StateChanged(
            module,
            key,
            _signal_variant(old),
            _signal_variant(new),
        )
        # Regenerate module configs when we know how.
        if module in generate.MODULES:
            files = generate.generate_module(module, st, source="dbus", changelog_setting=key)
            self.ConfigRegenerated(module, files)
        return True

    @dbus.service.method(IFACE, in_signature="", out_signature="b")
    def GenerateAll(self) -> bool:
        st = state.load()
        result = generate.generate_all(st, source="dbus")
        for mod, files in result.items():
            self.ConfigRegenerated(mod, files)
        return True

    @dbus.service.method(IFACE, in_signature="s", out_signature="b")
    def GenerateModule(self, module: str) -> bool:
        st = state.load()
        files = generate.generate_module(module, st, source="dbus")
        self.ConfigRegenerated(module, files)
        return True

    @dbus.service.method(IFACE, in_signature="", out_signature="s")
    def DetectHardware(self) -> str:
        st = state.init_state()
        detect.detect(st)
        return _json_dumps(st.get("hardware", {}))

    @dbus.service.method(IFACE, in_signature="i", out_signature="s")
    def GetChangelog(self, count: int) -> str:
        entries = changelog.load()
        if count > 0:
            entries = entries[-count:]
        return _json_dumps(entries)

    @dbus.service.method(IFACE, in_signature="s", out_signature="b")
    def Rollback(self, entry_id: str) -> bool:
        entry = changelog.find(entry_id)
        st = state.load()
        module = entry["module"]
        setting = entry["setting"]
        if setting in ("generate", "generate-all"):
            raise dbus.DBusException(
                "cannot rollback a generate entry; use SetSetting",
                name="org.spike.Config.Error.InvalidRollback",
            )
        before = state.get_value(st, module, setting)
        st[module][setting] = entry["old_value"]
        state.save(st)
        files: list[str] = []
        if module in generate.MODULES:
            files = generate.generate_module(
                module, st, source="dbus-rollback", changelog_setting=setting
            )
        changelog.append(
            module=module,
            setting=setting,
            old_value=before,
            new_value=entry["old_value"],
            source="dbus-rollback",
            files_regenerated=files,
        )
        self.StateChanged(
            module,
            setting,
            _signal_variant(before),
            _signal_variant(entry["old_value"]),
        )
        if files:
            self.ConfigRegenerated(module, files)
        return True

    @dbus.service.method(IFACE, in_signature="", out_signature="b")
    def ValidateAll(self) -> bool:
        st = state.load()
        generate.validate_generated(st)
        return True

    @dbus.service.signal(IFACE, signature="ssvv")
    def StateChanged(self, module: str, key: str, old_value: Any, new_value: Any) -> None:
        """Emitted after SetSetting / Rollback."""

    @dbus.service.signal(IFACE, signature="sas")
    def ConfigRegenerated(self, module: str, files: list[str]) -> None:
        """Emitted after templates are rewritten for a module."""


def main(argv: list[str] | None = None) -> int:
    _ = argv
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SystemBus()
    # Request name; fail if another owner holds it.
    try:
        bus.request_name(BUS_NAME, dbus.bus.NAME_FLAG_DO_NOT_QUEUE)
    except dbus.DBusException as exc:
        print(f"error: cannot own {BUS_NAME}: {exc}", file=sys.stderr)
        return 1

    ConfigService(bus)
    print(f"spike-config-dbus: owning {BUS_NAME} on system bus", flush=True)
    loop = GLib.MainLoop()
    try:
        loop.run()
    except KeyboardInterrupt:
        return 0
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
