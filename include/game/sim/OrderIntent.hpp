#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/Types.hpp"

namespace game::sim {

enum class SendRatio {
    Quarter,
    Half,
    Full
};

struct OrderIntent {
    NationId issuer{NationId::Neutral};
    TileCoord origin{};
    TileCoord target{};
    SendRatio ratio{SendRatio::Half};
    bool emergency{false};
};

float ratioMultiplier(SendRatio ratio);
int requestedTroops(int availableTroops, SendRatio ratio);

} // namespace game::sim
