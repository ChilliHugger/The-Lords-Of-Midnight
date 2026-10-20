# Builds the Windows installer for one game from an existing Release build.
#   scripts/build_installer.ps1 -App midnight|revenge|citadel [-BuildDir <Release folder>]
# Needs Inno Setup 6 (iscc on PATH, or installed in the default location).
# The version comes from main/midnight/VERSION (see set_version.sh).
param(
    [Parameter(Mandatory)][ValidateSet('midnight', 'revenge', 'citadel')][string]$App,
    [string]$BuildDir
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent

$iscc = (Get-Command iscc -ErrorAction SilentlyContinue).Source
if (-not $iscc) { $iscc = "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe" }
if (-not (Test-Path $iscc)) { throw 'Inno Setup 6 not found (choco install innosetup)' }

if (-not $BuildDir) { $BuildDir = Join-Path $root "Builds\win-$App\bin\$App\Release" }
if (-not (Test-Path (Join-Path $BuildDir "$App.exe"))) { throw "No $App.exe in $BuildDir - build the Release version first" }

$version = (Get-Content (Join-Path $root 'VERSION')).Split(' ')[0]
& $iscc "/DApp=$App" "/DAppVersion=$version" "/DBuildDir=$((Resolve-Path $BuildDir).Path)" (Join-Path $root 'installer\game.iss')
if ($LASTEXITCODE -ne 0) { throw "iscc failed ($LASTEXITCODE)" }
Write-Host "Installer in installer\Output"
