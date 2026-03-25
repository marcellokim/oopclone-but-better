#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/WorldState.hpp"

namespace game::sim::RegenSystem {

void update(WorldState& world, float deltaSeconds, const MatchConfig& config = defaultMatchConfig());

} // namespace game::sim::RegenSystem
