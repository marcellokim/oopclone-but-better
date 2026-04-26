# Territory War

Territory War is a playable C++20/SFML 3 real-time strategy prototype where four university-themed factions fight over a handcrafted tile map with asymmetric abilities, AI opponents, command power, and a score debrief.

## Why It Exists

This repository demonstrates a small but complete game loop: faction selection, real-time orders, terrain-aware movement, combat, passive and active faction abilities, AI decision-making, victory conditions, and post-match scoring. The code is structured so the simulation rules can be tested independently from the SFML renderer.

## Highlights

- Four playable factions with distinct passives and active abilities:
  - Sogang Univ. - faster tempo and launch-cap pressure
  - Hanyang Univ. - road-based mobility and route capacity
  - Sungkyunkwan Univ. - capital/highland defense and regeneration
  - Chung-Ang Univ. - adaptive frontline reinforcement
- Real-time tile command system with 25% / 50% / 100% send ratios.
- Terrain rules for roads, plains, capitals, highlands, mountains, and sea blockers.
- Three AI-controlled opponents in every match.
- Command Power resource, cooldowns, active durations, and AI ability usage.
- Score and grade debrief sorted by final standings after the match.
- Regression tests for simulation rules, victory conditions, AI behavior, abilities, scoring, input, renderer layout, and UI tuning.

## Tech Stack

- C++20
- SFML 3.0.2, fetched through CMake `FetchContent`
- CMake 3.24+
- Custom lightweight C++ test harness in `tests/`
- No runtime environment variables are required.

## Quick Start For Players

Download the ZIP for your OS, unzip it, then double-click the launcher:

- macOS: `Play.command`
- Windows: `Play.bat`

Players do not need to clone the repository, run CMake manually, or open a build folder.

macOS Gatekeeper note: if the first launch is blocked, right-click `Play.command` and choose `Open`.

## Create A Shareable ZIP

Build the ZIP on the same OS and CPU architecture you want to distribute.

macOS:

```bash
./tools/package_portable.sh --test
```

Windows PowerShell:

```powershell
.\tools\package_portable.ps1 -Test
```

Generated examples:

- `dist/TerritoryWar-macOS-arm64.zip`
- `dist/TerritoryWar-Windows-x64.zip`

## Run From Source

Prerequisites:

- C++20 compiler
- CMake 3.24+
- Network access on the first configure, because SFML is downloaded by CMake
- macOS: Xcode Command Line Tools
- Windows: Visual Studio Build Tools or another C++20-compatible toolchain

macOS:

```bash
./Play.command
```

Windows:

```powershell
.\Play.bat
```

The launcher configures CMake, builds the game, and starts the executable. The first run can take longer because SFML is fetched and compiled.

Optional build overrides:

- `TERRITORY_WAR_BUILD_DIR`: source-run build directory, default `build/`
- `TERRITORY_WAR_PACKAGE_BUILD_DIR`: packaging build directory, default `build-package/`
- `TERRITORY_WAR_CONFIG`: CMake configuration, default `Release`

## Controls

- Left click: select a tile or issue an order
- Right click / Esc: clear selection
- `1` / `2` / `3`: send 25% / 50% / 100% of available troops
- `Space`: activate the current faction ability when Command Power is ready
- Ability panel click: same as `Space`
- Enter or left click on debrief: return to faction select

## Build And Test

Configure and build:

```bash
cmake -S . -B build
cmake --build build -j4 --target territory_war territory_war_tests territory_war_preview
```

Run tests:

```bash
ctest --test-dir build --output-on-failure
```

Build, test, and launch through the helper:

```bash
./tools/bootstrap_and_run.sh --test
```

Create a local install-style copy without launching:

```bash
./tools/bootstrap_and_run.sh --install --no-run
```

Output:

- `dist/territory-war/`

## Preview And Playtest Tools

Generate offscreen UI preview images:

```bash
cmake --build build -j4 --target territory_war_preview
./build/territory_war_preview
```

Generated files:

- `.omx/screens/preview-nation-select.png`
- `.omx/screens/preview-match.png`
- `.omx/screens/preview-game-over.png`

Start a manual playtest checklist:

```bash
./tools/start_playtest.sh
```

Generated files:

- `docs/playtest-runs/playtest-<timestamp>.md`
- `docs/playtest-ui-balance-checklist.md`

No portfolio screenshots are tracked in the repository yet. The preview renderer above is the current reproducible way to generate local UI images.

## Project Structure

```text
src/                 Application, simulation, AI, and UI implementation
include/             Public headers
tests/               Regression tests
assets/fonts/        Redistributable UI fonts and font licenses
tools/               Run, package, preview, and playtest helper scripts
docs/                UI style, release, smoke-test, and playtest notes
```

Key implementation areas:

- `src/sim/` - deterministic simulation systems
- `src/ai/` - AI order and ability decisions
- `src/ui/` - SFML renderer, layout, input, and HUD views
- `src/App.cpp` - application loop and scene orchestration

## Documentation

- `docs/manual-smoke-checklist.md`
- `docs/playtest-ui-balance-checklist.md`
- `docs/public-release-checklist.md`
- `docs/ui-style-guide.md`
- `docs/combined-ui-upgrades-phase2-verification.md`

## Release Notes

Current distribution is intentionally simple: portable ZIP files for macOS and Windows. The project does not currently include code signing, notarization, a store page, CI release automation, or a public demo URL.

## License

Code is distributed under the repository license in `LICENSE`. Bundled fonts are covered by the OFL license files in `assets/fonts/`.
