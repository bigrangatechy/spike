cmake_minimum_required(VERSION 3.16)
project(spike-rescue VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(spike-rescue
  src/main.cpp
  src/MainWindow.cpp
  src/MainWindow.hpp
  src/RescueEngine.cpp
  src/RescueEngine.hpp
  src/RescueTypes.hpp
  resources/spike-rescue.qrc
)

target_include_directories(spike-rescue PRIVATE src)
target_link_libraries(spike-rescue PRIVATE Qt6::Widgets)

install(TARGETS spike-rescue RUNTIME DESTINATION bin)
install(FILES data/spike-rescue.desktop DESTINATION share/applications)
install(FILES data/spike-rescue-desktop.desktop
        DESTINATION share/spike/live
        RENAME spike-rescue.desktop)
