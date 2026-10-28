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
        -i `
        $sourceFile.FullName

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

Write-Host "Formatted $($sourceFiles.Count) C++ files."