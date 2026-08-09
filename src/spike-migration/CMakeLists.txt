cmake_minimum_required(VERSION 3.16)
project(spike-migration VERSION 0.0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SPIKE_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../spike-common")

add_executable(spike-migration
  src/main.cpp
  "${SPIKE_COMMON_DIR}/SpikeBackupLayout.cpp"
  "${SPIKE_COMMON_DIR}/SpikeBackupLayout.hpp"
)
target_include_directories(spike-migration PRIVATE "${SPIKE_COMMON_DIR}")
target_link_libraries(spike-migration PRIVATE Qt6::Widgets)

install(TARGETS spike-migration RUNTIME DESTINATION bin)
install(FILES data/spike-migration.desktop DESTINATION share/applications)
install(FILES data/spike-migration-desktop.desktop
        DESTINATION share/spike/live
        RENAME spike-migration.desktop)
