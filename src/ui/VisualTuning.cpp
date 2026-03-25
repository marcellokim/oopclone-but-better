#include "game/ui/VisualTuning.hpp"

#include <cmath>

namespace game::ui {
namespace {

float unitPulse(const float timeSeconds, const float speed) {
    return 0.5F + 0.5F * std::sin(timeSeconds * speed);
}

} // namespace

TileVisualTuning tileVisualTuning(const bool selected, const bool hovered, const bool capital, const float timeSeconds) {
    const float pulse = unitPulse(timeSeconds, selected ? 5.4F : (hovered ? 4.2F : 3.2F));

    TileVisualTuning tuning{};
    tuning.fillBoost = selected ? 1.12F + pulse * 0.06F : (hovered ? 1.04F + pulse * 0.02F : 1.F);
    tuning.innerBoost = selected ? 0.98F + pulse * 0.08F : (capital ? 1.10F + pulse * 0.04F : (hovered ? 0.95F : 0.9F));
    tuning.innerAlpha = selected ? 236 : (hovered ? 228 : 220);
    tuning.outlineThickness = selected ? 3.4F + pulse * 0.6F : (hovered ? 2.1F + pulse * 0.25F : 1.F);
    tuning.outlineAlpha = selected ? static_cast<std::uint8_t>(228 + pulse * 24.F)
                                   : (hovered ? static_cast<std::uint8_t>(166 + pulse * 18.F) : 95);
    tuning.glowPadding = selected ? 6.F + pulse * 2.F : (hovered ? 3.F + pulse : (capital ? 2.5F : 0.F));
    tuning.glowAlpha = selected ? static_cast<std::uint8_t>(148 + pulse * 48.F)
                                : (hovered ? static_cast<std::uint8_t>(72 + pulse * 24.F)
                                           : (capital ? static_cast<std::uint8_t>(42 + pulse * 18.F) : 0));
    tuning.troopChipAlpha = selected ? 208 : (hovered ? 192 : 175);
    tuning.troopChipOutlineAlpha = selected ? 82 : (hovered ? 58 : 40);

    if (capital) {
        tuning.capitalHaloScale = 0.30F + pulse * 0.03F;
        tuning.capitalHaloAlpha = selected ? static_cast<std::uint8_t>(118 + pulse * 38.F)
                                           : static_cast<std::uint8_t>(84 + pulse * 28.F);
        tuning.capitalBeaconHeight = 8.F + pulse * 6.F;
        tuning.capitalCrestScale = (selected ? 1.18F : (hovered ? 1.10F : 1.F)) + pulse * 0.05F;
    }

    return tuning;
}

TransitVisualTuning transitVisualTuning(const float timeSeconds) {
    const float pulse = unitPulse(timeSeconds, 5.2F);
    return {
        .beamThickness = 2.5F + pulse * 0.9F,
        .haloThickness = 6.2F + pulse * 1.4F,
        .tokenRadius = 10.5F + pulse * 2.2F,
        .beamAlpha = static_cast<std::uint8_t>(144 + pulse * 38.F),
        .haloAlpha = static_cast<std::uint8_t>(50 + pulse * 26.F),
        .labelChipAlpha = static_cast<std::uint8_t>(182 + pulse * 24.F),
    };
}

float nationCardAccentPulse(const bool highlighted, const float timeSeconds) {
    if (!highlighted) {
        return 0.F;
    }
    return unitPulse(timeSeconds, 3.6F);
}

} // namespace game::ui
