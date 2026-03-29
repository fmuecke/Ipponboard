# ADR: Define User-Scoped Storage Locations

- Status: Proposed
- Date: 2026-03-14
- Deciders: Ipponboard maintainers

## Context

Ipponboard currently lacks a documented contract for where installers place the executable payload and where the application persists configuration files and user-generated data. Windows customers expect Ipponboard to be installable without administrative rights, and Linux users increasingly rely on sandboxed package managers that respect the [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/latest/). Without an explicit decision, packaging scripts, runtime code, and support documentation risk diverging, which complicates migration and end-user support.

In addition to installation and configuration paths, the application also needs a consistent model for domain data such as clubs, fighters, logos, and weight classes. These data sets are not the same as application settings. Ipponboard ships with defaults, but users must be able to extend or override them without modifying the installed bundle. If the application writes directly into bundled files, updates become fragile, packaged builds become harder to support, and read-only deployment targets such as signed app bundles or Qt resources become impractical.

The current runtime code already distinguishes between:

- program-owned files that are read relative to the executable directory (`QCoreApplication::applicationDirPath()`), and
- user-writable files that are resolved through `QStandardPaths`.

That split now needs to be documented explicitly for macOS because app bundles add a second path boundary: bundle resources are immutable program data, while user settings, logs, and edited copies of shipped configuration files must live outside the bundle in standard user-writable locations.

## Decision

Adopt user-scoped storage anchored to the operating system's standard application data directories:

- **Windows**
  - **Static program data** (installed binaries, Qt runtime, immutable assets): `%LOCALAPPDATA%\Ipponboard`
  - **Configuration files and user data** (profiles, logs, saved tournaments): `%APPDATA%\Ipponboard`
- **Linux**
  - **Static program data**: `~/.local/share/Ipponboard` (overridable through `$XDG_DATA_HOME`)
  - **Configuration files and user data**: `~/.config/Ipponboard` (overridable through `$XDG_CONFIG_HOME`), with large data exports or logs stored under `~/.local/share/Ipponboard`.
- **macOS**
  - **Static program data**: inside the application bundle, using bundle-relative paths rooted at `Ipponboard.app/Contents/Resources` for immutable assets and `Ipponboard.app/Contents/MacOS` only for executables.
  - **Configuration files**: `~/Library/Preferences/Ipponboard` or the `QStandardPaths::AppConfigLocation` equivalent selected by Qt for the active sandboxing model.
  - **User data, logs, and generated content**: `~/Library/Application Support/Ipponboard` or the `QStandardPaths::AppLocalDataLocation` equivalent selected by Qt.

For a standard non-sandboxed macOS installation, support and packaging documentation should assume these concrete paths:

- Program executable: `/Applications/Ipponboard.app/Contents/MacOS/Ipponboard`
- Program-owned resources: `/Applications/Ipponboard.app/Contents/Resources`
- User configuration: `/Users/<user>/Library/Preferences/Ipponboard`
- User data and logs: `/Users/<user>/Library/Application Support/Ipponboard`

If the bundle is installed outside `/Applications`, the bundle-relative `Contents/MacOS` and `Contents/Resources` paths still apply, and only the absolute bundle prefix changes.

Runtime file handling must follow these rules:

- Shipped defaults such as templates, sounds, translations, and read-only configuration seeds are program data and must be read from the installed program location.
- User-edited configuration files such as `Ipponboard.config`, `clubs.config`, `categories.config`, and `tournament_modes.config` must be written to the user-writable config/data directories, never back into the application bundle.
- Read paths may fall back from the user-writable location to the installed program copy during migration or first-run bootstrap, but writes must always target the user-writable location.
- Support documentation, packaging, and diagnostics must refer to both the bundle-owned paths and the user-writable paths so macOS issues can be reproduced without ambiguity.

Implementation note: runtime code now resolves immutable macOS assets from `Contents/Resources`, not from `Contents/MacOS`. Config/template fallback reads can use bundle resources as the read-only seed source, while all writes stay in the Qt-selected user-scoped config/data paths.

Installers/scripts must create these directories on first run with user-only permissions, migrate legacy content into the new locations when detected, and update documentation accordingly.

For domain data, separate shipped defaults from user-specific overrides:

- **Shipped defaults, read-only**
  - Store immutable default domain data inside the application bundle, preferably in Qt resources under `:/...`.
  - Examples:
    - `:/data/clubs.json`
    - `:/data/weightclasses.json`
    - `:/logos/...`
- **User-specific overrides, writable**
  - Store user-provided or user-modified domain data under `QStandardPaths::AppDataLocation`.
  - Examples:
    - `AppDataLocation/clubs.json`
    - `AppDataLocation/weightclasses.json`
    - `AppDataLocation/logos/...`

At startup, the application must resolve domain data in this order:

1. Load shipped defaults.
2. Load user-specific overrides.
3. Apply user-specific overrides on top of the shipped defaults.

The application must never patch bundled defaults in place. Updates may replace bundled defaults, but user-specific overrides remain external and take precedence at runtime.

`QSettings` remains reserved for persistent application settings such as window geometry, theme, language selection, recent printer choice, and similar UI or runtime preferences. Domain data such as fighters, clubs, logos, and weight classes must not be stored in `QSettings`.

## Consequences

### Positive

- Aligns with Microsoft and freedesktop guidelines, enabling non-administrative installs and predictable backups.
- Keeps user data separate from executables, simplifying updates and allowing clean uninstall without losing profiles when requested.
- Enables roaming of preferences on Windows through `%APPDATA%` while keeping large binaries local to each workstation.
- Allows the application to ship curated defaults while still supporting user customization without mutating the installed bundle.
- Works with read-only deployment models such as Qt resources, signed bundles, and sandboxed package formats.
- Makes migration and support easier because the precedence rule for defaults versus overrides is explicit.
- Makes the macOS bundle layout explicit, reducing ambiguity around where runtime resources end and user data begins.
- Matches the current Qt-based path helpers, which already separate user-writable storage from executable-relative fallback reads.

### Negative/Risks

- Linux packages distributed via system package managers (e.g., `.deb`, Flatpak) may prefer `/usr` or sandbox paths; installers must translate these rules or document deviations.
- Existing installs that wrote beside the executable must be migrated carefully to prevent data loss.
- WSL inherits the host Windows filesystem semantics; installers must normalize path handling to avoid mixing Windows-style environment variables with POSIX paths.
- Merge behavior for domain data must be defined per file type. Some assets may be additive, while others may require key-based replacement or explicit deletion markers.
- Resource-based defaults are easy to read but cannot be edited in place, so development and migration tooling must support exporting or inspecting effective merged data when needed.
- macOS packaging must ensure bundle resources are copied into the correct bundle subdirectories; code that assumes the working directory matches the executable directory remains fragile until all resource lookups are normalized.
- Some macOS APIs and packaging tools prefer `Contents/Resources` over `Contents/MacOS` for non-executable assets, so bundle layout and runtime lookup rules must be kept in sync.

## Rollback Strategy

If the chosen directories cause regressions, revert to the previous layout by:

1. Detecting the legacy paths (e.g., binary directory siblings) before writing new data.
2. Reading configuration from both locations with a precedence order (new path first, fallback to legacy) during the rollback window.
3. Adjusting installers to stop relocating files and documenting the reversion in release notes.

Once stability is restored, we can revisit the directory strategy with updated requirements.
