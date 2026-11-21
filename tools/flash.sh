#!/usr/bin/env bash
# Flash a built app over the board's ST-LINK.
#   bash tools/flash.sh h7s78_dk [app]     app defaults to "blink"
#   bash tools/flash.sh n6570_dk [app]
# Build first:  cmake --preset <board> && cmake --build --preset <board>
set -euo pipefail

BOARD="${1:-}"
APP="${2:-blink}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build/$BOARD/apps/$APP"
CP_BIN="/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
CLI="$CP_BIN/STM32_Programmer_CLI.exe"
win() { cygpath -w "$1"; }   # CubeProgrammer is a Windows program: give it C:\... paths

case "$BOARD" in
  h7s78_dk)
    ELF="$OUT/$APP.elf"
    [ -f "$ELF" ] || { echo "$ELF not found - build first: cmake --build --preset h7s78_dk"; exit 1; }
    # port=swd | mode=UR: connect while holding reset | -d: ELF carries its addresses
    # -v: verify | -rst: reset and run
    "$CLI" -c port=swd mode=UR reset=HWrst -d "$(win "$ELF")" -v -rst
    ;;
  n6570_dk)
    IMG="$OUT/$APP-trusted.bin"
    LOADER="$CP_BIN/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"
    [ -f "$IMG" ] || { echo "$IMG not found - build first: cmake --build --preset n6570_dk"; exit 1; }
    echo "N6570-DK: unplug CN6 -> SW1/BOOT1 = H, SW2/BOOT0 = L -> plug CN6 back in."
    read -r -p "Press Enter when done " _
    # mode=HOTPLUG ap=1: attach via the Cortex-M55 access port without reset
    # -el: external loader that programs the NOR flash; 0x70000000 = its start
    "$CLI" -c port=swd mode=HOTPLUG ap=1 -el "$(win "$LOADER")" -d "$(win "$IMG")" 0x70000000 -v
    echo "Flashed. To run: unplug CN6 -> SW1/BOOT1 = L -> plug CN6 back in."
    ;;
  *)
    echo "usage: bash tools/flash.sh h7s78_dk|n6570_dk [app]"; exit 1 ;;
esac
