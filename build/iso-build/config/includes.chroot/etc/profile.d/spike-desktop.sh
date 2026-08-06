# Spike live: Qt platform theme for plasma-integration (KDE apps / KCMs).
export QT_QPA_PLATFORMTHEME="${QT_QPA_PLATFORMTHEME:-kde}"
# Include KDE so xdg-desktop-portal selects the KDE backend (kde.portal UseIn=KDE).
export XDG_CURRENT_DESKTOP="${XDG_CURRENT_DESKTOP:-Spike:KDE}"
