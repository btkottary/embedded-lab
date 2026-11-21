# Minimal serial terminal for the ST-LINK virtual COM ports (115200 8N1).
#   .\tools\serial.ps1               # one ST-LINK plugged in: connects to it
#   .\tools\serial.ps1 -List         # show all COM ports
#   .\tools\serial.ps1 -Port COM7    # pick one
# Press Esc to quit. From Git Bash use tools/serial.sh (same options).
param([string]$Port, [int]$Baud = 115200, [switch]$List)

$ports = @(Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match '\((COM\d+)\)' } |
    ForEach-Object { [pscustomobject]@{ Port = ([regex]::Match($_.Name, 'COM\d+')).Value; Name = $_.Name } })
$stlink = @($ports | Where-Object { $_.Name -match 'STLink|STMicroelectronics' })

if ($List -or (-not $Port -and $stlink.Count -ne 1)) {
    $ports | Format-Table -AutoSize
    if (-not $List) {
        if ($stlink.Count -eq 0) { Write-Host 'No ST-LINK virtual COM port found - is a board plugged in?' -ForegroundColor Yellow }
        else { Write-Host 'Several ST-LINK ports: pick one with -Port COMx (unplug a board to see which is which).' -ForegroundColor Yellow }
    }
    return
}
if (-not $Port) { $Port = $stlink[0].Port }

$sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
$sp.Open()
Write-Host "Connected to $Port @ $Baud 8N1. Press Esc to quit." -ForegroundColor Green
try {
    while ($true) {
        if ($sp.BytesToRead -gt 0) { [Console]::Write($sp.ReadExisting()) }
        while ([Console]::KeyAvailable) {
            $k = [Console]::ReadKey($true)
            if ($k.Key -eq 'Escape') { return }
            if ($k.Key -eq 'Enter') { $sp.Write("`r") } else { $sp.Write([string]$k.KeyChar) }
        }
        Start-Sleep -Milliseconds 15
    }
}
finally {
    $sp.Close()
}
