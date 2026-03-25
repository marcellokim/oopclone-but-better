#include "game/sim/VictorySystem.hpp"
#include "game/sim/WorldState.hpp"
#include "test_harness.hpp"

TEST_CASE(victory_player_loses_when_capital_falls) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    const auto capital = game::sim::capitalOf(world, game::NationId::SwiftLeague);
    game::sim::tileAt(world, capital).owner = game::NationId::IronLegion;
    game::sim::tileAt(world, capital).troops = 15;

    const auto updated = game::sim::VictorySystem::update(world);
    test::require(updated, "victory system should react to a capital loss");
    test::require(world.matchEnded, "player defeat should end the match");
    test::require(world.winner.has_value(), "winner should be recorded after defeat");
}

TEST_CASE(victory_total_troop_loss_eliminates_a_nation) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    for (auto& tile : world.tiles) {
        if (tile.owner == game::NationId::IronLegion) {
            tile.troops = 0;
        }
    }

    game::sim::VictorySystem::update(world);
    test::require(world.nationStates.at(game::sim::nationIndex(game::NationId::IronLegion)).eliminated,
                  "nation with no remaining forces should be eliminated");
}

TEST_CASE(victory_simultaneous_elimination_results_in_stalemate) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    for (auto& tile : world.tiles) {
        tile.troops = 0;
        if (tile.hasCapital) {
            tile.owner = game::NationId::Neutral;
        }
    }
    world.activeTransits.clear();

    const auto updated = game::sim::VictorySystem::update(world);
    test::require(updated, "victory system should resolve a zero-survivor board state");
    test::require(world.matchEnded, "simultaneous elimination should end the match");
    test::require(!world.winner.has_value(), "zero survivors should produce a stalemate with no winner");
}
