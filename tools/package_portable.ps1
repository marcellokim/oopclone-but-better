param(
    [switch]$Test,
    [switch]$Debug
)

$ErrorActionPreference = "Stop"

$RootDir = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildDir = if ($env:TERRITORY_WAR_PACKAGE_BUILD_DIR) { $env:TERRITORY_WAR_PACKAGE_BUILD_DIR } else { Join-Path $RootDir "build-package" }
$DistDir = Join-Path $RootDir "dist"
$Config = if ($Debug) { "Debug" } elseif ($env:TERRITORY_WAR_CONFIG) { $env:TERRITORY_WAR_CONFIG } else { "Release" }

function Require-Command($Name) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "Missing required command: $Name"
    }
}

function Get-PlatformName {
    if ($env:OS -eq "Windows_NT") {
        return "Windows"
    }
    if ((Get-Variable IsWindows -ErrorAction SilentlyContinue) -and $IsWindows) {
        return "Windows"
    }
    if ((Get-Variable IsMacOS -ErrorAction SilentlyContinue) -and $IsMacOS) {
        return "macOS"
    }
    if ((Get-Variable IsLinux -ErrorAction SilentlyContinue) -and $IsLinux) {
        return "Linux"
    }
    return "UnknownOS"
}

function Get-ArchName {
    $Arch = if ($env:PROCESSOR_ARCHITEW6432) {
        $env:PROCESSOR_ARCHITEW6432
    } elseif ($env:PROCESSOR_ARCHITECTURE) {
        $env:PROCESSOR_ARCHITECTURE
    } else {
        try {
            [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString()
        } catch {
            "unknown"
        }
    }

    switch ($Arch) {
        "X64" { return "x64" }
        "AMD64" { return "x64" }
        "Arm64" { return "arm64" }
        "ARM64" { return "arm64" }
        "X86" { return "x86" }
        "x86" { return "x86" }
        default { return $Arch.ToLowerInvariant() }
    }
}

function Write-PackageLauncher($StageDir) {
    $PlayPs1 = Join-Path $StageDir "Play.ps1"
    $PlayBat = Join-Path $StageDir "Play.bat"

    @'
$ErrorActionPreference = "Stop"

$AppDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Exe = Join-Path $AppDir "bin\territory_war.exe"

if (-not (Test-Path $Exe)) {
    throw "territory_war.exe was not found next to this launcher."
}

Push-Location $AppDir
try {
    & $Exe
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
'@ | Set-Content -Path $PlayPs1 -Encoding ASCII

    @'
@echo off
set "APP_DIR=%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%APP_DIR%Play.ps1" %*
'@ | Set-Content -Path $PlayBat -Encoding ASCII
}

function New-PortableZip($StageDir, $ArchivePath) {
    if (Test-Path $ArchivePath) {
        Remove-Item $ArchivePath -Force
    }

    Push-Location (Split-Path -Parent $StageDir)
    try {
        Compress-Archive -Path (Split-Path -Leaf $StageDir) -DestinationPath $ArchivePath -Force
    } finally {
        Pop-Location
    }
}

Require-Command cmake

$Platform = Get-PlatformName
$Arch = Get-ArchName
$PackageBasename = "TerritoryWar-$Platform-$Arch"
$StageDir = Join-Path $DistDir $PackageBasename
$ArchivePath = Join-Path $DistDir "$PackageBasename.zip"

Write-Host "Project: $RootDir"
Write-Host "Build:   $BuildDir"
Write-Host "Package: $ArchivePath"
Write-Host "Config:  $Config"

cmake -S $RootDir -B $BuildDir -DCMAKE_BUILD_TYPE=$Config

$Targets = @("territory_war")
if ($Test) {
    $Targets += "territory_war_tests"
}

cmake --build $BuildDir --config $Config --target $Targets

if ($Test) {
    ctest --test-dir $BuildDir --build-config $Config --output-on-failure
}

if (Test-Path $StageDir) {
    Remove-Item $StageDir -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $DistDir | Out-Null

cmake --install $BuildDir --config $Config --prefix $StageDir
Write-PackageLauncher $StageDir
New-PortableZip $StageDir $ArchivePath

Write-Host "Ready:"
Write-Host "  Folder: $StageDir"
Write-Host "  ZIP:    $ArchivePath"
