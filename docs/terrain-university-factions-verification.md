# Terrain + University Factions Verification Guide

Use this guide when validating the `terrain-university-factions-update` branch or any integrated build that includes it.
The goal is to keep the new terrain semantics deterministic while making the university retheme readable, playful, and respectful.

## Scope
- New terrain semantics: `Mountain`, `Sea`, existing `Road` / `Plains` / `Highland` / `Capital`
- University faction retheme: `서강대학교`, `성균관대학교`, `한양대학교`, `중앙대학교`
- Deterministic orthogonal pathing and terrain bottleneck launch rules
- Handcrafted map readability and non-rectangular starting regions

## Automated Verification Commands
Run these commands from repo root:

```bash
cmake -S . -B build
cmake --build build -j4 --target territory_war territory_war_tests
ctest --test-dir build --output-on-failure
```

## Automated Evidence Matrix
Capture the exact command outputs above and map them to the following acceptance gates.

| Acceptance gate | Expected evidence |
| --- | --- |
| Sea blocks normal land pathing | deterministic sim test proves path/order rejection across sea |
| Mountain slows movement | deterministic sim test compares mountain travel against plains/road baseline |
| Terrain bottleneck caps launched troops | deterministic sim test checks launched troop count uses the strictest traversed-tile cap |
| University rename is visible player-facing | tests assert doctrine-card / runtime labels use the four university names |
| Terrain labels stay consistent | tests assert hover/terrain-name output includes Mountain and Sea |
| Existing regression coverage stays green | full `ctest` pass with no unrelated regressions |

## Automated Test Targets
The current deterministic regression pass should call out the concrete tests covering this feature:

- `tests/sim_rules_test.cpp`
  - `sim_world_layout_adds_sea_mountain_and_irregular_opening_territories`
  - `sim_pathfinder_rejects_sea_barriers_and_order_state_remains_stable`
  - `sim_mountain_paths_reduce_speed_and_throughput_relative_to_open_routes`
- `tests/renderer_layout_test.cpp`
  - `renderer_player_facing_names_follow_university_retheme`
  - `renderer_hover_lines_surface_university_names_and_new_terrain_labels`

## Manual Smoke Additions
After automated checks pass, launch the game and verify the following manually:

1. Nation select shows the four university names and distinct strategic identities without mocking language.
2. Hovering tiles reveals `Mountain` and `Sea` labels where expected.
3. Sea visually reads as a hard barrier and click-to-move orders do not create illegal land crossings.
4. Mountain routes feel slower than roads/plains and visibly communicate a constrained lane.
5. Sending 25%, 50%, and 100% orders through a narrow mountain corridor launches fewer troops than an open road route when the path bottleneck differs.
6. Opening territories read as handcrafted blobs/coastlines rather than four simple rectangles.
7. Every faction still has a readable outward lane from its capital; no capital appears visually stranded.

## Respectful Content Check
During review, confirm all player-facing strings satisfy the PRD guardrail:
- gameplay-archetype language only
- no insults, stereotypes, or factual claims about the institutions
- doctrine text stays broad, playful, and strategy-focused

## Reporting Template
When this feature is integrated, record results in the following format:

```text
Verification:
- PASS/FAIL cmake configure — <command/output summary>
- PASS/FAIL build — <command/output summary>
- PASS/FAIL ctest — <command/output summary>
- PASS/FAIL runtime smoke — <bounded manual summary>
- PASS/FAIL respectful-content review — <notes>
```

## Integration Note
This guide is intentionally stricter than the legacy smoke list: do not sign off on the terrain update until both the automated gates and the manual terrain/faction checks are captured together.
