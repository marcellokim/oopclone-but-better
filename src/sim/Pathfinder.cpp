#include "game/sim/Pathfinder.hpp"

#include <algorithm>
#include <queue>
#include <unordered_map>

namespace game::sim::Pathfinder {

namespace {

bool isSeaTile(const WorldState& world, const TileCoord coord) {
    return terrainName(tileAt(world, coord).terrain) == "Sea";
}

} // namespace

std::vector<TileCoord> findPath(const WorldState& world, const TileCoord start, const TileCoord goal) {
    if (!inBounds(world, start) || !inBounds(world, goal)) {
        return {};
    }
    if (isSeaTile(world, start) || isSeaTile(world, goal)) {
        return {};
    }
    if (start == goal) {
        return {start};
    }

    std::queue<TileCoord> frontier;
    std::unordered_map<TileCoord, TileCoord> cameFrom;
    frontier.push(start);
    cameFrom.emplace(start, start);

    while (!frontier.empty()) {
        const auto current = frontier.front();
        frontier.pop();

        if (current == goal) {
            break;
        }

        for (const auto& direction : kCardinalDirections) {
            const TileCoord next{current.x + direction.x, current.y + direction.y};
            if (!inBounds(world, next) || cameFrom.contains(next) || isSeaTile(world, next)) {
                continue;
            }
            cameFrom.emplace(next, current);
            frontier.push(next);
        }
    }

    if (!cameFrom.contains(goal)) {
        return {};
    }

    std::vector<TileCoord> path;
    for (TileCoord cursor = goal;; cursor = cameFrom.at(cursor)) {
        path.push_back(cursor);
        if (cursor == start) {
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace game::sim::Pathfinder
