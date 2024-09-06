# flash.ps1

# Path to the ST-LINK CLI executable
$ST_LINK_CLI = "C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"

# Path to the hex file
$HEX_FILE = "build\main.hex"

# Check if the hex file exists
if (-not (Test-Path $HEX_FILE)) {
    Write-Host "Error: $HEX_FILE not found." -ForegroundColor Red
    exit 1
}

# Construct the command
$command = "& `"$ST_LINK_CLI`" -c SWD -P `"$HEX_FILE`" 0x08000000 -Rst"

# Echo the command being executed
Write-Host "Executing: $command" -ForegroundColor Yellow

# Execute the command
try {
    Invoke-Expression $command
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Flashing complete." -ForegroundColor Green
    } else {
        Write-Host "Flashing failed with exit code $LASTEXITCODE" -ForegroundColor Red
    }
} catch {
    Write-Host "An error occurred: $_" -ForegroundColor Red
}