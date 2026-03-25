#include "game/ui/InputController.hpp"
#include "test_harness.hpp"

TEST_CASE(input_controller_selects_and_queues_orders) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ui::InputController input;
    game::sim::CommandQueue queue;

    test::require(input.handleTileClick(world, game::NationId::SwiftLeague, {1, 1}, queue),
                  "first click should select a player-owned tile");
    test::require(world.selectedTile.has_value(), "selection should be stored");
    test::require(input.handleTileClick(world, game::NationId::SwiftLeague, {2, 1}, queue),
                  "second click should create an order");
    test::require(!world.selectedTile.has_value(), "selection should clear after queuing an order");
    test::require(queue.drain().size() == 1, "command queue should contain a single order");
}

TEST_CASE(input_controller_rejects_non_owned_initial_clicks) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ui::InputController input;
    game::sim::CommandQueue queue;

    test::require(!input.handleTileClick(world, game::NationId::SwiftLeague, {9, 6}, queue),
                  "neutral tiles should not become command origins");
    test::require(queue.empty(), "no command should be queued from a neutral tile click");
}

TEST_CASE(input_controller_clears_selection_on_same_tile_click) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ui::InputController input;
    game::sim::CommandQueue queue;

    test::require(input.handleTileClick(world, game::NationId::SwiftLeague, {1, 1}, queue),
                  "first click should select the origin tile");
    test::require(!input.handleTileClick(world, game::NationId::SwiftLeague, {1, 1}, queue),
                  "clicking the same tile again should cancel the selection");
    test::require(!world.selectedTile.has_value(), "selection should clear on same-tile click");
    test::require(queue.empty(), "same-tile click should not enqueue a command");
}
