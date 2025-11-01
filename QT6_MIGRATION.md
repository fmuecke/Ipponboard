# Qt 6 Migration Playbook

## Context & Target
- **Current state**: Project builds against Qt 5.15.2 via manually managed DLL copies and scripted env setup. All CMake targets use `Qt5::` linkage; documentation and packaging assume Qt 5.
- **Desired state**: Primary toolchain uses Qt 6.9.2 (`~/Qt/6.9.2/gcc_64` on Linux; matching MSVC kit on Windows). CMake links against Qt 6 targets; runtime bundles Qt 6 plugins; docs reference Qt 6; legacy Qt 5 artefacts removed or archived.

## 1. Environment & Tooling Preparation
- Update `env_cfg.bat` and templates (`scripts/init_env_cfg.cmd`, `build.sh`) to point at Qt 6.9.2 install roots (`LINUX_QTDIR=~/Qt/6.9.2/gcc_64`, Windows kits as appropriate). Ensure `BOOST_DIR` handling still works.
- Confirm `CMAKE_PREFIX_PATH`/`CMAKE_LIBRARY_PATH` populated via the new `QTDIR`.
- Verify Qt 6 tool binaries (`lrelease`, `windeployqt`/`macdeployqt`, `qtpaths`) are reachable for build and packaging scripts.
- Check CI/build agents have Qt 6.9.2 available; plan cache or install steps.

## 2. CMake & Build System Changes
- Root `CMakeLists.txt`
  - Switch `find_package` calls to `Qt6 COMPONENTS ... REQUIRED` and remove the `USE_QT5` option gate.
  - Audit `QT_DISABLE_DEPRECATED_BEFORE` macro: raise to at least `0x060000` (consider `0x060600` once all API updates are in) to surface deprecated Qt 6 APIs early.
  - Revisit module list; ensure `Qt6::Network` gets linked where `QNetworkAccessManager` is used (currently only `IpponboardUi`).
  - Keep `AUTOMOC/AUTOUIC/AUTORCC` unchanged; Qt 6’s CMake API is backward compatible.
- Subdirectory CMake files (`core/`, `base/`, `Widgets/`, `test/`)
  - Replace `Qt5::` targets with `Qt6::` equivalents.
  - Drop the `include_directories(${Qt5..._INCLUDE_DIRS})` blocks—Qt 6 CMake targets propagate include paths.
  - Remove `XmlPatterns` from component lists; the codebase no longer uses Qt XML Patterns, so the dependency can be dropped outright.
  - Ensure tests link against `Qt6::Test` only if we adopt it; today they use pure Catch2.
  - Use generator expressions for runtime path copying sparingly; we will standardize on Qt deployment tooling instead (see §4).
- Regenerate build system (`cmake --fresh -S . -B _build/Ipponboard-Linux`) after changes.

## 3. Source Code Updates
- **Header hygiene**
  - Remove unused legacy headers (numerous `#include <QDesktopWidget>` and `<QtGui>` in `base/` and `Widgets/` still compile under Qt 5 but are gone or discouraged in Qt 6).
  - Replace PCH inclusions (`base/pch.h`) with module-specific headers from Qt 6 (e.g., include `<QGuiApplication>` instead of `<QtGui/QApplication>`; drop duplicated includes).
- **UI/Widgets**
  - Replace any `QDesktopWidget` usage with `QGuiApplication::primaryScreen()` / `QScreen` (code already uses `QGuiApplication::screens()`; ensure no residual `QApplication::desktop()` calls).
  - Verify `QPrinter`/`QPrintPreviewDialog` APIs in `MainWindowTeam.cpp`—signatures unchanged in Qt 6.
- **Multimedia**
  - Confirm `QSoundEffect` usage aligns with Qt 6 expectations (`setVolume` range is still 0.0–1.0). Ensure build links `Qt6::Multimedia`.
  - Evaluate runtime plugin needs (WASAPI on Windows, PulseAudio/PipeWire on Linux).
- **Network**
  - Ensure `OnlineVersionChecker` includes continue to compile; `QNetworkReply::error()` enum values differ slightly (Qt 6 uses scoped enums). Adjust comparisons if compiler warns.
