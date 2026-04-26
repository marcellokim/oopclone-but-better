#include "game/sim/ScoreSystem.hpp"

#include <algorithm>

namespace game::sim::ScoreSystem {

int scoreForNation(const WorldState& world, const NationId nation) {
    const auto& runtime = world.nationStates.at(nationIndex(nation));
    const auto& stats = runtime.stats;
    int score = 0;
    if (world.winner && *world.winner == nation) {
        score += 1000;
    }
    score += ownedTileCount(world, nation) * 40;
    score += totalTroops(world, nation) * 2;
    score += stats.tilesCaptured * 70;
    score += stats.capitalsCaptured * 180;
    score += stats.troopsDefeated * 3;
    score += stats.abilitiesUsed * 25;
    score -= stats.troopsLost;
    return std::max(0, score);
}

std::string gradeForScore(const int score) {
    if (score >= 1300) {
        return "S";
    }
    if (score >= 850) {
        return "A";
    }
    if (score >= 450) {
        return "B";
    }
    if (score >= 100) {
        return "C";
    }
    return "D";
}

std::vector<ScoreBreakdown> standings(const WorldState& world) {
    std::vector<ScoreBreakdown> rows;
    rows.reserve(playableNations().size());
    for (const auto nation : playableNations()) {
        const int score = scoreForNation(world, nation);
        rows.push_back({
            .nation = nation,
            .score = score,
            .grade = gradeForScore(score),
            .winner = world.winner && *world.winner == nation,
            .ownedTiles = ownedTileCount(world, nation),
            .totalTroops = totalTroops(world, nation),
            .stats = world.nationStates.at(nationIndex(nation)).stats,
        });
    }

    std::sort(rows.begin(), rows.end(), [](const ScoreBreakdown& lhs, const ScoreBreakdown& rhs) {
        if (lhs.score != rhs.score) {
            return lhs.score > rhs.score;
        }
        return static_cast<int>(lhs.nation) < static_cast<int>(rhs.nation);
    });
    return rows;
}

} // namespace game::sim::ScoreSystem
