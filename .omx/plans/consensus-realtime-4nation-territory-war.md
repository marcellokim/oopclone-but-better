# Consensus Plan — Realtime 4-Nation Territory War

## Plan Summary
- **Artifacts:**
  - `.omx/plans/prd-realtime-4nation-territory-war.md`
  - `.omx/plans/test-spec-realtime-4nation-territory-war.md`
  - `.omx/plans/consensus-realtime-4nation-territory-war.md`
- **Repo fact:** greenfield repository; only `.omx/` exists today.
- **Complexity:** Medium-high greenfield feature set, but suitable for a solo vertical slice if scoped to one map and deterministic systems.
- **Recommended stack:** C++20 + SFML 3.x + CMake + CTest, with a deterministic sim core separated from rendering/input.

## Requirements Summary
Create a 2D real-time territory-control war game where one human-selected nation fights 3 AI nations on a tile map. Units move in real time, combat on enemy contact, and regenerate from capitals according to territory owned. Nations differ strategically through mobility / attack / defense / regeneration tuning and AI behavior weighting. Victory occurs on capital capture or full military elimination.

## RALPLAN-DR Summary
### Principles
1. Keep the simulation deterministic and renderer-agnostic.
2. Ship a playable vertical slice before expanding features.
3. Optimize for readability of territory state and troop information.
4. Express nation asymmetry through data and AI weighting, not bespoke rule forks.
5. Keep dependencies and asset requirements minimal for a greenfield solo build.

### Decision Drivers
1. **Solo-achievable scope:** the first version must stay small enough to actually finish.
2. **Rule clarity:** real-time outcomes must remain readable and testable.
3. **Extensibility:** the MVP should allow later maps, tuning, and richer AI without rewrites.

### Viable Options
| Option | Description | Pros | Cons |
|---|---|---|---|
| A (favored) | **SFML 3.x + deterministic custom sim core** | Idiomatic C++ workflow; simple 2D rendering/input/audio; easier onboarding for a greenfield game; clean separation between simulation and presentation | Slightly higher-level abstraction may hide some low-level control; still requires custom UI/gameplay architecture |
| B | **raylib + thin C++ wrapper + deterministic custom sim** | Very lightweight, fast to prototype, game-oriented drawing helpers | Core API is C-first rather than C++-idiomatic; larger codebase conventions may need more wrapping for clean architecture |
| C | **SDL3 + custom rendering/UI layer + deterministic sim** | Lowest-level flexibility and broad portability | More boilerplate for rendering/UI/event handling; slower path to first playable slice |

### Favored Direction
Choose **Option A: SFML 3.x + deterministic custom sim core**.

## Architect Review Notes
- **Antithesis (steelman):** SDL3 may be the safer long-term base if the project later grows into a more custom engine, because it avoids committing early to a higher-level rendering abstraction.
- **Tradeoff tension:** faster MVP delivery (SFML) versus lower-level control and potentially broader future portability (SDL3).
- **Synthesis:** keep the simulation, AI, and command systems framework-agnostic so the window/render/input layer can be swapped later if the project outgrows SFML.

## Critic Gate Summary
- **Principle/option consistency:** Pass — favored option best matches solo-scope, readability, and low-boilerplate goals.
- **Alternatives depth:** Pass — three viable framework approaches considered with bounded pros/cons.
- **Risk/verification rigor:** Pass — plan includes deterministic tests, manual acceptance checks, and clear MVP exclusions.
- **Open caution:** keep module boundaries strict so SFML does not leak into simulation rules.

## Acceptance Criteria
1. Player can select 1 of 4 nations before entering the match.
2. One match launches on a single 2D tile map with 4 capitals and neutral frontier tiles.
3. Hovering a tile shows owner, troop count, terrain effect, and capital status.
4. Clicking from an owned tile to a friendly tile transfers troops; clicking to an enemy tile attacks.
5. Troop movement, combat, regeneration, and victory all update in real time.
6. Nation asymmetry is perceptible through stat tuning, regen differences, and AI behavior.
7. AI opponents continually issue and revise orders during the match.
8. Territory growth increases capital regeneration.
9. Capital capture and total troop loss both trigger defeat correctly.
10. Automated deterministic tests cover core simulation rules.

