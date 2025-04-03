# FPGA-Based Oscilloscope Project

This project implements a simple digital oscilloscope on an FPGA using the Xilinx MicroBlaze soft processor. The oscilloscope is capable of signal acquisition, FFT analysis, touch screen interaction, and visual waveform display on a TFT screen.

## Features

- **Signal Sampling** using FIFO buffers
- **Real-Time FFT (Fast Fourier Transform)**
- **TFT Display** output for waveform visualization
- **Touch Screen Support** for interactive controls (scaling, levels)
- **MicroSD Card Support** via SPI for data logging
- **Frequency and Voltage Level Adjustment**
- **Modular Design** (with separate directories for each functional block)

## Directory Structure

| Folder/File            | Description                                 |
|------------------------|---------------------------------------------|
| `CTIIC/`               | I2C communication support                   |
| `MicroSD/`             | MicroSD card access via SPI + FAT16         |
| `keys/`                | Key/button handling                         |
| `osci/`                | Oscilloscope signal sampling and FFT        |
| `tft/`                 | TFT LCD display control                     |
| `touch/`               | Touchscreen driver                          |
| `helloworld.c`         | Main application logic and demo             |
| `lscript.ld`           | Linker script for memory layout             |
| `platform.c/h`         | Platform initialization and configuration   |
| `platform_config.h`    | Hardware platform-specific definitions      |

## Requirements

- **Xilinx FPGA Board** with MicroBlaze soft core
- **TFT Display**
- **Touch Screen Module**
- **MicroSD Card Slot**
- **Xilinx SDK or Vitis**
- **Vivado Design Suite**

## How It Works

1. **Platform Initialization:** Sets up interrupt controller, timer, GPIOs, and platform drivers.
2. **Signal Capture:** Periodic timer ISR triggers sampling logic and FFT computation.
3. **Touch UI:** Touch events change display scales (time/div, volts/div) and signal levels.
4. **Display Output:** Waveforms and FFT data are rendered on a TFT screen.
5. **MicroSD Logging (Optional):** Supports saving data to an SD card using FAT16.

## Touchscreen Controls

| Gesture / Region Tap  | Action                    |
|------------------------|---------------------------|
| Tap upper Y-scale      | Increase vertical scale   |
| Tap lower Y-scale      | Decrease vertical scale   |
| Tap upper X-scale      | Increase horizontal scale |
| Tap lower X-scale      | Decrease horizontal scale |
| Tap level up/down      | Adjust signal level       |
