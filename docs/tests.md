# Test Matrix & TDD Record

This project embraces test-driven development: before changing firmware or scripts, describe the intended test, execute it, record the result, and note any errors. Each row below corresponds to a test scenario tied to a feature or requirement.

| Date | Test Case | Target | Steps | Expected Outcome | Result / Notes |
|------|-----------|--------|-------|------------------|----------------|
| 2026-02-18 | Expression command | Core2 parser | 1. Launch firmware on Core2
2. Run `python control_stackchan.py --expression Happy --face 2 --speech "こんにちは"`
3. Observe serial ACK and display | Serial responds `OK`; avatar face 2 with Happy expression appears and speech bubble shows message | pending |
| 2026-02-18 | Duration auto-clear | Core2 parser | 1. Send command with `--duration 2000`
2. Observe speech bubble clears after ~2 s | Speech clears automatically, `avatar.setSpeechText` called with empty string | pending |
| 2026-02-18 | Clear flag | Core2 parser | 1. Show speech via Libray
2. Send `{ "clear": true }` via `control_stackchan.py --clear`
3. Confirm bubble empties and ACK received | Speech cleared instantly, `OK` returned | pending |
| 2026-02-18 | Invalid expression | Parser error handling | Send `{ "expression": "Grumpy" }` | Parser replies `ERR invalid expression`, script exits nonzero | pending |
| Future | Batch scenario | CLI script | Create JSON file, call `control_stackchan.py --file file.json` with multiple commands | Each command acknowledged; final state matches script| pending |

*Update this table before making changes, and move entries to "pass"/"fail" once executed.*
