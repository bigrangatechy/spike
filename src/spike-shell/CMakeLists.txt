cmake_minimum_required(VERSION 3.16)
project(spike-shell VERSION 0.0.15 LANGUAGES CXX)

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
  src/audio/VolumeClient.cpp
  src/audio/VolumeClient.hpp
  src/power/BatteryClient.cpp
  src/power/BatteryClient.hpp
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
  src/settings/DateTimePage.cpp
  src/settings/DateTimePage.hpp
  src/settings/KeyboardPage.cpp
  src/settings/KeyboardPage.hpp
  src/settings/MousePage.cpp
  src/settings/MousePage.hpp
  src/settings/KeyboardLayoutPage.cpp
  src/settings/KeyboardLayoutPage.hpp
  resources/spike-shell.qrc
)

target_include_directories(spike-shell PRIVATE src)
target_link_libraries(spike-shell PRIVATE Qt6::Widgets Qt6::DBus LayerShellQt::Interface)

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
install(FILES session/spike.desktop DESTINATION share/wayland-sessions)
install(PROGRAMS session/spike-session DESTINATION bin)
