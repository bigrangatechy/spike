cmake_minimum_required(VERSION 3.16)
project(spike-migration VERSION 0.0.1 LANGUAGES CXX)

# Scaffold — full UI not implemented yet. When built, link spike-common:
#   add_executable(spike-migration …)
#   target_include_directories(… PRIVATE ../spike-common)
#   target_sources(… PRIVATE ../spike-common/SpikeBackupLayout.cpp)

message(STATUS "spike-migration ${PROJECT_VERSION}: scaffold only (see README.md)")