- **General**
  - Watch for API renames (e.g., `Qt::AlignHCenter` now flagged as deprecated alias; prefer `Qt::AlignmentFlag::AlignHCenter`).
  - Re-run clang-tidy/format after edits to match project style.

## 4. Packaging & Deployment
- **Windows (MSVC 2022, 64 bit)**
  - Replace manual DLL/plugin copies in `base/CMakeLists.txt` and `test/CMakeLists.txt` by invoking `windeployqt` (Qt 6.9.2 MSVC 2022 x64 kit) during packaging. Ensure `setup/setup.iss` consumes the `windeployqt` output rather than hard-coded Qt files.
  - Update GitHub workflows to run `windeployqt` as part of the release pipeline, keeping runtime bundles aligned with the chosen Qt version.
- **Linux**
  - Do not bundle Qt—document Qt 6.9.2 runtime as a prerequisite. Provide installation instructions (e.g., Qt Online Installer or distro packages) in build docs and README.
  - Adjust CMake install/package steps to avoid assuming Qt libraries are redistributed; verify RPATH/RUNPATH configuration allows system Qt resolution.
- **Translations**
  - Ensure `lrelease` invocation uses Qt 6 binary (`${QTDIR}/bin/lrelease[.exe]` still valid).
- **Licensing**
  - Replace the `doc/licenses/Qt5` bundle with the Qt 6 LGPL text (dynamic linking). Update packaging scripts to copy the new folder.

## 5. Documentation & Scripts
- Refresh `HOW_TO_BUILD.md`, `README.md`, and `doc/USER_MANUAL-*.md` to reference Qt 6.9.2 installation steps (`aqtinstall`, Qt online installer), covering Linux prerequisite installation rather than bundling.
- Remove Qt 5 build instructions entirely where Qt 6 steps replace them; note the migration in `CHANGELOG.md` / `TODO.md`.
- Consider an ADR documenting the decision to drop Qt 5 compatibility (per contribution guidelines).

## 6. Validation & QA
- Full rebuild on Windows and Linux (Release + Debug).
- Run automated suite: `cmake --build ... --target IpponboardTest`, then `QT_QPA_PLATFORM=offscreen IPPONBOARD_ENABLE_NETWORK_TESTS=0 ./_bin/Test-release/IpponboardTest`.
- Manual smoke tests:
  1. Launch main UI; ensure window layout and fonts unchanged.
  2. Verify translations load, sounds play, printer preview opens.
  3. Exercise network update check (with `IPPONBOARD_ENABLE_NETWORK_TESTS=1`).
- For Windows packaging, install produced setup and confirm runtime dependencies shipped correctly.

## 7. Risks & Open Questions
- **Multimedia backend**: Qt 6 may require additional codecs/backends; verify on target OSes.
- **CI availability**: Ensure hosted agents supply Qt 6.9.2 MSVC 2022 x64 and Linux GCC builds; otherwise add caching/install steps.
- **Third-party libraries**: Re-test Boost interactions; expect no direct Qt dependency but monitor build warnings.
- **Toolchain drift**: Qt 6 prefers CMake 3.21+; project already uses 3.28, so no action needed.

## Appendix A – Files Referencing Qt 5 (as of analysis)
- Build scripts: `CMakeLists.txt`, `base/CMakeLists.txt`, `core/CMakeLists.txt`, `Widgets/CMakeLists.txt`, `test/CMakeLists.txt`, `build.sh`, `scripts/init_env_cfg.cmd`, `env_cfg.bat`.
- Packaging: `setup/setup.iss`.
- Documentation: `HOW_TO_BUILD.md`, `README.md`, `CHANGELOG.md`, `doc/USER_MANUAL-DE.md`, `doc/USER_MANUAL-EN.md`, `doc/licenses/Qt5/`.
- Misc references: `CONTRIBUTORS.md`, `TODO.md`, `doc/licenses` index.

Keep this playbook updated as individual items are addressed; mark sections complete in the PR checklist once corresponding changes land.
