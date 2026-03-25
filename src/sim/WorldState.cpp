#include "game/sim/WorldState.hpp"

#include <algorithm>
#include <stdexcept>

namespace game::sim {

namespace {
template <std::size_t N>
void paintTerrain(WorldState& world, const std::array<TileCoord, N>& coords, const TerrainType terrain) {
    for (const auto coord : coords) {
        if (!inBounds(world, coord)) {
            continue;
        }

        tileAt(world, coord).terrain = terrain;
    }
}

template <std::size_t N>
void claimTerritory(WorldState& world,
                    const std::array<TileCoord, N>& coords,
                    const NationId nation,
                    const int troopsPerTile) {
    for (const auto coord : coords) {
        if (!inBounds(world, coord)) {
            continue;
        }

        auto& tile = tileAt(world, coord);
        tile.owner = nation;
        tile.troops = troopsPerTile;
    }
}

void placeCapital(WorldState& world,
                  const std::size_t nationSlot,
                  const NationId nation,
                  const TileCoord capital,
                  const MatchConfig& config) {
    world.nationStates.at(nationSlot).capital = capital;

    auto& tile = tileAt(world, capital);
    tile.owner = nation;
    tile.troops = config.capitalTroops;
    tile.terrain = TerrainType::Capital;
    tile.hasCapital = true;
}

void paintHandcraftedTerrain(WorldState& world) {
    paintTerrain(world,
                 std::array<TileCoord, 14>{{
                     {3, 3},
                     {4, 3},
                     {5, 3},
                     {1, 8},
                     {2, 8},
                     {3, 8},
                     {2, 9},
                     {3, 9},
                     {4, 9},
                     {7, 6},
                     {8, 6},
                     {10, 6},
                     {13, 8},
                     {14, 8},
                 }},
                 TerrainType::Highland);

    paintTerrain(world,
                 std::array<TileCoord, 12>{{
                     {6, 3},
                     {7, 3},
                     {10, 3},
                     {11, 3},
                     {5, 6},
                     {6, 6},
                     {12, 6},
                     {13, 6},
                     {5, 8},
                     {6, 8},
                     {11, 8},
                     {12, 8},
                 }},
                 TerrainType::Mountain);

    paintTerrain(world,
                 std::array<TileCoord, 28>{{
                     {2, 1},
                     {3, 1},
                     {4, 1},
                     {5, 1},
                     {6, 1},
                     {6, 2},
                     {6, 4},
                     {7, 4},
                     {8, 4},
                     {9, 2},
                     {9, 3},
                     {9, 4},
                     {9, 5},
                     {9, 6},
                     {9, 7},
                     {9, 8},
                     {9, 9},
                     {10, 2},
                     {11, 2},
                     {12, 2},
                     {13, 2},
                     {14, 2},
                     {15, 2},
                     {3, 10},
                     {4, 10},
                     {5, 10},
                     {6, 10},
                     {12, 9},
                 }},
                 TerrainType::Road);

    paintTerrain(world,
                 std::array<TileCoord, 8>{{
                     {13, 9},
                     {14, 9},
                     {15, 9},
                     {12, 8},
                     {12, 10},
                     {6, 9},
                     {10, 4},
                     {11, 4},
                 }},
                 TerrainType::Road);

    paintTerrain(world,
                 std::array<TileCoord, 21>{{
                     {6, 0},
                     {7, 0},
                     {8, 0},
                     {9, 0},
                     {10, 0},
                     {7, 1},
                     {8, 1},
                     {9, 1},
                     {0, 4},
                     {0, 5},
                     {0, 6},
                     {1, 5},
                     {1, 6},
                     {17, 4},
                     {17, 5},
                     {17, 6},
                     {16, 5},
                     {16, 6},
                     {7, 11},
                     {8, 11},
                     {9, 11},
                 }},
                 TerrainType::Sea);

    paintTerrain(world,
                 std::array<TileCoord, 3>{{
                     {10, 11},
                     {8, 10},
                     {9, 10},
                 }},
                 TerrainType::Sea);
}

void claimOpeningTerritories(WorldState& world, const MatchConfig& config) {
    claimTerritory(world,
                   std::array<TileCoord, 9>{{
                       {1, 0},
                       {2, 0},
                       {0, 1},
                       {1, 1},
                       {2, 1},
                       {1, 2},
                       {2, 2},
                       {3, 1},
                       {2, 3},
                   }},
                   NationId::SwiftLeague,
                   config.startingTroops);

    claimTerritory(world,
                   std::array<TileCoord, 9>{{
                       {16, 0},
                       {17, 0},
                       {15, 1},
                       {16, 1},
                       {17, 1},
                       {14, 1},
                       {15, 2},
                       {16, 2},
                       {14, 2},
                   }},
                   NationId::IronLegion,
                   config.startingTroops);

    claimTerritory(world,
                   std::array<TileCoord, 9>{{
                       {0, 10},
                       {1, 9},
                       {1, 10},
                       {2, 9},
                       {2, 10},
                       {1, 11},
                       {2, 11},
                       {2, 8},
                       {3, 9},
                   }},
                   NationId::BastionDirectorate,
                   config.startingTroops);

    claimTerritory(world,
                   std::array<TileCoord, 9>{{
                       {13, 9},
                       {14, 9},
                       {15, 9},
                       {16, 9},
                       {14, 10},
                       {15, 10},
                       {16, 10},
                       {15, 11},
                       {14, 8},
                   }},
                   NationId::CrownConsortium,
                   config.startingTroops);

    const std::array<TileCoord, 4> capitals{{
        {1, 1},
        {16, 1},
        {1, world.height - 2},
        {world.width - 2, world.height - 2},
    }};

    for (std::size_t index = 0; index < playableNations().size(); ++index) {
        placeCapital(world, index, playableNations().at(index), capitals.at(index), config);
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

    paintHandcraftedTerrain(world);
    claimOpeningTerritories(world, config);

    refreshNationState(world);
    return world;
}

} // namespace game::sim
