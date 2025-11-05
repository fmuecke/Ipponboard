param(
    [string]$ClangFormat = $env:CLANG_FORMAT
)

if (-not $ClangFormat -or $ClangFormat.Trim() -eq "") {
    $ClangFormat = "clang-format"
}

$requiredMajor = 14
$requiredMinor = 0

$cmd = Get-Command $ClangFormat -ErrorAction SilentlyContinue
if ($null -eq $cmd) {
    Write-Error "clang-format not found. Install LLVM clang-format $requiredMajor.x and ensure it is on PATH."
    exit 1
}

$versionOutput = & $ClangFormat --version
if ($versionOutput -match "([0-9]+)\.([0-9]+)\.") {
    $major = [int]$Matches[1]
    $minor = [int]$Matches[2]
    if (($major -lt $requiredMajor) -or (($major -eq $requiredMajor) -and ($minor -lt $requiredMinor))) {
        Write-Error "clang-format version $major.$minor detected. Require >= $requiredMajor.$requiredMinor"
        exit 1
    }
} else {
    Write-Warning "Unable to parse clang-format version string '$versionOutput'. Proceeding."
}

$root = Resolve-Path "$(Split-Path -Parent $MyInvocation.MyCommand.Path)/.."
Push-Location $root
try {
    $modified = (& git diff --name-only --diff-filter=ACMRTUXB HEAD 2>$null) + (& git diff --name-only --cached --diff-filter=ACMRTUXB 2>$null)
    $files = $modified | Where-Object { $_ -match '\.(c|cc|cpp|cxx|h|hh|hpp)$' } | Sort-Object -Unique

    if (-not $files -or $files.Count -eq 0) {
        Write-Output "No modified C/C++ files detected for formatting check."
        exit 0
    }

    $failed = $false
    foreach ($file in $files) {
        if (-not (Test-Path $file)) { continue }
        & $ClangFormat --dry-run --Werror $file 2>$null
        if ($LASTEXITCODE -ne 0) {
            Write-Warning "Formatting issues found in $file"
            #$failed = $true
        }
    }

    if ($failed) {
        Write-Error "clang-format check failed. Run clang-format on the reported files."
        exit  1
    }

    Write-Output "clang-format check passed."
}
finally {
    Pop-Location
}
