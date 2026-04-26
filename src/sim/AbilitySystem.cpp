#include "game/sim/AbilitySystem.hpp"

#include <algorithm>

namespace game::sim::AbilitySystem {
namespace {

constexpr float kMaxCommandPower = 100.F;
constexpr float kAbilityCost = 50.F;
constexpr float kAbilityCooldown = 18.F;
constexpr float kNoticeSeconds = 2.2F;

bool isActive(const WorldState& world, const NationId nation) {
    return world.nationStates.at(nationIndex(nation)).abilityActiveRemaining > 0.F;
}

bool pathHasRoad(const WorldState& world, const std::vector<TileCoord>& path) {
    return std::any_of(path.begin(), path.end(), [&](const TileCoord coord) {
        return inBounds(world, coord) && tileAt(world, coord).terrain == TerrainType::Road;
    });
}

bool ownsActiveTransit(const WorldState& world, const NationId nation) {
    return std::any_of(world.activeTransits.begin(), world.activeTransits.end(), [nation](const ArmyTransit& transit) {
        return transit.owner == nation;
    });
}

int ownedRoadCount(const WorldState& world, const NationId nation) {
    return static_cast<int>(std::count_if(world.tiles.begin(), world.tiles.end(), [nation](const TileState& tile) {
        return tile.owner == nation && tile.terrain == TerrainType::Road;
    }));
}

float averageOwnedTiles(const WorldState& world) {
    float total = 0.F;
    for (const auto nation : playableNations()) {
        total += static_cast<float>(ownedTileCount(world, nation));
    }
    return total / static_cast<float>(playableNations().size());
}

bool isThreatenedOwnedTile(const WorldState& world, const NationId nation, const TileCoord coord) {
    if (!inBounds(world, coord)) {
        return false;
    }
    const auto& tile = tileAt(world, coord);
    if (tile.owner != nation) {
        return false;
    }
    for (const auto neighbor : neighbors(world, coord)) {
        const auto owner = tileAt(world, neighbor).owner;
        if (owner != nation && owner != NationId::Neutral) {
            return true;
        }
    }
    return std::any_of(world.activeTransits.begin(), world.activeTransits.end(), [&](const ArmyTransit& transit) {
        return transit.destination == coord && transit.owner != nation;
    });
}

std::optional<TileCoord> reserveTarget(const WorldState& world, const NationId nation) {
    const auto capital = capitalOf(world, nation);
    if (isThreatenedOwnedTile(world, nation, capital)) {
        return capital;
    }

    std::optional<TileCoord> best{};
    int bestTroops = 999999;
    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            const TileCoord coord{x, y};
            if (!isThreatenedOwnedTile(world, nation, coord)) {
                continue;
            }
            const auto troops = tileAt(world, coord).troops;
            if (!best || troops < bestTroops) {
                best = coord;
                bestTroops = troops;
            }
        }
    }
    return best;
}

void setNotice(WorldState& world, const NationId nation, const std::string& message) {
    auto& runtime = world.nationStates.at(nationIndex(nation));
    runtime.abilityNotice = message;
    runtime.abilityNoticeRemaining = kNoticeSeconds;
}

float commandPowerGainRate(const WorldState& world, const NationId nation) {
    float rate = 0.55F + static_cast<float>(ownedTileCount(world, nation)) * 0.015F;
    switch (nation) {
    case NationId::SwiftLeague:
        if (ownsActiveTransit(world, nation)) {
            rate += 0.35F;
        }
        break;
    case NationId::IronLegion:
        rate += static_cast<float>(ownedRoadCount(world, nation)) * 0.035F;
        break;
    case NationId::CrownConsortium:
        if (static_cast<float>(ownedTileCount(world, nation)) <= averageOwnedTiles(world)) {
            rate *= 1.25F;
        }
        break;
    case NationId::BastionDirectorate:
    case NationId::Neutral:
        break;
    }
    return rate;
}

