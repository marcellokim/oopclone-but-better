#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/Types.hpp"

#include <cstddef>
#include <vector>

namespace game::sim {

struct ArmyTransit {
    NationId owner{NationId::Neutral};
    TileCoord origin{};
    TileCoord destination{};
    std::vector<TileCoord> path{};
    std::size_t nextWaypoint{1};
    float progressToNext{0.F};
    float speedTilesPerSecond{1.F};
    int troops{0};
    bool assault{false};
};

} // namespace game::sim
