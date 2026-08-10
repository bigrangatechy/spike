cmake_minimum_required(VERSION 3.16)
project(spike-shell VERSION 0.0.43 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(LayerShellQt REQUIRED)

# KF6 KCMUtils — prefer system find_package; fall back to headers/libs under
# build/deps-extract (same pattern as LayerShellQt for offline/dev hosts).
set(SPIKE_HAVE_KCMUTILS FALSE)
find_package(KF6 QUIET COMPONENTS KCMUtils)
if(KF6KCMUtils_FOUND)
  set(SPIKE_HAVE_KCMUTILS TRUE)
else()
  get_filename_component(_spike_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
  set(_kf6_extract "${_spike_root}/build/deps-extract/root/usr")
  if(EXISTS "${_kf6_extract}/include/KF6/KCMUtils/kcmoduleloader.h"
     AND EXISTS "${_kf6_extract}/lib/x86_64-linux-gnu/libKF6KCMUtils.so")
    set(SPIKE_HAVE_KCMUTILS TRUE)
    set(SPIKE_KF6_EXTRACT "${_kf6_extract}")
    message(STATUS "Using extracted KF6 KCMUtils from ${SPIKE_KF6_EXTRACT}")
  endif()
endif()

if(NOT SPIKE_HAVE_KCMUTILS)
  message(FATAL_ERROR
    "KF6 KCMUtils required for in-window Settings KCM host.\n"
    "  sudo apt install libkf6kcmutils-dev\n"
    "  or extract under build/deps-extract/ (see package-spike-shell.sh)")
endif()

add_executable(spike-shell
  src/main.cpp
  src/panel/Panel.cpp
  src/panel/Panel.hpp
  src/panel/applets/TrayHelpers.cpp
  src/panel/applets/TrayHelpers.hpp
  src/panel/applets/ClockApplet.cpp
  src/panel/applets/ClockApplet.hpp
  src/panel/applets/NetworkApplet.cpp
  src/panel/applets/NetworkApplet.hpp
  src/panel/applets/SessionMenuApplet.cpp
  src/panel/applets/SessionMenuApplet.hpp
  src/panel/applets/VolumeApplet.cpp
  src/panel/applets/VolumeApplet.hpp
  src/panel/applets/BatteryApplet.cpp
  src/panel/applets/BatteryApplet.hpp
  src/panel/applets/NotificationsApplet.cpp
  src/panel/applets/NotificationsApplet.hpp
  src/panel/applets/BrightnessApplet.cpp
  src/panel/applets/BrightnessApplet.hpp
  src/panel/applets/RemovableDevicesApplet.cpp
  src/panel/applets/RemovableDevicesApplet.hpp
  src/panel/applets/UpdateNotifierApplet.cpp
  src/panel/applets/UpdateNotifierApplet.hpp
  src/panel/applets/NightLightApplet.cpp
  src/panel/applets/NightLightApplet.hpp
  src/panel/applets/BluetoothApplet.cpp
  src/panel/applets/BluetoothApplet.hpp
  src/panel/applets/AirplaneModeApplet.cpp
  src/panel/applets/AirplaneModeApplet.hpp
  src/panel/applets/KeyboardLayoutApplet.cpp
  src/panel/applets/KeyboardLayoutApplet.hpp
  src/panel/applets/WindowListApplet.cpp
  src/panel/applets/WindowListApplet.hpp
  src/panel/applets/TaskListAdaptor.cpp
  src/panel/applets/TaskListAdaptor.hpp
  src/notify/NotificationDaemon.cpp
  src/notify/NotificationDaemon.hpp
  src/notify/NotificationAdaptor.cpp
  src/notify/NotificationAdaptor.hpp
  src/audio/VolumeClient.cpp
  src/audio/VolumeClient.hpp
  src/power/BatteryClient.cpp
  src/power/BatteryClient.hpp
  src/power/BrightnessClient.cpp
  src/power/BrightnessClient.hpp
  src/power/SleepInhibit.cpp
  src/power/SleepInhibit.hpp
  src/lock/PamAuth.cpp
  src/lock/PamAuth.hpp
  src/lock/SpikeLockScreen.cpp
  src/lock/SpikeLockScreen.hpp
  src/lock/LockController.cpp
  src/lock/LockController.hpp
  src/network/NmClient.cpp
  src/network/NmClient.hpp
  src/network/NetworkPanelWidget.cpp
  src/network/NetworkPanelWidget.hpp
  src/launcher/Launcher.cpp
  src/launcher/Launcher.hpp
  src/settings/ConfigClient.cpp
  src/settings/ConfigClient.hpp
  src/settings/KcmHost.cpp
  src/settings/KcmHost.hpp
  src/settings/SettingsWindow.cpp
  src/settings/SettingsWindow.hpp
  src/settings/PanelPages.cpp
  src/settings/PanelPages.hpp
  src/settings/DateTimePage.cpp
  src/settings/DateTimePage.hpp
  src/settings/KeyboardPage.cpp
  src/settings/KeyboardPage.hpp
  src/settings/MousePage.cpp
  src/settings/MousePage.hpp
  src/settings/PowerPage.cpp
  src/settings/PowerPage.hpp
  src/settings/UpdatesPage.cpp
  src/settings/UpdatesPage.hpp
  src/settings/KernelModulesPage.cpp
  src/settings/KernelModulesPage.hpp
  src/settings/StoragePage.cpp
  src/settings/StoragePage.hpp
  src/settings/AppearanceLive.cpp
  src/settings/AppearanceLive.hpp
  src/desktop/DesktopBackground.cpp
  src/desktop/DesktopBackground.hpp
  src/desktop/DesktopIcons.cpp
  src/desktop/DesktopIcons.hpp
  src/firstrun/FirstRunHooks.cpp
  src/firstrun/FirstRunHooks.hpp
  src/firstrun/FirstRunWizard.cpp
  src/firstrun/FirstRunWizard.hpp
  src/settings/KeyboardLayoutPage.cpp
  src/settings/KeyboardLayoutPage.hpp
  src/settings/MemoryPage.cpp
  src/settings/MemoryPage.hpp
  src/settings/BootPage.cpp
  src/settings/BootPage.hpp
  src/settings/LanguagePage.cpp
  src/settings/LanguagePage.hpp
  src/settings/UsersPage.cpp
  src/settings/UsersPage.hpp
  src/settings/VpnPage.cpp
  src/settings/VpnPage.hpp
  src/settings/NotificationsPage.cpp
  src/settings/NotificationsPage.hpp
  src/settings/AccessibilityPage.cpp
  src/settings/AccessibilityPage.hpp
  src/settings/SoftwareSourcesPage.cpp
  src/settings/SoftwareSourcesPage.hpp
  src/settings/InputConfig.cpp
  src/settings/InputConfig.hpp
  resources/spike-shell.qrc
)

target_include_directories(spike-shell PRIVATE src)
target_link_libraries(spike-shell PRIVATE Qt6::Widgets Qt6::DBus LayerShellQt::Interface ${CMAKE_DL_LIBS})

if(KF6KCMUtils_FOUND)
  target_link_libraries(spike-shell PRIVATE KF6::KCMUtils)
else()
  target_include_directories(spike-shell PRIVATE
    "${SPIKE_KF6_EXTRACT}/include/KF6/KCMUtils"
    "${SPIKE_KF6_EXTRACT}/include/KF6/KCMUtilsCore"
    "${SPIKE_KF6_EXTRACT}/include/KF6/KCoreAddons"
  )
  target_link_libraries(spike-shell PRIVATE
    "${SPIKE_KF6_EXTRACT}/lib/x86_64-linux-gnu/libKF6KCMUtils.so"
    "${SPIKE_KF6_EXTRACT}/lib/x86_64-linux-gnu/libKF6KCMUtilsCore.so"
    "${SPIKE_KF6_EXTRACT}/lib/x86_64-linux-gnu/libKF6CoreAddons.so"
  )
endif()

install(TARGETS spike-shell RUNTIME DESTINATION bin)

add_executable(spike-greeter
  src/greeter/main.cpp
  src/lock/PamAuth.cpp
  src/lock/PamAuth.hpp
)
target_include_directories(spike-greeter PRIVATE src)
target_link_libraries(spike-greeter PRIVATE Qt6::Widgets ${CMAKE_DL_LIBS})
install(TARGETS spike-greeter RUNTIME DESTINATION bin)
install(FILES session/spike-greeter.service DESTINATION lib/systemd/system)

install(FILES session/spike.desktop DESTINATION share/wayland-sessions)
install(PROGRAMS session/spike-session DESTINATION bin)
install(PROGRAMS session/spike-seed-home DESTINATION bin)
install(PROGRAMS session/spike-fix-mozilla-home DESTINATION bin)
install(FILES pam/spike-lock DESTINATION etc/pam.d)
install(DIRECTORY kwin-scripts/spike-tasklist
        DESTINATION share/kwin/scripts)
