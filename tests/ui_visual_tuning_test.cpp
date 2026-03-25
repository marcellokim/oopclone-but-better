#include "game/ui/VisualTuning.hpp"
#include "test_harness.hpp"

TEST_CASE(ui_tile_tuning_prioritizes_selection_over_hover) {
    const auto hovered = game::ui::tileVisualTuning(false, true, false, 0.8F);
    const auto selected = game::ui::tileVisualTuning(true, false, false, 0.8F);

    test::require(selected.outlineThickness > hovered.outlineThickness, "selection should have a stronger outline than hover");
    test::require(selected.glowAlpha > hovered.glowAlpha, "selection glow should dominate hover glow");
    test::require(selected.troopChipAlpha > hovered.troopChipAlpha, "selection should improve troop-chip contrast");
}

TEST_CASE(ui_tile_tuning_keeps_capitals_distinct_under_selection) {
    const auto selectedCapital = game::ui::tileVisualTuning(true, false, true, 1.1F);
    const auto selectedNormal = game::ui::tileVisualTuning(true, false, false, 1.1F);

    test::require(selectedCapital.capitalHaloAlpha > 0, "capital tiles should keep a readable halo");
    test::require(selectedCapital.capitalBeaconHeight > 0.F, "capital tiles should keep a vertical beacon cue");
    test::require(selectedCapital.capitalHaloAlpha > selectedNormal.capitalHaloAlpha,
                  "capital emphasis should survive while the tile is selected");
}

TEST_CASE(ui_transit_tuning_stays_within_readable_bounds) {
    const auto transit = game::ui::transitVisualTuning(0.4F);

    test::require(transit.haloThickness > transit.beamThickness, "transit halo should stay wider than the core path");
    test::require(transit.tokenRadius >= 10.5F && transit.tokenRadius <= 12.7F, "token pulse should remain subtle");
    test::require(transit.labelChipAlpha >= 182 && transit.labelChipAlpha <= 206,
                  "transit label chip should stay opaque enough for battlefield readability");
}
