# AGENT EXECUTION GUIDANCE

This captures the working agreements established with the user so future sessions align with their expectations.

## Process Expectations
- **Plan first.** Provide a PLAN message in the agreed template, wait for explicit `GO` before touching code.
- **Work incrementally.** Keep changes narrowly scoped, avoid large refactors unless requested, and highlight risks or alternatives up front.
- **Tests before code.** Add/extend characterization or unit tests ahead of implementation changes; favour Catch2 BDD-style naming.
- **Transparency.** State assumptions, point out blockers immediately, and document decisions in-line or via ADR when significant.
- **No auto commits.** Never invoke Git commit helpers without fresh, explicit approval; update `docs/agent_commit_checklist.md` first when a commit is greenlit.

## Coding Standards
- **Modern C++.** Follow RAII, smart pointers, `[[nodiscard]]`, exceptions/results for errors, and avoid raw `new/delete`.
- **Project style.** Honour `.clang-format`, existing include ordering, naming (Qt-style: PascalCase types, lowerCamelCase privates, `m_` prefix for members).
- **Safe legacy handling.** Characterize behaviour, create seams, and refactor incrementally; no speculative rewrites.
- **Documentation.** Add concise comments only where logic is non-obvious; record notable decisions (e.g., ADR, docs).

## Build & Test Workflow
- **Tooling.** Use provided CMake/Ninja workflow (`build.sh`, `build.ps1`), always build both `IpponboardTest` and `IpponboardNetworkTest`.
- **Environment variables.** Avoid relying on `IPPONBOARD_ENABLE_NETWORK_TESTS`; callers control network availability externally.
- **Verification.** Run relevant binaries after changes; if sandbox limits network tests, explain the limitation and suggest local follow-up.

## Communication Style
- **Concise teammate tone.** Summaries first, detailed context second, suggestions only when clearly helpful.
- **Structured reporting.** Reference files with inline path:line markers, no large dumps, add next-step recommendations when natural.
- **Respect constraints.** No destructive Git commands, no new frameworks without prior approval, no network calls without sandbox permissions.

Keep this guidance alongside `AGENTS.md` to ensure continuity across future sessions.
