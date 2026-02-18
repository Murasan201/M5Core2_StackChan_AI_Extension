# Work Journal

## 2026-02-18
- Documented the Pi-to-Core2 interface requirements and policy that button compatibility is expendable (via `docs/interface-requirements.md`).
- Added `AGENTS.md` describing the repo, hardware (CP210x `/dev/ttyUSB0`), and guidelines for future contributions.
- Implemented the Core2 JSON parser in `M5Core2_SG90_StackChan_VoiceText_Ataru.ino`, which now listens on serial, applies expression/speech/face/palette commands, and responds with `OK`/`ERR`.
- Added `control_stackchan.py` and documented how it serializes CLI arguments into JSON commands, waits for acknowledgements, and handles errors.
- Updated planning files (`task_plan.md`, `progress.md`, `findings.md`) to reflect the new focus on the serial interface and to mark Phases 1-3 progress.
- Linked the development workflow doc in `docs/interface-requirements.md` so implementers know how to build/upload via `arduino-cli`.
- Logged all significant changes by creating this journal and a troubleshooting record.
