# Findings & Decisions

## Requirements
- Replace the button-driven avatar actions with a terminal-controlled interface that lets a Raspberry Pi 5 script set StackChan expressions and speech bubbles via the USB serial link.
- The Pi script should be able to choose `Expression` values, toggle the speech bubble text (including clearing it), and optionally switch the face/palette without requiring physical interaction.
- The interface must surface feedback (e.g., `OK`/`ERR`) so the script knows the Core2 command succeeded.
- Document the full command format, necessary Core2 APIs, and host-side expectations so future development can implement the parser.

## Research Findings
- Hardware link: `lsusb` reports the Core2 via Silicon Labs `10c4:ea60 CP210x UART Bridge`; Linux exposes `/dev/ttyUSB0` (no `/dev/ttyACM*`). `arduino-cli board list` sees `/dev/ttyUSB0` as a „Serial Port" (board name `Unknown`).
- The `m5avatar` library exposes the `Expression` enum with values `Happy`, `Angry`, `Sad`, `Doubt`, `Sleepy`, `Neutral` (`m5stack-avatar/src/Expression.h`). These are the canonical states we will surface.
- Avatar APIs available in `src/Avatar.h`: `setExpression(Expression)`, `getExpression()`, `setSpeechText(const char*)`, `setSpeechFont(...)`, `setFace(Face*)`, `setColorPalette(ColorPalette)`, `setMouthOpenRatio(float)`, etc. We will leverage these to apply commands received from the Pi script.
- Current `loop()` branches on `M5.BtnA/B/C.wasPressed()` to trigger voice/expressions/balloons. We will remove these and instead keep `M5.update()` plus a serial parser.
- Serial port uses `115200` baud (driven by `M5.begin()`); on the Pi side we can open this device using `python` or a CLI utility.

## Technical Decisions
| Decision | Rationale |
|----------|-----------|
| Document the requirements and interface in `docs/interface-requirements.md` | Provides a reusable reference for the upcoming firmware/script work without prematurely altering the sketch |
| Define a newline-delimited JSON command protocol | Simple to parse on the ESP32, flexible, and easy for the Pi script to emit via standard JSON libraries |
| Maintain `avatar.setSpeechFont(&fonts::lgfxJapanGothic_12)` and the existing palettes | Keeps the visual quality consistent while allowing the new commands to swap faces/palettes on demand |

## Issues Encountered
| Issue | Resolution |
|-------|------------|
| None yet | N/A |

## Resources
- `/tmp/m5stack-avatar/src/Expression.h` (expression enum reference)
- `/tmp/m5stack-avatar/src/Avatar.h` (API surface used by this firmware)
- `docs/development-environment.md` (previous environment doc for the Pi, already committed)
- Raspberry Pi serial info: `lsusb` and `arduino-cli board list` commands executed earlier (records stored in `progress.md`).

## Visual/Browser Findings
- Not applicable for this task.

---
*Update this file after every 2 view/browser/search operations*
*This prevents visual information from being lost*
