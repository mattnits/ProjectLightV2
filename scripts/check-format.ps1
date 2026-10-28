$ErrorActionPreference = "Stop"

$clangFormat = Get-Command clang-format -ErrorAction SilentlyContinue

if ($null -eq $clangFormat) {
    Write-Error "clang-format was not found on PATH."
    exit 1
}

$sourceFiles = Get-ChildItem `
    -Path .\engine, .\editor, .\sandbox, .\tests `
    -Recurse `
    -File `
    -Include *.cpp, *.h |
    Sort-Object FullName

foreach ($sourceFile in $sourceFiles) {
    & $clangFormat.Source `
        --style=file `
        --dry-run `
        --Werror `
        $sourceFile.FullName

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Formatting check failed. Run .\scripts\format.ps1 and stage the changes."
        exit $LASTEXITCODE
    }
}

Write-Host "Formatting check passed for $($sourceFiles.Count) C++ files."