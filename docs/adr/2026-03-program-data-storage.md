# ADR: Define User-Scoped Storage Locations

- Status: Proposed
- Date: 2026-03-14
- Deciders: Ipponboard maintainers

## Context

Ipponboard currently lacks a documented contract for where installers place the executable payload and where the application persists configuration files and user-generated data. Windows customers expect Ipponboard to be installable without administrative rights, and Linux users increasingly rely on sandboxed package managers that respect the [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/latest/). Without an explicit decision, packaging scripts, runtime code, and support documentation risk diverging, which complicates migration and end-user support.

## Decision

Adopt user-scoped storage anchored to the operating system's standard application data directories:

- **Windows**
  - **Static program data** (installed binaries, Qt runtime, immutable assets): `%LOCALAPPDATA%\Ipponboard`
  - **Configuration files and user data** (profiles, logs, saved tournaments): `%APPDATA%\Ipponboard`
- **Linux**
  - **Static program data**: `~/.local/share/Ipponboard` (overridable through `$XDG_DATA_HOME`)
  - **Configuration files and user data**: `~/.config/Ipponboard` (overridable through `$XDG_CONFIG_HOME`), with large data exports or logs stored under `~/.local/share/Ipponboard`.

Installers/scripts must create these directories on first run with user-only permissions, migrate legacy content into the new locations when detected, and update documentation accordingly.

## Consequences

### Positive

- Aligns with Microsoft and freedesktop guidelines, enabling non-administrative installs and predictable backups.
- Keeps user data separate from executables, simplifying updates and allowing clean uninstall without losing profiles when requested.
- Enables roaming of preferences on Windows through `%APPDATA%` while keeping large binaries local to each workstation.

### Negative/Risks

- Linux packages distributed via system package managers (e.g., `.deb`, Flatpak) may prefer `/usr` or sandbox paths; installers must translate these rules or document deviations.
- Existing installs that wrote beside the executable must be migrated carefully to prevent data loss.
- WSL inherits the host Windows filesystem semantics; installers must normalize path handling to avoid mixing Windows-style environment variables with POSIX paths.

## Rollback Strategy

If the chosen directories cause regressions, revert to the previous layout by:

1. Detecting the legacy paths (e.g., binary directory siblings) before writing new data.
2. Reading configuration from both locations with a precedence order (new path first, fallback to legacy) during the rollback window.
3. Adjusting installers to stop relocating files and documenting the reversion in release notes.

Once stability is restored, we can revisit the directory strategy with updated requirements.
