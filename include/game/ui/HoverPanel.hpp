#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/WorldState.hpp"

#include <string>
#include <vector>

namespace game::ui {

std::vector<std::string> buildHoverLines(const sim::WorldState& world,
                                         sim::TileCoord coord,
                                         const MatchConfig& config = defaultMatchConfig());

} // namespace game::ui
