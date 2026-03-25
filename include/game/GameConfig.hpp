#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace game {

enum class NationId : std::uint8_t {
    SwiftLeague = 0,
    IronLegion,
    BastionDirectorate,
    CrownConsortium,
    Neutral
};

enum class TerrainType : std::uint8_t {
    Plains = 0,
    Road,
    Highland,
    Mountain,
    Sea,
    Capital
};

struct Rgb {
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
};

struct NationDefinition {
    NationId id{NationId::Neutral};
    std::string_view name{};
    std::string_view compactName{};
    std::string_view doctrineLine{};
    std::string_view doctrineTag{};
    float mobility{1.F};
    float attack{1.F};
    float defense{1.F};
    float regen{1.F};
    Rgb color{};
    bool playable{false};
};

struct MatchConfig {
    int width{18};
    int height{12};
    float simTickSeconds{0.1F};
    int startingTroops{24};
    int capitalTroops{70};
    int capitalTroopCap{220};
    float baseRegenPerSecond{0.6F};
    float territoryRegenFactor{0.05F};
    float transitSpeedTilesPerSecond{1.6F};
};

const MatchConfig& defaultMatchConfig();
const NationDefinition& nationDefinition(NationId id);
const std::array<NationId, 4>& playableNations();
std::string_view nationName(NationId id);
std::string_view nationCompactName(NationId id);
std::string_view nationDoctrineLine(NationId id);
std::string_view nationDoctrineTag(NationId id);
std::string_view terrainName(TerrainType terrain);
float terrainDefenseBonus(TerrainType terrain);
float terrainMovementMultiplier(TerrainType terrain);
int terrainThroughputCap(TerrainType terrain);
bool terrainPassableForLand(TerrainType terrain);
bool isPlayableNation(NationId id);

} // namespace game
