#pragma once

#include "game/sim/MovementSystem.hpp"
#include "game/sim/WorldState.hpp"

namespace game::sim::CombatSystem {

void resolveArrival(WorldState& world, const MovementSystem::ArrivalEvent& arrival);

} // namespace game::sim::CombatSystem
