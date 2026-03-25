#include "game/GameConfig.hpp"

#include <stdexcept>

namespace game {

namespace {
constexpr MatchConfig kDefaultConfig{};
constexpr std::array<NationDefinition, 5> kNationDefinitions{{
    {NationId::SwiftLeague,
     "서강대학교",
     "서강",
     "Precision lanes. Strike on tempo.",
     "tempo doctrine",
     1.15F,
     1.10F,
     0.95F,
     1.00F,
     {188, 79, 92},
     true},
    {NationId::IronLegion,
     "한양대학교",
     "한양",
     "Own the roads. Push with pressure.",
     "infrastructure doctrine",
     1.00F,
     1.15F,
     1.00F,
     1.05F,
     {77, 166, 255},
     true},
    {NationId::BastionDirectorate,
     "성균관대학교",
     "성균관",
     "Hold the basin. Win the long game.",
     "stability doctrine",
     0.95F,
     1.00F,
     1.10F,
     1.10F,
     {94, 184, 128},
     true},
    {NationId::CrownConsortium,
     "중앙대학교",
     "중앙",
     "Flex the center. Reinforce any front.",
     "adaptive doctrine",
     1.05F,
     0.95F,
     1.05F,
     1.15F,
     {142, 133, 255},
     true},
    {NationId::Neutral,
     "Neutral",
     "Neutral",
     "Unaligned territory.",
     "neutral",
     1.00F,
     1.00F,
     1.00F,
     0.00F,
     {110, 110, 110},
     false},
}};
constexpr std::array<NationId, 4> kPlayableNations{{
    NationId::SwiftLeague,
    NationId::IronLegion,
    NationId::BastionDirectorate,
    NationId::CrownConsortium,
}};
} // namespace

const MatchConfig& defaultMatchConfig() { return kDefaultConfig; }

const NationDefinition& nationDefinition(const NationId id) {
    for (const auto& definition : kNationDefinitions) {
        if (definition.id == id) {
            return definition;
        }
    }

    throw std::runtime_error("Unknown nation id");
}

const std::array<NationId, 4>& playableNations() { return kPlayableNations; }

std::string_view nationName(const NationId id) { return nationDefinition(id).name; }

std::string_view nationCompactName(const NationId id) { return nationDefinition(id).compactName; }

std::string_view nationDoctrineLine(const NationId id) { return nationDefinition(id).doctrineLine; }

std::string_view nationDoctrineTag(const NationId id) { return nationDefinition(id).doctrineTag; }

std::string_view terrainName(const TerrainType terrain) {
    switch (terrain) {
    case TerrainType::Plains:
        return "Plains";
    case TerrainType::Road:
        return "Road";
    case TerrainType::Highland:
        return "Highland";
    case TerrainType::Mountain:
        return "Mountain";
    case TerrainType::Sea:
        return "Sea";
    case TerrainType::Capital:
        return "Capital";
    }

    return "Unknown";
}

float terrainDefenseBonus(const TerrainType terrain) {
    switch (terrain) {
    case TerrainType::Plains:
    case TerrainType::Road:
    case TerrainType::Sea:
        return 0.F;
    case TerrainType::Highland:
        return 0.18F;
    case TerrainType::Mountain:
        return 0.30F;
    case TerrainType::Capital:
        return 0.35F;
    }

    return 0.F;
}

float terrainMovementMultiplier(const TerrainType terrain) {
    switch (terrain) {
    case TerrainType::Road:
        return 1.30F;
    case TerrainType::Highland:
        return 0.90F;
    case TerrainType::Mountain:
        return 0.55F;
    case TerrainType::Sea:
        return 0.00F;
    case TerrainType::Capital:
        return 1.00F;
    case TerrainType::Plains:
    default:
        return 1.00F;
    }
}

int terrainThroughputCap(const TerrainType terrain) {
    switch (terrain) {
    case TerrainType::Road:
        return 72;
    case TerrainType::Plains:
    case TerrainType::Capital:
        return 48;
    case TerrainType::Highland:
        return 32;
    case TerrainType::Mountain:
        return 18;
    case TerrainType::Sea:
        return 0;
    }

    return 0;
}

bool terrainPassableForLand(const TerrainType terrain) {
    return terrain != TerrainType::Sea;
}

bool isPlayableNation(const NationId id) {
    return id != NationId::Neutral;
}

} // namespace game
