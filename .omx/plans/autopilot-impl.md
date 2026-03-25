# Autopilot Implementation Plan — Tactical Command Deck Redesign

## Phase 1 Plan
1. **Establish a stronger visual system in `Renderer`**
   - Add multi-font loading helpers and clearer font roles.
   - Add reusable premium panel/background drawing helpers.
   - Define upgraded layout geometry for select/match/game-over screens.
   - Make draw + hit-test consume the same layout geometry.

2. **Redesign nation-select into dossier cards**
   - Replace stacked button feel with editorial heading block + asymmetric nation cards.
   - Add doctrine summary, stat chips, and stronger hover/selection presence.

3. **Redesign in-match HUD into tactical cockpit**
   - Keep map dominant.
   - Introduce top operational strip, right command rail, floating selected-tile command chip.
   - Improve tile/capital/transit visual distinction and map depth.

4. **Redesign game-over into debrief overlay**
   - Use a dedicated result panel with winner/stalemate summary and standings.
   - Dim battlefield and keep replay flow intact.

5. **QA + validation**
   - Rebuild, rerun tests, runtime smoke.
   - Validate visual/code quality and note any residual risks.

## File Targets
- Primary: `src/ui/Renderer.cpp`, `include/game/ui/Renderer.hpp`
- Secondary: `src/App.cpp`
- Optional support: `assets/fonts/`

## Risks / Mitigations
- **Risk:** too much visual chrome hurts readability
  - **Mitigation:** keep map dominant, minimize accent count, preserve numerical clarity
- **Risk:** premium fonts unavailable
  - **Mitigation:** keep font-role fallbacks and no-font title fallback intact
- **Risk:** redesign accidentally changes gameplay behavior
  - **Mitigation:** keep logic out of sim layer; confine behavior changes to presentation
- **Risk:** asymmetrical layout breaks click targets
  - **Mitigation:** centralize layout rectangles and reuse them for hit-testing
- **Risk:** fixed 1280x800 screen becomes overcrowded
  - **Mitigation:** enforce a minimum map viewport and cap panel widths

## Verification
- `cmake -S . -B build`
- `cmake --build build -j4 --target territory_war territory_war_tests`
- `ctest --test-dir build --output-on-failure`
- runtime smoke: launch `./build/territory_war` and confirm process stays alive briefly
