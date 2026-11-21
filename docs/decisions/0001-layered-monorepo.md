# 0001 - Layered monorepo with a board API

- Status: accepted
- Date: 2026-11-21

## Context
Two very different boards (H7S78-DK Cortex-M7 running from flash; N6570-DK Cortex-M55 running from RAM after
a signed boot) should share as much code as possible. The repo also serves as a public portfolio.

## Decision
- One repository. The board is selected with CMake presets (`BOARD` variable).
- Apps, libs and drivers use only `boards/include/board.h` plus their own headers. Only `boards/<board>/`
  touches the ST HAL.
- ST HAL, CMSIS device, CMSIS 6 and Unity are git submodules pinned to release tags under `third_party/`. They
  replace the full STM32Cube packages, saving about 550 MB.
- Plain CMake + Ninja + GCC. No IDE project files, no CubeMX-generated code.

## Consequences
- \+ Same app on both boards; logic is unit-testable on a PC; reviewers see a clear structure.
- \+ Reproducible: anyone gets the exact same vendor code with `git submodule update --init`.
- \- Board features need a `board.h` function before an app can use them, which is a little more upfront work.
- \- No CubeMX, so clock and pin setup is written by hand. That's deliberate, for learning.
