#include "game/sim/VictorySystem.hpp"

namespace game::sim::VictorySystem {

bool update(WorldState& world) {
    refreshNationState(world);

    int aliveCount = 0;
    NationId lastAlive = NationId::SwiftLeague;
    for (const auto nation : playableNations()) {
        if (!world.nationStates.at(nationIndex(nation)).eliminated) {
            ++aliveCount;
            lastAlive = nation;
        }
    }

    if (aliveCount == 0) {
        world.matchEnded = true;
        world.winner.reset();
        return true;
    }

    if (aliveCount == 1) {
        world.matchEnded = true;
        world.winner = lastAlive;
        return true;
    }

    if (world.nationStates.at(nationIndex(world.playerNation)).eliminated) {
        world.matchEnded = true;
        world.winner = strongestRemainingNation(world);
        return true;
    }

    return false;
}

} // namespace game::sim::VictorySystem
