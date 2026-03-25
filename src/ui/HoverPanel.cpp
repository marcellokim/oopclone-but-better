#include "game/ui/HoverPanel.hpp"

#include <iomanip>
#include <sstream>

namespace game::ui {

namespace {

std::string movementLine(const TerrainType terrain) {
    if (terrain == TerrainType::Sea) {
        return "Move: blocked for land orders";
    }

    std::ostringstream moveStream;
    moveStream << std::fixed << std::setprecision(2) << terrainMovementMultiplier(terrain);
    return "Move: " + moveStream.str() + "x speed";
}

std::string throughputLine(const TerrainType terrain) {
    const int cap = game::terrainThroughputCap(terrain);
    return cap > 0 ? "Launch cap: " + std::to_string(cap) + " troops" : "Launch cap: blocked";
}

} // namespace

std::vector<std::string> buildHoverLines(const sim::WorldState& world,
                                         const sim::TileCoord coord,
                                         const MatchConfig& config) {
    if (!sim::inBounds(world, coord)) {
        return {"Hover a tile to inspect it."};
    }

    const auto& tile = sim::tileAt(world, coord);
    std::vector<std::string> lines;
    lines.push_back("Tile (" + std::to_string(coord.x) + ", " + std::to_string(coord.y) + ") | " +
                    std::string(terrainName(tile.terrain)));
    lines.push_back(std::string("Owner: ") + std::string(nationName(tile.owner)) + " | troops " + std::to_string(tile.troops));

    if (tile.hasCapital) {
        const float regenRate = config.baseRegenPerSecond +
                                static_cast<float>(sim::ownedTileCount(world, tile.owner)) * config.territoryRegenFactor *
                                    nationDefinition(tile.owner).regen;
        std::ostringstream regenStream;
        regenStream << std::fixed << std::setprecision(2) << regenRate;
        lines.push_back("Capital: yes | regen/s " + regenStream.str());
    } else {
        lines.push_back("Capital: no");
    }

    lines.push_back(movementLine(tile.terrain));
    lines.push_back(throughputLine(tile.terrain));

    return lines;
}

} // namespace game::ui