## Proposed NEW File Layout
- `NEW CMakeLists.txt`
- `NEW src/main.cpp`
- `NEW include/game/App.hpp`, `NEW src/App.cpp`
- `NEW include/game/GameConfig.hpp`, `NEW src/GameConfig.cpp`
- `NEW include/game/sim/{WorldState,ArmyTransit,OrderIntent,CommandQueue,Pathfinder,MovementSystem,CombatSystem,RegenSystem,VictorySystem}.hpp`
- `NEW src/sim/{WorldState,ArmyTransit,CommandQueue,Pathfinder,MovementSystem,CombatSystem,RegenSystem,VictorySystem}.cpp`
- `NEW include/game/ai/AiDirector.hpp`, `NEW src/ai/AiDirector.cpp`
- `NEW include/game/ui/{InputController,Renderer,HoverPanel}.hpp`
- `NEW src/ui/{InputController,Renderer,HoverPanel}.cpp`
- `NEW tests/{sim_rules_test,ai_behavior_test,victory_conditions_test,input_command_test}.cpp`

## Architecture Guardrails
- `WorldState` owns all authoritative sim data; rendering and input never mutate it directly.
- Player input and AI both produce `OrderIntent` commands that flow through a shared `CommandQueue`.
- Movement uses deterministic 4-way BFS pathing on the square grid; no diagonal rules are introduced in the MVP.
- Equal-length path ties resolve in a fixed **North -> East -> South -> West** neighbor order.
- Moving troops exist as `ArmyTransit` records so in-flight forces remain visible to tests, HUD, and combat resolution.
- AI reevaluates on a fixed cadence with a minimum commitment window, except when the capital is under direct threat.

## Implementation Steps
1. **Project bootstrap + scene shell**
   - Add build configuration, app entrypoint, window lifecycle, and nation-select -> game -> result scene flow.
2. **Simulation core**
   - Implement world state, `OrderIntent`/`CommandQueue`, fixed-tick update loop, BFS pathing, movement orders, combat resolution, capital regen, and victory checks.
3. **Presentation + command UX**
   - Render the grid, nation colors, selection highlight, visible send-ratio state, troop labels, hover tooltip, and click command flow.
4. **AI + nation identity tuning**
   - Add utility-based AI with nation-weight profiles, regeneration modifiers, conservative action cooldowns, and commitment windows to avoid thrashing.
5. **Verification + polish**
   - Add deterministic tests, run manual playtest script, and tune pacing/readability without expanding feature scope.

## Risks and Mitigations
| Risk | Why it matters | Mitigation |
|---|---|---|
| AI thrashes between objectives | makes game feel random | action cooldowns + commitment threshold |
| Regen snowball dominates outcomes | weakens strategy variety | territory factor caps + test fixtures |
| Pathing/command rules stay underspecified | causes implementation churn | explicit `CommandQueue`, orthogonal BFS, and command tests |
| UI overload on busy fronts | hurts readability | numeric/tile-centric presentation, minimal clutter |
| Framework leaks into sim layer | hurts testability | pure sim systems with framework-free interfaces |

## Verification Steps
1. Configure and build with CMake.
2. Run deterministic simulation tests via CTest.
3. Manually validate one full match for selection, hover info, AI activity, regeneration scaling, and both victory paths.
4. Confirm nation asymmetry produces visibly different play patterns over repeated short runs.

## ADR
### Decision
Build the MVP as a **single-map vertical slice in C++20 using SFML 3.x, CMake, and a deterministic custom simulation core**.

### Drivers
- Minimize boilerplate for a greenfield solo build.
- Preserve deterministic, testable rules.
- Keep room for future content/tuning without rewriting the sim.

