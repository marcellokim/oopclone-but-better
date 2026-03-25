#include "game/sim/RegenSystem.hpp"

namespace game::sim::RegenSystem {

void update(WorldState& world, const float deltaSeconds, const MatchConfig& config) {
    for (const auto nation : playableNations()) {
        auto& runtime = world.nationStates.at(nationIndex(nation));
        if (runtime.eliminated) {
            continue;
        }

        const auto capital = runtime.capital;
        auto& capitalTile = tileAt(world, capital);
        if (capitalTile.owner != nation) {
            continue;
        }

        const float regenRate = config.baseRegenPerSecond +
                                static_cast<float>(ownedTileCount(world, nation)) * config.territoryRegenFactor *
                                    nationDefinition(nation).regen;
        runtime.regenAccumulator += regenRate * deltaSeconds;
        const int reinforcements = static_cast<int>(runtime.regenAccumulator);
        if (reinforcements > 0) {
            capitalTile.troops = std::min(config.capitalTroopCap, capitalTile.troops + reinforcements);
            runtime.regenAccumulator -= static_cast<float>(reinforcements);
        }
    }
}

} // namespace game::sim::RegenSystem
