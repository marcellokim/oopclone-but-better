#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/CommandQueue.hpp"
#include "game/sim/WorldState.hpp"

namespace game::ai {

class AiDirector {
  public:
    void update(sim::WorldState& world,
                sim::CommandQueue& queue,
                float deltaSeconds,
                const MatchConfig& config = defaultMatchConfig());
};

} // namespace game::ai
