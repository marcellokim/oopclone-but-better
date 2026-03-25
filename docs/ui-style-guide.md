# UI Style Guide — Phase 2

## Resolution Guardrail
- Design and readability remain anchored to **1280x800**.
- Keep the map as the primary visual object; supporting chrome must not overpower the battlefield.
- Preserve shared layout geometry for rendering and hit-testing.

## Font Roles
The premium UI path now uses bundled redistributable fonts only.

| Role | Primary | Fallback 1 | Fallback 2 |
| --- | --- | --- | --- |
| Display | IBM Plex Serif Regular | Lato Regular | IBM Plex Mono Regular |
| Body | Lato Regular | IBM Plex Serif Regular | IBM Plex Mono Regular |
| Mono | IBM Plex Mono Regular | Lato Regular | IBM Plex Serif Regular |

## Font Policy
- Do not reintroduce copied system fonts.
- Keep role fallback explicit in code so missing preferred assets degrade safely.
- If all bundled fonts fail to load, the app must still remain usable via the existing title-bar/textless fallback behavior.

## Renderer Structure
- Shared geometry helpers stay centralized in `Renderer` for both draw and hit-test paths.
- Screen-specific draw flows live in separate source files to keep nation-select, live-match, and debrief logic independently maintainable.
- Lightweight visual polish should stay SFML-native and subordinate to readability.
