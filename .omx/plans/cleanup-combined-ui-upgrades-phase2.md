# Cleanup / Refactor Plan — Combined UI Upgrades Phase 2

## Scope
- `assets/fonts/*`
- `include/game/ui/Renderer.hpp`
- `src/ui/Renderer.cpp`
- `src/App.cpp`
- `tests/renderer_layout_test.cpp` (new)
- `docs/ui-style-guide.md` (new)
- `docs/playtest-ui-balance-checklist.md` (new)
- `CMakeLists.txt`

## Behavior Lock First
1. Add renderer layout regression tests before renderer decomposition.
2. Keep existing build + CTest suite green before and after refactor.

## Smell-by-Smell Order
1. **Missing behavior lock**
   - Add tests for nation-select hit targets and map hit-testing geometry.
2. **Unsafe asset source**
   - Replace machine-derived fonts with redistributable fonts and document fallback policy.
3. **Needless concentration**
   - Split `Renderer.cpp` by responsibility (core/layout, nation select, match HUD, debrief).
4. **Visual polish pass**
   - Add lightweight SFML-native emphasis only after refactor stabilizes.
5. **UI balance support**
   - Add style/playtest docs and tune spacing/readability without changing gameplay rules.

## Quality Gates
- `cmake --build build -j4 --target territory_war territory_war_tests`
- `ctest --test-dir build --output-on-failure`
- runtime smoke for `./build/territory_war`
- diagnostics on touched C++ files where available
