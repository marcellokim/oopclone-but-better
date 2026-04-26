param(
    [switch]$Test,
    [switch]$Install,
    [switch]$NoRun,
    [switch]$Debug
)

$ErrorActionPreference = "Stop"

$RootDir = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildDir = if ($env:TERRITORY_WAR_BUILD_DIR) { $env:TERRITORY_WAR_BUILD_DIR } else { Join-Path $RootDir "build" }
$Config = if ($Debug) { "Debug" } elseif ($env:TERRITORY_WAR_CONFIG) { $env:TERRITORY_WAR_CONFIG } else { "Release" }

function Require-Command($Name) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "Missing required command: $Name. Install CMake and a C++20 compiler, then run this script again."
    }
}

function Find-TerritoryWarExecutable {
    $Candidates = @(
        (Join-Path $BuildDir "territory_war.exe"),
        (Join-Path $BuildDir "$Config/territory_war.exe"),
        (Join-Path $BuildDir "bin/territory_war.exe"),
        (Join-Path $BuildDir "bin/$Config/territory_war.exe")
    )

    foreach ($Candidate in $Candidates) {
        if (Test-Path $Candidate) {
            return $Candidate
        }
    }

    return $null
}

Require-Command cmake

Write-Host "Project: $RootDir"
Write-Host "Build:   $BuildDir"
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

if ($Install) {
    $InstallDir = Join-Path $RootDir "dist/territory-war"
    cmake --install $BuildDir --config $Config --prefix $InstallDir
    Write-Host "Installed local copy: $InstallDir"
}

if (-not $NoRun) {
    $Executable = Find-TerritoryWarExecutable
    if (-not $Executable) {
        throw "Build completed, but territory_war.exe was not found in $BuildDir."
    }

    Write-Host "Launching: $Executable"
    Push-Location $RootDir
    try {
        & $Executable
    } finally {
        Pop-Location
    }
}
