$compiler = "gcc"
$source = "*.c"
$output = "app.exe"

# Make sure this path points to the folder containing 'include' and 'lib'
$basePath = "C:/SDL3/SDL3-3.4.8/x86_64-w64-mingw32"

$cflags = "-I$basePath/include"
$ldflags = "-L$basePath/lib"
$libs = "-lSDL3"

Write-Host "Compiling $source..." -ForegroundColor Cyan
$command = "$compiler $source -o $output $cflags $ldflags $libs"
Invoke-Expression $command

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green
    if (-not (Test-Path "SDL3.dll")) {
        Copy-Item "$basePath/bin/SDL3.dll" -Destination "."
    }
    Write-Host "Launching $output..." -ForegroundColor Yellow
    & "./$output"
}
else {
    Write-Host "Build failed." -ForegroundColor Red
}
