# Test Spec — Realtime 4-Nation Territory War

## Metadata
- **Task slug:** `realtime-4nation-territory-war`
- **Repo state:** Greenfield
- **Testing stance:** dependency-light, deterministic, CMake/CTest-centered

## Test Strategy
Separate the simulation core from rendering/input so most game rules can be validated without opening a window. Prefer deterministic fixtures over visual-only validation.

## Existing Facts vs Planned Test Targets
### Existing repo facts
- No tests exist yet.

### Proposed NEW test files
- `NEW tests/sim_rules_test.cpp`
- `NEW tests/ai_behavior_test.cpp`
- `NEW tests/victory_conditions_test.cpp`
- `NEW tests/input_command_test.cpp`

## Test Layers
### 1. Simulation rule tests
Target: `NEW include/game/sim/*`, `NEW src/sim/*`
- Movement between adjacent and multi-hop tiles resolves on deterministic ticks.
- Orthogonal BFS route selection never uses diagonal shortcuts.
- Friendly transfer merges correct troop totals.
- Enemy engagement applies attack/defense/terrain modifiers correctly.
- Ownership flips only when defender reaches 0 and attackers survive.
- Regeneration increases when owned territory count rises.
- Nation stat/regeneration modifiers affect formulas as expected.

### 2. AI behavior sanity tests
Target: `NEW include/game/ai/AiDirector.hpp`, `NEW src/ai/AiDirector.cpp`
- Swift League prefers exposed neutral or lightly defended tiles.
- Iron Legion prefers aggressive assault when advantage threshold is met.
- Bastion Directorate prioritizes threatened capital/frontline defense.
- Crown Consortium keeps a reserve and expands when safe.
- AI re-evaluates after map-state changes rather than committing forever to stale orders.
- Non-emergency AI orders respect the minimum commitment window before retargeting.

### 3. Victory-condition tests
Target: `NEW include/game/sim/VictorySystem.hpp`, `NEW src/sim/VictorySystem.cpp`
- Capital capture ends match immediately.
- Zero total troops ends match immediately.
- A nation with a surviving capital and nonzero forces is not falsely eliminated.
- Simultaneous elimination edge case has a defined winner-resolution rule.
- Same-tick capital-capture vs zero-troop outcomes follow the documented resolution order.

### 4. Manual playtest checklist
Target: full application
- Nation selection screen works for all 4 choices.
- Hover tooltip appears on owned, enemy, neutral, and capital tiles.
- Click-to-move and click-to-attack are understandable without debug tools.
- Current send ratio is visible before the player issues a command.
- AI remains active for an entire 10+ minute match.
- End screen reports winner and supports restart/quit.

### 5. Input and command tests
Target: `NEW include/game/sim/{OrderIntent,CommandQueue,Pathfinder}.hpp`, `NEW src/sim/{CommandQueue,Pathfinder}.cpp`
- Friendly and enemy click commands produce the expected `OrderIntent`.
- Invalid commands (empty tile, zero transferable troops, same-tile no-op) are rejected safely.
- Route generation is deterministic for repeated identical map states.

## Determinism Requirements
- Use a fixed simulation step (recommended 10–20 Hz).
- Seed any randomized AI tie-breakers with explicit test values.
- Keep combat/regeneration formulas pure enough for fixture-based assertions.
- Rendering must not mutate game rules.

## Suggested Test Cases
| ID | Scenario | Expected Result |
|---|---|---|
| SIM-01 | 20 troops move from owned tile A to owned tile B | B gains exactly sent troops; A loses exactly sent troops |
| SIM-02 | 20 attackers hit 10 defenders on plains with Iron Legion stats | Defenders lose according to formula; survivors occupy only if defenders hit 0 |
| SIM-03 | Bastion defenders on highland vs equal attackers | Defender survives longer than plains baseline |
| SIM-04 | Capital owner expands from 8 to 12 tiles | Regen per second increases |
| SIM-05 | Two shortest paths exist on the same grid seed | Pathfinder returns the documented deterministic route order |
| AI-01 | Swift League sees 2 equal-front options | Chooses faster-expansion candidate |
| AI-02 | Bastion capital threatened | Defensive action scores above expansion |
| AI-03 | Same frontier reevaluated before commitment window expires | AI keeps prior non-emergency plan |
| VIC-01 | Capital tile captured with surviving peripheral troops elsewhere | Captured nation still loses immediately |
| VIC-02 | Nation loses all troops but retains uncaptured capital | Nation loses immediately |
| CMD-01 | Player selects 100% send ratio then targets enemy tile | Attack order carries full transferable force |
| CMD-02 | Player targets same tile twice | No-op command is rejected without mutating state |

## Acceptance Gates
1. All simulation tests pass in CTest.
2. AI sanity tests pass with deterministic seeds.
3. Victory-condition tests pass with no contradictory outcomes.
4. Input/command tests pass for order creation and route determinism.
5. Manual checklist confirms all 10 PRD acceptance criteria are observable.
6. No rendering/input bug may be closed as “done” without a reproducible manual validation note.

## Verification Commands (planned)
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Exit Criteria for Execution Phase
Execution is not complete until:
- Build succeeds.
- All automated tests pass.
- A full manual match confirms hover info, command flow, AI behavior, regeneration, and both victory paths.
