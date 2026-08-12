// Spike media / Fn keys — register global shortcuts and call org.spike.Shell.
// Requires kglobalacceld (started by spike-session / ShellShortcuts).

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

bind("spikeVolumeUp", "Spike Volume Up", "XF86AudioRaiseVolume", "volumeUp");
bind("spikeVolumeDown", "Spike Volume Down", "XF86AudioLowerVolume", "volumeDown");
bind("spikeVolumeMute", "Spike Volume Mute", "XF86AudioMute", "volumeMute");

bind("spikeBrightnessUp", "Spike Brightness Up", "XF86MonBrightnessUp", "brightnessUp");
bind("spikeBrightnessDown", "Spike Brightness Down", "XF86MonBrightnessDown", "brightnessDown");

bind("spikeMediaPlay", "Spike Play/Pause", "XF86AudioPlay", "mediaPlayPause");
bind("spikeMediaPause", "Spike Pause", "XF86AudioPause", "mediaPlayPause");
bind("spikeMediaKey", "Spike Media Key", "XF86AudioMedia", "mediaPlayPause");
bind("spikeMediaNext", "Spike Next Track", "XF86AudioNext", "mediaNext");
bind("spikeMediaPrev", "Spike Previous Track", "XF86AudioPrev", "mediaPrevious");

bind("spikeLock", "Spike Lock Screen", "Meta+L", "lock");
bind("spikeLauncher", "Spike Launcher", "Meta+Space", "launcher");
