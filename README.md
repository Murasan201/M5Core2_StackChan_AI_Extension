# StackChan Core2 JSON Interface

This repository builds on the original `M5Core2_SG90_StackChan_VoiceText_Ataru` sketch and shifts the project toward a Raspberry Pi-controlled workflow. Instead of relying on the button-driven experience, the Core2 firmware now exposes a newline-delimited JSON protocol over `/dev/ttyUSB0` and the Pi sends expressions, speech bubble text, and duration commands through the companion Python CLI.

## Key features

- **Serial JSON parser** in `M5Core2_SG90_StackChan_VoiceText_Ataru.ino` that interprets `expression`, `speech`, `palette`, `duration`, and `clear` fields and always uses the default StackChan face (`faces[2]`).
- **Python CLI (`control_stackchan.py`)** that encodes CLI flags into JSON, opens `/dev/ttyUSB0` at 115200 bps, waits for the `OK` handshake, and reports a failure when `ERR` or a timeout occurs.
- **Documentation suite** covering development setup, interface requirements, testing rituals, and troubleshooting notes (see `docs/README.md` for the doc index).
- **TDD logging** under `docs/tests.md` plus a running `docs/journal.md` that summarizes what was attempted and what worked on each day.

## Getting started

1. **Install toolchain & libs** (per `docs/development-environment.md`):
   ```sh
   arduino-cli core update-index
   arduino-cli core install esp32:esp32@3.3.7
   arduino-cli lib install "M5Unified" "ServoEasing" "ESP32Servo" "ESP8266Audio" "ArduinoJson"
   # Copy / sync `m5stack-avatar` sources into ~/Arduino/libraries/M5Stack-Avatar
   ```
2. **Build & upload** the firmware:
   ```sh
   arduino-cli compile --fqbn esp32:esp32:m5stack_core2 M5Core2_SG90_StackChan_VoiceText_Ataru/M5Core2_SG90_StackChan_VoiceText_Ataru.ino
   arduino-cli upload --fqbn esp32:esp32:m5stack_core2 --port /dev/ttyUSB0 M5Core2_SG90_StackChan_VoiceText_Ataru/M5Core2_SG90_StackChan_VoiceText_Ataru.ino
   ```
3. **Use the CLI**:
   ```sh
   python3 control_stackchan.py \
     --expression Happy \
     --speech "Hello StackChan" \
     --palette 2 \
     --duration 3000
   ```
   The script waits for `OK` before exiting; any timeout or `ERR` is surfaced so the caller can re-run tests.
4. **Verify** the display, expressions, and bubble timing manually, then log the outcome in `docs/tests.md` and update `docs/journal.md`.

## Documentation

The `docs/` folder contains the development guide, interface requirements, working journal, troubleshooting log, and test matrix. A Japanese-language index lives at `docs/README.md` for quick navigation.