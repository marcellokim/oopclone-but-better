#include "game/ai/AiDirector.hpp"
#include "game/sim/WorldState.hpp"
#include "test_harness.hpp"

#include <algorithm>

TEST_CASE(ai_bastion_prioritizes_capital_defense_when_threatened) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto capital = game::sim::capitalOf(world, game::NationId::BastionDirectorate);
    auto threatened = game::sim::neighbors(world, capital).front();
    game::sim::tileAt(world, threatened).owner = game::NationId::IronLegion;
    game::sim::tileAt(world, threatened).troops = 40;

    game::ai::AiDirector ai;
    game::sim::CommandQueue queue;
    ai.update(world, queue, 1.0F);
    const auto orders = queue.drain();
    test::require(!orders.empty(), "AI should react to a threatened capital");
    const auto defendedCapital = std::any_of(orders.begin(), orders.end(), [&](const game::sim::OrderIntent& order) {
        return order.issuer == game::NationId::BastionDirectorate && order.target == capital;
    });
    test::require(defendedCapital, "Bastion defense should reinforce the capital under threat");
}

TEST_CASE(ai_issues_orders_for_non_player_nations) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ai::AiDirector ai;
    game::sim::CommandQueue queue;
    ai.update(world, queue, 1.0F);
    const auto orders = queue.drain();
    test::require(!orders.empty(), "AI should create at least one action when the match begins");
}

TEST_CASE(ai_commitment_window_blocks_immediate_non_emergency_retargeting) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ai::AiDirector ai;
    game::sim::CommandQueue queue;

    ai.update(world, queue, 1.0F);
    const auto firstBurst = queue.drain();
    test::require(!firstBurst.empty(), "AI should create an initial order burst");

    ai.update(world, queue, 0.2F);
    const auto secondBurst = queue.drain();
    test::require(secondBurst.empty(), "non-emergency AI should respect the commitment window before retargeting");
}
