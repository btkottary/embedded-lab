# Board: STM32N6570-DK (STM32N657X0, Cortex-M55 @ 64 MHz HSI after reset)
# No internal flash: the image is stored in external NOR flash (0x70000000);
# at boot the ROM checks its header and copies it to AXISRAM2 (0x34180400),
# where it runs as the "FSBL" (first-stage boot loader).

set(HAL ${LAB_THIRD_PARTY}/stm32n6/hal)
set(DEV ${LAB_THIRD_PARTY}/stm32n6/cmsis_device)

# ---- CPU flags: Cortex-M55, hard float, TrustZone secure side (-mcmse) -----
set(MCU_FLAGS -mcpu=cortex-m55 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -mcmse)
target_compile_options(lab_mcu INTERFACE ${MCU_FLAGS} -ffunction-sections -fdata-sections)
target_link_options(lab_mcu INTERFACE ${MCU_FLAGS})

# Ask lab_add_firmware() to produce <app>-trusted.bin for the boot ROM
set(LAB_BOARD_SIGN_FSBL ON CACHE INTERNAL "")

# ---- Vendor code -----------------------------------------------------------
add_library(board_vendor OBJECT
  ${DEV}/Source/Templates/gcc/startup_stm32n657xx_fsbl.s
  ${DEV}/Source/Templates/system_stm32n6xx_fsbl.c
  ${HAL}/Src/stm32n6xx_hal.c
  ${HAL}/Src/stm32n6xx_hal_cortex.c
  ${HAL}/Src/stm32n6xx_hal_gpio.c
  ${HAL}/Src/stm32n6xx_hal_rcc.c
  ${HAL}/Src/stm32n6xx_hal_rcc_ex.c
  ${HAL}/Src/stm32n6xx_hal_pwr.c
  ${HAL}/Src/stm32n6xx_hal_pwr_ex.c
  ${HAL}/Src/stm32n6xx_hal_uart.c
  ${HAL}/Src/stm32n6xx_hal_uart_ex.c
)
target_include_directories(board_vendor SYSTEM PUBLIC
  ${HAL}/Inc
  ${DEV}/Include
  ${LAB_THIRD_PARTY}/cmsis_6/CMSIS/Core/Include
)
target_include_directories(board_vendor PUBLIC include)   # stm32n6xx_hal_conf.h
target_compile_definitions(board_vendor PUBLIC STM32N657xx USE_HAL_DRIVER)
target_link_libraries(board_vendor PUBLIC lab_mcu)

# ---- Our board support code ------------------------------------------------
add_library(board OBJECT
  src/board.c
  src/board_console.c
  src/stm32n6xx_it.c
  ../common/syscalls.c
)
target_include_directories(board PUBLIC ../include PRIVATE ../common)
target_link_libraries(board PUBLIC board_vendor PRIVATE lab_warnings)
target_link_options(board PUBLIC
  -T${DEV}/Source/Templates/gcc/linker/STM32N657XX_AXISRAM2_fsbl.ld
  --specs=nano.specs --specs=nosys.specs
  -Wl,--gc-sections
)
