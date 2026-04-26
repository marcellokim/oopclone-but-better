#include "game/ai/AiDirector.hpp"

#include "game/sim/AbilitySystem.hpp"
#include "game/sim/MovementSystem.hpp"

#include <optional>
#include <tuple>

namespace game::ai {
namespace {

struct Candidate {
    sim::OrderIntent order{};
    float score{0.F};
    bool emergency{false};
};

float aggressionThreshold(const NationId nation) {
    switch (nation) {
    case NationId::IronLegion:
        return 0.95F;
    case NationId::SwiftLeague:
        return 1.05F;
    case NationId::CrownConsortium:
        return 1.10F;
    case NationId::BastionDirectorate:
        return 1.20F;
    case NationId::Neutral:
        return 999.F;
    }
    return 1.0F;
}

sim::SendRatio preferredRatio(const NationId nation, const bool emergency) {
    if (emergency) {
        return sim::SendRatio::Full;
    }
    switch (nation) {
    case NationId::SwiftLeague:
        return sim::SendRatio::Quarter;
    case NationId::IronLegion:
        return sim::SendRatio::Full;
    case NationId::BastionDirectorate:
        return sim::SendRatio::Half;
    case NationId::CrownConsortium:
        return sim::SendRatio::Half;
    case NationId::Neutral:
        return sim::SendRatio::Half;
    }
    return sim::SendRatio::Half;
}

bool isFrontline(const sim::WorldState& world, const sim::TileCoord coord, const NationId nation) {
    for (const auto& neighbor : sim::neighbors(world, coord)) {
        const auto owner = sim::tileAt(world, neighbor).owner;
        if (owner != nation && owner != NationId::Neutral) {
            return true;
        }
    }
    return false;
}

std::optional<Candidate> findBestCandidate(const sim::WorldState& world, const NationId nation, const bool emergency) {
    std::optional<Candidate> best{};
    const auto& profile = nationDefinition(nation);
    const auto capital = sim::capitalOf(world, nation);

    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            const sim::TileCoord origin{x, y};
            const auto& originTile = sim::tileAt(world, origin);
            if (originTile.owner != nation || originTile.troops < 8) {
                continue;
            }

            for (const auto& neighbor : sim::neighbors(world, origin)) {
                const auto& targetTile = sim::tileAt(world, neighbor);
                Candidate candidate{};
                candidate.order.issuer = nation;
                candidate.order.origin = origin;
                candidate.order.target = neighbor;
                candidate.emergency = emergency;
                candidate.order.emergency = emergency;
                candidate.order.ratio = preferredRatio(nation, emergency);

                if (emergency && neighbor == capital && origin != capital) {
                    candidate.score = 100.F + static_cast<float>(originTile.troops);
                } else if (targetTile.owner == NationId::Neutral) {
                    candidate.score = 25.F + profile.mobility * 12.F + static_cast<float>(originTile.troops) * 0.35F;
                    if (nation == NationId::SwiftLeague) {
                        candidate.score += 10.F;
                    }
                } else if (targetTile.owner == nation) {
                    if (neighbor == capital && sim::capitalThreatened(world, nation)) {
                        candidate.score = 85.F + profile.defense * 10.F;
                    } else if (isFrontline(world, neighbor, nation) && originTile.troops > targetTile.troops) {
                        candidate.score = 18.F + profile.defense * 8.F;
                    } else {
                        continue;
                    }
                } else {
                    const float forceRatio = static_cast<float>(originTile.troops) / static_cast<float>(std::max(1, targetTile.troops));
                    if (forceRatio < aggressionThreshold(nation)) {
                        continue;
                    }
                    candidate.score = 30.F + profile.attack * 14.F + forceRatio * 12.F;
                    if (targetTile.hasCapital) {
                        candidate.score += 50.F;
                    }
                    if (nation == NationId::IronLegion) {
                        candidate.score += 8.F;
                    }
                }

                if (!best || candidate.score > best->score) {
                    best = candidate;
                }
            }
        }
    }

    return best;
}

} // namespace

void AiDirector::update(sim::WorldState& world,
                        sim::CommandQueue& queue,
                        const float deltaSeconds,
                        const MatchConfig& config) {
    for (const auto nation : playableNations()) {
        if (nation == world.playerNation) {
            continue;
        }

        auto& runtime = world.nationStates.at(sim::nationIndex(nation));
        if (runtime.eliminated) {
            continue;
        }

        static_cast<void>(sim::AbilitySystem::activateForAi(world, nation, config));

        runtime.aiAccumulator += deltaSeconds;
        runtime.commitmentRemaining = std::max(0.F, runtime.commitmentRemaining - deltaSeconds);
        const bool emergency = sim::capitalThreatened(world, nation);
        if (!emergency && (runtime.aiAccumulator < 0.8F || runtime.commitmentRemaining > 0.F)) {
            continue;
        }

        const auto candidate = findBestCandidate(world, nation, emergency);
        if (candidate) {
            queue.push(candidate->order);
            runtime.aiAccumulator = 0.F;
            runtime.commitmentRemaining = emergency ? 0.4F : 2.F;
        }
    }
}

} // namespace game::ai
