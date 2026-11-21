# Configure, build and run the unit tests on the PC (PowerShell version of test.sh).
#   .\tools\test.ps1
# MSYS2's gcc must come first in PATH (see test.sh); changed for this process only.
$ErrorActionPreference = 'Stop'
Set-Location (Split-Path -Parent $PSScriptRoot)

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) { . "$PSScriptRoot\env.ps1" | Out-Null }
$msys = 'C:\msys64\ucrt64\bin'
if (Test-Path "$msys\gcc.exe") { $env:Path = "$msys;$env:Path" }
if (-not (Get-Command gcc -ErrorAction SilentlyContinue)) { throw 'No host gcc found (install MSYS2 ucrt64 gcc)' }

cmake --preset host-tests;       if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build --preset host-tests; if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
ctest --preset host-tests;       exit $LASTEXITCODE
