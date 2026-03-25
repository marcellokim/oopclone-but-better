#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/ArmyTransit.hpp"
#include "game/sim/Types.hpp"

#include <array>
#include <optional>
#include <vector>

namespace game::sim {

struct TileState {
    TerrainType terrain{TerrainType::Plains};
    NationId owner{NationId::Neutral};
    int troops{0};
    bool hasCapital{false};
};

struct NationRuntimeState {
    bool eliminated{false};
    float regenAccumulator{0.F};
    float aiAccumulator{0.F};
    float commitmentRemaining{0.F};
    TileCoord capital{};
};

struct WorldState {
    int width{};
    int height{};
    std::vector<TileState> tiles{};
    std::vector<ArmyTransit> activeTransits{};
    std::array<NationRuntimeState, 4> nationStates{};
    NationId playerNation{NationId::SwiftLeague};
    std::optional<TileCoord> selectedTile{};
    std::optional<TileCoord> hoveredTile{};
    int tickCount{0};
    bool matchEnded{false};
    std::optional<NationId> winner{};
};

WorldState createInitialWorld(NationId playerNation, const MatchConfig& config = defaultMatchConfig());
std::size_t nationIndex(NationId id);
bool inBounds(const WorldState& world, TileCoord coord);
std::size_t tileIndex(const WorldState& world, TileCoord coord);
TileState& tileAt(WorldState& world, TileCoord coord);
const TileState& tileAt(const WorldState& world, TileCoord coord);
std::vector<TileCoord> neighbors(const WorldState& world, TileCoord coord);
int ownedTileCount(const WorldState& world, NationId nation);
int totalTroops(const WorldState& world, NationId nation);
int totalTroopsOnTiles(const WorldState& world, NationId nation);
TileCoord capitalOf(const WorldState& world, NationId nation);
bool capitalThreatened(const WorldState& world, NationId nation);
void refreshNationState(WorldState& world);
std::optional<NationId> strongestRemainingNation(const WorldState& world);

} // namespace game::sim
