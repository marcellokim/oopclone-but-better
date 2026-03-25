#pragma once

#include <array>
#include <compare>
#include <cstddef>
#include <functional>

namespace game::sim {

struct TileCoord {
    int x{};
    int y{};

    auto operator<=>(const TileCoord&) const = default;
};

inline constexpr std::array<TileCoord, 4> kCardinalDirections{{
    {0, -1},
    {1, 0},
    {0, 1},
    {-1, 0},
}};

} // namespace game::sim

namespace std {
template <>
struct hash<game::sim::TileCoord> {
    std::size_t operator()(const game::sim::TileCoord& coord) const noexcept {
        const auto xHash = std::hash<int>{}(coord.x);
        const auto yHash = std::hash<int>{}(coord.y);
        return xHash ^ (yHash << 1U);
    }
};
} // namespace std
