#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/Types.hpp"
#include "game/sim/WorldState.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace game::sim::AbilitySystem {

struct ActivationResult {
    bool activated{false};
    std::string message{};
};

[[nodiscard]] float maxCommandPower();
[[nodiscard]] float abilityCost(NationId nation);
[[nodiscard]] float abilityCooldown(NationId nation);
[[nodiscard]] float abilityDuration(NationId nation);
[[nodiscard]] std::string_view abilityName(NationId nation);
[[nodiscard]] std::string_view passiveLine(NationId nation);
[[nodiscard]] std::string_view activeLine(NationId nation);

void update(WorldState& world, float deltaSeconds, const MatchConfig& config = defaultMatchConfig());
[[nodiscard]] ActivationResult activate(WorldState& world,
                                        NationId nation,
                                        const MatchConfig& config = defaultMatchConfig());
[[nodiscard]] bool activateForAi(WorldState& world,
                                 NationId nation,
                                 const MatchConfig& config = defaultMatchConfig());

[[nodiscard]] float movementSpeedMultiplier(const WorldState& world,
                                            NationId nation,
                                            const std::vector<TileCoord>& path);
[[nodiscard]] int launchCapBonus(const WorldState& world,
                                 NationId nation,
                                 const std::vector<TileCoord>& path);
[[nodiscard]] float defenseBonus(const WorldState& world, NationId defender, TileCoord coord);
[[nodiscard]] float capitalRegenBonus(const WorldState& world, NationId nation);
[[nodiscard]] bool hasThreatenedReserveTarget(const WorldState& world, NationId nation);

} // namespace game::sim::AbilitySystem
