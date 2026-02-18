# Task Plan: Define the Pi-driven StackChan control interface requirements

## Goal
Capture the application requirements and implementation constraints for a Raspberry Pi 5 → M5Stack Core2 control interface that lets terminal scripts set expressions, speech bubbles, and visual cues on StackChan without button input.

## Current Phase
Phase 1

## Phases

### Phase 1: Requirements & Discovery
- [x] Understand user intent (Pi script-driven control)
- [x] Identify existing Core2 behaviors and hardware context
- [ ] Document findings in findings.md
- **Status:** in_progress

### Phase 2: Planning & Structure
- [ ] Define how the Pi will communicate commands (serial protocol, data format)
- [ ] Determine what Core2 APIs (Avatar expressions, speech text, display) need to be exposed
- [ ] Choose documentation layout (requirements + implementation info)
- **Status:** pending

### Phase 3: Implementation
- [x] Write requirements document (docs/interface-requirements.md)
- [x] Include command examples, necessary Core2 functions, and notes about removing current button logic
- [x] List the information needed to implement firmware + Pi script (command parsing, serial port, dependencies)
- [ ] Implement Core2 JSON parser and Pi CLI script
- **Status:** in_progress

### Phase 4: Testing & Verification
- [ ] Ensure doc references actual code sections (Avatar usage, serial port data)
- [ ] Update progress.md with actions/tests/observations as we gather info
- [ ] Confirm plan reflects all blockers or missing info
- **Status:** pending

### Phase 5: Delivery
- [ ] Commit artifacts (`docs/interface-requirements.md`, planning files)
- [ ] Summarize findings for the user
- [ ] Ensure README-index notes this new doc if needed
- **Status:** pending

## Key Questions
1. What Avatar APIs (expressions, speech text, fonts) will the Core2 firmware expose to this interface?
2. What command/serial protocol should the Pi script send to trigger expressions and balloons?

## Decisions Made
| Decision | Rationale |
|----------|-----------|
| Keep a dedicated `docs/interface-requirements.md` | Captures requirements and info for future implementation without mixing with user-facing README |

## Errors Encountered
| Error | Attempt | Resolution |
|-------|---------|------------|
|       | 1       |            |

## Notes
- Keep this plan up to date as we gather new info
- Re-read before major decisions
- Log ALL errors/issues immediately
