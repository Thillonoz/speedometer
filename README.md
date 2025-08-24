# Team A
# Speedometer Project

This project implements a speedometer system with both **Client** and **Server GUIs**, supporting multiple communication methods and platforms.  
It can be run on desktop machines or ESP devices.

## Features
- Client GUI and Server GUI
- Communication modes:
  - **TCP/IP**
  - **UART**
  - **BLE** (ESP ↔ ESP)
- Supported configurations:
  - Desktop ↔ Desktop
  - Desktop ↔ ESP
  - ESP ↔ ESP (BLE)

---

## Building & Running

### Desktop (TCP/IP or UART)
```bash
# Create build folder
mkdir build
cd build

# Generate build files
cmake ..

# Build for TCP/IP
make use_tcpip

# Build for UART
make use_uart

# From build folder
make client server

# Upload to devices
make upload_client
make upload_server