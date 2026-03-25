# Tactical Command Deck UI Style Guide

This guide captures the phase-2 guardrails for the SFML tactical-command-deck UI so polish, font swaps, and renderer cleanup do not erode readability or input confidence.

## 1. Canvas + Geometry Guardrails
- Treat **1280x800** as the primary readability budget.
- Keep the **map** as the first-read focal surface; secondary chrome must not overpower the battlefield.
- Preserve a **single geometry source of truth** for anything the player can click or target. If a panel, tile, or card moves visually, its hit-testing must move from the same layout data.
- When tuning spacing, prefer trimming copy density or decorative weight before shrinking core playable surfaces.

## 2. Font Roles + Fallback Policy

| Role | Primary | Fallback 1 | Fallback 2 |
| --- | --- | --- | --- |
| Display | IBM Plex Serif Regular | Lato Regular | IBM Plex Mono Regular |
| Body | Lato Regular | IBM Plex Serif Regular | IBM Plex Mono Regular |
| Mono | IBM Plex Mono Regular | Lato Regular | IBM Plex Serif Regular |

- Maintain explicit font-role separation:
  - **Display** — headlines, doctrine names, result headlines
  - **Body** — descriptive copy, HUD copy, checklist text
  - **Mono** — telemetry, ratios, troop counts, compact labels
- The preferred phase-2 path is **bundled redistributable fonts**. Normal operation should not require machine-local system fonts.
- Keep an explicit fallback chain in code so the UI still renders if the preferred bundled family fails to load.
- Fallback behavior must remain **safe, legible, and role-aware** even when ideal typography is unavailable.

## 3. Readability Hierarchy
- **Selected tile > hovered tile** at all times.
- **Capital emphasis** must remain recognizable even while other emphasis layers are active.
- **Transit/path feedback** should read as motion support, not as a replacement for tile ownership or troop information.
- The **right rail** should support decisions without stealing first attention from the map.
- Troop chips, ownership tint, and command-preview copy should stay legible during congested fronts.

## 4. Motion + Polish Constraints
- Prefer lightweight **SFML-native** feedback: glow, emphasis, pulse, or restrained transition work.
- Keep motion **subtle, fast, and informative**; avoid ornamental animation loops that delay input parsing.
- Do not let polish obscure:
  - troop counts
  - selectable edges
  - capital markers
  - debrief result hierarchy

## 5. Renderer Structure
- Shared geometry helpers stay centralized in `Renderer` for both draw and hit-test paths.
- Screen-specific draw flows live in separate source files to keep nation-select, live-match, and debrief logic independently maintainable.
- `VisualTuning` is the testable polish layer; keep readability rules there rather than scattering pulse constants through layout code.

## 6. Verification Expectations
Phase-2 UI work is not complete until the following stay true together:
1. Build + regression commands pass.
2. Runtime smoke stays alive long enough to inspect the UI shell.
3. The playtest checklist is executed with concrete notes.
4. Any readability tuning deltas are recorded, not implied.
5. Regression confidence is stated explicitly before sign-off.
