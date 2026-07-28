$rootPath = Split-Path $PSScriptRoot -Parent
$uiPath = Join-Path $rootPath "ui"

Start-Process wt.exe `
    -ArgumentList @(
        "-d", $uiPath,
        "powershell.exe",
        "-NoExit",
        "-Command",
        "npm run dev"
    )