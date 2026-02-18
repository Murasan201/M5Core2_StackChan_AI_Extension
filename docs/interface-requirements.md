# StackChan Expression Interface Requirements

## 1. Background & scope
- The M5Stack Core2 (`StackChan` build) is wired to this Raspberry Pi 5 via USB. Linux sees it through the Silicon Labs CP210x bridge as `/dev/ttyUSB0` and is accessible by `arduino-cli` (see `lsusb` / `arduino-cli board list`).
- We are replacing the button-driven behavior inside `M5Core2_SG90_StackChan_VoiceText_Ataru.ino` with a terminal-driven control path: a Pi-side script will send expression/balloon requests over the serial link, and the Core2 firmware will apply them via the `m5avatar::Avatar` API.
- Existing button handlers (A/B/C) that triggered VoiceText/speech/expressions can be removed because the Pi script will perform all expression updates and bubble text. This simplifies the Core2 loop to a serial command parser plus the shared avatar drawing/task loop.

## 2. Functional requirements
1. **Expression control** – The Pi script must set `Avatar` expressions (e.g., Happy, Angry, Sad, Doubt, Sleepy, Neutral) programmatically without interacting with buttons.
2. **Speech bubble text** – The Pi script must be able to update the displayed dialogue (`avatar.setSpeechText`) and optionally clear it; the Core2 firmware must honor a `speech` string and duration/clear instruction.
3. **Face/palette selection (optional)** – Allow the Pi script to choose which face/palette pair (Aataru/Lam/StackChan) is active when the expression is displayed.
4. **Heartbeat/ack** – The serial exchange should yield a simple acknowledgement (`OK`/`ERR`) so higher-level orchestrators know when a command finished.
5. **No button dependencies** – After the interface is in place, the main loop should continue calling `M5.update()` but no longer gate avatar updates on button presses.

## 3. Protocol definition (Pi → Core2)
- Use newline-delimited JSON to keep parsing simple on the Core2 (ESP32) side. Each line contains a JSON object with optional fields.
- Example command:
  ```json
  {"expression":"Happy","speech":"こんにちは！","face":2,"duration":4000,"clear":true}
  ```
- Supported keys:
  | Key | Description |
  | --- | ----------- |
  | `expression` | One of `Happy`, `Angry`, `Sad`, `Doubt`, `Sleepy`, `Neutral` (see section 4). |
  | `speech` | UTF-8 string shown in the speech bubble via `avatar.setSpeechText()`. An empty string clears text when `clear` is true. |
  | `face` | Integer index (0/1/2) selecting `AtaruFace`, `RamFace`, or the default `StackChan` face. |
  | `palette` | Optional palette id to apply via `avatar.setColorPalette(*cps[palette])`. 0/1/2 map to existing palettes. |
  | `duration` | Milliseconds to keep the speech bubble visible before automatically clearing it (Core2 side handles the delay). |
  | `clear` | Boolean indicating whether to forcibly clear the speech text immediately. |
- Core2 responses can be simple ASCII strings such as `OK
` or `ERR <message>
` to let the script know the command parsed.

## 4. Parser and protocol design

### Compatibility policy
- We do **not** preserve compatibility with the existing button-driven sketch. Functionality and performance take priority, so rewrites and refactors are acceptable.

### Core2-side parser
- `loop()` continues to call `M5.update()` but now also checks `Serial.available()` at 115200 bps. Incoming lines (`Serial.readStringUntil('\n')`) are parsed as JSON using ArduinoJson (or a tailored parser).
- Supported fields: `expression`, `speech`, `face`, `palette`, `duration`, `clear`. Expressions map to `m5avatar::Expression` enum (`Happy`, `Angry`, `Sad`, `Doubt`, `Sleepy`, `Neutral`). Face/palette indices select the prebuilt `faces[0..2]`/`cps[0..2]` entries initialized in `setup()`.
- Handler logic: on parsed command, call `avatar.setExpression(...)`, `avatar.setFace(...)`, `avatar.setColorPalette(...)`, `avatar.setSpeechText(...)`, update `avatar.setMouthOpenRatio(...)` if desired, and start/reset a timer for `duration`. If `clear` is true or `duration` elapses, clear the speech text.
- Respond over serial with `OK` or `ERR <message>` so the Pi script can know when the command completes. Additional debugging logs (`Serial.printf("CMD parsed: %s\n", line.c_str());`) help trace issues.
- Button handlers (`M5.BtnA/B/C`) and associated audio/dialogue sequences can be removed entirely.

