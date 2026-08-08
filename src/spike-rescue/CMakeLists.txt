cmake_minimum_required(VERSION 3.16)
project(spike-rescue VERSION 0.0.9 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SPIKE_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../spike-common")

add_executable(spike-rescue
  src/main.cpp
  src/MainWindow.cpp
  src/MainWindow.hpp
  src/RescueEngine.cpp
  src/RescueEngine.hpp
  src/RescueTypes.hpp
  "${SPIKE_COMMON_DIR}/SpikeBackupLayout.cpp"
  "${SPIKE_COMMON_DIR}/SpikeBackupLayout.hpp"
  resources/spike-rescue.qrc
)

target_include_directories(spike-rescue PRIVATE src "${SPIKE_COMMON_DIR}")
target_link_libraries(spike-rescue PRIVATE Qt6::Widgets)

install(TARGETS spike-rescue RUNTIME DESTINATION bin)
install(PROGRAMS data/spike-rescue-mount DESTINATION lib/spike)
install(FILES data/spike-rescue.sudoers
        DESTINATION etc/sudoers.d
        RENAME spike-rescue
        PERMISSIONS OWNER_READ GROUP_READ)
install(FILES data/spike-rescue.desktop DESTINATION share/applications)
install(FILES data/spike-rescue-desktop.desktop
        DESTINATION share/spike/live
        RENAME spike-rescue.desktop)
# Shared layout headers for installer / migration (same tree on live ISO via this package).
install(FILES "${SPIKE_COMMON_DIR}/SpikeBackupLayout.hpp"
        DESTINATION include/spike)
