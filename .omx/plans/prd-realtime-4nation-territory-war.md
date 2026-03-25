# PRD — Realtime 4-Nation Territory War

## Metadata
- **Task slug:** `realtime-4nation-territory-war`
- **Plan mode:** `ralplan / consensus / short`
- **Repo state:** Greenfield. Existing repository fact: only `.omx/` scaffolding exists.
- **Planned implementation style:** C++20 + CMake + 2D framework (recommended: SFML 3.x)

## Product Summary
Build a real-time, tile-based territory war game with a war-simulation presentation but approachable RTS-lite controls. The player chooses 1 of 4 asymmetric nations and fights 3 AI-controlled rivals on a single 2D map. Troops move continuously between tiles, attack on contact, and regenerate from capitals based on territory control. A match ends when a capital falls or a nation loses all remaining military forces.

## Existing Facts vs Proposed Additions
### Existing repo facts
- `.omx/` exists.
- No source files, build files, assets, or tests exist yet.

### Proposed NEW files/modules
- `NEW CMakeLists.txt`
- `NEW src/main.cpp`
- `NEW include/game/App.hpp`, `NEW src/App.cpp`
- `NEW include/game/GameConfig.hpp`, `NEW src/GameConfig.cpp`
- `NEW include/game/sim/WorldState.hpp`, `NEW src/sim/WorldState.cpp`
- `NEW include/game/sim/ArmyTransit.hpp`, `NEW src/sim/ArmyTransit.cpp`
- `NEW include/game/sim/OrderIntent.hpp`, `NEW include/game/sim/CommandQueue.hpp`, `NEW src/sim/CommandQueue.cpp`
- `NEW include/game/sim/MovementSystem.hpp`, `NEW src/sim/MovementSystem.cpp`
- `NEW include/game/sim/Pathfinder.hpp`, `NEW src/sim/Pathfinder.cpp`
- `NEW include/game/sim/CombatSystem.hpp`, `NEW src/sim/CombatSystem.cpp`
- `NEW include/game/sim/RegenSystem.hpp`, `NEW src/sim/RegenSystem.cpp`
- `NEW include/game/sim/VictorySystem.hpp`, `NEW src/sim/VictorySystem.cpp`
- `NEW include/game/ai/AiDirector.hpp`, `NEW src/ai/AiDirector.cpp`
- `NEW include/game/ui/InputController.hpp`, `NEW src/ui/InputController.cpp`
- `NEW include/game/ui/Renderer.hpp`, `NEW src/ui/Renderer.cpp`
- `NEW include/game/ui/HoverPanel.hpp`, `NEW src/ui/HoverPanel.cpp`
- `NEW tests/sim_rules_test.cpp`, `NEW tests/ai_behavior_test.cpp`, `NEW tests/victory_conditions_test.cpp`, `NEW tests/input_command_test.cpp`

## Target Experience
- Match length: 10–18 minutes for the MVP map.
- Feel: immediate and readable, closer to a streamlined real-time territory strategy game than a full military sim.
- Inputs: mouse-first, low APM, high readability.

## Core Gameplay Loop
1. Player selects a nation from 4 choices on the title/setup screen.
2. Match starts on one handcrafted square-tile map with each nation owning a starting region and capital, with remaining frontier tiles beginning neutral.
3. Player clicks an owned tile to select it, then clicks another tile to issue a move/transfer/attack order.
4. Troops travel in real time along tile paths.
5. Friendly destination tiles merge forces; enemy destination tiles trigger combat.
6. Hovering any tile shows owner, troop count, terrain modifier, and capital/regen relevance.
7. Capitals regenerate troops continuously; more owned territory increases regeneration throughput.
8. Match ends when a capital is captured or a nation’s total troops drop to zero.

## MVP Ruleset Decisions
### Map + tiles
- One square-grid map for v1 vertical slice.
- Recommended MVP size: **18 x 12** tiles.
- Tile properties: owner, troop count, terrain type, capital flag, adjacency, selection state.
- Use **4-direction orthogonal adjacency only** in the MVP; no diagonal movement.
- Movement route for a command is computed by deterministic BFS over passable adjacent tiles at order issue time.
- Deterministic neighbor expansion order for pathfinding is **North -> East -> South -> West** so equal-length routes resolve predictably.
- Terrain set for MVP:
  - `Plains`: no modifier
  - `Road`: +movement speed bonus
  - `Highland`: +defense bonus
  - `Capital`: stronger defense + regeneration anchor

### Simulation boundaries
- `WorldState` is the single authoritative gameplay state for owned tiles, capitals, troop counts, active transits, and defeat flags.
- UI and AI must not mutate `WorldState` directly; they emit `OrderIntent` objects into a `CommandQueue`.
- `MovementSystem`, `CombatSystem`, `RegenSystem`, and `VictorySystem` consume queued commands on a fixed simulation tick.
- Moving troops between tiles are represented as `ArmyTransit` records so rendering and tests can inspect in-flight forces without touching tile garrisons.
- Renderer/HUD consume a read-only snapshot view of the sim state.

### Nation identities
| Nation | Identity | Mobility | Attack | Defense | Regen | AI bias |
|---|---|---:|---:|---:|---:|---|
| Swift League | expansion / flanking | 1.30 | 0.90 | 0.85 | 1.00 | grabs weak or neutral edges quickly |
| Iron Legion | offensive pressure | 0.95 | 1.25 | 0.95 | 0.95 | concentrates attacks on weakest front |
| Bastion Directorate | attrition / defense | 0.80 | 0.95 | 1.30 | 1.00 | fortifies chokepoints, counters only on advantage |
| Crown Consortium | macro / reserve play | 1.05 | 0.95 | 1.05 | 1.20 | expands steadily, protects supply, then strikes opportunistically |

