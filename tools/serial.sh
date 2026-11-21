#!/usr/bin/env bash
# Git Bash wrapper for serial.ps1 (Windows serial ports are easiest from PowerShell).
#   bash tools/serial.sh               # auto-pick the only ST-LINK port
#   bash tools/serial.sh -List
#   bash tools/serial.sh -Port COM7
# Press Esc to quit.
exec powershell.exe -NoProfile -ExecutionPolicy Bypass \
  -File "$(cygpath -w "$(dirname "$0")/serial.ps1")" "$@"
