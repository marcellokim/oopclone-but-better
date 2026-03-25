#include "game/sim/CombatSystem.hpp"
#include "game/sim/MovementSystem.hpp"
#include "game/sim/Pathfinder.hpp"
#include "game/sim/RegenSystem.hpp"
#include "game/sim/WorldState.hpp"
#include "test_harness.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <string_view>

namespace {

constexpr std::array<game::TerrainType, 6> kKnownTerrainTypes{game::TerrainType::Plains,
                                                               game::TerrainType::Road,
                                                               game::TerrainType::Highland,
                                                               game::TerrainType::Mountain,
                                                               game::TerrainType::Sea,
                                                               game::TerrainType::Capital};

std::optional<game::TerrainType> terrainNamed(const std::string_view target) {
    for (const auto terrain : kKnownTerrainTypes) {
        if (game::terrainName(terrain) == target) {
            return terrain;
        }
    }
    return std::nullopt;
}

bool worldContainsTerrain(const game::sim::WorldState& world, const game::TerrainType terrain) {
    for (const auto& tile : world.tiles) {
        if (tile.terrain == terrain) {
            return true;
        }
    }
    return false;
}

int ownedBoundingBoxArea(const game::sim::WorldState& world, const game::NationId nation) {
    int minX = world.width;
    int minY = world.height;
    int maxX = -1;
    int maxY = -1;

    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            const auto& tile = game::sim::tileAt(world, {x, y});
            if (tile.owner != nation) {
                continue;
            }
            minX = std::min(minX, x);
            minY = std::min(minY, y);
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
        }
    }

    if (maxX < minX || maxY < minY) {
        return 0;
    }

    return (maxX - minX + 1) * (maxY - minY + 1);
}

void prepareRouteComparisonWorld(game::sim::WorldState& world) {
    constexpr std::array<game::sim::TileCoord, 4> kRouteTiles{{{2, 1}, {3, 1}, {4, 1}, {5, 1}}};
    for (const auto coord : kRouteTiles) {
        auto& tile = game::sim::tileAt(world, coord);
        tile.owner = coord.x == 2 ? game::NationId::SwiftLeague : game::NationId::Neutral;
        tile.troops = coord.x == 2 ? 120 : 0;
        tile.hasCapital = false;
        tile.terrain = game::TerrainType::Plains;
    }
}

} // namespace

TEST_CASE(sim_pathfinder_uses_orthogonal_deterministic_routes) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    const auto path = game::sim::Pathfinder::findPath(world, {1, 1}, {3, 3});
    test::require(path.size() == 5, "path should contain 5 points from (1,1) to (3,3)");
    test::require(path[1] == game::sim::TileCoord{2, 1},
                  "path should begin with the deterministic east-first shortest route");
}

TEST_CASE(sim_world_layout_adds_sea_mountain_and_irregular_opening_territories) {
    const auto mountain = terrainNamed("Mountain");
    const auto sea = terrainNamed("Sea");
    test::require(mountain.has_value(), "terrain update should expose a Mountain terrain label");
    test::require(sea.has_value(), "terrain update should expose a Sea terrain label");

    const auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    test::require(worldContainsTerrain(world, *mountain), "opening map should contain at least one mountain tile");
    test::require(worldContainsTerrain(world, *sea), "opening map should contain at least one sea tile");

    for (const auto nation : game::playableNations()) {
        const int ownedTiles = game::sim::ownedTileCount(world, nation);
        const int boundingArea = ownedBoundingBoxArea(world, nation);
        test::require(ownedTiles > 0, "each playable nation should still own starting territory");
        test::require(boundingArea > ownedTiles,
                      "opening territories should no longer be perfect rectangles around each capital");
    }
}

TEST_CASE(sim_opening_territories_do_not_claim_impassable_sea_tiles) {
    const auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);

    for (const auto nation : game::playableNations()) {
        for (int y = 0; y < world.height; ++y) {
            for (int x = 0; x < world.width; ++x) {
                const auto& tile = game::sim::tileAt(world, {x, y});
                if (tile.owner != nation) {
                    continue;
                }
                test::require(game::terrainPassableForLand(tile.terrain) || tile.hasCapital,
                              "starting/owned faction territory should not sit on impassable sea tiles");
            }
        }
    }
}

