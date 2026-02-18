# Findings & Decisions

## Requirements
- Describe the current Raspberry Pi 5 development environment for Core2 (connected via USB) — port names, drivers, and any noted detection quirks.
- Confirm whether serial communication and programming from this Pi to the Core2 is usable.
- Provide step-by-step development instructions: dependencies, compile/upload commands (using `arduino-cli` or equivalent), and any verification steps.
- Capture any constraints (e.g., `ttyUSB0` only, voice/servo tasks disabled) that impact writing firmware on this machine.

## Research Findings
- `lsusb` shows a Silicon Labs CP210x UART bridge (ID 10c4:ea60) on Bus 003 Device 009, meaning the Core2 is accessible via USB-to-UART and appears as `/dev/ttyUSB0` on the Pi.
- No `/dev/ttyACM*` devices were present, so only the CP210x driver is currently relevant.
- `arduino-cli board list` reports `/dev/ttyUSB0` as a serial port: recognized but labeled “Unknown.” Installing the ESP32 index will allow compiling/uploading the Core2 firmware from this interface.
- The repository already contains the modified `M5Core2_SG90_StackChan_VoiceText_Ataru.ino` that serves as the Arduino sketch for the AI Extension.

## Technical Decisions
| Decision | Rationale |
|----------|-----------|
| Document the Pi-specific instructions inside a new `docs/` or top-level Markdown so it lives with the firmware | Developers working in this repo will expect setup guidance and will have the hardware on this same machine |
| Use `arduino-cli` commands (update-index, core install, compile, upload) in the doc instead of describing GUI IDE workflows | The Pi session is headless/CLI-first, and `arduino-cli` is already available in the environment |

## Issues Encountered
| Issue | Resolution |
|-------|------------|
|       |            |

## Resources
- `lsusb` output (see earlier command) indicates CP210x on Bus 003 Device 009 → `/dev/ttyUSB0`.
- `arduino-cli board list` output for `/dev/ttyUSB0` and `/dev/ttyAMA10` (serial ports discovered).
- Project README for reference: `README.md` in this repo describes the forked StackChan firmware.

## Visual/Browser Findings
-

---
*Update this file after every 2 view/browser/search operations*
*This prevents visual information from being lost*
