# Test Spec — Combined UI Upgrades Phase 2

## Metadata
- **Task slug:** `combined-ui-upgrades-phase2`
- **Repo state:** Brownfield
- **Testing stance:** preserve existing regression coverage, add targeted support for refactor safety and UI-balance evidence

## Existing Test Baseline
- Deterministic simulation, AI behavior, victory logic, and input-command tests already exist.
- Current redesign already passes build, CTest, and runtime smoke.

## Test Strategy
Keep the current gameplay regression suite intact while adding targeted coverage around the four new tracks.

## Test Layers
### 1. Build + regression gate
Target: full project
- Existing build/test pipeline must remain green.
- Refactor work must not change sim or command semantics.

### 2. Font/fallback safety checks
Target: renderer/app fallback paths
- Preferred bundled font assets load when present.
- Renderer still degrades safely if bundled fonts fail or are missing.
- No path outside the allowlisted font strategy is required for normal operation.

### 3. Renderer-geometry regression checks
Target: renderer layout helpers / hit-test geometry
- Nation-select click regions still match rendered card locations.
- Map hit-testing still matches the rendered tile field after refactor.
- Shared layout geometry remains the source of truth for both drawing and hit-testing.

### 4. Visual feedback sanity checks
Target: renderer polish layer
- Selected tile emphasis is visually stronger than hover emphasis.
- Capital emphasis remains distinguishable from standard ownership tint.
- Transit/path feedback remains readable over the map.
- Debrief overlay remains legible and does not obscure result hierarchy.

### 5. Playtest/readability scenarios
Target: full runtime
Document and execute repeatable scenarios such as:
- opening match: can the player immediately identify their nation, capital, and command rail?
- congested front: can the player distinguish selected tile, hovered tile, and active transit simultaneously?
- late game: can the player still read troop counts and live-front summaries under pressure?
- result screen: can the player parse winner/stalemate and standings at a glance?

## Suggested New Test/Doc Targets
- `NEW/CHANGED docs/playtest-ui-balance-checklist.md`
- `NEW/CHANGED docs/ui-style-guide.md`
- `NEW/CHANGED tests/` only where UI-supportive logic is testable without overcoupling to SFML internals

## Acceptance Gates
1. Existing CTest suite remains green.
2. Renderer refactor does not break nation-select or map hit-testing behavior.
3. Bundled-font path and graceful fallback path are both explicitly handled.
4. Playtest checklist produces concrete readability findings and resulting tuning adjustments.
5. No gameplay-rule regressions are introduced.

## Verification Commands
- `cmake -S . -B build`
- `cmake --build build -j4 --target territory_war territory_war_tests`
- `ctest --test-dir build --output-on-failure`
- runtime smoke: `./build/territory_war`

## Exit Criteria
Execution is not complete until:
- build succeeds
- automated tests pass
- runtime smoke passes
- playtest/readability checklist is executed and any chosen tuning adjustments are captured
