#include "game/sim/CombatSystem.hpp"

#include "game/GameConfig.hpp"

#include <algorithm>
#include <cmath>

namespace game::sim::CombatSystem {

void resolveArrival(WorldState& world, const MovementSystem::ArrivalEvent& arrival) {
    auto& target = tileAt(world, arrival.destination);

    if (target.owner == arrival.owner || target.owner == NationId::Neutral) {
        if (target.owner == NationId::Neutral) {
            target.owner = arrival.owner;
        }
        target.troops += arrival.troops;
        return;
    }

    int attackers = arrival.troops;
    int defenders = target.troops;
    const float attackFactor = nationDefinition(arrival.owner).attack;
    const float defenseFactor = nationDefinition(target.owner).defense + terrainDefenseBonus(target.terrain);

    while (attackers > 0 && defenders > 0) {
        const int defenderLosses = std::max(1, static_cast<int>(std::lround(attackers * attackFactor * 0.18F)));
        const int attackerLosses = std::max(1, static_cast<int>(std::lround(defenders * defenseFactor * 0.16F)));
        defenders = std::max(0, defenders - defenderLosses);
        attackers = std::max(0, attackers - attackerLosses);
    }

    if (attackers > 0) {
        target.owner = arrival.owner;
        target.troops = attackers;
    } else {
        target.troops = defenders;
    }
}

} // namespace game::sim::CombatSystem
