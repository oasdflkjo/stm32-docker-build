# Define the Docker image name
$DOCKER_IMAGE = "stm32-build-env"

# Check if the Docker image exists
$imageExists = docker images -q $DOCKER_IMAGE

if (-not $imageExists) {
    Write-Host "Docker image not found. Please build the image first." -ForegroundColor Red
    exit 1
}

# Run the Docker container
Write-Host "Running Docker container..." -ForegroundColor Yellow
docker run --rm -it `
    --privileged `
    -v "${PWD}:/workspaces" `
    --device-cgroup-rule='c *:* rmw' `
    -v //./pipe/docker_engine:/var/run/docker.sock `
    $DOCKER_IMAGE bash 