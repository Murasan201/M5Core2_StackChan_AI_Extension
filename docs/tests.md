# Test Matrix & TDD Record

This project embraces test-driven development: before changing firmware or scripts, describe the intended test, execute it, record the result, and note any errors. Each row below corresponds to a test scenario tied to a feature or requirement.

| Date | Test Case | Target | Steps | Expected Outcome | Result / Notes |
|------|-----------|--------|-------|------------------|----------------|
| 2026-02-18 | Expression command | Core2 parser | 1. Build/upload the latest firmware
2. Run `python control_stackchan.py --expression Happy --face 2 --speech "こんにちは"`
3. Observe serial ACK and display | `control_stackchan.py` now prints `> {...} -> OK` after a short boot delay (the board also logs `RX: ...`), so the parser is receiving commands. Visual confirmation of the new expression is pending on the device side. | handshake resolved; waiting on visual check |
| 2026-02-18 | Duration auto-clear | Core2 parser | 1. Send command with `--duration 2000`
2. Observe speech bubble clears after ~2 s | Command acknowledged (`OK`), but manual verification of the bubble clearing still needs inspection on the Core2 display. | ack confirmed; bubble clearance pending |
| 2026-02-18 | Clear flag | Core2 parser | 1. Show speech via Libray
2. Send `{ "clear": true }` via `control_stackchan.py --clear`
3. Confirm bubble empties and ACK received | Speech cleared instantly, `OK` returned | pending |
| 2026-02-18 | Invalid expression | Parser error handling | Send `{ "expression": "Grumpy" }` | Parser replies `ERR invalid expression`, script exits nonzero | pending |
| 2026-02-19 | Face selection command | Core2 parser | 1. Send `python control_stackchan.py --face 0 --expression Neutral --speech "Ataru Face" --duration 3000`
2. Send `python control_stackchan.py --face 1 --expression Happy --speech "Ram Face" --duration 3000`
3. Send `python control_stackchan.py --face 2 --expression Happy --speech "StackChan Face" --duration 3000`
4. Observe each face renders (Ataru, Ram, StackChan) and `OK` ack follows every command | CLI prints `OK` for all three commands while Core2 logs `RX:` for each, confirming the parser accepts every face selection. | pass (faces changed visually) |
| Future | Batch scenario | CLI script | Create JSON file, call `control_stackchan.py --file file.json` with multiple commands | Each command acknowledged; final state matches script| pending |

*Update this table before making changes, and move entries to "pass"/"fail" once executed.*
