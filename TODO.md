A. ✅ Modularise CMake: completed by introducing dedicated `IpponboardCore`, `IpponboardWidgets`, and `IpponboardUi` libraries that the app/tests now consume.

B. Controller Decomposition:
   ✅ Characterised fight navigation (`NextFight`/`PrevFight`) with unit tests covering round transitions and golden-score resets.
   ✅ Introduced `TournamentNavigator` to own round/fight indices and drive controller updates.
   ✅ Added `TournamentRepository` to manage fight persistence (`save`, `reset`, `clear`) and called from controller.
   Next: evaluate remaining responsibilities (view fan-out, rules/options plumbing) for similar extraction.

C. State Machine Clarity: the Boost.MSM table in core/StateMachine.h:220 is hard to reason about. Introduce named transition helpers or wrap MSM events in a thin façade so guard/action ordering becomes explicit, easing future rule tweaks.

D. Qt Abstraction: base/View.cpp mixes narration logic (GVF_ swaps) with direct widget manipulations. Extract a presenter or view-model that transforms controller state into a simple struct; the QWidget then just renders it. This reduces Qt test coupling and clarifies the fighter-swapping behaviour.

E. Shared UI Utilities: Widgets/ScaledText.cpp and ScaledImage.cpp are duplicated for app and tests. Package them into a small library (with headers installed via target_include_directories) so they aren’t recompiled manually and their APIs (e.g., new GetText()) stay consistent.

F. ✅ Modernise Qt Usage: audited remaining modules; legacy SIGNAL/SLOT, QRegExp, and QSound usages have been replaced with Qt6-ready APIs across core/base/widgets/gamepad/util/test.

F2. Qt Thread Storage shutdown noise: investigate the lingering `QThreadStorage ... destroyed` warning that appears after tests to confirm no latent lifecycle issues before moving to Qt6.

G. Testing Strategy:
   1. Extend controller fixture tests to cover fight navigation, save/reset flows, and option toggles (auto-adjust, open-ended GS) so refactors stay safe.
   2. Add characterisation tests for `TournamentModel` sorting/filtering and the score view presenters before touching their internals.
   3. For UI-heavy dialogs, prefer presenter/adapter tests (e.g., `SettingsDlg` sound preview, `ClubManagerDlg` file handling) instead of QWidget assertions.

H. ✅ Coding Conventions: `.clang-format` and refreshed naming guidance now live in the repo. Next push is enforcing RAII/[[nodiscard]] rules per core/Score.h style and untangling `../` includes via target dirs.

H2. Integrate clang-format into the build toolchain (pre-commit/CI) and document installation checks for Windows/Linux scripts so contributors get guided setup.

I. Documentation/ADR: capture legacy decisions (Boost.MSM choice, fighter orientation in View) in short ADRs so future contributors understand the constraints before refactoring.