### Pi-side CLI
- A Python script (e.g., `control_stackchan.py`) opens `/dev/ttyUSB0` at 115200 bps and writes newline-delimited JSON commands built from CLI arguments (`--expression`, `--speech`, `--face`, `--palette`, `--duration`, `--clear`).
- After sending each command, the script waits (1 s timeout) for an `OK`/`ERR` response, retrying or reporting failure on timeout.
- Example usage:
  ```sh
  python control_stackchan.py \
    --expression Happy \
    --speech "こんにちは" \
    --face 2 \
    --duration 4000
  ```
- The script can accept macros (batch files or JSON command sequences) for scripted interactions and can expose `--clear` or `--voice-text` flags if the firmware supports them later.

## 5. Core2-side implementation notes
- The firmware already initializes `Avatar avatar;`, loads three faces (`AtaruFace`, `RamFace`, `avatar.getFace()`), and builds three `ColorPalette` objects. The new interface will reuse those resources.
- Expression manipulation is handled by the `m5avatar::Avatar` API:
  * `avatar.setExpression(Expression::Happy)` etc. Expression enum values are defined in `m5stack-avatar/src/Expression.h` (`Happy`, `Angry`, `Sad`, `Doubt`, `Sleepy`, `Neutral`).
  * `avatar.setSpeechText(const char*)` updates the bubble text; `avatar.setSpeechFont(&fonts::lgfxJapanGothic_12)` sets the font.
  * `avatar.setFace(Face*)` plus `avatar.setColorPalette(ColorPalette)` switch between face/palette pairings.
  * `avatar.setMouthOpenRatio(float)` can be used if we want to animate mouth excursions; otherwise, the existing `behavior` task handles lip sync when `USE_VOICE_TEXT` is active.
- The Serial interface runs at the default baud (115200) established when `M5.begin()` runs. The parser can read `Serial.readStringUntil('\n')` or similar.
- Button handling in `loop()` (calls to `M5.BtnA/B/C.wasPressed()`, t-tone feedback, `displayDialogue`) will be removed; instead, `loop()` should focus on `M5.update()`, parsing serial commands, and optionally a timer that clears speech text when `duration` elapsed.
- Consider using `ArduinoJson` (lightweight) to parse inbound JSON; otherwise simple delimiter parsing is acceptable given the predictable format.

## 5. Host-side implementation notes (Raspberry Pi 5)
- The Pi script should open `/dev/ttyUSB0` at 115200 bps (`SerialPort` shown by `arduino-cli board list`). If the CP210x driver exposes a different port, adapt accordingly.
- Example Python CLI invocation:
  ```sh
  python control_stackchan.py --expression Happy --speech "こんにちは" --face 2 --duration 4000
  ```
  The script serializes the arguments into the JSON format above, sends it (with newline), and waits for the Core2 `OK` reply before exiting.
- A helper script may also support `--clear` to send `{ "clear": true }` or `--speech-font <font-name>` if the firmware exposes additional commands.
- The script must handle failures (serial timeouts, parse errors) gracefully and report them to the caller.

## 6. Implementation information to collect
1. The exact `Expression` enum names and their behavior (`m5stack-avatar/src/Expression.h` and the `Face`/`Effect` implementations already in this repo). (Collected: Happy/Angry/Sad/Doubt/Sleepy/Neutral.)
2. Which `Avatar` fonts and palettes are already configured by `setup()` so the interface can reuse them.
3. Which delay/clear semantics are needed for speech bubbles (e.g., the Core2 must clear after `duration` ms, or after a `clear` command arrives).
4. Serial heartbeat: the script should retry if the Core2 does not respond within 1 second.

## 7. Future considerations
- If `USE_VOICE_TEXT` is re-enabled later, the Pi script could optionally provide text for TTS (VoiceText) by sending `voice: true` plus parameters; the firmware can reuse `VoiceText_tts(text, tts_parms)`.
- Additional commands (e.g., servo gaze/pan, light control) can be layered onto the same JSON interface as the project evolves.