bool shouldAiActivate(const WorldState& world, const NationId nation) {
    switch (nation) {
    case NationId::BastionDirectorate:
        return capitalThreatened(world, nation);
    case NationId::CrownConsortium:
        return hasThreatenedReserveTarget(world, nation);
    case NationId::IronLegion:
        return ownedRoadCount(world, nation) >= 2;
    case NationId::SwiftLeague:
        return ownsActiveTransit(world, nation) || ownedTileCount(world, nation) <= 10;
    case NationId::Neutral:
        return false;
    }
    return false;
}

} // namespace

float maxCommandPower() { return kMaxCommandPower; }

float abilityCost(const NationId) { return kAbilityCost; }

float abilityCooldown(const NationId) { return kAbilityCooldown; }

float abilityDuration(const NationId nation) {
    switch (nation) {
    case NationId::SwiftLeague:
    case NationId::BastionDirectorate:
        return 8.F;
    case NationId::IronLegion:
        return 10.F;
    case NationId::CrownConsortium:
        return 6.F;
    case NationId::Neutral:
        return 0.F;
    }
    return 0.F;
}

std::string_view abilityName(const NationId nation) {
    switch (nation) {
    case NationId::SwiftLeague:
        return "Tempo Surge";
    case NationId::IronLegion:
        return "Road Authority";
    case NationId::BastionDirectorate:
        return "Fortified Basin";
    case NationId::CrownConsortium:
        return "Adaptive Reserve";
    case NationId::Neutral:
        return "No Ability";
    }
    return "No Ability";
}

std::string_view passiveLine(const NationId nation) {
    switch (nation) {
    case NationId::SwiftLeague:
        return "Passive: active columns earn CP.";
    case NationId::IronLegion:
        return "Passive: roads add CP and caps.";
    case NationId::BastionDirectorate:
        return "Passive: rough capitals defend better.";
    case NationId::CrownConsortium:
        return "Passive: underdog territory earns CP.";
    case NationId::Neutral:
        return "Passive: none.";
    }
    return "Passive: none.";
}

std::string_view activeLine(const NationId nation) {
    switch (nation) {
    case NationId::SwiftLeague:
        return "Active: speed and launch caps.";
    case NationId::IronLegion:
        return "Active: road speed and cap surge.";
    case NationId::BastionDirectorate:
        return "Active: defense and capital regen.";
    case NationId::CrownConsortium:
        return "Active: reinforce threatened tile.";
    case NationId::Neutral:
        return "Active: none.";
    }
    return "Active: none.";
}

void update(WorldState& world, const float deltaSeconds, const MatchConfig&) {
    for (const auto nation : playableNations()) {
        auto& runtime = world.nationStates.at(nationIndex(nation));
        runtime.abilityCooldownRemaining = std::max(0.F, runtime.abilityCooldownRemaining - deltaSeconds);
        runtime.abilityActiveRemaining = std::max(0.F, runtime.abilityActiveRemaining - deltaSeconds);
        runtime.abilityNoticeRemaining = std::max(0.F, runtime.abilityNoticeRemaining - deltaSeconds);
        if (runtime.abilityActiveRemaining <= 0.F) {
            runtime.abilityFocusTile.reset();
        }
        if (runtime.abilityNoticeRemaining <= 0.F) {
            runtime.abilityNotice.clear();
        }
        if (runtime.eliminated) {
            continue;
        }

        const float before = runtime.commandPower;
        runtime.commandPower = std::min(kMaxCommandPower, runtime.commandPower + commandPowerGainRate(world, nation) * deltaSeconds);
        runtime.stats.commandPowerEarned += runtime.commandPower - before;
    }
}

