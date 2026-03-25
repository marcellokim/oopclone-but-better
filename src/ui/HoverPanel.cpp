#include "game/ui/HoverPanel.hpp"

#include <iomanip>
#include <sstream>

namespace game::ui {

std::vector<std::string> buildHoverLines(const sim::WorldState& world,
                                         const sim::TileCoord coord,
                                         const MatchConfig& config) {
    if (!sim::inBounds(world, coord)) {
        return {"Hover a tile to inspect it."};
    }

    const auto& tile = sim::tileAt(world, coord);
    std::vector<std::string> lines;
    lines.push_back("Tile (" + std::to_string(coord.x) + ", " + std::to_string(coord.y) + ")");
    lines.push_back(std::string("Owner: ") + std::string(nationName(tile.owner)));
    lines.push_back("Troops: " + std::to_string(tile.troops));
    lines.push_back(std::string("Terrain: ") + std::string(terrainName(tile.terrain)));

    if (tile.hasCapital) {
        const float regenRate = config.baseRegenPerSecond +
                                static_cast<float>(sim::ownedTileCount(world, tile.owner)) * config.territoryRegenFactor *
                                    nationDefinition(tile.owner).regen;
        std::ostringstream regenStream;
        regenStream << std::fixed << std::setprecision(2) << regenRate;
        lines.push_back("Capital: yes");
        lines.push_back("Regen/s: " + regenStream.str());
    } else {
        lines.push_back("Capital: no");
    }

    return lines;
}

} // namespace game::ui
