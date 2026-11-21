# Architecture

## Layers

```
┌──────────────────────────────────────────────┐
│ apps/<app>/main.c          what the product does │  includes board.h, libs, drivers
├──────────────────────────────────────────────┤
│ libs/  drivers/            reusable, testable   │  no HAL; drivers use a bus interface
├──────────────────────────────────────────────┤
│ boards/include/board.h     the board API        │  board_init, board_led_set, ...
├──────────────────────────────────────────────┤
│ boards/<board>/src/*.c     board implementation │  pins, clocks, power domains
├──────────────────────────────────────────────┤
│ third_party/ ST HAL + CMSIS (pinned submodules)  │  never edited
└──────────────────────────────────────────────┘
```

Each layer may only call **down**. That buys three things:
- **Portability**: `apps/blink/main.c` builds unchanged for the H7S (4 LEDs) and the N6 (2 LEDs).
- **Testability**: `libs/` and `drivers/` compile with the PC's gcc and run under Unity (`tests/`).
- **Replaceability**: moving to another MCU means writing a new `boards/<board>/`, nothing else.

## Build system

- `CMakePresets.json` has one preset per target: `h7s78_dk`, `n6570_dk` and `host-tests`. Each builds into `build/<preset>/`.
- The `BOARD` cache variable picks `boards/<BOARD>/CMakeLists.txt`. That file defines three targets:
  - `lab_mcu`: CPU and FPU flags (`-mcpu=...`), applied to everything built for the board.
  - `board_vendor`: OBJECT library with ST startup, `system_*.c` and the HAL modules in use. It builds with default warnings.
  - `board`: OBJECT library with our `board.c`, IRQ handlers and syscalls. It builds with strict `lab_warnings` (`-Werror`).
- `lab_add_firmware(<app> SOURCES ...)` in `cmake/lab.cmake` links an app with `board` + `board_vendor`, then
  produces `.elf/.hex/.bin/.map`, a size report and, on the N6, `<app>-trusted.bin`.

Why OBJECT libraries rather than static ones: a static library only contributes object files that something
references. Interrupt handlers like `SysTick_Handler` are only reached through weak aliases in the vector
table, so the linker would silently drop them, `HAL_Delay()` would hang, and nothing would warn you.
OBJECT libraries link every object file.

## Boot flow

| | H7S78-DK | N6570-DK |
|---|---|---|
| Stored in | internal flash 0x08000000 | external NOR flash 0x70000000, with a 1 KB ST header |
| Runs from | internal flash (XIP) | AXISRAM2 0x34180400; the boot ROM copies it there |
| Reset → main | `Reset_Handler` → `SystemInit()` → C runtime init → `main()` | same, after the boot ROM has checked the header |

## Where things come from

| File | Source |
|---|---|
| `startup_*.s`, `system_*.c`, `*.ld` | `third_party/<chip>/cmsis_device/Source/Templates/` |
| HAL drivers | `third_party/<chip>/hal/` |
| `core_cm7.h`, `core_cm55.h` | `third_party/cmsis_6/CMSIS/Core/Include` |
| `*_hal_conf.h` | ST template with unused modules commented out, in `boards/<board>/include/` |
