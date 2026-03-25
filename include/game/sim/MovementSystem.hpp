#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/CommandQueue.hpp"
#include "game/sim/WorldState.hpp"

#include <vector>

namespace game::sim::MovementSystem {

struct ArrivalEvent {
    NationId owner{NationId::Neutral};
    TileCoord destination{};
    int troops{0};
};

bool applyOrder(WorldState& world, const OrderIntent& order, const MatchConfig& config = defaultMatchConfig());
void flushOrders(WorldState& world, CommandQueue& queue, const MatchConfig& config = defaultMatchConfig());
std::vector<ArrivalEvent> update(WorldState& world, float deltaSeconds);

} // namespace game::sim::MovementSystem
