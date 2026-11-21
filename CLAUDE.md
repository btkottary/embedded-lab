# CLAUDE.md - rules for AI assistance in this repo

## Who and why
The owner is **learning embedded firmware** and building a freelance portfolio.
- Prefer explaining over just doing. For non-obvious hardware behaviour, name the source: reference manual
  section, datasheet table, or the BSP file and function.
- When asked to review, review like a senior embedded engineer: correctness, ISR safety, error handling,
  portability, readability.
- Keep changes small and focused, one topic per branch/PR.

## Layout and layering (do not break)
`apps/` → `libs/`, `drivers/` → `boards/include/board.h` → `boards/<board>/` → `third_party/` (ST HAL, CMSIS)
- Only `boards/<board>/` may include ST HAL / CMSIS-device headers.
- `apps/`, `libs/` and `drivers/` include `board.h` or their own headers, never `stm32*_hal.h`.
- Drivers for external chips take a bus-interface struct (function pointers); see `drivers/README.md`.
- **Never edit anything under `third_party/`.** It holds pinned submodules. To change versions, update the
  submodule commit in its own PR.
- New board: `boards/<name>/` with `CMakeLists.txt` defining `board`, `board_vendor` and `lab_mcu` flags, an
  implementation of every function in `board.h`, and a preset in `CMakePresets.json`.

## Build, flash, test
```bash
source tools/env.sh                          # Git Bash: tools on PATH
cmake --preset h7s78_dk && cmake --build --preset h7s78_dk
cmake --preset n6570_dk && cmake --build --preset n6570_dk
bash tools/test.sh                           # host unit tests (Unity)
bash tools/flash.sh <h7s78_dk|n6570_dk> [app]
```
- New app: `apps/<name>/CMakeLists.txt` with `lab_add_firmware(<name> SOURCES ...)`, plus `add_subdirectory` in `apps/CMakeLists.txt`.
- New lib: static library linking `lab_mcu` and `lab_warnings`, plus a test in `tests/` via `lab_add_test()`.
- Every change must build for **both** boards with zero warnings (`-Werror`) and keep the unit tests green.

## Coding standard (summary - full version in docs/coding-standard.md)
- C11 (gnu11). Format with the repo `.clang-format`: 4 spaces, braces on their own line, 100 columns.
- Names: `module_verb_noun()` functions, `snake_case` variables, `UPPER_CASE` macros and constants,
  `s_` prefix for file-static variables, `_t` suffix for typedefs.
- Every non-static function has a prototype in a header with a Doxygen `/** ... */` comment.
- `static` everything that isn't part of a module's API. Fixed-width types (`uint32_t`) for hardware and protocol data.
- No dynamic allocation (`malloc`) in firmware. No recursion. Check every HAL return value.
- ISRs: short, no blocking, no HAL_Delay; share data via `libs/ringbuf` or `volatile` flags.
- Unsigned literals with `U` (`100U`). Use parentheses in compound conditions. One exit point where it helps clarity.

## Git
- Branches: `feat/…`, `fix/…`, `docs/…`, `refactor/…`, `test/…`, `ci/…`.
- Commits: Conventional Commits, e.g. `feat(ringbuf): add peek`, `fix(n6): enable VDDIO2 before GPIOO`.
- Never commit build output or secrets. Don't push or merge unless asked.
