#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/WorldState.hpp"

#include <string>
#include <vector>

namespace game::sim::ScoreSystem {

struct ScoreBreakdown {
    NationId nation{NationId::Neutral};
    int score{0};
    std::string grade{};
    bool winner{false};
    int ownedTiles{0};
    int totalTroops{0};
    NationStats stats{};
};

[[nodiscard]] int scoreForNation(const WorldState& world, NationId nation);
[[nodiscard]] std::string gradeForScore(int score);
[[nodiscard]] std::vector<ScoreBreakdown> standings(const WorldState& world);

} // namespace game::sim::ScoreSystem
