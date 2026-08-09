// Spike panel task list — publish open windows to org.spike.Shell via D-Bus.
// Spec: DESKTOP.md (open-app icons on the panel for minimize/restore).

function publish() {
    const rows = [];
    workspace.windowList().forEach((w) => {
        if (!w.normalWindow || w.skipTaskbar || w.desktopWindow || w.dock) {
            return;
        }
        const id = w.internalId.toString();
        const min = w.minimized ? "1" : "0";
        const cls = String(w.resourceClass || "");
        const name = String(w.resourceName || "");
        const cap = String(w.caption || "").replace(/[\t\n\r]/g, " ");
        rows.push([id, min, cls, name, cap].join("\t"));
    });
    callDBus(
        "org.spike.Shell",
        "/TaskList",
        "org.spike.Shell.TaskList",
        "setWindows",
        rows.join("\n"),
    );
}

function watchWindow(w) {
    if (!w) {
        return;
    }
    try {
        w.minimizedChanged.connect(publish);
        w.captionChanged.connect(publish);
        w.skipTaskbarChanged.connect(publish);
    } catch (e) {
        // Older KWin script engines may lack some signals.
    }
}

workspace.windowList().forEach(watchWindow);
workspace.windowAdded.connect((w) => {
    watchWindow(w);
    publish();
});
workspace.windowRemoved.connect(publish);
workspace.windowActivated.connect(publish);

publish();
