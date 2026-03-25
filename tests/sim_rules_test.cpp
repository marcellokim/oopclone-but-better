#include "game/sim/CombatSystem.hpp"
#include "game/sim/MovementSystem.hpp"
#include "game/sim/Pathfinder.hpp"
#include "game/sim/RegenSystem.hpp"
#include "game/sim/WorldState.hpp"
#include "test_harness.hpp"

TEST_CASE(sim_pathfinder_uses_orthogonal_deterministic_routes) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    const auto path = game::sim::Pathfinder::findPath(world, {1, 1}, {3, 3});
    test::require(path.size() == 5, "path should contain 5 points from (1,1) to (3,3)");
    test::require(path[1] == game::sim::TileCoord{2, 1},
                  "path should begin with the deterministic east-first shortest route");
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
