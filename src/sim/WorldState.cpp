#include "game/sim/WorldState.hpp"

#include <algorithm>
#include <stdexcept>

namespace game::sim {

namespace {
void assignTerritoryPatch(WorldState& world, const TileCoord center, const NationId nation, const int troopsPerTile) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            const TileCoord coord{center.x + dx, center.y + dy};
            if (!inBounds(world, coord)) {
                continue;
            }

            auto& tile = tileAt(world, coord);
            tile.owner = nation;
            tile.troops = troopsPerTile;
        }
    }
}
} // namespace

std::size_t nationIndex(const NationId id) {
    switch (id) {
    case NationId::SwiftLeague:
        return 0;
    case NationId::IronLegion:
        return 1;
    case NationId::BastionDirectorate:
        return 2;
    case NationId::CrownConsortium:
        return 3;
    case NationId::Neutral:
        break;
    }

    throw std::runtime_error("Neutral nation has no runtime index");
}

bool inBounds(const WorldState& world, const TileCoord coord) {
    return coord.x >= 0 && coord.y >= 0 && coord.x < world.width && coord.y < world.height;
}

std::size_t tileIndex(const WorldState& world, const TileCoord coord) {
    return static_cast<std::size_t>(coord.y * world.width + coord.x);
}

TileState& tileAt(WorldState& world, const TileCoord coord) { return world.tiles.at(tileIndex(world, coord)); }

const TileState& tileAt(const WorldState& world, const TileCoord coord) { return world.tiles.at(tileIndex(world, coord)); }

std::vector<TileCoord> neighbors(const WorldState& world, const TileCoord coord) {
    std::vector<TileCoord> results;
    results.reserve(kCardinalDirections.size());
    for (const auto& direction : kCardinalDirections) {
        const TileCoord candidate{coord.x + direction.x, coord.y + direction.y};
        if (inBounds(world, candidate)) {
            results.push_back(candidate);
        }
    }
    return results;
}

int ownedTileCount(const WorldState& world, const NationId nation) {
    return static_cast<int>(std::count_if(world.tiles.begin(), world.tiles.end(), [nation](const TileState& tile) {
        return tile.owner == nation;
    }));
}

int totalTroopsOnTiles(const WorldState& world, const NationId nation) {
    int total = 0;
    for (const auto& tile : world.tiles) {
        if (tile.owner == nation) {
            total += tile.troops;
        }
    }
    return total;
}

int totalTroops(const WorldState& world, const NationId nation) {
    int total = totalTroopsOnTiles(world, nation);
    for (const auto& transit : world.activeTransits) {
        if (transit.owner == nation) {
            total += transit.troops;
        }
    }
    return total;
}

TileCoord capitalOf(const WorldState& world, const NationId nation) {
    return world.nationStates.at(nationIndex(nation)).capital;
}

bool capitalThreatened(const WorldState& world, const NationId nation) {
    const auto capital = capitalOf(world, nation);
    for (const auto& neighbor : neighbors(world, capital)) {
        if (tileAt(world, neighbor).owner != nation && tileAt(world, neighbor).owner != NationId::Neutral) {
            return true;
        }
    }

    return std::any_of(world.activeTransits.begin(), world.activeTransits.end(), [&](const ArmyTransit& transit) {
        return transit.destination == capital && transit.owner != nation;
    });
}

void refreshNationState(WorldState& world) {
    for (const auto nation : playableNations()) {
        auto& state = world.nationStates.at(nationIndex(nation));
        state.commitmentRemaining = std::max(0.F, state.commitmentRemaining);

        const auto capital = state.capital;
        if (!inBounds(world, capital)) {
            state.eliminated = true;
            continue;
        }

        const auto capitalOwner = tileAt(world, capital).owner;
        const auto troopsRemaining = totalTroops(world, nation);
        state.eliminated = capitalOwner != nation || troopsRemaining <= 0;
    }
}

std::optional<NationId> strongestRemainingNation(const WorldState& world) {
    std::optional<NationId> strongest{};
    int strongestTroops = -1;
    for (const auto nation : playableNations()) {
        const auto& state = world.nationStates.at(nationIndex(nation));
        if (state.eliminated) {
            continue;
        }
        const int force = totalTroops(world, nation);
        if (force > strongestTroops) {
            strongestTroops = force;
            strongest = nation;
        }
    }
    return strongest;
}

WorldState createInitialWorld(const NationId playerNation, const MatchConfig& config) {
    WorldState world{};
    world.width = config.width;
    world.height = config.height;
    world.playerNation = playerNation;
    world.tiles.resize(static_cast<std::size_t>(world.width * world.height));

    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            auto& tile = tileAt(world, {x, y});
            tile.owner = NationId::Neutral;
            tile.troops = 0;
            tile.terrain = TerrainType::Plains;
            tile.hasCapital = false;
        }
    }

    const int middleRow = world.height / 2;
    const int middleCol = world.width / 2;
    for (int x = 0; x < world.width; ++x) {
        tileAt(world, {x, middleRow}).terrain = TerrainType::Road;
    }
    for (int y = 0; y < world.height; ++y) {
        tileAt(world, {middleCol, y}).terrain = TerrainType::Road;
    }

    const std::array<TileCoord, 4> capitals{{
        {1, 1},
        {world.width - 2, 1},
        {1, world.height - 2},
        {world.width - 2, world.height - 2},
    }};

    for (std::size_t index = 0; index < playableNations().size(); ++index) {
        const auto nation = playableNations().at(index);
        const auto capital = capitals.at(index);
        world.nationStates.at(index).capital = capital;
        assignTerritoryPatch(world, capital, nation, config.startingTroops);
        auto& capitalTile = tileAt(world, capital);
        capitalTile.owner = nation;
        capitalTile.troops = config.capitalTroops;
        capitalTile.terrain = TerrainType::Capital;
        capitalTile.hasCapital = true;
    }

    for (const TileCoord highland : std::array<TileCoord, 8>{{
             {4, 3},
             {world.width - 5, 3},
             {4, world.height - 4},
             {world.width - 5, world.height - 4},
             {middleCol - 2, middleRow - 1},
             {middleCol + 2, middleRow - 1},
             {middleCol - 2, middleRow + 1},
             {middleCol + 2, middleRow + 1},
         }}) {
        auto& tile = tileAt(world, highland);
        if (!tile.hasCapital) {
            tile.terrain = TerrainType::Highland;
        }
    }

    refreshNationState(world);
    return world;
}

} // namespace game::sim