TEST_CASE(sim_pathfinder_rejects_sea_barriers_and_order_state_remains_stable) {
    const auto sea = terrainNamed("Sea");
    test::require(sea.has_value(), "terrain update should expose a Sea terrain label");

    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    for (int y = 0; y < world.height; ++y) {
        auto& barrierTile = game::sim::tileAt(world, {2, y});
        barrierTile.terrain = *sea;
        barrierTile.owner = game::NationId::Neutral;
        barrierTile.troops = 0;
        barrierTile.hasCapital = false;
    }

    const int originTroops = game::sim::tileAt(world, {1, 1}).troops;
    const auto path = game::sim::Pathfinder::findPath(world, {1, 1}, {3, 1});
    test::require(path.empty(), "pathfinder should not route a normal land path across a sea barrier");
    test::require(
        !game::sim::MovementSystem::applyOrder(
            world,
            {game::NationId::SwiftLeague, {1, 1}, {3, 1}, game::sim::SendRatio::Quarter, false}),
        "movement orders across an all-sea barrier should fail safely");
    test::require(game::sim::tileAt(world, {1, 1}).troops == originTroops,
                  "failing sea-crossing orders should not consume origin troops");
    test::require(world.activeTransits.empty(), "failing sea-crossing orders should not create transits");
}

TEST_CASE(sim_mountain_paths_reduce_speed_and_throughput_relative_to_open_routes) {
    const auto mountain = terrainNamed("Mountain");
    test::require(mountain.has_value(), "terrain update should expose a Mountain terrain label");

    auto fastWorld = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto slowWorld = fastWorld;
    prepareRouteComparisonWorld(fastWorld);
    prepareRouteComparisonWorld(slowWorld);

    game::sim::tileAt(fastWorld, {3, 1}).terrain = game::TerrainType::Road;
    game::sim::tileAt(fastWorld, {4, 1}).terrain = game::TerrainType::Road;
    game::sim::tileAt(slowWorld, {3, 1}).terrain = *mountain;
    game::sim::tileAt(slowWorld, {4, 1}).terrain = *mountain;

    const game::sim::OrderIntent order{game::NationId::SwiftLeague, {2, 1}, {5, 1}, game::sim::SendRatio::Full, false};
    test::require(game::sim::MovementSystem::applyOrder(fastWorld, order),
                  "open-lane control route should create a transit");
    test::require(game::sim::MovementSystem::applyOrder(slowWorld, order),
                  "mountain-constrained route should still create a transit");
    test::require(!fastWorld.activeTransits.empty() && !slowWorld.activeTransits.empty(),
                  "comparison routes should both yield active transits");

    const auto& fastTransit = fastWorld.activeTransits.front();
    const auto& slowTransit = slowWorld.activeTransits.front();
    test::require(slowTransit.speedTilesPerSecond < fastTransit.speedTilesPerSecond,
                  "mountain-constrained routes should travel slower than open lanes");
    test::require(slowTransit.troops < fastTransit.troops,
                  "mountain-constrained routes should bottleneck launched troops");
}

TEST_CASE(sim_movement_and_friendly_merge_work) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::sim::CommandQueue queue;
    queue.push({game::NationId::SwiftLeague, {1, 1}, {2, 1}, game::sim::SendRatio::Quarter, false});
    game::sim::MovementSystem::flushOrders(world, queue);
    test::require(!world.activeTransits.empty(), "move order should create a transit");
    for (int i = 0; i < 10; ++i) {
        for (const auto& arrival : game::sim::MovementSystem::update(world, 0.2F)) {
            game::sim::CombatSystem::resolveArrival(world, arrival);
        }
    }
    test::require(game::sim::tileAt(world, {2, 1}).troops > 24, "friendly tile should gain troops after transit arrives");
}

TEST_CASE(sim_regen_increases_with_owned_territory) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto expanded = world;
    game::sim::tileAt(expanded, {5, 5}).owner = game::NationId::SwiftLeague;
    game::sim::tileAt(expanded, {5, 5}).troops = 5;

    const int baseBefore = game::sim::tileAt(world, game::sim::capitalOf(world, game::NationId::SwiftLeague)).troops;
    const int expandedBefore = game::sim::tileAt(expanded, game::sim::capitalOf(expanded, game::NationId::SwiftLeague)).troops;

    for (int i = 0; i < 50; ++i) {
        game::sim::RegenSystem::update(world, 0.2F);
        game::sim::RegenSystem::update(expanded, 0.2F);
    }

    const int baseAfter = game::sim::tileAt(world, game::sim::capitalOf(world, game::NationId::SwiftLeague)).troops;
    const int expandedAfter = game::sim::tileAt(expanded, game::sim::capitalOf(expanded, game::NationId::SwiftLeague)).troops;
    test::require(expandedAfter - expandedBefore > baseAfter - baseBefore,
                  "more territory should yield more capital regeneration over time");
}

TEST_CASE(sim_combat_capture_changes_owner_when_attackers_survive) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto& target = game::sim::tileAt(world, {15, 1});
    target.owner = game::NationId::IronLegion;
    target.troops = 6;

    game::sim::CombatSystem::resolveArrival(world, {game::NationId::SwiftLeague, {15, 1}, 40});

    test::require(target.owner == game::NationId::SwiftLeague, "successful attack should capture the tile");
    test::require(target.troops > 0, "capturing attackers should leave surviving troops on the tile");
}