ActivationResult activate(WorldState& world, const NationId nation, const MatchConfig& config) {
    if (!isPlayableNation(nation)) {
        return {false, "No valid target"};
    }

    auto& runtime = world.nationStates.at(nationIndex(nation));
    if (runtime.eliminated) {
        setNotice(world, nation, "No valid target");
        return {false, "No valid target"};
    }
    if (runtime.commandPower + 0.001F < abilityCost(nation)) {
        setNotice(world, nation, "Not enough Command Power");
        return {false, "Not enough Command Power"};
    }
    if (runtime.abilityCooldownRemaining > 0.F) {
        setNotice(world, nation, "Cooldown");
        return {false, "Cooldown"};
    }

    std::optional<TileCoord> focus{};
    if (nation == NationId::CrownConsortium) {
        focus = reserveTarget(world, nation);
        if (!focus) {
            setNotice(world, nation, "No valid target");
            return {false, "No valid target"};
        }
    }

    runtime.commandPower = std::max(0.F, runtime.commandPower - abilityCost(nation));
    runtime.abilityCooldownRemaining = abilityCooldown(nation);
    runtime.abilityActiveRemaining = abilityDuration(nation);
    runtime.abilityFocusTile = focus;
    ++runtime.stats.abilitiesUsed;

    if (focus) {
        auto& tile = tileAt(world, *focus);
        const int cap = tile.hasCapital ? config.capitalTroopCap : 999999;
        tile.troops = std::min(cap, tile.troops + 24);
    }

    const std::string message = std::string(abilityName(nation)) + " active";
    setNotice(world, nation, message);
    return {true, message};
}

bool activateForAi(WorldState& world, const NationId nation, const MatchConfig& config) {
    if (!isPlayableNation(nation)) {
        return false;
    }
    const auto& runtime = world.nationStates.at(nationIndex(nation));
    if (runtime.commandPower + 0.001F < abilityCost(nation) || runtime.abilityCooldownRemaining > 0.F ||
        !shouldAiActivate(world, nation)) {
        return false;
    }
    return activate(world, nation, config).activated;
}

float movementSpeedMultiplier(const WorldState& world, const NationId nation, const std::vector<TileCoord>& path) {
    float multiplier = 1.F;
    if (nation == NationId::SwiftLeague && isActive(world, nation)) {
        multiplier *= 1.25F;
    }
    if (nation == NationId::IronLegion && isActive(world, nation) && pathHasRoad(world, path)) {
        multiplier *= 1.35F;
    }
    return multiplier;
}

int launchCapBonus(const WorldState& world, const NationId nation, const std::vector<TileCoord>& path) {
    int bonus = 0;
    if (nation == NationId::SwiftLeague && isActive(world, nation)) {
        bonus += 8;
    }
    if (nation == NationId::IronLegion && pathHasRoad(world, path)) {
        bonus += 6;
        if (isActive(world, nation)) {
            bonus += 18;
        }
    }
    return bonus;
}

float defenseBonus(const WorldState& world, const NationId defender, const TileCoord coord) {
    if (!isPlayableNation(defender) || !inBounds(world, coord)) {
        return 0.F;
    }
    float bonus = 0.F;
    const auto& tile = tileAt(world, coord);
    if (defender == NationId::BastionDirectorate &&
        (tile.hasCapital || tile.terrain == TerrainType::Highland || tile.terrain == TerrainType::Mountain)) {
        bonus += 0.14F;
    }
    if (defender == NationId::BastionDirectorate && isActive(world, defender) && tile.owner == defender) {
        bonus += 0.18F;
    }
    const auto& runtime = world.nationStates.at(nationIndex(defender));
    if (defender == NationId::CrownConsortium && isActive(world, defender) && runtime.abilityFocusTile == coord) {
        bonus += 0.12F;
    }
    return bonus;
}

float capitalRegenBonus(const WorldState& world, const NationId nation) {
    if (nation == NationId::BastionDirectorate && isActive(world, nation)) {
        return 1.2F;
    }
    return 0.F;
}

bool hasThreatenedReserveTarget(const WorldState& world, const NationId nation) {
    return reserveTarget(world, nation).has_value();
}

} // namespace game::sim::AbilitySystem
