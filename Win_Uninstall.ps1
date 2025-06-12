$shortcutPath = "$env:APPDATA\Microsoft\Windows\Start Menu\Programs\Startup\spotify-pauser.lnk"

if (Test-Path $shortcutPath) {
    Remove-Item $shortcutPath -Force
    Write-Output "Startup shortcut removed."
} else {
    Write-Output "Startup shortcut not found."
}

$processName = "spotify-pauser"

$runningProcesses = Get-Process -Name $processName -ErrorAction SilentlyContinue
if ($runningProcesses) {
    $runningProcesses | ForEach-Object { $_.Kill() }
    Write-Output "Running $processName processes terminated."
} else {
    Write-Output "No running $processName processes found."
}
