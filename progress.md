# Progress Log

## Session: 2026-02-18

### Phase 1: Requirements Gathering & Discovery
- **Status:** complete
- **Started:** 2026-02-18 14:46:00 JST
- **Ended:** 2026-02-18 15:10:00 JST
- Actions taken:
  - Ran the planning-with-files catch-up script to make sure no prior context was pending.
  - Documented the Pi↔Core2 hardware context (`lsusb`, `ttyUSB0`, and `arduino-cli board list`).
  - Scoped the new script-driven interface and collected research (Avatar expressions, available API functions) from the `m5stack-avatar` repo.
  - Updated `task_plan.md`/`findings.md` to align with the new goal and recorded the hardware/communication constraints.
- Files created/modified:
  - `task_plan.md` (new goal and phase plan)
  - `findings.md` (requirements and research data)
  - `progress.md` (this entry)

### Phase 2: Planning & Structure
- **Status:** complete
- Actions taken:
  - Documented the JSON-based parser/protocol in `docs/interface-requirements.md` and linked the Pi-side script and upload guide.
  - Captured the dependency decisions (lines of code, handshake requirements) in findings.
- Files created/modified:
  - `docs/interface-requirements.md`

### Phase 3: Implementation
- **Status:** in_progress
- Actions taken:
  - Implemented the Core2 serial parser/gallery that reads newline-delimited JSON, maps commands to Avatar calls, and replies `OK/ERR`.
  - Added `control_stackchan.py` (pyserial-based CLI) to send expression/speech commands from `/dev/ttyUSB0`.
  - Confirmed documentation updates referencing the new script and development workflow guide.
  - Disabled the VoiceText path (guarding the Wi-Fi/audio headers) and refactored the Ataru/Ram face helpers to rely on the public `DrawContext` getters so the current `M5Stack-Avatar` release compiles.
  - Enabled the USB serial console (`cfg.serial_baudrate = 115200`) and left debugging logs that echo incoming JSON so the CLI traffic is visible.
  - Added CLI timing fixes (flush, 2–3 s boot delay, no forced DTR) so `control_stackchan.py` now receives `OK` for expression and duration commands.
- Files created/modified:
  - `M5Core2_SG90_StackChan_VoiceText_Ataru/M5Core2_SG90_StackChan_VoiceText_Ataru.ino`
  - `control_stackchan.py`
  - `docs/interface-requirements.md`
  - `docs/troubleshooting.md`
  - `docs/tests.md`
  - `docs/dev-guide.md`

## Test Results
| Test | Input | Expected | Actual | Status |
|------|-------|----------|--------|--------|
| Expression command handshake | `python control_stackchan.py --expression Happy --face 2 --speech "こんにちは"` | Parser replies `OK` and, when observed on-device, updates the expression | CLI now prints `OK` and the board logs `RX: {...}` | ✓ |
| Duration auto-clear (ack) | `python control_stackchan.py --speech "長めのテスト" --duration 2000` | Speech command acknowledged, bubble clears ~2 s later | CLI prints `OK`; still awaiting manual confirmation of the bubble clearing | pending confirmation |

## Error Log
| Timestamp | Error | Attempt | Resolution |
|-----------|-------|---------|------------|
| 2026-02-18 22:35 JST | CLI never saw `OK/ERR` after sending JSON | 1 | `control_stackchan.py` had forced `ser.dtr = False`, which kept the ESP32 in reset; removing that line, flushing the input buffer, and waiting a few seconds before writing restored the handshake |

## 5-Question Reboot Check
| Question | Answer |
|----------|--------|
| Where am I? | Phase 3 (implementation/testing) of the task plan, refining the firmware/workflow and verifying CLI handshakes |
| Where am I going? | Finish the remaining manual confirmations (visual expression/duration checks), update docs/tests accordingly, then push the branch |
| What's the goal? | Deliver a reliable JSON-over-serial control surface, a tested CLI, and up-to-date docs so operators can exercise StackChan expressions/speech from the Pi |
| What have I learned? | USB serial must be explicitly enabled via `cfg.serial_baudrate` on Core2, and forcing DTR low blocks the parser; once the board boots, the CLI now sees `OK` replies |
| What have I done? | Fixed the compile blockers, added serial logging in the sketch, tweaked the CLI timing, and confirmed expression/duration commands now return `OK` |

---
*Update after completing each phase or encountering errors*