### Command model
- Left click owned tile: select origin.
- Left click second tile: issue order from selected origin.
- Default send amount: 50% of origin troops.
- Hotkeys `1 / 2 / 3` switch send ratio to 25% / 50% / 100%.
- The currently selected send ratio must be visible in the HUD so click behavior stays predictable.
- Clicking a friendly tile transfers troops.
- Clicking an enemy tile sends an assault force.
- Right click or `Esc`: clear selection.

### Combat model
- Use deterministic fixed-tick combat, not particle-by-particle simulation.
- When an army reaches an enemy tile, combat resolves on sim ticks using attacker attack multiplier, defender defense multiplier, terrain bonus, and troop counts.
- If defender troops reach 0, surviving attackers occupy the tile.
- If attackers reach 0 first, the tile remains under defender control.

### Regeneration model
- Only capitals generate new troops in the MVP.
- Capital regeneration formula should increase with territory count, e.g.:
  - `regen_per_second = base_regen + owned_tiles * territory_factor * nation_regen_modifier`
- Start with global caps to prevent runaway snowballing:
  - capital garrison cap
  - optional reserve overflow denial if cap reached

### AI model
- Real-time utility AI reevaluates every **8 simulation ticks at a 10 Hz sim rate** (~0.8s).
- Each AI scores candidate actions: defend capital, reinforce frontline, capture neutral tile, counterattack weak enemy tile, regroup.
- Nation identity adjusts score weights rather than using separate hardcoded AI trees.
- AI uses a minimum 2-second commitment window for non-emergency orders to reduce thrashing.
- Capital-threat states may preempt commitment windows and force immediate defensive retargeting.

### Victory resolution rule
- Capital capture defeats that nation immediately, even if remote armies still exist.
- If a nation reaches 0 total troops, it loses immediately even if its capital still stands.
- If multiple defeat events happen on the same simulation tick, resolve in this order:
  1. capital-capture defeats
  2. zero-troop defeats
  3. if only one nation remains, it wins
  4. if multiple nations still remain, continue the match

## Scope
### In scope
- One playable map
- One-player vs 3 AI nations
- Nation select screen
- Real-time tile selection, movement, and combat
- Hover tooltip / tile info panel
- Nation asymmetry via stats + AI weighting
- Capital-based regeneration scaled by territory
- Win/loss flow and match restart
- Deterministic simulation core with automated tests

### Out of scope (MVP)
- Multiplayer / networking
- Fog of war
- Campaign mode
- Multiple maps or map editor
- Save/load
- Complex diplomacy or technology tree
- Advanced audiovisual polish beyond readability

## Acceptance Criteria
1. The player can choose any of 4 nations before a match starts.
2. A single match launches into a 2D tile map with 4 starting capitals and initial troop allocations.
3. Hovering a tile reveals at least owner, troop count, terrain modifier, and whether it is a capital.
4. Selecting an owned tile and then another tile issues a real-time move/attack order.
5. Friendly tiles merge troops correctly; enemy tiles resolve combat and can change ownership.
6. Each nation has distinct mobility/attack/defense tuning that meaningfully changes AI and play feel.
7. AI opponents issue orders continuously during play and adapt after front-line changes.
8. Capital regeneration increases as owned territory grows and is visibly reflected in troop replenishment.
9. Victory triggers immediately on capital capture or complete troop elimination.
10. Simulation rules are deterministic enough to support repeatable automated tests.

## Implementation Plan (3–6 steps)
1. **Bootstrap the playable shell**
   - Create `NEW CMakeLists.txt`, app entrypoint, window loop, and scene flow for nation selection -> match -> result screen.
2. **Build deterministic simulation systems**
   - Implement `WorldState`, `ArmyTransit`, `OrderIntent`, `CommandQueue`, pathfinding, movement, combat, regeneration, and victory evaluation as fixed-tick systems with no rendering dependencies.
3. **Add player interaction + rendering**
   - Implement tile rendering, nation colors, selection highlighting, visible send-ratio state, hover tooltip, and click-to-command input.
4. **Add AI strategy profiles + asymmetry tuning**
   - Implement utility-based AI with nation-specific score weights and balance constants.
5. **Lock behavior with automated verification + match polish**
   - Add deterministic simulation tests, AI sanity tests, and manual playtest checklist; tune pacing/readability until acceptance criteria pass.

## Risks and Mitigations
- **Risk:** Real-time AI becomes noisy or chaotic.
  - **Mitigation:** Use action cooldowns, score thresholds, and minimum force ratios.
- **Risk:** Territory-scaled regeneration causes runaway snowballing.
  - **Mitigation:** Add capital cap, conservative territory factor, and tune with test scenarios.
- **Risk:** Pathing and command resolution become ambiguous during busy battles.
  - **Mitigation:** Fix orthogonal BFS pathing, centralize commands through `CommandQueue`, and test command acceptance separately.
- **Risk:** UI clarity suffers when many armies move.
  - **Mitigation:** Keep armies numeric and tile-centric in MVP; avoid per-unit sprite clutter.
- **Risk:** Sim/render coupling slows iteration.
  - **Mitigation:** Keep simulation deterministic and independent from rendering/input modules.

## Verification Steps
- Configure build via CMake and run headless simulation tests through CTest.
- Prove deterministic combat/movement/regen/pathing with repeatable seeded scenarios.
- Manual QA: verify selection, hover info, AI activity, victory conditions, and restart flow.

## Open Questions (safe defaults chosen for now)
- Framework choice is recommended as SFML 3.x, but an alternative can be substituted before execution if needed.
- Art/audio style remains minimalist for MVP.
- Additional maps and economy layers are deferred until post-vertical-slice review.
