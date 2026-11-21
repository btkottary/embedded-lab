# Toolchain file: cross-compile for bare-metal Arm Cortex-M with GNU Arm GCC.
# Selected by the board presets in CMakePresets.json. The tools are found on
# PATH (tools/env.sh or tools/env.ps1 add the GCC that ships with STM32CubeIDE;
# CI installs the official Arm GNU Toolchain).
#
# CPU flags (-mcpu=...) are NOT here: they belong to the board, see
# boards/<board>/CMakeLists.txt.

set(CMAKE_SYSTEM_NAME      Generic)   # no OS -> bare metal
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE    arm-none-eabi-size    REQUIRED)

# A bare-metal program can't link without a linker script, so CMake's
# compiler check only builds a static library.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# Optimisation per build type: Debug stays debuggable, Release is small.
set(CMAKE_C_FLAGS_DEBUG_INIT          "-Og -g3")
set(CMAKE_C_FLAGS_RELEASE_INIT        "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-Os -g3 -DNDEBUG")
set(CMAKE_ASM_FLAGS_DEBUG_INIT        "-g3")
