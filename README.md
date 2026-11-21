# embedded-lab

<!-- After pushing to GitHub, replace <user> and enable the badge: -->
<!-- [![CI](https://github.com/<user>/embedded-lab/actions/workflows/ci.yml/badge.svg)](https://github.com/<user>/embedded-lab/actions/workflows/ci.yml) -->

Bare-metal STM32 firmware built from ST's official drivers with **GCC + CMake + Ninja**. There's no IDE or
code generator. One code base runs on several boards:

| Board | MCU | Core | Status |
|---|---|---|---|
| STM32H7S78-DK | STM32H7S7L8 | Cortex-M7 | blink ✅ |
| STM32N6570-DK | STM32N657X0 | Cortex-M55 | blink ✅ |

## Layout

```
apps/        applications (each builds for any board)        apps/blink
boards/      board support: board.h API + one folder per board
drivers/     drivers for external chips (sensors, PHYs ...) - no HAL inside
libs/        hardware-independent code                      libs/ringbuf
tests/       unit tests run on the PC (Unity)
third_party/ pinned git submodules: ST HAL, CMSIS device, CMSIS 6, Unity
cmake/       toolchain file + lab_add_firmware() helper
tools/       env / flash / test scripts (Git Bash + PowerShell)
docs/        architecture, hardware notes, decision records
```

Code is layered as **app → libs/drivers → `board.h` → `boards/<board>` → ST HAL → hardware**. Apps never
include vendor headers, which is why `apps/blink/main.c` runs unchanged on both boards. See
[docs/architecture.md](docs/architecture.md).

## Quick start (Windows, Git Bash)

Prerequisites: STM32CubeIDE 2.x (only its bundled GCC, CMake and Ninja are used) and STM32CubeProgrammer 2.18+.
The unit tests also need MSYS2 with the `ucrt64` gcc.

```bash
git clone --recurse-submodules --shallow-submodules https://github.com/<user>/embedded-lab.git
cd embedded-lab
source tools/env.sh                       # puts the tools on PATH for this terminal

cmake --preset h7s78_dk                   # configure (once)
cmake --build --preset h7s78_dk           # build -> build/h7s78_dk/apps/blink/blink.elf
bash tools/flash.sh h7s78_dk blink        # flash over ST-LINK

cmake --preset n6570_dk && cmake --build --preset n6570_dk
bash tools/flash.sh n6570_dk blink        # asks you to set BOOT1 first

bash tools/test.sh                        # unit tests on the PC
```

PowerShell users: `. .\tools\env.ps1`, `.\tools\flash.ps1 -Board h7s78_dk`, `.\tools\test.ps1`.
In VS Code: **Ctrl+Shift+B** builds, and **Terminal → Run Task** lists flash and test.

Board jumpers, boot switches and other gotchas: [docs/hardware.md](docs/hardware.md).

## Development workflow

GitHub Flow with Conventional Commits: see [docs/workflow.md](docs/workflow.md). In short:
1. Branch from `main`, for example `feat/uart-printf`.
2. Commit small steps, e.g. `feat(uart): add printf over VCP`.
3. Open a PR. CI must pass: firmware for both boards, unit tests, clang-format and cppcheck.
4. Squash-merge, then tag releases with versions like `v0.2.0`.

## License

MIT, see [LICENSE](LICENSE). Code under `third_party/` keeps its own licenses: BSD-3-Clause (ST), Apache-2.0 (CMSIS), MIT (Unity).