### Alternatives considered
- raylib-based MVP with thin C++ wrapper
- SDL3-based MVP with custom rendering/UI layer

### Why chosen
SFML best balances C++ ergonomics, 2D rendering/input convenience, and speed-to-first-playable while still allowing the simulation to remain decoupled.

### Consequences
- Positive: faster playable prototype, lower implementation overhead, clearer code organization for a solo builder.
- Negative: rendering/input layer depends on a higher-level framework choice that may need replacement later if engine demands grow.

### Follow-ups
- Validate the first playable loop before adding multiple maps or deeper economies.
- Externalize balance data after the vertical slice proves fun.
- Reassess framework choice only if performance or extensibility problems actually appear.

## Available-Agent-Types Roster
- `planner`
- `architect`
- `critic`
- `executor`
- `debugger`
- `verifier`
- `test-engineer`
- `designer`
- `writer`
- `build-fixer`
- `security-reviewer`
- `code-reviewer`
- `researcher`
- `explore`

## Follow-up Staffing Guidance
### If handed to `$ralph`
- **Lane 1 — implementation owner:** `executor` (high) for project bootstrap, sim systems, rendering/input, and AI integration.
- **Lane 2 — regression evidence:** `test-engineer` (medium) for deterministic test design and acceptance coverage.
- **Lane 3 — final sign-off:** `architect` (medium/high) plus `verifier` (high) for boundary review and evidence validation.
- **Why this works:** Ralph is best when one owner drives a sequential build with explicit verification gates.

### If handed to `$team`
- **Recommended headcount:** 3 workers.
- **Worker mix:**
  1. `executor` (high) — bootstrap/build/app shell + rendering/input lane
  2. `executor` (high) — simulation/AI systems lane
  3. `test-engineer` (medium) — deterministic tests + manual QA checklist lane
- **Runtime fallback note:** current `omx team` launch uses one shared worker role prompt, so if mixed roles are unavailable at launch time, use `3:executor` and dedicate worker 3 to the verification lane via task assignment.
- **Optional support after first pass:** `build-fixer` (high) if CMake/toolchain issues stall progress; `verifier` (high) for release evidence before shutdown.
- **Why each lane exists:** split rendering/UI from sim/AI to reduce merge pressure, while keeping one dedicated verification lane active throughout.

## Launch Hints
### Ralph handoff hint
```bash
$ralph implement .omx/plans/prd-realtime-4nation-territory-war.md with test spec .omx/plans/test-spec-realtime-4nation-territory-war.md
```

### Team handoff hints
```bash
$team 3:executor "Implement the approved realtime-4nation-territory-war PRD and keep one lane focused on verification using .omx/plans/prd-realtime-4nation-territory-war.md and .omx/plans/test-spec-realtime-4nation-territory-war.md"
```

```bash
omx team 3:executor "Implement the approved realtime-4nation-territory-war PRD and keep one verification lane active using the plan files under .omx/plans/"
```

## Team Verification Path
1. **Worker evidence before shutdown**
   - bootstrap lane proves project config/build works
   - sim/AI lane proves deterministic system behavior and nation asymmetry are implemented
   - verification lane proves automated tests and manual checklist results are recorded
2. **Leader completion gate**
   - no pending or in-progress team tasks
   - build succeeds
   - CTest passes
   - manual match checklist covers hover, command flow, regeneration scaling, AI adaptation, and both victory paths
3. **Post-team optional Ralph follow-up**
   - only if verification finds unresolved issues that need a single-owner cleanup loop

## Changelog (consensus-ready draft)
- Chose a vertical-slice-first scope instead of a broad full RTS roadmap.
- Standardized on deterministic simulation boundaries.
- Added explicit agent roster, staffing guidance, launch hints, and team verification path for execution handoff.
- Added architecture guardrails for authoritative sim state, command queue boundaries, orthogonal BFS pathing, and anti-thrashing AI cadence.
- Clarified deterministic path tie-break order and the shared-role fallback for current team-runtime launch constraints.
