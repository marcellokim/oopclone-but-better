#include "game/sim/CombatSystem.hpp"

#include "game/GameConfig.hpp"
#include "game/sim/AbilitySystem.hpp"

#include <algorithm>
#include <cmath>

namespace game::sim::CombatSystem {

void resolveArrival(WorldState& world, const MovementSystem::ArrivalEvent& arrival) {
    auto& target = tileAt(world, arrival.destination);
    const auto originalOwner = target.owner;

    if (target.owner == arrival.owner || target.owner == NationId::Neutral) {
        if (target.owner == NationId::Neutral) {
            target.owner = arrival.owner;
            world.nationStates.at(nationIndex(arrival.owner)).stats.tilesCaptured += 1;
        }
        target.troops += arrival.troops;
        return;
    }

    int attackers = arrival.troops;
    int defenders = target.troops;
    const int originalAttackers = attackers;
    const int originalDefenders = defenders;
    const float attackFactor = nationDefinition(arrival.owner).attack;
    const float abilityDefense = AbilitySystem::defenseBonus(world, target.owner, arrival.destination);
    const float defenseFactor = nationDefinition(target.owner).defense + terrainDefenseBonus(target.terrain) + abilityDefense;
    const float defenderLossDivisor = std::max(1.F, 1.F + abilityDefense * 13.0F);

    while (attackers > 0 && defenders > 0) {
        const int defenderLosses = std::max(1, static_cast<int>(std::lround((attackers * attackFactor * 0.18F) / defenderLossDivisor)));
        const int attackerLosses = std::max(1, static_cast<int>(std::lround(defenders * defenseFactor * 0.16F)));
        defenders = std::max(0, defenders - defenderLosses);
        attackers = std::max(0, attackers - attackerLosses);
    }

    const int attackerLosses = originalAttackers - attackers;
    const int defenderLosses = originalDefenders - defenders;
    if (isPlayableNation(arrival.owner)) {
        auto& attackerStats = world.nationStates.at(nationIndex(arrival.owner)).stats;
        attackerStats.troopsLost += attackerLosses;
        attackerStats.troopsDefeated += defenderLosses;
    }
    if (isPlayableNation(originalOwner)) {
        auto& defenderStats = world.nationStates.at(nationIndex(originalOwner)).stats;
        defenderStats.troopsLost += defenderLosses;
        defenderStats.troopsDefeated += attackerLosses;
    }

    if (attackers > 0) {
        target.owner = arrival.owner;
        target.troops = attackers;
        if (isPlayableNation(arrival.owner)) {
            auto& stats = world.nationStates.at(nationIndex(arrival.owner)).stats;
            stats.tilesCaptured += 1;
            if (target.hasCapital) {
                stats.capitalsCaptured += 1;
            }
        }
    } else {
        target.troops = defenders;
    }
}

} // namespace game::sim::CombatSystem
