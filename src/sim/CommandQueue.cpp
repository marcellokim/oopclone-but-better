#include "game/sim/CommandQueue.hpp"

namespace game::sim {

float ratioMultiplier(const SendRatio ratio) {
    switch (ratio) {
    case SendRatio::Quarter:
        return 0.25F;
    case SendRatio::Half:
        return 0.50F;
    case SendRatio::Full:
        return 1.00F;
    }

    return 0.50F;
}

int requestedTroops(const int availableTroops, const SendRatio ratio) {
    const auto computed = static_cast<int>(availableTroops * ratioMultiplier(ratio));
    if (ratio == SendRatio::Full) {
        return availableTroops;
    }
    return computed > 0 ? computed : (availableTroops > 0 ? 1 : 0);
}

void CommandQueue::push(OrderIntent order) { m_orders.push_back(order); }

bool CommandQueue::empty() const { return m_orders.empty(); }

std::vector<OrderIntent> CommandQueue::drain() {
    auto drained = m_orders;
    m_orders.clear();
    return drained;
}

} // namespace game::sim
