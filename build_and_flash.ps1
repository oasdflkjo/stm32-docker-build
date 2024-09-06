# build_and_flash.ps1

# First, run the build process
Write-Host "Starting build process..." -ForegroundColor Cyan
& .\build.ps1

# Check if build was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed. Aborting flash process." -ForegroundColor Red
    exit 1
}

Write-Host "Build successful. Starting flash process..." -ForegroundColor Cyan

# Now, run the flash process
& .\flash.ps1

# Check if flash was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Flash process failed." -ForegroundColor Red
    exit 1
}

Write-Host "Build and flash processes completed successfully." -ForegroundColor Green