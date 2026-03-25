# Autopilot Spec — Tactical Command Deck Redesign

## Input
User request: redesign the existing game using design-related skills.

## Context
- Context snapshot: `.omx/context/realtime-4nation-territory-war-redesign-20260325T060312Z.md`
- Brownfield app: SFML/C++ strategy prototype with working gameplay, tests, and runtime smoke.
- Main redesign surface: `src/ui/Renderer.cpp` with light orchestration changes in `src/App.cpp`.

## Requirements Summary
### Preserve
- Existing gameplay rules, controls, fixed-tick simulation, AI, and victory logic.
- Click-to-command interaction, hover info, send-ratio keys, and reset flow.
- Build/test/runtime-smoke health.

### Upgrade
- Replace the current utilitarian UI with a premium **tactical command deck** presentation.
- Strengthen visual hierarchy across:
  1. nation select
  2. in-match HUD/map
  3. game-over/debrief
- Improve immediate readability for:
  - selected tile
  - hovered tile
  - capital status
  - nation ownership
  - troop counts and live fronts
- Introduce a more deliberate typography system and premium panel styling.
- Keep the map visually dominant; make the HUD read as subordinate instrumentation.

## Design Direction
### Mood
- Dark command-room / premium war-room / high-end tactical display
- Quiet, precise, cinematic, restrained

### Layout Archetype
- **Nation select:** editorial split + asymmetric dossier cards
- **Match:** map-first tactical cockpit with top strip, right-side command rail, and floating bottom command chip
- **Game over:** debrief overlay panel over the dimmed battlefield

### Typography
- Title/display font: premium serif/editorial or strong display face when available
- UI/body font: clean sans-serif
- Numbers: mono/tabular leaning font for operational data
- Font fallback must remain graceful if premium fonts are unavailable

### Color/Surface Rules
- No pure black; use charcoal / navy-black foundation
- Nation colors remain recognizable but slightly more controlled
- One global tactical highlight for hover/selection emphasis
- Panels use layered/double-bezel surfaces rather than flat boxes

### Motion / Feedback
- Lightweight only: hover, selection, emphasis, and overlay transitions implied through opacity, glow, and layered shapes
- No dependency-heavy animation system; stay inside SFML primitives

## Acceptance Criteria
1. Nation-select screen reads as a premium doctrine-selection screen rather than a plain menu.
2. The match screen clearly separates map, command context, hovered tile details, and live-fronts status.
3. Selected tiles, hovered tiles, capitals, and moving armies are more visually distinct than before.
4. The game-over screen reads as a debrief/result screen, not only a text overlay.
5. Typography, spacing, and panel styling feel materially more deliberate and premium.
6. Existing gameplay behavior remains unchanged.
7. `cmake`, build, tests, and runtime smoke still pass after redesign.

## Constraints
- No new code dependencies.
- Brownfield redesign only; no engine rewrite.
- Keep changes scoped primarily to renderer/UI orchestration.
- Maintain graceful fallback when fonts/assets are unavailable.

## Implementation Guardrails
- Layout geometry for nation-select hitboxes and map hit-testing must be computed from a shared renderer layout source of truth.
- Typography must use explicit role separation: display / body / numbers, with a documented fallback order.
- Decorative upgrades must preserve map readability at the fixed `1280x800` window size.
- Map remains the primary visual surface; panel growth must not crowd command clarity.
