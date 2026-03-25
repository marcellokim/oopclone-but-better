#pragma once

#include <cstdint>

namespace game::ui {

struct TileVisualTuning {
    float fillBoost{1.F};
    float innerBoost{0.9F};
    std::uint8_t innerAlpha{220};
    float outlineThickness{1.F};
    std::uint8_t outlineAlpha{95};
    float glowPadding{0.F};
    std::uint8_t glowAlpha{0};
    std::uint8_t troopChipAlpha{175};
    std::uint8_t troopChipOutlineAlpha{40};
    float capitalHaloScale{0.F};
    std::uint8_t capitalHaloAlpha{0};
    float capitalBeaconHeight{0.F};
    float capitalCrestScale{1.F};
};

struct TransitVisualTuning {
    float beamThickness{2.4F};
    float haloThickness{6.F};
    float tokenRadius{10.F};
    std::uint8_t beamAlpha{140};
    std::uint8_t haloAlpha{52};
    std::uint8_t labelChipAlpha{182};
};

[[nodiscard]] TileVisualTuning tileVisualTuning(bool selected, bool hovered, bool capital, float timeSeconds);
[[nodiscard]] TransitVisualTuning transitVisualTuning(float timeSeconds);
[[nodiscard]] float nationCardAccentPulse(bool highlighted, float timeSeconds);

} // namespace game::ui
