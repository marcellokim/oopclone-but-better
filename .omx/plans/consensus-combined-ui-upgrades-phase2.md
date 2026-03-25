# Consensus Plan — Combined UI Upgrades Phase 2

## Plan Summary
- **Artifacts:**
  - `.omx/plans/prd-combined-ui-upgrades-phase2.md`
  - `.omx/plans/test-spec-combined-ui-upgrades-phase2.md`
  - `.omx/plans/consensus-combined-ui-upgrades-phase2.md`
- **Repo fact:** brownfield SFML strategy prototype with validated gameplay and an already-completed first-pass redesign.
- **Complexity:** Medium. This is a coordinated polish/productization increment across UI assets, structure, and validation workflow.
- **Recommended stance:** one combined phase-2 increment, executed as a controlled brownfield refinement rather than four disconnected mini-projects.

## Requirements Summary
Plan a second-phase UI/productization increment that combines font safety, renderer maintainability, lightweight polish, and actual-play UI tuning without altering the core gameplay/sim behavior.

## RALPLAN-DR Summary
### Principles
1. Preserve gameplay and sim behavior; change presentation, not core rules.
2. Prefer incremental brownfield refinement over renderer/engine rewrites.
3. Keep the map and command readability above decorative flair.
4. Use redistributable assets and documented fallbacks for long-term portability.
5. Ground polish/tuning decisions in repeatable playtest evidence, not aesthetics alone.

### Decision Drivers
1. **Portability:** current font handling is workable locally but weak for redistribution.
2. **Maintainability:** the renderer is now valuable but too concentrated in one file.
3. **Readability under pressure:** phase-2 polish must improve real play, not just static appearance.

### Viable Options
| Option | Description | Pros | Cons |
|---|---|---|---|
| A (favored) | **One integrated phase-2 increment** covering fonts, renderer decomposition, light polish, and playtest-driven tuning | Coherent design language, one regression cycle, avoids duplicated rework | Requires slightly broader coordination in one pass |
| B | **Sequential micro-phases** (fonts first, then refactor, then polish, then balance) | Lower local risk per pass, easier to stop between steps | More repeated verification, more churn, higher chance of aesthetic drift |
| C | **Large renderer/theme rewrite** with deeper system changes | Highest long-term cleanliness potential | Too broad for current brownfield scope; risks destabilizing validated gameplay/UI |

### Favored Direction
Choose **Option A: one integrated phase-2 brownfield increment** with strict scope control and shared verification.

## Architect Review Notes
- **Antithesis (steelman):** Sequential micro-phases reduce blast radius and make each verification cycle easier to attribute when something breaks.
- **Tradeoff tension:** integrated coherence and reduced repeated churn vs smaller, easier-to-debug slices.
- **Synthesis:** keep the work as one planned increment, but execute internally in ordered sub-phases (fonts -> renderer decomposition -> polish -> UI-balance tuning) with verification gates between them.

## Critic Gate Summary
- **Principle/option consistency:** Pass — the favored path best fits brownfield constraints and coherence goals.
- **Alternatives depth:** Pass — at least three realistic execution strategies considered.
- **Risk/verification rigor:** Pass — build/test/runtime/playtest verification is explicit and testable.
- **Open caution:** keep UI-balance tuning scoped to readability and information hierarchy, not gameplay rebalance.

## Acceptance Criteria
1. The next increment addresses all four follow-up themes in one coordinated plan.
2. Renderer decomposition remains brownfield-safe and preserves hit-test correctness.
3. Font strategy becomes redistribution-safe with explicit fallback behavior.
4. Added polish remains lightweight and SFML-native.
5. UI balance/readability tuning is backed by structured playtest scenarios.
6. Build/tests/runtime smoke remain the completion gate.

## Existing Facts vs Proposed NEW Changes
### Existing facts
- Current redesign already introduced tactical-command-deck structure.
- Renderer and App boundaries are workable.
- Validation already approved the redesign with non-blocking follow-ups.

### Proposed NEW / changed work
- `CHANGED assets/fonts/*` with redistributable replacements
- `CHANGED include/game/ui/Renderer.hpp`
- `CHANGED src/ui/Renderer.cpp`
- `CHANGED src/App.cpp` as needed for UI orchestration/fallbacks
- `NEW/CHANGED docs/ui-style-guide.md`
- `NEW/CHANGED docs/playtest-ui-balance-checklist.md`
- `NEW/CHANGED tests/*` only where testable UI-support logic exists

## Implementation Steps
1. **Replace fonts with redistribution-safe assets**
   - Select OFL-safe display/body/mono fonts and document fallback order.
2. **Refactor renderer by screen/responsibility**
   - Separate shared geometry/helpers from nation-select, match, and debrief rendering paths.
