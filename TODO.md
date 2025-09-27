A. Modularise CMake: break the single app target (base/CMakeLists.txt:24) into libraries (IpponboardCore, IpponboardUi, etc.). That lets unit tests link only what they need and simplifies future Qt6 dual-builds.

B. Controller Decomposition: split core/Controller.cpp into smaller services (timer handling, tournament navigation, persistence). Move low-level helpers (e.g., weight/club setters) into dedicated classes and expose them via interfaces.

C. State Machine Clarity: the Boost.MSM table in core/StateMachine.h:220 is hard to reason about. Introduce named transition helpers or wrap MSM events in a thin façade so guard/action ordering becomes explicit, easing future rule tweaks.

D. Qt Abstraction: base/View.cpp mixes narration logic (GVF_ swaps) with direct widget manipulations. Extract a presenter or view-model that transforms controller state into a simple struct; the QWidget then just renders it. This reduces Qt test coupling and clarifies the fighter-swapping behaviour.

E. Shared UI Utilities: Widgets/ScaledText.cpp and ScaledImage.cpp are duplicated for app and tests. Package them into a small library (with headers installed via target_include_directories) so they aren’t recompiled manually and their APIs (e.g., new GetText()) stay consistent.

F. Modernise Qt Usage: replace deprecated Qt5 classes (QRegExp, QSound, manual connect with SIGNAL/SLOT) with Qt6-ready APIs (QRegularExpression, lambda connects). Doing this incrementally now smooths the full Qt6 update later.

G. Testing Strategy: broaden coverage around the controller/tournament flows by reusing the fixtures. For UI, favour presenter-level tests that assert plain structs rather than widget hierarchies—reduces the need for full QWidget builds during unit runs.

H. Coding Conventions: adopt a project-wide formatter (clang-format) and enforce RAII/[[nodiscard]] rules per core/Score.h style. Many headers still pull in with relative ../ paths; switching to target includes clarifies dependencies.

I. Documentation/ADR: capture legacy decisions (Boost.MSM choice, fighter orientation in View) in short ADRs so future contributors understand the constraints before refactoring.