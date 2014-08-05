cmake_minimum_required(VERSION 2.4)

project(multiminer C)

# header include directories
set(PLUGIN_INCLUDE_DIR ${CMAKE_BINARY_DIR}/curses-shell/src/plugins)
include_directories(${PLUGIN_INCLUDE_DIR})
message(${PLUGIN_INCLUDE_DIR})

# plugin sources
add_subdirectory(src)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)