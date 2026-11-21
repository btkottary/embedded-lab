# Board: STM32H7S78-DK (STM32H7S7L8, Cortex-M7 @ 64 MHz HSI after reset)
# Program location: internal flash 0x08000000 (64 KB), data in AXI SRAM, stack in DTCM.

set(HAL ${LAB_THIRD_PARTY}/stm32h7rs/hal)
set(DEV ${LAB_THIRD_PARTY}/stm32h7rs/cmsis_device)

# ---- CPU flags: used by every target built for this board -----------------
set(MCU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
target_compile_options(lab_mcu INTERFACE ${MCU_FLAGS} -ffunction-sections -fdata-sections)
target_link_options(lab_mcu INTERFACE ${MCU_FLAGS})

# ---- Vendor code: CMSIS startup/system + the HAL modules we use -----------
add_library(board_vendor OBJECT
  ${DEV}/Source/Templates/gcc/startup_stm32h7s7xx.s   # vector table + Reset_Handler
  ${DEV}/Source/Templates/system_stm32h7rsxx.c        # SystemInit(), SystemCoreClock
  ${HAL}/Src/stm32h7rsxx_hal.c
  ${HAL}/Src/stm32h7rsxx_hal_cortex.c
  ${HAL}/Src/stm32h7rsxx_hal_gpio.c
  ${HAL}/Src/stm32h7rsxx_hal_rcc.c
  ${HAL}/Src/stm32h7rsxx_hal_rcc_ex.c
  ${HAL}/Src/stm32h7rsxx_hal_pwr.c
  ${HAL}/Src/stm32h7rsxx_hal_pwr_ex.c
  ${HAL}/Src/stm32h7rsxx_hal_flash.c
  ${HAL}/Src/stm32h7rsxx_hal_flash_ex.c
  ${HAL}/Src/stm32h7rsxx_hal_uart.c
  ${HAL}/Src/stm32h7rsxx_hal_uart_ex.c
)
# SYSTEM: warnings inside vendor headers don't fail OUR -Werror build
target_include_directories(board_vendor SYSTEM PUBLIC
  ${HAL}/Inc
  ${DEV}/Include
  ${LAB_THIRD_PARTY}/cmsis_6/CMSIS/Core/Include
)
target_include_directories(board_vendor PUBLIC include)   # stm32h7rsxx_hal_conf.h
target_compile_definitions(board_vendor PUBLIC STM32H7S7xx USE_HAL_DRIVER)
target_link_libraries(board_vendor PUBLIC lab_mcu)

# ---- Our board support code (implements boards/include/board.h) -----------
add_library(board OBJECT
  src/board.c
  src/board_console.c
  src/stm32h7rsxx_it.c
  ../common/syscalls.c
)
target_include_directories(board PUBLIC ../include PRIVATE ../common)
target_link_libraries(board PUBLIC board_vendor PRIVATE lab_warnings)
target_link_options(board PUBLIC
  -T${DEV}/Source/Templates/gcc/linker/stm32h7s7xx_flash.ld
  --specs=nano.specs --specs=nosys.specs
  -Wl,--gc-sections
)
