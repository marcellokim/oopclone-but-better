#pragma once

#include "game/sim/OrderIntent.hpp"

#include <vector>

namespace game::sim {

class CommandQueue {
  public:
    void push(OrderIntent order);
    [[nodiscard]] bool empty() const;
    std::vector<OrderIntent> drain();

  private:
    std::vector<OrderIntent> m_orders{};
};

} // namespace game::sim
