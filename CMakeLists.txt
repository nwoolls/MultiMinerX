cmake_minimum_required(VERSION 2.4)

project(multiminer C)

set(PLUGIN_INCLUDE_DIR ${CMAKE_BINARY_DIR}/curses-shell/src/plugins)
include_directories(${PLUGIN_INCLUDE_DIR})
message(${PLUGIN_INCLUDE_DIR})

add_subdirectory(src)