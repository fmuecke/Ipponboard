function Check-cmake {
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if ($cmake -eq $null) {
        Write-Host "CMake not found. Please install CMake and make sure it is in the PATH."
        exit 1
    }
}

function Check-Ninja {
    $ninja = Get-Command ninja -ErrorAction SilentlyContinue
    if ($ninja -eq $null) {
        Write-Host "Ninja not found. Please install Ninja (https://ninja-build.org/) and make sure it is in the PATH."
        exit 1
    }
}

function Init-Environment {
    & .\scripts\init_env_cfg.cmd 
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Read-Env-Cfg
    $global:BUILD_DIR = "$IPPONBOARD_ROOT_DIR\_build\build-Ipponboard"
    $global:CONFIG = "release"
    $global:BIN_DIR = "$IPPONBOARD_ROOT_DIR\_bin\Ipponboard-$CONFIG"
    $global:TEST_BIN_DIR = "$IPPONBOARD_ROOT_DIR\_bin\Test-$CONFIG"
}

function Invoke-ClangFormatCheck {
    & "$PSScriptRoot\scripts\check-format.ps1"
    return ($LASTEXITCODE -eq 0)
}

# Read settings from env_cfg.cmd file. The settings are in the format "set VAR=VALUE"
function Read-Env-Cfg {
    $env_cfg = Get-Content .\env_cfg.bat
    foreach ($line in $env_cfg) {
        if ($line -match '^set "(.*)=(.*)"\s*$') {
            Set-Variable -Name $matches[1] -Value $matches[2] -Scope Global
            Write-Host "Setting $($matches[1]) to $($matches[2])"
        }
    }
}

function Show-Menu {
    Clear-Host

    $menu = @"

    Current config ($CONFIG):

        QTDIR     : $QTDIR
        BOOST_DIR : $BOOST_DIR
        ROOT_DIR  : $IPPONBOARD_ROOT_DIR
        BUILD_DIR : $BUILD_DIR
        BIN_DIR   : $BIN_DIR
        INNO_DIR  : $INNO_DIR

    Select build mode:

        (1) clean ALL
        (2) create makefiles
        (3) tests only
        (4) build all
        (5) run Ipponboard
        (6) build doc
        (7) translate resources
        (8) build setup
        (9) clean build with setup (release)
        (s) switch debug/release
        (q) quit
"@

    Write-Host $menu
}

# write a function that takes another function from this scruipt as a parameter and calls it and measures the time it took
function Execute-And-Measure {
    param (
        [scriptblock]$function
    )

    $start = Get-Date
    & $function
    $end = Get-Date
    $elapsed = $end - $start
    #$formattedTime = '{0:hh\:mm\:ss\.fff}' -f $elapsed
    Write-Host "Elapsed time: $([int]$elapsed.TotalSeconds)s"
    Pause
}

function MainLoop {
    Init-Environment

    do {
        Show-Menu
        $choice = [System.Console]::ReadKey($true).KeyChar
        switch ($choice) {
            '1' { Execute-And-Measure ${function:Clean-All} }
            '2' { Execute-And-Measure ${function:Create-Makefiles} }
            '3' { Execute-And-Measure ${function:Build-and-run-tests} }
            '4' { Execute-And-Measure ${function:Build-ALL} }
            '5' { Execute-And-Measure ${function:Run} }
            '6' { Execute-And-Measure ${function:Build-Doc} }
            '7' { Execute-And-Measure ${function:Translate-Resources} }
            '8' { Execute-And-Measure ${function:Build-Setup} }
            '9' { Execute-And-Measure ${function:CleanBuildWithSetup} }
            's' { Switch-Config }
            'q' { break }
            default { Write-Host "Invalid choice" }
        }
    } while ($choice -ne 'q')
}

function Clean-All {
    if (Test-Path $BUILD_DIR) {
        foreach ($cfg in @("Release", "Debug")) {
            Write-Host "Cleaning build outputs ($cfg) in $BUILD_DIR"
            cmake --build "$BUILD_DIR" --config $cfg --target clean | Out-Null
            if ($LASTEXITCODE -ne 0) {
                Write-Host "WARN: CMake clean failed for configuration $cfg (continuing)."
            }
        }
    }
    else {
        Write-Host "Build directory not found: $BUILD_DIR (skipping CMake clean)."
    }

    Write-Host "Removing binary output directories"
    $dirs = "$IPPONBOARD_ROOT_DIR\_bin", "$IPPONBOARD_ROOT_DIR\_output"
    foreach ($item in $dirs) {
        if (Test-Path $item) {
            Write-Host "  Removing $item"
            Remove-Item -Path $item -Recurse -Force -ErrorAction SilentlyContinue
        }
    }

    Write-Host "Removing versioninfo.h"
    Remove-Item -Path "$IPPONBOARD_ROOT_DIR\base\versioninfo.h" -ErrorAction SilentlyContinue
}

