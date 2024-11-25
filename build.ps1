param(
    [switch]$Debug
)

# Docker image name
$DOCKER_IMAGE = "stm32-build-env"

# Check if the Docker image exists
$imageExists = docker images -q $DOCKER_IMAGE

if (-not $imageExists) {
    Write-Host "Docker image not found. Building image..." -ForegroundColor Yellow
    docker build -t $DOCKER_IMAGE .
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to build Docker image." -ForegroundColor Red
        exit 1
    }
    Write-Host "Docker image built successfully." -ForegroundColor Green
}

if ($Debug) {
    # Start debug session
    Write-Host "Starting debug session..." -ForegroundColor Yellow
    
    # Get ST-Link device ID
    $stlink = Get-PnpDevice | Where-Object { $_.DeviceID -like "*VID_0483*PID_3752*" }
    if ($stlink) {
        Write-Host "Found ST-Link device: $($stlink.Name)" -ForegroundColor Green
    } else {
        Write-Host "ST-Link device not found!" -ForegroundColor Red
        exit 1
    }

    docker run --rm -it `
        --privileged `
        -v ${PWD}:/work `
        --device-cgroup-rule='c *:* rmw' `
        -v //./pipe/docker_engine:/var/run/docker.sock `
        -p 3333:3333 `
        -p 4444:4444 `
        $DOCKER_IMAGE bash
} else {
    # Normal build process
    Write-Host "Building the program..." -ForegroundColor Yellow
    docker run --rm -v ${PWD}:/work $DOCKER_IMAGE make CFLAGS="-g -O0"

    if ($LASTEXITCODE -eq 0) {
        Write-Host "Program built successfully." -ForegroundColor Green
    } else {
        Write-Host "Program build failed." -ForegroundColor Red
        exit 1
    }

    # Check if the output files exist
    $elfPath = "build\main.elf"
    $hexPath = "build\main.hex"
    $binPath = "build\main.bin"

    if ((Test-Path $elfPath) -and (Test-Path $hexPath) -and (Test-Path $binPath)) {
        Write-Host "Output files generated successfully:" -ForegroundColor Green
        Write-Host "  - $elfPath" -ForegroundColor Green
        Write-Host "  - $hexPath" -ForegroundColor Green
        Write-Host "  - $binPath" -ForegroundColor Green
    } else {
        Write-Host "Some output files are missing:" -ForegroundColor Yellow
        if (-not (Test-Path $elfPath)) { Write-Host "  - $elfPath is missing" -ForegroundColor Yellow }
        if (-not (Test-Path $hexPath)) { Write-Host "  - $hexPath is missing" -ForegroundColor Yellow }
        if (-not (Test-Path $binPath)) { Write-Host "  - $binPath is missing" -ForegroundColor Yellow }
    }
}