$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

$Input = Join-Path $ScriptDir "logo.png"
$OutputDir = Join-Path $ScriptDir "..\resources"
$Output = Join-Path $OutputDir "app.ico"

if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

magick `
    $Input `
    -trim +repage `
    -resize 256x256 `
    -gravity center `
    -background none `
    -extent 256x256 `
    -define icon:auto-resize=256,128,64,48,40,32,24,20,16 `
    $Output

Write-Host ""
Write-Host "? Icon generated:" -ForegroundColor Green
Write-Host $Output -ForegroundColor Cyan