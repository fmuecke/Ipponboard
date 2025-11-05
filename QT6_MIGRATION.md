# Qt 6 Migration Playbook (Living Document)

This checklist tracks the migration from Qt 5.15.x to Qt 6.9.2. Update it as tasks move forward.

Legend:
- `[done]` completed
- `[todo]` pending
- `[review]` needs investigation/decision

## Context and Goal
- Target toolchain: Qt 6.9.2 (`~/Qt/6.9.2/gcc_64` on Linux; matching MSVC 2022 x64 kit on Windows).
- Objective: All builds/tests/documentation assume Qt 6; Qt 5 artefacts are removed or archived. Licensing remains compliant with LGPL via dynamic linkage.

## 1. Environment and Tooling
- `[done]` `env_cfg.bat`, `scripts/init_env_cfg.cmd`, and `build.sh` default to Qt 6.9.2; Boost handling unchanged.
- `[done]` Local/Linux tests run headless using `QT_QPA_PLATFORM=offscreen` and suppress the Qt Multimedia PipeWire warning.
- `[done]` Windows PowerShell/CI runs export `QT_QPA_PLATFORM_PLUGIN_PATH=%QTDIR%\plugins\platforms` and `QT_QPA_FONTDIR=%WINDIR%\Fonts` so headless tests load the offscreen plugin and fonts.
- `[todo]` Confirm Windows GitHub runners provide Qt 6.9.2 MSVC 2022 x64; add installer/cache step if missing.
- `[done]` Documented required Qt helper tools (`lrelease`, `windeployqt`, `qtpaths`) and the standard SDK layout in HOW_TO_BUILD.md.

## 2. CMake and Build System
- `[done]` Root and child CMake scripts use `find_package(Qt6 ...)` and link Qt 6 targets; Qt 5 option removed.
- `[done]` Windows post-build steps share `ipponboard_add_windeploy()` helper to wrap `windeployqt` with the required MSVC environment, avoiding per-target boilerplate.
- `[done]` Increase `QT_DISABLE_DEPRECATED_BEFORE` to `0x060900` to surface Qt 6.9 deprecations during builds.
- `[done]` Re-audit subdirectory `CMakeLists.txt`: removed Linux-only `-fpermissive` hacks, rely on target-scoped includes/PIC, and confirmed Qt module usage is minimal.
- `[done]` Require C++20 toolchain-wide; Boost 1.89 resolves prior MSM incompatibility.
- `[todo]` Regenerate build trees with `cmake --fresh` on Linux and Windows to verify no stale cache assumptions remain.

## 3. Source Code Migration
- `[done]` Resolved initial Qt 6 deprecation warnings (`ModeManagerDlg`, `MainWindowTeam` context menus).
- `[done]` Removed legacy desktop APIs; code paths rely on `QGuiApplication`/`QScreen` and no sources include `<QtGui>` or `QDesktopWidget`.
- `[done]` Build with tighter deprecation guards (`-DQT_DISABLE_DEPRECATED_BEFORE=0x060900`); no outstanding warnings after recompilation.
- `[done]` Verified Qt 6.9.2 multimedia stack: application starts, plugins load, and sound playback works on Linux and Windows hosts.
- `[todo]` Re-run clang-format/clang-tidy as needed after code cleanups, keeping repository style.

## 4. Packaging and Deployment
- `[todo]` Windows: drive packaging via `windeployqt`, update `setup/setup.iss`, and align the release workflow.
- `[todo]` Linux: keep Qt as a system prerequisite; document installation options and verify RPATH/RUNPATH settings for installed binaries.
- `[todo]` Licensing: replace `doc/licenses/Qt5` with the Qt 6 LGPL bundle and adjust scripts that copy these files.
- `[todo]` Ensure translation tooling (`lrelease`) uses the Qt 6 binary when generating `.qm` files.

## 5. Documentation and Process
- `[todo]` Update `HOW_TO_BUILD.md`, `README.md`, user manuals, and contributor docs for the Qt 6 toolchain (Linux, Windows, optional macOS).
- `[todo]` Record the Qt 6 migration in `CHANGELOG.md` / `TODO.md` and produce an ADR capturing the decision, impact, and rollback plan.
- `[todo]` Align workflow guidance (`AGENTS.md`, CI tips) with the final packaging/testing steps.

## 6. Validation and QA
- `[done]` Automated Linux tests pass under Qt 6 with network tests disabled.
- `[done]` Network-dependent tests now live in the always-built `IpponboardNetworkTest` target; run it in network-enabled environments when validating Qt 6 behaviour.
- `[todo]` Execute manual smoke tests (UI layout, translations, audio, printing) on Linux and Windows once code changes settle.
- `[todo]` Windows packaging smoke test: build installer, install on clean VM, confirm runtime dependencies and licensing documentation.

## 7. Risks and Follow-ups
- `[review]` Multimedia backend differences (PipeWire vs PulseAudio vs WASAPI) might require additional dependencies; monitor during manual tests.
- `[review]` Ensure GitHub-hosted runners, especially Windows, consistently expose the required Qt 6 SDK; automate provisioning if necessary.
- `[review]` Watch third-party library interactions (Boost, Catch2) when enabling stricter Qt deprecation guards.

## Appendix – High-priority Files
- Build/config: `CMakeLists.txt` (all), `setup/setup.iss`, `scripts/init_env_cfg.cmd`, `env_cfg.bat`, `build.sh`, `build.cmd` (if present).
- Documentation: `HOW_TO_BUILD.md`, `README.md`, `CHANGELOG.md`, `doc/USER_MANUAL-*.md`, `doc/licenses/*`, `CONTRIBUTING.md`, `AGENTS.md`.
- Packaging assets: `doc/licenses/`, `setup/resources/`, `.github/workflows/*`.

Update this playbook as tasks move; once the migration is complete, summarize remaining lessons in an ADR and archive the checklist.
