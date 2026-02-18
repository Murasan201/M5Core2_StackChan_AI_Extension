# Work Journal

## 2026-02-18
- Documented the Pi-to-Core2 interface requirements and policy that button compatibility is expendable (via `docs/interface-requirements.md`).
- Added `AGENTS.md` describing the repo, hardware (CP210x `/dev/ttyUSB0`), and guidelines for future contributions.
- Implemented the Core2 JSON parser in `M5Core2_SG90_StackChan_VoiceText_Ataru.ino`, which now listens on serial, applies expression/speech/face/palette commands, and responds with `OK`/`ERR`.
- Added `control_stackchan.py` and documented how it serializes CLI arguments into JSON commands, waits for acknowledgements, and handles errors.
- Updated planning files (`task_plan.md`, `progress.md`, `findings.md`) to reflect the new focus on the serial interface and to mark Phases 1-3 progress.
- Linked the development workflow doc in `docs/interface-requirements.md` so implementers know how to build/upload via `arduino-cli`.
- Attempted to compile/upload the new firmware via `arduino-cli`, but the build fails due to mismatched Avatar library headers (`WiFiClient`, private `DrawContext` members). Documented this failure in `docs/troubleshooting.md` so future work can fix dependencies and run tests.
- Logged all significant changes by creating this journal and a troubleshooting record.
- Added `docs/dev-guide.md`, which catalogs the Raspberry Pi-to-Core2 development workflow, records the commands run so far, and sets the upcoming steps (building, testing, logging).
- Enabled the USB serial console (`cfg.serial_baudrate = 115200`) and log every incoming JSON line (`Serial.print("RX: ")` + `Serial.println(...)`) so the CLI traffic is visible while debugging.
- Adjusted `control_stackchan.py` (flush input, longer startup delay, no forced DTR) and now the CLI successfully receives `OK` for the test expression/duration commands, proving the parser is reachable.
