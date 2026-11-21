# embedded-lab - one repo, several boards, several apps.
#
# Pick the board with a preset (see CMakePresets.json):
#   cmake --preset h7s78_dk      firmware for STM32H7S78-DK
#   cmake --preset n6570_dk      firmware for STM32N6570-DK
#   cmake --preset host-tests    unit tests that run on the PC
cmake_minimum_required(VERSION 3.22)
project(embedded_lab VERSION 0.1.0 LANGUAGES C ASM)

set(BOARD "" CACHE STRING "Target: h7s78_dk, n6570_dk or host")
set_property(CACHE BOARD PROPERTY STRINGS h7s78_dk n6570_dk host)
if(NOT BOARD)
  message(FATAL_ERROR "BOARD is not set - configure with a preset, e.g. cmake --preset h7s78_dk")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)          # gnu11: needed by CMSIS/HAL (asm, attributes)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LAB_THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
if(NOT EXISTS ${LAB_THIRD_PARTY}/unity/src/unity.c)
  message(FATAL_ERROR "third_party/ is empty - run: git submodule update --init --depth 1")
endif()

include(cmake/lab.cmake)

if(BOARD STREQUAL "host")
  enable_testing()
  add_subdirectory(libs)
  add_subdirectory(drivers)
  add_subdirectory(tests)
else()
  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD}/CMakeLists.txt)
    message(FATAL_ERROR "Unknown BOARD '${BOARD}' (no boards/${BOARD}/CMakeLists.txt)")
  endif()
  add_subdirectory(boards/${BOARD})   # defines: board, board_vendor, lab_mcu
  add_subdirectory(libs)
  add_subdirectory(drivers)
  add_subdirectory(apps)
endif()
