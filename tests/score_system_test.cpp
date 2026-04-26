#include "game/sim/ScoreSystem.hpp"
#include "game/sim/WorldState.hpp"
#include "test_harness.hpp"

#include <algorithm>

TEST_CASE(score_system_orders_standings_by_score_and_marks_winner) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    world.matchEnded = true;
    world.winner = game::NationId::BastionDirectorate;
    world.nationStates.at(game::sim::nationIndex(game::NationId::SwiftLeague)).stats.abilitiesUsed = 1;
    world.nationStates.at(game::sim::nationIndex(game::NationId::BastionDirectorate)).stats.capitalsCaptured = 1;
    world.nationStates.at(game::sim::nationIndex(game::NationId::BastionDirectorate)).stats.troopsDefeated = 50;

    const auto standings = game::sim::ScoreSystem::standings(world);

    test::require(standings.size() == game::playableNations().size(),
                  "score standings should include every playable nation");
    test::require(standings.front().nation == game::NationId::BastionDirectorate,
                  "winner with battle stats should lead the debrief standings");
    test::require(standings.front().winner, "winner row should be flagged");
    test::require(standings.front().score >= standings.back().score,
                  "standings should be sorted descending by score");
}

TEST_CASE(score_system_grade_reflects_score_thresholds) {
    test::require(game::sim::ScoreSystem::gradeForScore(1450) == "S",
                  "dominant scores should receive an S grade");
    test::require(game::sim::ScoreSystem::gradeForScore(900) == "A",
                  "strong scores should receive an A grade");
    test::require(game::sim::ScoreSystem::gradeForScore(500) == "B",
                  "solid scores should receive a B grade");
    test::require(game::sim::ScoreSystem::gradeForScore(150) == "C",
                  "low positive scores should receive a C grade");
}
