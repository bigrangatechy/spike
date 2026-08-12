cmake_minimum_required(VERSION 3.16)
project(spike-installer VERSION 0.0.23 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SPIKE_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../spike-common")

add_executable(spike-installer
  src/main.cpp
  src/InstallWizard.cpp
  src/InstallWizard.hpp
  src/InstallEngine.cpp
  src/InstallEngine.hpp
  src/InstallState.hpp
  src/InstallLocale.cpp
  src/InstallLocale.hpp
  src/detect/Detect.cpp
  src/detect/Detect.hpp
  "${SPIKE_COMMON_DIR}/SpikeBackupLayout.cpp"
  "${SPIKE_COMMON_DIR}/SpikeBackupLayout.hpp"
)

target_include_directories(spike-installer PRIVATE src "${SPIKE_COMMON_DIR}")
target_link_libraries(spike-installer PRIVATE Qt6::Widgets)

install(TARGETS spike-installer RUNTIME DESTINATION bin)
install(FILES data/spike-installer.desktop DESTINATION share/applications)
install(FILES data/spike-installer-desktop.desktop
        DESTINATION share/spike/live
        RENAME spike-installer.desktop)
