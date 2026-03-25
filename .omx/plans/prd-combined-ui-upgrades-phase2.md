# PRD — Combined UI Upgrades Phase 2

## Metadata
- **Task slug:** `combined-ui-upgrades-phase2`
- **Plan mode:** `ralplan / consensus / short`
- **Repo state:** Brownfield. Existing game prototype, redesign pass, tests, and runtime smoke are already in place.
- **Primary context:** `.omx/context/combined-ui-upgrades-phase2-20260325T064500Z.md`

## Product Summary
Execute a second-phase productization pass on the current tactical-command-deck redesign by combining four approved follow-up tracks into one coordinated increment:
1. replace system-derived/bundled fonts with redistributable fonts,
2. refactor the oversized renderer into maintainable slices,
3. add selective high-value visual polish/animation within SFML,
4. tune UI balance/readability with structured playtest scenarios.

## Existing Facts vs Proposed Additions
### Existing repo facts
- Gameplay, AI, deterministic sim, and current controls already work.
- The UI was recently upgraded into a tactical command deck and validated.
- The main redesign surface is currently concentrated in `src/ui/Renderer.cpp` and `src/App.cpp`.
- Fonts currently rely on locally copied system fonts and fallback paths.

### Proposed NEW / changed deliverables
- `CHANGED include/game/ui/Renderer.hpp`
- `CHANGED src/ui/Renderer.cpp`
- `CHANGED src/App.cpp`
- `CHANGED assets/fonts/*`
- `NEW/CHANGED docs/ui-style-guide.md`
- `NEW/CHANGED docs/playtest-ui-balance-checklist.md`
- `NEW/CHANGED tests/*` for UI-supportive coverage where practical

## Phase-2 Goals
1. **Font Safety + Portability**
   - Replace current machine-derived font assets with redistributable/OFL-safe fonts.
   - Preserve explicit display/body/mono role separation.
2. **Renderer Maintainability**
   - Reduce the single-file burden in `Renderer.cpp` without engine rewrite.
   - Separate screen-specific drawing helpers or modules while preserving shared geometry logic.
3. **Premium Polish**
   - Add subtle SFML-native motion/feedback: hover emphasis, selection energy, combat/path feedback, screen transition refinement.
   - Avoid heavy animation systems or dependency additions.
4. **UI Balance / Readability Tuning**
   - Validate whether spacing, panel density, tile contrast, and information priority actually help during play.
   - Tune using repeatable playtest scenarios, not aesthetic guesswork alone.

## Non-goals
- Rewriting the simulation engine
- Changing AI rules or combat formulas for balance unrelated to UI readability
- Adding networking, campaign, or new maps as part of this increment
- Introducing heavyweight animation/middleware dependencies

## Acceptance Criteria
1. The repo no longer depends on system-derived font copies for its premium UI path.
2. Renderer responsibilities are decomposed enough that nation select / match HUD / game-over logic no longer read as one monolith.
3. Selection, hover, capitals, and active transits receive stronger but still readable visual feedback.
4. At least one structured playtest checklist exists and drives specific UI readability adjustments.
5. The redesigned UI still preserves the same gameplay controls and sim behavior.
6. Build, tests, and runtime smoke still pass after the phase-2 work.
7. Residual font fallback behavior remains safe when bundled fonts fail to load.

## Implementation Plan (3–6 steps)
1. **Replace fonts safely**
   - Introduce redistributable font assets and document the font-role/fallback policy.
2. **Refactor renderer by responsibility**
   - Extract shared layout helpers plus screen-specific drawing helpers/modules.
3. **Add polish pass**
   - Introduce lightweight SFML-native emphasis for hover/selection/transits/debrief transitions.
4. **Run structured UI-balance tuning**
   - Use repeatable playtest scenarios to adjust spacing, contrast, panel density, and chip readability.
5. **Lock the increment with verification**
   - Update docs/tests as needed and rerun build/tests/runtime smoke.

## Risks and Mitigations
- **Risk:** refactoring the renderer breaks click hitboxes.
  - **Mitigation:** keep shared geometry as the single source of truth for draw + hit-test.
- **Risk:** new fonts improve style but hurt portability.
  - **Mitigation:** use redistributable fonts with documented fallback order.
- **Risk:** polish effects reduce readability or distract during combat.
  - **Mitigation:** keep motion subtle, map-first, and playtest-driven.
- **Risk:** UI-balance tuning drifts into gameplay rebalance.
  - **Mitigation:** scope adjustments to presentation density/contrast/feedback only.

## Verification Steps
- `cmake -S . -B build`
- `cmake --build build -j4 --target territory_war territory_war_tests`
- `ctest --test-dir build --output-on-failure`
- runtime smoke: launch `./build/territory_war` and confirm it stays alive briefly
- execute the new playtest checklist and record UI-tuning deltas
