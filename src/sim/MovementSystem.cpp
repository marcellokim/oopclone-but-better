#include "game/sim/MovementSystem.hpp"

#include "game/GameConfig.hpp"
#include "game/sim/Pathfinder.hpp"

#include <algorithm>

namespace game::sim::MovementSystem {

bool applyOrder(WorldState& world, const OrderIntent& order, const MatchConfig& config) {
    if (!isPlayableNation(order.issuer) || !inBounds(world, order.origin) || !inBounds(world, order.target)) {
        return false;
    }
    if (order.origin == order.target) {
        return false;
    }

    auto& originTile = tileAt(world, order.origin);
    if (originTile.owner != order.issuer || originTile.troops <= 0) {
        return false;
    }

    const auto path = Pathfinder::findPath(world, order.origin, order.target);
    if (path.size() < 2) {
        return false;
    }

    const int troopsToSend = requestedTroops(originTile.troops, order.ratio);
    if (troopsToSend <= 0 || troopsToSend > originTile.troops) {
        return false;
    }

    originTile.troops -= troopsToSend;

    ArmyTransit transit{};
    transit.owner = order.issuer;
    transit.origin = order.origin;
    transit.destination = order.target;
    transit.path = path;
    transit.speedTilesPerSecond = config.transitSpeedTilesPerSecond * nationDefinition(order.issuer).mobility *
                                  terrainMovementMultiplier(originTile.terrain);
    transit.troops = troopsToSend;
    transit.assault = tileAt(world, order.target).owner != order.issuer;
    world.activeTransits.push_back(transit);
    return true;
}

void flushOrders(WorldState& world, CommandQueue& queue, const MatchConfig& config) {
    for (const auto& order : queue.drain()) {
        applyOrder(world, order, config);
    }
}

std::vector<ArrivalEvent> update(WorldState& world, const float deltaSeconds) {
    std::vector<ArrivalEvent> arrivals;
    std::vector<ArmyTransit> survivors;
    survivors.reserve(world.activeTransits.size());

    for (auto transit : world.activeTransits) {
        float travelBudget = transit.speedTilesPerSecond * deltaSeconds;
        bool delivered = false;

        while (travelBudget > 0.F && transit.nextWaypoint < transit.path.size()) {
            const float distanceRemaining = 1.F - transit.progressToNext;
            if (travelBudget + 0.0001F >= distanceRemaining) {
                travelBudget -= distanceRemaining;
                transit.progressToNext = 0.F;
                ++transit.nextWaypoint;
                if (transit.nextWaypoint >= transit.path.size()) {
                    arrivals.push_back({transit.owner, transit.destination, transit.troops});
                    delivered = true;
                    break;
                }
            } else {
                transit.progressToNext += travelBudget;
                travelBudget = 0.F;
            }
        }

        if (!delivered) {
            survivors.push_back(std::move(transit));
        }
    }

    world.activeTransits = std::move(survivors);
    return arrivals;
}

} // namespace game::sim::MovementSystem
