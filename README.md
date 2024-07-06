# The Embedoors: Smart Door System

## Overview

The Embedoors is a Smart Door System designed to assist individuals with mobility challenges. This system allows users to unlock and open doors using RFID and Motion Sensor technology, providing a seamless and accessible entry solution.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Usage](#usage)
- [System Architecture](#system-architecture)
- [Troubleshooting](#troubleshooting)

## Features
- Unlock and open doors using RFID and Motion Sensor
- Web interface for managing RFID permissions and access logs
- Auditory feedback with a buzzer during RFID interactions
- Utilizes BeagleBone Green and ZenCape Red for hardware integration

## Hardware Requirements
- BeagleBone Green
- ZenCape Red
- Servo Motor
- Motion Sensor
- RFID Scanner
- Buzzer
- Power supply
- Connecting wires

## Software Requirements
- C Compiler
- CMake
- Node.js
- PRU Software Support Package
- Additional libraries for hardware components

## Installation

### Hardware Setup
1. Connect the RFID Scanner, Motion Sensor, Servo Motor, and Buzzer to the BeagleBone Green with ZenCape Red.
2. Ensure all connections are secure and powered appropriately.

## Usage
1. Compile the C code for the PRU:
    ```sh
    make all
    ```
2. Upload the compiled code to the PRU.
3. Start the Node.js server for the web interface:
    ```sh
    node server.js
    ```

## System Architecture
- **RFID and Motion Sensor**: Detects authorized users and movement near the door.
- **Servo Motor**: Controls the door locking and unlocking mechanism.
- **Buzzer**: Provides auditory feedback during RFID interactions.
- **BeagleBone Green and ZenCape Red**: Main control unit managing all hardware components.
- **Node.js Web Interface**: Manages RFID permissions and logs access.

## Troubleshooting
- **RFID not scanning**: Check the connections and ensure the RFID scanner is properly powered.
- **Servo Motor not responding**: Verify the connections and ensure the motor is receiving the correct signals.
- **Buzzer not sounding**: Ensure the buzzer is correctly connected and functioning.
- **Web interface not loading**: Check if the Node.js server is running and accessible.
