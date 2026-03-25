#include "game/GameConfig.hpp"

#include <stdexcept>

namespace game {

namespace {
constexpr MatchConfig kDefaultConfig{};
constexpr std::array<NationDefinition, 5> kNationDefinitions{{
    {NationId::SwiftLeague, "Swift League", 1.30F, 0.90F, 0.85F, 1.00F, {89, 191, 255}, true},
    {NationId::IronLegion, "Iron Legion", 0.95F, 1.25F, 0.95F, 0.95F, {235, 99, 99}, true},
    {NationId::BastionDirectorate, "Bastion Directorate", 0.80F, 0.95F, 1.30F, 1.00F, {134, 117, 255}, true},
    {NationId::CrownConsortium, "Crown Consortium", 1.05F, 0.95F, 1.05F, 1.20F, {255, 196, 77}, true},
    {NationId::Neutral, "Neutral", 1.00F, 1.00F, 1.00F, 0.00F, {110, 110, 110}, false},
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

std::string_view terrainName(const TerrainType terrain) {
    switch (terrain) {
    case TerrainType::Plains:
        return "Plains";
    case TerrainType::Road:
        return "Road";
    case TerrainType::Highland:
        return "Highland";
    case TerrainType::Capital:
        return "Capital";
    }

    return "Unknown";
}

float terrainDefenseBonus(const TerrainType terrain) {
    switch (terrain) {
    case TerrainType::Plains:
    case TerrainType::Road:
        return 0.F;
    case TerrainType::Highland:
        return 0.25F;
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
        return 0.85F;
    case TerrainType::Capital:
        return 0.95F;
    case TerrainType::Plains:
    default:
        return 1.00F;
    }
}

bool isPlayableNation(const NationId id) {
    return id != NationId::Neutral;
}

} // namespace game