function Create-Makefiles {
    & .\scripts\create-versioninfo.cmd "$IPPONBOARD_ROOT_DIR\base"
    if ($LASTEXITCODE -ne 0) { return $false }

    cmake -S "$IPPONBOARD_ROOT_DIR" -B "$BUILD_DIR" -G "Ninja Multi-Config" --fresh
    if ($LASTEXITCODE -ne 0) { return $false }
}

function Run-Tests {
    $exe = "$TEST_BIN_DIR\IpponboardTest.exe"
    if (-not (Test-Path $exe)) {
        Write-Host "Test app not found: $exe"
        return $false
    }
    Set-Location $TEST_BIN_DIR
    $env:QT_QPA_PLATFORM = "offscreen"
    $env:QT_QPA_PLATFORM_PLUGIN_PATH = "$QTDIR\plugins\platforms"
    if (-not $env:QT_QPA_FONTDIR) {
        $env:QT_QPA_FONTDIR = "$env:WINDIR\Fonts"
    }
    & "$exe"
    $success = ($LASTEXITCODE -eq 0)

    if ($success) {
        $networkExe = "$TEST_BIN_DIR\IpponboardNetworkTest.exe"
        if (-not (Test-Path $networkExe)) {
            Write-Host "Network test app not found: $networkExe"
            $success = $false
        }
        else {
            & "$networkExe"
            $success = ($LASTEXITCODE -eq 0)
        }
    }

    Remove-Item Env:QT_QPA_PLATFORM -ErrorAction SilentlyContinue
    Remove-Item Env:QT_QPA_PLATFORM_PLUGIN_PATH -ErrorAction SilentlyContinue
    Remove-Item Env:QT_QPA_FONTDIR -ErrorAction SilentlyContinue
    Set-Location -Path $PSScriptRoot
    return $success
}

function Build-and-run-tests {
    if (-not (Invoke-ClangFormatCheck)) { return $false }
    cmake --build "$BUILD_DIR" --config $CONFIG --target IpponboardTest 
    if ($LASTEXITCODE -ne 0) { return $false }
    
    cmake --build "$BUILD_DIR" --config $CONFIG --target IpponboardNetworkTest 
    if ($LASTEXITCODE -ne 0) { return $false }

    $success = Run-Tests
    return $success
}

function Build-ALL {
    if (-not (Invoke-ClangFormatCheck)) { return $false }
    cmake --build "$BUILD_DIR" --config $CONFIG
    if ($LASTEXITCODE -ne 0) { return $false }
    
    $success = Run-Tests
    if (-not $success) { return $false }

    $success = Build-Doc
    return $success
}

function Run {
    $app = "$BIN_DIR\Ipponboard.exe"
    if (-not (Test-Path $app)) {
        Write-Host "Application not found: $app"
        return $false
    }
    Set-Location $BIN_DIR
    & "$app"
    $success = ($LASTEXITCODE -eq 0)
    Set-Location -Path $PSScriptRoot
    return $success
}

function CleanBuildWithSetup {
    Clean-All

    if ($CONFIG -ne "release") { Switch-Config }

    $success = Create-Makefiles
    if (-not $success) { return $false }

    $success = Build-ALL
    if (-not $success) { return $false }

    #Translate-Resources
    $success = Build-Setup
    return $success
}

function Build-Doc {
    & .\scripts\build-doc.cmd
    return ($LASTEXITCODE -eq 0)
}

function Build-Setup {
    & .\scripts\build-setup.cmd
    return ($LASTEXITCODE -eq 0)
}

function Switch-Config {
    if ($global:CONFIG -eq "release") {
        $global:CONFIG = "debug"
    }
    else {
        $global:CONFIG = "release"
    }

    $global:BIN_DIR = "$IPPONBOARD_ROOT_DIR\_bin\Ipponboard-$CONFIG"
    $global:TEST_BIN_DIR = "$IPPONBOARD_ROOT_DIR\_bin\Test-$CONFIG"
}

function Translate-Resources {
    & .\scripts\translate.cmd
    return ($LASTEXITCODE -eq 0)
}

# Main
try {
    Check-cmake
    Check-Ninja
    MainLoop
}
catch {
    Write-Host "An error occurred: $_"
    exit 1
}
