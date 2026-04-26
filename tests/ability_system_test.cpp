#include "game/sim/AbilitySystem.hpp"
#include "game/sim/CombatSystem.hpp"
#include "game/sim/MovementSystem.hpp"
#include "game/sim/RegenSystem.hpp"
#include "game/sim/WorldState.hpp"
#include "test_harness.hpp"

#include <algorithm>

namespace {

void giveCommandPower(game::sim::WorldState& world, const game::NationId nation, const float amount) {
    auto& runtime = world.nationStates.at(game::sim::nationIndex(nation));
    runtime.commandPower = amount;
    runtime.abilityCooldownRemaining = 0.F;
    runtime.abilityActiveRemaining = 0.F;
}

game::sim::OrderIntent orderThrough(game::NationId nation,
                                    game::sim::TileCoord origin,
                                    game::sim::TileCoord target,
                                    game::sim::SendRatio ratio = game::sim::SendRatio::Full) {
    return {nation, origin, target, ratio, false};
}

} // namespace

TEST_CASE(ability_system_charges_command_power_and_clamps_to_max) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto& sogang = world.nationStates.at(game::sim::nationIndex(game::NationId::SwiftLeague));
    sogang.commandPower = game::sim::AbilitySystem::maxCommandPower() - 1.F;

    game::sim::AbilitySystem::update(world, 20.F);

    test::require(sogang.commandPower == game::sim::AbilitySystem::maxCommandPower(),
                  "command power should clamp at the system maximum");
    test::require(sogang.stats.commandPowerEarned >= 1.F,
                  "command power gains should be recorded in nation stats");
}

TEST_CASE(ability_system_activation_spends_power_starts_cooldown_and_expires) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    giveCommandPower(world, game::NationId::SwiftLeague, game::sim::AbilitySystem::abilityCost(game::NationId::SwiftLeague));

    const auto result = game::sim::AbilitySystem::activate(world, game::NationId::SwiftLeague);

    const auto& runtime = world.nationStates.at(game::sim::nationIndex(game::NationId::SwiftLeague));
    test::require(result.activated, "ability should activate when command power is available");
    test::require(runtime.commandPower == 0.F, "activation should spend command power");
    test::require(runtime.abilityCooldownRemaining > 0.F, "activation should start cooldown");
    test::require(runtime.abilityActiveRemaining > 0.F, "activation should start active duration");
    test::require(runtime.stats.abilitiesUsed == 1, "activation should increment ability usage stats");

    game::sim::AbilitySystem::update(world, 30.F);
    test::require(runtime.abilityCooldownRemaining == 0.F, "cooldown should tick down to zero");
    test::require(runtime.abilityActiveRemaining == 0.F, "active duration should expire");
}

TEST_CASE(ability_system_rejects_activation_without_power_or_valid_target) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);

    const auto noPower = game::sim::AbilitySystem::activate(world, game::NationId::SwiftLeague);
    test::require(!noPower.activated, "ability should reject activation without command power");
    test::require(noPower.message == "Not enough Command Power",
                  "no-power activation should return the player-facing notice");

    giveCommandPower(world,
                     game::NationId::CrownConsortium,
                     game::sim::AbilitySystem::abilityCost(game::NationId::CrownConsortium));
    const auto reserve = game::sim::AbilitySystem::activate(world, game::NationId::CrownConsortium);
    test::require(!reserve.activated,
                  "Adaptive Reserve should reject activation when no threatened owned tile exists");
    test::require(reserve.message == "No valid target",
                  "targetless activation should return the player-facing notice");
}

