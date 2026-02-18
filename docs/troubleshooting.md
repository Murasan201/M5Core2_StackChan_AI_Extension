# Troubleshooting Log

## 2026-02-18
1. **`apply_patch` command missing** — running `apply_patch` failed with `/bin/bash: apply_patch: command not found`. Resolution: used a short Python script to edit the file instead of `apply_patch`. Logged this action so future agents know the command is unavailable in this shell.
2. **`patch` command reported `Only garbage was found in the patch input`** — the inline patch data was not accepted because the hunk headers did not match the existing file context. Resolution: swapped to a Python-driven replacement that inserted the desired block exactly at the known marker.
3. **`patch` command still failing when trying to replace the loop block** — the file had been modified earlier, causing `patch` not matching. Resolution: replaced the section programmatically with the intended new content via a Python rewrite.
