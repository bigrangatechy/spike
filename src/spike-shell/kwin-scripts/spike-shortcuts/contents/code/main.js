// Spike Meta shortcuts — KWin registerShortcut (needs kglobalacceld).
// Volume / brightness / media Fn keys are handled by ShellShortcuts via evdev
// (/dev/input) because XF86 binds often never register on Spike's KWin-only session.

function callSpike(method) {
    callDBus(
        "org.spike.Shell",
        "/Shortcuts",
        "org.spike.Shell.Shortcuts",
        method,
    );
}

function bind(id, label, keys, method) {
    registerShortcut(id, label, keys, function () {
        callSpike(method);
    });
}

bind("spikeLock", "Spike Lock Screen", "Meta+L", "lock");
bind("spikeLauncher", "Spike Launcher", "Meta+Space", "launcher");
