#pragma once

#include "game/sim/Types.hpp"
#include "game/sim/WorldState.hpp"

#include <vector>

namespace game::sim::Pathfinder {

std::vector<TileCoord> findPath(const WorldState& world, TileCoord start, TileCoord goal);

} // namespace game::sim::Pathfinder
