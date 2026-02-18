# Repository Rules

## Purpose
Keep this repository aligned with the StackChan voice and expression control project. Treat every change as part of a controlled, documented workflow.

## Test-Driven Development
- Before making any substantive firmware or script change, write or update a corresponding test description in `docs/tests.md`. Tests do not need to be automated to start; detail the expected steps, inputs, and verification criteria before modifying code.
- Run the described tests after implementation, note results directly inside `docs/tests.md`, and record any issues in `docs/troubleshooting.md` immediately.
- Every behavioral change (e.g., new serial commands, parser modifications) must be paired with a test scenario that exercises it.

## Documentation Journals
- Keep `docs/journal.md` and `docs/troubleshooting.md` in sync with work: log what was done, what commands were run, and what errors were resolved.
- Update `AGENTS.md` if the workflow changes in a way that affects contributors.
