# Troubleshooting Log

## 2026-02-18
1. **`apply_patch` command missing** — running `apply_patch` failed with `/bin/bash: apply_patch: command not found`. Resolution: used a short Python script to edit the file instead of `apply_patch`. Logged this action so future agents know the command is unavailable in this shell.
2. **`patch` command reported `Only garbage was found in the patch input`** — the inline patch data was not accepted because the hunk headers did not match the existing file context. Resolution: swapped to a Python-driven replacement that inserted the desired block exactly at the known marker.
3. **`patch` command still failing when replacing the loop block** — the file had been modified earlier, causing `patch` to fail. Resolution: replaced the section programmatically with the intended new content via a Python rewrite.
4. **`arduino-cli compile` fails after installing dependencies** — missing `WiFiClient` declaration and private `DrawContext` getters due to the vendor's `AudioFileSourceVoiceTextStream` and `M5Stack-Avatar` versions. Outcome: the build stops before CLI tests can run, so hardware tests are blocked until the dependency versions or code adjustments (e.g., adding WiFi includes, aligning avatar dependencies) are fixed. Document this so future agents know build fails at this stage.
