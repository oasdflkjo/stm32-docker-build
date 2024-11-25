
# Debugging STM32 in a Container

This guide explains how to set up hardware debugging for STM32 microcontrollers using VS Code, Docker containers, and OpenOCD.

## Prerequisites

- Windows with WSL2
- VS Code with Dev Containers extension
- ST-LINK/V2 debugger
- STM32 Nucleo board

## USB Passthrough Setup

1. Install usbipd on Windows (PowerShell as Administrator):
   ```powershell
   winget install usbipd
   ```

2. List available USB devices:
   ```powershell
   usbipd list
   ```

3. Attach ST-LINK to WSL:
   Replace `2-2` with your ST-LINK's `BUSID` from the list:
   ```powershell
   usbipd bind --busid 2-2
   usbipd attach --wsl --busid 2-2
   ```

## Container Configuration

### Dockerfile
```dockerfile
FROM ubuntu:24.04
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    gdb-multiarch \
    make \
    ccache \
    openocd \
    udev \
    stlink-tools \
    libusb-1.0-0 \
    usbutils \
    linux-tools-generic \
    hwdata \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/
WORKDIR /workspaces
```

### devcontainer.json
```json
{
    "name": "STM32 Development",
    "build": {
        "dockerfile": "../dockerfile",
        "context": ".."
    },
    "runArgs": [
        "--privileged"
    ],
    "mounts": [
        "source=/dev,target=/dev,type=bind"
    ],
    "customizations": {
        "vscode": {
            "extensions": [
                "marus25.cortex-debug",
                "ms-vscode.cpptools"
            ]
        }
    },
    "remoteUser": "root"
}
```

## Debug Configuration

### launch.json
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "OpenOCD Debug",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/main.elf",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32l1.cfg"
            ],
            "gdbPath": "/usr/bin/gdb-multiarch",
            "device": "STM32L152RE",
            "runToEntryPoint": "main"
        }
    ]
}
```

## Usage

1. Start VS Code and open your project in the container.
2. Verify ST-LINK connection:
   ```bash
   lsusb # Should show ST-LINK/V2
   openocd -f board/st_nucleo_l1.cfg # Should connect to the board
   ```

3. Build your project:
   ```bash
   make
   ```

4. Start debugging:
   - Set breakpoints in your code.
   - Press F5 to start debugging.
   - Use F10 to step over, F11 to step into.

## Notes

- USB passthrough must be re-established after win boot???
- The container must be run with `--privileged` for USB access???