TEST_CASE(ability_system_applies_sogang_and_hanyang_movement_bonuses) {
    auto baseWorld = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto sogangWorld = baseWorld;
    giveCommandPower(sogangWorld,
                     game::NationId::SwiftLeague,
                     game::sim::AbilitySystem::abilityCost(game::NationId::SwiftLeague));
    test::require(game::sim::AbilitySystem::activate(sogangWorld, game::NationId::SwiftLeague).activated,
                  "Sogang ability should activate for movement comparison");

    const auto routeOrder = orderThrough(game::NationId::SwiftLeague, {1, 1}, {4, 1});
    test::require(game::sim::MovementSystem::applyOrder(baseWorld, routeOrder),
                  "base route should create a transit");
    test::require(game::sim::MovementSystem::applyOrder(sogangWorld, routeOrder),
                  "ability route should create a transit");
    test::require(sogangWorld.activeTransits.front().speedTilesPerSecond > baseWorld.activeTransits.front().speedTilesPerSecond,
                  "Tempo Surge should speed up new orders");
    test::require(sogangWorld.activeTransits.front().troops > baseWorld.activeTransits.front().troops,
                  "Tempo Surge should raise launch caps");

    auto roadBase = game::sim::createInitialWorld(game::NationId::IronLegion);
    auto roadBoost = roadBase;
    game::sim::tileAt(roadBase, {15, 2}).troops = 120;
    game::sim::tileAt(roadBoost, {15, 2}).troops = 120;
    giveCommandPower(roadBoost, game::NationId::IronLegion, game::sim::AbilitySystem::abilityCost(game::NationId::IronLegion));
    test::require(game::sim::AbilitySystem::activate(roadBoost, game::NationId::IronLegion).activated,
                  "Hanyang ability should activate for road comparison");
    const auto roadOrder = orderThrough(game::NationId::IronLegion, {15, 2}, {12, 2});
    test::require(game::sim::MovementSystem::applyOrder(roadBase, roadOrder),
                  "base road route should create a transit");
    test::require(game::sim::MovementSystem::applyOrder(roadBoost, roadOrder),
                  "boosted road route should create a transit");
    test::require(roadBoost.activeTransits.front().speedTilesPerSecond > roadBase.activeTransits.front().speedTilesPerSecond,
                  "Road Authority should speed up road routes");
    test::require(roadBoost.activeTransits.front().troops > roadBase.activeTransits.front().troops,
                  "Road Authority should increase road launch caps");
}

TEST_CASE(ability_system_applies_skku_defense_and_regen_bonuses) {
    auto baseWorld = game::sim::createInitialWorld(game::NationId::BastionDirectorate);
    auto fortifiedWorld = baseWorld;
    const auto capital = game::sim::capitalOf(baseWorld, game::NationId::BastionDirectorate);
    game::sim::tileAt(baseWorld, capital).troops = 20;
    game::sim::tileAt(fortifiedWorld, capital).troops = 20;

    giveCommandPower(fortifiedWorld,
                     game::NationId::BastionDirectorate,
                     game::sim::AbilitySystem::abilityCost(game::NationId::BastionDirectorate));
    test::require(game::sim::AbilitySystem::activate(fortifiedWorld, game::NationId::BastionDirectorate).activated,
                  "SKKU ability should activate for defense comparison");

    game::sim::CombatSystem::resolveArrival(baseWorld, {game::NationId::IronLegion, capital, 42});
    game::sim::CombatSystem::resolveArrival(fortifiedWorld, {game::NationId::IronLegion, capital, 42});
    const auto baseLosses = baseWorld.nationStates.at(game::sim::nationIndex(game::NationId::BastionDirectorate)).stats.troopsLost;
    const auto fortifiedLosses =
        fortifiedWorld.nationStates.at(game::sim::nationIndex(game::NationId::BastionDirectorate)).stats.troopsLost;
    test::require(fortifiedLosses < baseLosses,
                  "Fortified Basin should reduce losses on defended tiles");

    const int before = game::sim::tileAt(fortifiedWorld, capital).troops;
    for (int i = 0; i < 20; ++i) {
        game::sim::RegenSystem::update(fortifiedWorld, 0.25F);
    }
    test::require(game::sim::tileAt(fortifiedWorld, capital).troops - before >= 6,
                  "Fortified Basin should add a visible active capital regen bonus");
}

TEST_CASE(ability_system_chungang_reserve_reinforces_threatened_frontline) {
    auto world = game::sim::createInitialWorld(game::NationId::CrownConsortium);
    const auto capital = game::sim::capitalOf(world, game::NationId::CrownConsortium);
    const auto threatened = game::sim::neighbors(world, capital).front();
    auto& enemyTile = game::sim::tileAt(world, threatened);
    enemyTile.owner = game::NationId::IronLegion;
    enemyTile.troops = 40;
    auto& capitalTile = game::sim::tileAt(world, capital);
    capitalTile.troops = 30;
    giveCommandPower(world, game::NationId::CrownConsortium, game::sim::AbilitySystem::abilityCost(game::NationId::CrownConsortium));

    const auto result = game::sim::AbilitySystem::activate(world, game::NationId::CrownConsortium);

    const auto& runtime = world.nationStates.at(game::sim::nationIndex(game::NationId::CrownConsortium));
    test::require(result.activated, "Adaptive Reserve should activate when a capital/frontline is threatened");
    test::require(runtime.abilityFocusTile == capital,
                  "Adaptive Reserve should focus the most threatened capital/frontline tile");
    test::require(game::sim::tileAt(world, capital).troops == 54,
                  "Adaptive Reserve should immediately reinforce its focused tile");
    test::require(game::sim::AbilitySystem::defenseBonus(world, game::NationId::CrownConsortium, capital) > 0.F,
                  "Adaptive Reserve should grant a temporary focus defense bonus");
}
