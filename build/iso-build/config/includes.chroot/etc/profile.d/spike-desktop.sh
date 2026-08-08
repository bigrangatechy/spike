# Spike live: Qt platform theme for plasma-integration (KDE apps / KCMs).
export QT_QPA_PLATFORMTHEME="${QT_QPA_PLATFORMTHEME:-kde}"
export QT_QUICK_CONTROLS_STYLE="${QT_QUICK_CONTROLS_STYLE:-org.kde.desktop}"
# Include KDE so xdg-desktop-portal selects the KDE backend (kde.portal UseIn=KDE).
export XDG_CURRENT_DESKTOP="${XDG_CURRENT_DESKTOP:-Spike:KDE}"
