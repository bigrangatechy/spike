cmake_minimum_required(VERSION 3.16)
project(spike-shell VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(spike-shell
  src/main.cpp
  src/panel/Panel.cpp
  src/panel/Panel.hpp
  src/panel/applets/ClockApplet.cpp
  src/panel/applets/ClockApplet.hpp
  src/panel/applets/SessionMenuApplet.cpp
  src/panel/applets/SessionMenuApplet.hpp
  src/launcher/Launcher.cpp
  src/launcher/Launcher.hpp
  resources/spike-shell.qrc
)

target_include_directories(spike-shell PRIVATE src)
target_link_libraries(spike-shell PRIVATE Qt6::Widgets)

install(TARGETS spike-shell RUNTIME DESTINATION bin)
install(FILES session/spike.desktop DESTINATION share/wayland-sessions)
install(PROGRAMS session/spike-session DESTINATION bin)
