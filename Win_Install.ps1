$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$exePath = Join-Path $scriptDir "spotify-pauser.exe"
$shortcutPath = "$env:APPDATA\Microsoft\Windows\Start Menu\Programs\Startup\spotify-pauser.lnk"

$WScriptShell = New-Object -ComObject WScript.Shell
$shortcut = $WScriptShell.CreateShortcut($shortcutPath)
$shortcut.TargetPath = $exePath
$shortcut.WorkingDirectory = $scriptDir
$shortcut.Save()

Start-Process $exePath
