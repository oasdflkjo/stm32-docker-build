# GitHub Actions Self-Hosted Runner on Orange Pi Zero3

This guide explains how to set up an Orange Pi Zero3 as a GitHub Actions runner for automated firmware flashing.

## Hardware Requirements

- Orange Pi Zero3
- Power supply (USB-C, 5V/2A recommended)
- MicroSD card (16GB+ recommended)
- ST-LINK programmer connected via USB
- Network connection (Ethernet or WiFi)

## Software Setup

### 1. Initial Ubuntu Server Setup

1. Download Ubuntu Server for Orange Pi Zero3
2. Flash the image to MicroSD card
3. Boot and perform initial setup:
   ```bash
   sudo apt update && sudo apt upgrade -y
   sudo apt install git make cmake gcc g++ python3 pip -y
   ```

### 2. Install ST-LINK Tools

```bash
# Install dependencies
sudo apt install libusb-1.0-0-dev

# Install ST-LINK tools
git clone https://github.com/stlink-org/stlink
cd stlink
cmake .
make
sudo make install
sudo ldconfig
```

### 3. Set up GitHub Actions Runner

1. On GitHub, go to your repository → Settings → Actions → Runners
2. Click "New self-hosted runner"
3. Select Linux as the operating system
4. Follow the provided instructions, which will look similar to:

```bash
# Create runner directory
mkdir actions-runner && cd actions-runner

# Download runner package
curl -o actions-runner-linux-arm64-2.xxx.x.tar.gz -L https://github.com/actions/runner/releases/download/v2.xxx.x/actions-runner-linux-arm64-2.xxx.x.tar.gz

# Extract
tar xzf ./actions-runner-linux-arm64-2.xxx.x.tar.gz

# Configure
./config.sh --url https://github.com/[YOUR_USERNAME]/[YOUR_REPO] --token [YOUR_TOKEN]

# Install and start service
sudo ./svc.sh install
sudo ./svc.sh start
```

### 4. USB Permissions

Grant USB access to the runner:

```bash
# Create udev rule for ST-LINK
echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666"' | sudo tee /etc/udev/rules.d/99-stlink.rules

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## GitHub Workflow Example

Create `.github/workflows/flash.yml` in your repository:

```yaml
name: Flash Firmware

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  flash:
    runs-on: self-hosted  # Use our Orange Pi runner
    
    steps:
    - uses: actions/checkout@v3
      
    - name: Build firmware
      run: |
        ./build.ps1
        
    - name: Flash firmware
      run: |
        st-flash --reset write build/firmware.bin 0x8000000

    - name: Verify flash
      run: |
        st-flash verify build/firmware.bin 0x8000000
```

## Maintenance

### Monitoring Runner Status

```bash
# Check runner service status
sudo systemctl status actions.runner.*

# View logs
tail -f ~/actions-runner/_diag/*.log
```

### Updating Runner

```bash
# Stop service
sudo ./svc.sh stop

# Update runner
./config.sh remove
# Download and install new version
# Reconfigure as shown in setup section

# Start service
sudo ./svc.sh start
```

## Security Considerations

1. The runner has access to:
   - USB devices (ST-LINK)
   - Network connectivity
   - Repository secrets

2. Recommendations:
   - Use repository-level runners rather than organization-level
   - Regularly update the runner software
   - Monitor runner logs for suspicious activity
   - Use secrets for sensitive configuration
   - Limit runner permissions to necessary operations only

## Troubleshooting

### Common Issues

1. ST-LINK not detected:
   ```bash
   # Check USB devices
   lsusb
   # Check ST-LINK presence
   st-info --probe
   ```

2. Runner offline:
   ```bash
   # Check network connectivity
   ping github.com
   # Check service status
   sudo systemctl status actions.runner.*
   ```

3. Flash verification failed:
   - Check if board is properly connected
   - Verify correct firmware file path
   - Ensure sufficient power supply

## Support

For issues related to:
- Orange Pi Zero3: [Orange Pi Forums](http://www.orangepi.org/orangepiforum/)
- GitHub Actions: [GitHub Community](https://github.community/)
- ST-LINK: [ST-LINK GitHub](https://github.com/stlink-org/stlink)