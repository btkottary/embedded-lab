#!/usr/bin/env bash
# Configure, build and run the unit tests on the PC.
#   bash tools/test.sh
#
# On Windows the host compiler is MSYS2's gcc (C:\msys64\ucrt64). It must come
# BEFORE Git's own mingw64 folder in PATH, or it loads Git's older DLLs and
# fails silently - so PATH is changed here, only for this script.
set -euo pipefail
cd "$(dirname "$0")/.."

if [ -f tools/env.sh ] && ! command -v cmake >/dev/null 2>&1; then
  # shellcheck disable=SC1091
  source tools/env.sh >/dev/null
fi
if [ -x /c/msys64/ucrt64/bin/gcc.exe ]; then
  PATH="/c/msys64/ucrt64/bin:$PATH"
fi
command -v gcc >/dev/null || { echo "No host gcc found (install MSYS2 ucrt64 gcc)"; exit 1; }

cmake --preset host-tests
cmake --build --preset host-tests
ctest --preset host-tests
