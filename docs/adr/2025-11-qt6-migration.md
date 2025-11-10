# ADR: Adopt Qt 6 Toolchain

- Status: Accepted
- Date: 2025-11-05
- Deciders: Ipponboard maintainers

## Context

Qt 5.15 reached end-of-life for community users, and new platform support plus security fixes are delivered in Qt 6. Recent feature work already targeted a Qt 6 codebase (e.g., gamepad overhauls, C++20 usage). Remaining on Qt 5 blocked compiler upgrades, complicated packaging (Qt 5/6 coexistence), and prevented us from addressing deprecations surfaced in newer Qt releases.

## Decision

Standardise the project on Qt 6.9.2 (desktop kits: `gcc_64` and `msvc2022_64`) and C++20. All build scripts, CI workflows, documentation, and packaging assets now assume Qt 6:

- `build.ps1`/`build.sh` load the Qt 6 SDK, require Ninja, and configure CMake with C++20.
- The Windows installer bundles Qt 6 runtime directories (`platforms`, `styles`, `tls`) and MSVC VC143 CRTs via `windeployqt`.
- Documentation and license bundles reference Qt 6 (LGPLv3 + Qt exception) and direct users to the 6.9.2 source archive.
- The Windows CI workflow (Ninja Multi-Config) builds the tests/app and produces an installer artifact via the refreshed setup script.

## Consequences

### Positive

- Access to current Qt fixes and features (multimedia, translations, modern APIs).
- Consistent C++20 toolchain across platforms; removes legacy `-fpermissive` and Qt 5 compatibility shims.
- Simplified packaging: single Qt 6 license bundle, up-to-date runtime layout, reproducible CI builds.

### Negative/Risks

- Windows users require Visual Studio 2022 Build Tools; older compilers/toolsets are no longer supported.
- Linux packages must document Qt 6 as a prerequisite (no static bundle yet).
- CI requires a Windows runner with Qt 6 caches; until it is re-enabled, Windows automation depends on local validation.

## Rollback Strategy

Maintain the former Qt 5 build scripts and installer definitions on the `qt5` branch. If a critical regression surfaces, revert the toolchain bumps (Qt/CMake configs, installer changes) and consume the Qt 5 packaging workflow from that branch while the issue is investigated. Re-enabling Qt 5 builds requires restoring the Qt 5 license bundle and pointing `env_cfg` back to a Qt 5 SDK.
