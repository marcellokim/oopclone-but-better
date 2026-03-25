#include "game/ui/InputController.hpp"

namespace game::ui {

void InputController::setSendRatio(const sim::SendRatio ratio) { m_sendRatio = ratio; }

sim::SendRatio InputController::sendRatio() const { return m_sendRatio; }

std::string InputController::sendRatioLabel() const {
    switch (m_sendRatio) {
    case sim::SendRatio::Quarter:
        return "25%";
    case sim::SendRatio::Half:
        return "50%";
    case sim::SendRatio::Full:
        return "100%";
    }
    return "50%";
}

void InputController::clearSelection(sim::WorldState& world) const { world.selectedTile.reset(); }

bool InputController::handleTileClick(sim::WorldState& world,
                                      const NationId playerNation,
                                      const sim::TileCoord clicked,
                                      sim::CommandQueue& queue) const {
    if (!sim::inBounds(world, clicked)) {
        return false;
    }

    const auto& clickedTile = sim::tileAt(world, clicked);
    if (!world.selectedTile.has_value()) {
        if (clickedTile.owner == playerNation) {
            world.selectedTile = clicked;
            return true;
        }
        return false;
    }

    if (*world.selectedTile == clicked) {
        world.selectedTile.reset();
        return false;
    }

    const auto origin = *world.selectedTile;
    const auto& originTile = sim::tileAt(world, origin);
    if (originTile.owner != playerNation) {
        world.selectedTile.reset();
        return false;
    }

    queue.push({playerNation, origin, clicked, m_sendRatio, false});
    world.selectedTile.reset();
    return true;
}

} // namespace game::ui
