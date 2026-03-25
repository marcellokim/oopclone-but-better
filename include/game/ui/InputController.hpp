#pragma once

#include "game/sim/CommandQueue.hpp"
#include "game/sim/WorldState.hpp"

#include <string>

namespace game::ui {

class InputController {
  public:
    void setSendRatio(sim::SendRatio ratio);
    [[nodiscard]] sim::SendRatio sendRatio() const;
    [[nodiscard]] std::string sendRatioLabel() const;

    void clearSelection(sim::WorldState& world) const;
    bool handleTileClick(sim::WorldState& world,
                         NationId playerNation,
                         sim::TileCoord clicked,
                         sim::CommandQueue& queue) const;

  private:
    sim::SendRatio m_sendRatio{sim::SendRatio::Half};
};

} // namespace game::ui
