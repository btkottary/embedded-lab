# Shared CMake helpers for embedded-lab.
#
# Targets every part of the build can link to:
#   lab_warnings  strict warnings - link it to YOUR code only, never to vendor code
#   lab_mcu       CPU/FPU flags of the selected board (empty for host builds)
#
# Function:
#   lab_add_firmware(<name> SOURCES <files...> [LIBS <targets...>])
#       builds <name>.elf for the selected board, plus .hex/.bin, a .map file
#       and a size report; signs the image when the board asks for it (N6).

add_library(lab_warnings INTERFACE)
target_compile_options(lab_warnings INTERFACE
  -Wall -Wextra -Werror
  -Wshadow -Wdouble-promotion -Wformat=2 -Wundef
  -Wstrict-prototypes -Wmissing-prototypes
)

if(NOT TARGET lab_mcu)
  add_library(lab_mcu INTERFACE)   # boards fill this in
endif()

# Firmware version string, e.g. "v0.2.0-3-g1a2b3c4-dirty" (from git tags).
# Read at configure time: re-run "cmake --preset ..." to refresh it.
execute_process(
  COMMAND git describe --tags --always --dirty
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE LAB_GIT_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
if(NOT LAB_GIT_VERSION)
  set(LAB_GIT_VERSION "unknown")
endif()

function(lab_add_firmware name)
  cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
  if(BOARD STREQUAL "host")
    return()   # firmware is not built in host (unit test) builds
  endif()

  add_executable(${name} ${ARG_SOURCES})
  # board        = our board support code (strict warnings)
  # board_vendor = ST HAL + startup + system file (vendor code, default warnings)
  target_link_libraries(${name} PRIVATE board board_vendor lab_warnings ${ARG_LIBS})
  target_compile_definitions(${name} PRIVATE
    LAB_VERSION="${PROJECT_VERSION}"       # from project(... VERSION x.y.z)
    LAB_GIT_VERSION="${LAB_GIT_VERSION}"
  )
  target_link_options(${name} PRIVATE
    -Wl,-Map=$<TARGET_FILE_DIR:${name}>/${name}.map
    -Wl,--print-memory-usage
  )

  set(out "$<TARGET_FILE_DIR:${name}>/${name}")
  add_custom_command(TARGET ${name} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${name}> ${out}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${name}> ${out}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${name}>
    COMMENT "${name}: .hex / .bin / size"
    VERBATIM
  )

  # STM32N6: the boot ROM only runs images with an ST header ("signed",
  # here without keys). Skipped with a warning where the tool is missing (CI).
  if(LAB_BOARD_SIGN_FSBL)
    find_program(STM32_SIGNING_TOOL STM32_SigningTool_CLI
      PATHS "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin")
    if(STM32_SIGNING_TOOL)
      add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${STM32_SIGNING_TOOL} -bin ${out}.bin -nk -t fsbl -hv 2.3 -align -s
                -o ${out}-trusted.bin
        COMMENT "${name}: signing -> ${name}-trusted.bin"
        VERBATIM
      )
    else()
      message(WARNING "STM32_SigningTool_CLI not found: ${name}-trusted.bin will not be created")
    endif()
  endif()
endfunction()
