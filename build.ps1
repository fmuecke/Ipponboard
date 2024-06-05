function Init-Environment {
    & .\scripts\init_env_cfg.cmd 
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Read-Env-Cfg
    $global:BUILD_DIR="$IPPONBOARD_ROOT_DIR\_build\build-Ipponboard"
    $global:CONFIG="release"
}

# read settings from env_cfg.cmd file. The settings are in the format "set VAR=VALUE"
function Read-Env-Cfg {
	$env_cfg = Get-Content .\env_cfg.bat
	foreach ($line in $env_cfg) {
		if ($line -match '^set "(.*)=(.*)$"') {
            Set-Variable -Name $matches[1] -Value $matches[2] -Scope Global
		}
	}
}

function Show-Menu {
    $BIN_DIR="$IPPONBOARD_ROOT_DIR\_bin\$CONFIG"
    $TEST_BIN_DIR="$IPPONBOARD_ROOT_DIR\_bin\Test-$CONFIG"

    Clear-Host
    Write-Host ""
    Write-Host "Current config:  $CONFIG"
    Write-Host ""
    Write-Host "  QTDIR     : $QTDIR"
    Write-Host "  BOOST_DIR : $BOOST_DIR"
    Write-Host "  ROOT_DIR  : $IPPONBOARD_ROOT_DIR"
    Write-Host "  BUILD_DIR : $BUILD_DIR"
    Write-Host "  BIN_DIR   : $BIN_DIR"
    Write-Host "  INNO_DIR  : $INNO_DIR"
    Write-Host ""
    Write-Host "Select build mode:"
    Write-Host ""
    #Write-Host "  (1) Create makefiles"
    Write-Host "  (2) Build"
    Write-Host "  (7) Build doc"
    Write-Host "  (8) Build setup"
    Write-Host "  (c) Clean ALL"
    Write-Host "  (s) Switch build config (d/r)"
    Write-Host "  (t) Translate resources"
    Write-Host "  (q) Quit"
    Write-Host ""
}

function Main {
    Init-Environment

    do {
        Show-Menu
        $choice = [System.Console]::ReadKey($true).KeyChar
        switch ($choice) {
            #'1' { Create-makefiles }
            '2' { Build-ALL }
            '7' { Build-Doc }
            '8' { Build-Setup }
            'c' { Clean-Builds }
            's' { Switch-Config }
            't' { Translate-Resources }
            'q' { break }
            default { Write-Host "Invalid choice" }
        }
    } while ($choice -ne 'q')
}

function Clean-All {
    Remove-Directory -Path $BUILD_DIR -Recursive > $null
    Remove-Directory -Path $BIN_DIR -Recursive > $null
    Remove-Directory -Path $TEST_BIN_DIR -Recursive > $null
    pause
}

function Build-ALL {
    #& .\scripts\create-versioninfo.cmd $IPPONBOARD_ROOT_DIR\base   --> called in build-ALL.cmd script
    #if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & .\scripts\build-ALL.cmd $CONFIG
    #if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    pause
}

function Build-Doc {
	& .\scripts\build-doc.cmd
    if ($LASTEXITCODE -ne 0) { 
        Write-Error "build-doc returned with error" 
        pause
    }
}

function Build-Setup {
}

function Switch-Config {
    if ($global:CONFIG -eq "release") {
        $global:CONFIG = "debug"
    } else {
        $global:CONFIG = "release"
    }
}

function Translate-Resources {
	& .\scripts\translate.cmd
    pause

}


try {
    Main
} catch {
    Write-Host "An error occurred: $_"
    exit 1
}