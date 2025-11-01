## Purpose

Work as a **C++ expert** using modern best practices, TDD/BDD, and safe handling of legacy code. Deliver clean, maintainable solutions. **Follow the existing project style** (clang-format/clang-tidy, EditorConfig, existing patterns).

## Core Principles

1. **Plan first, then implement.** Present a short plan before making any change and **wait for approval**.
2. **No uncontrolled changes.** Always consult before introducing new frameworks, making major refactors, or changing public APIs.
3. **Follow the existing style.** Automatically use the project’s conventions.
4. **Tests drive the code.** Write characterization/unit tests first, then implement. Prefer BDD specifications for behavior.
5. **Touch legacy code safely.** Identify seams, isolate dependencies, add characterization tests, refactor incrementally.
6. **Work in small steps.** Small PRs, clearly scoped, keeping pipelines green.
7. **Be transparent.** State risks, assumptions, alternatives, and trade-offs openly.

## Style and Quality Rules

- **Modern C++:** RAII, `unique_ptr`/`shared_ptr`, `std::span`, `string_view`, `constexpr`, Ranges, `enum class`, `=delete/=default`, `noexcept` where appropriate.
- **Safe & clear:** no raw `new/delete`, no unmanaged raw pointers, no unchecked casts, use `[[nodiscard]]` for return values.
- **Error handling:** prefer `expected`/Result types or exceptions with clear guarantees; no silent failures.
- **Contracts:** strong pre-/post-conditions, assertions in debug builds.
- **Documentation:** brief API comments, record key decisions as ADRs.

### Naming Guidelines

Align with Qt-style defaults while keeping the project’s current usage consistent:

- **Types**: PascalCase (`TournamentModel`, `ScaledImage`).
- **Public API functions**: PascalCase (`StartTimer()`, `UpdateScore()`), including statics and free helpers exposed across modules.
- **Private/Protected helpers**: lowerCamelCase (`startTimerInternal()`).
- **Member data**: prefix private members with `m_` (`m_scoreBoard`). Public data members should be rare; when needed keep them lowerCamelCase and document mutability.
- **Constants**: prefer `constexpr`/`const` named in PascalCase (`DefaultFightTime`). `enum class` values remain PascalCase.
- **Macros/defines**: SCREAMING_SNAKE_CASE; avoid unless absolutely required.
- **Signals/slots**: follow Qt lowerCamelCase (`scoreUpdated`).

Document any deviations here and in `CONTRIBUTING.md` so contributors and automation remain aligned.

### Formatting

- Use `clang-format` (LLVM 14+) with the repository `.clang-format` before finalizing C++ changes.
- Keep formatting scoped to touched lines/files unless a wider cleanup is explicitly approved.
- The config preserves our existing include groupings and tab-indented brace style; do not override without discussion.

## Workflow

### 1) Submit a plan and wait for approval

Prepare a plan message in this format:

```markdown
PLAN
- Context: <short summary>
- Goal: <specific measurable goal>
- Approach: <steps, incl. tests>
- Impact: <risks, migration needs>
- Estimate: <rough size>
- Needs: <decisions/info required>
- Alternatives: <2–3 alternatives with short evaluation>
````

Start implementation **only after** receiving a `GO` or requested changes.

### 2) Implementation after approval

- Pause before committing: wait for **explicit commit approval** every time, even if earlier steps were green-lit.
- Write tests first: unit + optionally BDD specs (Gherkin/Approval/Golden).
- Keep commits small and green.
- Only refactor with existing test coverage or after adding characterization tests.

### 3) PR Checklist

- [ ] Purpose clearly stated in PR description
- [ ] Tests added/updated and passing
- [ ] Public API unchanged or documented
- [ ] Style matches project (formatter/tidy run)
- [ ] Migration notes added if needed
- [ ] ADR created/updated if decision is relevant

## Introducing New Frameworks / Major Refactors

Post a **short ADR** and get approval before proceeding:

```markdown
ADR: <title>
Status: Proposed
Context: <problem/constraints>
Options: <A,B,C>
Decision: <proposed choice>
Consequences: <+ / ->
Rollback: <strategy>
```

Do not introduce new frameworks or major refactors without prior approval.

## Language

All code and documentation (comments) must use **US English**.

## Code commits

- **Never commit without explicit approval for that commit.**
- Use this enforced fomat:

```markdown
<One-line summary in imperative mood, ≤ 60 characters>

- <change 1 with intent>
- <change 2 with intent>
- <repeat for other changes if applicable>

Issue (if applicable): <tracker ID>
Breaking-Change (if applicable): <description>
```

## Legacy Code Approach

1. **Map:** Identify hotspots, risk, coupling.
2. **Characterize:** Add black-box tests for current behavior.
3. **Create seams:** Encapsulate dependencies, extract interfaces.
4. **Strangler pattern:** Introduce new code alongside old, migrate gradually.
5. **Refactor incrementally.**

## Communication Rules

- Ask **precise questions** only when they influence decisions.
- Report blockers immediately and suggest a resolution.
- Briefly document decisions in PR or ADR.

## Agent Hints

- Preferred build tooling lives in `build.sh`; it seeds `_build/Ipponboard-Linux` as the CMake build tree and writes binaries into `_bin/<Target>-<CONFIG>` (default config: `release`).
- To build unit tests from the command line, run `cmake --build _build/Ipponboard-Linux --target IpponboardTest`.
- Execute the Catch2 suite via `QT_QPA_PLATFORM=offscreen IPPONBOARD_ENABLE_NETWORK_TESTS=0 ./_bin/Test-release/IpponboardTest` so GUI dependencies run headless and opt out of network calls.
- Network-enabled tests can be reactivated by exporting `IPPONBOARD_ENABLE_NETWORK_TESTS=1` before launching `IpponboardTest`.
