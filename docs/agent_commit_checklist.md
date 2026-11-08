# Agent Commit Checklist

## Current session

- Current approval status: APPROVED
- Last approval message: "committe die Änderungen mit einer Erläuterung der durchgefühten Anpassungen. Erkläre insbesondere, warum \"ContestIndex += 2)\" verwendet wird und warum deswegen \"falsch war."
- Approval timestamp: 2026-11-08 UTC
- Approver: repository owner (ralf), via chat commit request

## Changes in this commit

- core/Rules.h: add a virtual default destructor to AbstractRules
- util/helpers.hpp: remove obsolete final/override compatibility macros for pre-C++20 compilers
- base/OnlineVersionChecker.cpp: compile the Windows-only asset helper only on Windows
- core/Controller.cpp: express pair processing with contestIndex += 2; the old loop increment plus an increment in the body had the same stride, but split loop control and triggered Clang's loop-analysis warning
- test/TestController.cpp: characterize adjacent-pair switching and unchanged odd trailing contest
- Validation: Clang build succeeded; unit suite passed (503 assertions in 102 cases); CRLF-aware diff check passed

## Notes

- Commit uses scripts/agent_git_commit.sh per project rule.
