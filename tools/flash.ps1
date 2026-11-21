# Flash a built app over the board's ST-LINK (PowerShell version of flash.sh).
#   .\tools\flash.ps1 -Board h7s78_dk [-App blink]
#   .\tools\flash.ps1 -Board n6570_dk [-App blink]
param(
    [Parameter(Mandatory)][ValidateSet('h7s78_dk', 'n6570_dk')][string]$Board,
    [string]$App = 'blink',
    [switch]$NoPrompt
)
$ErrorActionPreference = 'Stop'
$root  = Split-Path -Parent $PSScriptRoot
$out   = Join-Path $root "build\$Board\apps\$App"
$cpBin = 'C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin'
$cli   = Join-Path $cpBin 'STM32_Programmer_CLI.exe'

if ($Board -eq 'h7s78_dk') {
    $elf = Join-Path $out "$App.elf"
    if (-not (Test-Path $elf)) { throw "$elf not found - build first: cmake --build --preset h7s78_dk" }
    & $cli -c port=swd mode=UR reset=HWrst -d $elf -v -rst
    if ($LASTEXITCODE -ne 0) { throw 'flashing failed' }
}
else {
    $img    = Join-Path $out "$App-trusted.bin"
    $loader = Join-Path $cpBin 'ExternalLoader\MX66UW1G45G_STM32N6570-DK.stldr'
    if (-not (Test-Path $img)) { throw "$img not found - build first: cmake --build --preset n6570_dk" }
    Write-Host 'N6570-DK: unplug CN6 -> SW1/BOOT1 = H, SW2/BOOT0 = L -> plug CN6 back in.' -ForegroundColor Yellow
    if (-not $NoPrompt) { [void](Read-Host 'Press Enter when done') }
    & $cli -c port=swd mode=HOTPLUG ap=1 -el $loader -d $img 0x70000000 -v
    if ($LASTEXITCODE -ne 0) { throw 'flashing failed' }
    Write-Host 'Flashed. To run: unplug CN6 -> SW1/BOOT1 = L -> plug CN6 back in.' -ForegroundColor Green
}