3. **Add targeted polish**
   - Layer subtle hover/selection/transit/debrief emphasis without introducing heavy animation infrastructure.
4. **Tune UI balance through playtest scenarios**
   - Run structured readability scenarios and adjust density, spacing, contrast, and emphasis.
5. **Re-verify everything**
   - Build, tests, runtime smoke, and playtest evidence all pass before completion.

## Risks and Mitigations
| Risk | Why it matters | Mitigation |
|---|---|---|
| Renderer refactor breaks hit-testing | user input becomes unreliable | shared geometry source for draw + hit-test |
| New fonts solve style but create licensing issues | weak distribution story | choose redistributable/OFL fonts only; document sources |
| Polish reduces legibility | game becomes harder to read | map-first hierarchy + playtest-driven tuning |
| “UI balance” expands into gameplay balance | scope creep | restrict tuning to readability, density, contrast, and presentation feedback |

## Verification Steps
1. `cmake -S . -B build`
2. `cmake --build build -j4 --target territory_war territory_war_tests`
3. `ctest --test-dir build --output-on-failure`
4. runtime smoke for `./build/territory_war`
5. execute the new UI-balance/playtest checklist and record the chosen adjustments

## ADR
### Decision
Execute the four follow-up proposals as **one integrated brownfield phase-2 UI/productization increment**.

### Drivers
- Solve font portability, renderer maintainability, polish, and readability together rather than repeatedly touching the same surfaces.
- Preserve current gameplay and validation confidence.
- Keep scope tight enough for execution without engine-level churn.

### Alternatives considered
- sequential micro-phases
- large renderer/theme rewrite

### Why chosen
A single integrated increment keeps the design system coherent, reduces repeated regression churn, and best matches the existing brownfield renderer boundary.

### Consequences
- Positive: one coherent verification cycle, tighter visual consistency, less duplicated rework.
- Negative: requires disciplined internal sequencing and careful scope control.

### Follow-ups
- Later consider a separate third phase for deeper renderer modularization only if phase-2 still leaves the file too large.
- Track font licensing/source documentation as a release-readiness item.

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
- **Lane 1 — implementation owner:** `executor` (high) for fonts, renderer decomposition, and UI polish.
- **Lane 2 — evidence lane:** `test-engineer` (medium) for regression strategy + playtest checklist + acceptance coverage.
- **Lane 3 — final sign-off:** `architect` (high) plus `verifier` (high) for brownfield-boundary review and final evidence validation.
- **Why this works:** the phase is integrated but still compact enough for a strong single-owner execution loop with explicit verification gates.

### If handed to `$team`
- **Recommended headcount:** 3 workers.
- **Worker mix:**
  1. `executor` (high) — font assets + renderer decomposition lane
  2. `executor` (high) — polish + UI-balance tuning lane
  3. `test-engineer` (medium) — regression + playtest evidence lane
- **Optional support:** `security-reviewer` (medium) for asset/source review; `build-fixer` (high) if build/toolchain issues appear.
- **Why each lane exists:** split maintainability work from polish/tuning while keeping one lane dedicated to regression and playtest evidence.

## Launch Hints
### Ralph handoff hint
```bash
$ralph implement .omx/plans/prd-combined-ui-upgrades-phase2.md with test spec .omx/plans/test-spec-combined-ui-upgrades-phase2.md
```

### Team handoff hints
```bash
$team 3:executor "Implement the approved combined-ui-upgrades-phase2 plan using .omx/plans/prd-combined-ui-upgrades-phase2.md and .omx/plans/test-spec-combined-ui-upgrades-phase2.md while keeping one lane dedicated to verification evidence."
```

```bash
omx team 3:executor "Implement the approved combined-ui-upgrades-phase2 plan with fonts, renderer decomposition, polish, and playtest-driven UI tuning while one lane owns verification."
```

## Team Verification Path
1. **Worker evidence before shutdown**
   - asset/refactor lane proves the new font strategy and renderer decomposition compile and preserve hit-testing
   - polish/tuning lane proves the UI improvements are implemented and documented against playtest scenarios
   - verification lane proves build/tests/runtime smoke/playtest checklist results are recorded
2. **Leader completion gate**
   - no pending or in-progress team tasks
   - build succeeds
   - CTest passes
   - runtime smoke passes
   - playtest/readability checklist results are captured with resulting tuning choices
3. **Post-team optional Ralph follow-up**
   - only if the combined increment reveals leftover issues better suited to a single-owner cleanup loop

## Changelog (consensus-ready draft)
- Combined four approved follow-up suggestions into one coherent phase-2 increment.
- Added explicit brownfield guardrails so this stays out of gameplay/sim rewrites.
- Added playtest-driven UI-balance as a first-class verification requirement.
- Added staffing/launch/verification guidance for both Ralph and Team follow-up execution.
