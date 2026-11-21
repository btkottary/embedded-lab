# Puts the command-line tools that ship inside STM32CubeIDE (GCC, CMake, Ninja,
# Make, GDB, ST-LINK GDB server) and STM32CubeProgrammer on PATH for THIS
# PowerShell session only. The IDE itself is never started.
#
# Usage (note the leading dot = "dot-source", so PATH changes stick):
#   . .\tools\env.ps1
param([string]$CubeIde = 'C:\ST\STM32CubeIDE_2.0.0\STM32CubeIDE')

$plugins = Join-Path $CubeIde 'plugins'
if (-not (Test-Path $plugins)) { throw "STM32CubeIDE plugins folder not found: $plugins (pass -CubeIde <path>)" }

# Plugin folder names carry version numbers, so match by prefix and take the newest.
function Get-PluginBin($pattern) {
    $p = Get-ChildItem $plugins -Directory -Filter $pattern | Sort-Object Name | Select-Object -Last 1
    if ($p) { Join-Path $p.FullName 'tools\bin' } else { Write-Warning "no $pattern in $plugins" }
}
$front = @(
    (Get-PluginBin 'com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*'),
    (Get-PluginBin 'com.st.stm32cube.ide.mcu.externaltools.cmake.*'),
    (Get-PluginBin 'com.st.stm32cube.ide.mcu.externaltools.ninja.*'),
    (Get-PluginBin 'com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.*'),
    'C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin'
) | Where-Object { $_ }
foreach ($d in $front) {
    if (($env:Path -split ';') -notcontains $d) { $env:Path = "$d;$env:Path" }
}
# The make plugin also ships busybox bash/sh/rm/mkdir/echo: append it LAST so
# it never shadows the real ones (we build with Ninja anyway).
$make = Get-PluginBin 'com.st.stm32cube.ide.mcu.externaltools.make.*'
if ($make -and (($env:Path -split ';') -notcontains $make)) { $env:Path = "$env:Path;$make" }

Write-Host 'Tools on PATH for this session:' -ForegroundColor Green
foreach ($t in 'arm-none-eabi-gcc', 'cmake', 'ninja', 'make', 'arm-none-eabi-gdb', 'ST-LINK_gdbserver', 'STM32_Programmer_CLI', 'STM32_SigningTool_CLI') {
    $c = Get-Command $t -ErrorAction SilentlyContinue
    if ($c) { Write-Host ("  {0,-22} {1}" -f $t, $c.Source) } else { Write-Host ("  {0,-22} MISSING" -f $t) -ForegroundColor Red }
}
