#pragma once

#include "game/GameConfig.hpp"
#include "game/sim/WorldState.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace game::ui::detail {

inline constexpr sf::Color kBackground{11, 16, 23};
inline constexpr sf::Color kShell{20, 26, 35, 235};
inline constexpr sf::Color kCore{30, 38, 51, 235};
inline constexpr sf::Color kBorder{102, 116, 138, 150};
inline constexpr sf::Color kTextPrimary{239, 242, 247};
inline constexpr sf::Color kTextMuted{160, 170, 185};
inline constexpr sf::Color kAccent{214, 189, 118};
inline constexpr sf::Color kNegative{223, 119, 119};
inline constexpr sf::Color kPositive{120, 200, 165};

inline sf::Color withAlpha(const sf::Color color, const std::uint8_t alpha) {
    return {color.r, color.g, color.b, alpha};
}

inline sf::Color brighten(const sf::Color color, const float factor) {
    return {
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(color.r * factor)), 0, 255)),
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(color.g * factor)), 0, 255)),
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(color.b * factor)), 0, 255)),
        color.a,
    };
}

inline sf::Color mix(const sf::Color a, const sf::Color b, const float t) {
    const auto lerp = [t](const std::uint8_t lhs, const std::uint8_t rhs) {
        return static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(lhs + (rhs - lhs) * t)), 0, 255));
    };
    return {lerp(a.r, b.r), lerp(a.g, b.g), lerp(a.b, b.b), lerp(a.a, b.a)};
}

inline sf::FloatRect makeRect(const float x, const float y, const float width, const float height) {
    return {{x, y}, {width, height}};
}

inline std::vector<std::filesystem::path> assetSearchRoots() {
    std::vector<std::filesystem::path> roots;
    const auto appendUnique = [&roots](const std::filesystem::path& path) {
        if (path.empty()) {
            return;
        }
        const auto normalized = path.lexically_normal();
        if (std::find(roots.begin(), roots.end(), normalized) == roots.end()) {
            roots.push_back(normalized);
        }
    };

    std::error_code error;
    auto current = std::filesystem::current_path(error);
    if (!error) {
        appendUnique(current);
        auto cursor = current;
        for (int depth = 0; depth < 4 && cursor.has_parent_path(); ++depth) {
            cursor = cursor.parent_path();
            appendUnique(cursor);
        }
    }

#ifdef TERRITORY_WAR_SOURCE_DIR
    appendUnique(std::filesystem::path(TERRITORY_WAR_SOURCE_DIR));
#endif

    return roots;
}

inline std::vector<std::filesystem::path> fontCandidates(const std::array<std::string_view, 3>& fileNames) {
    std::vector<std::filesystem::path> candidates;
    const auto roots = assetSearchRoots();
    for (const auto fileName : fileNames) {
        const auto relative = std::filesystem::path("assets") / "fonts" / fileName;
        for (const auto& root : roots) {
            candidates.push_back(root / relative);
        }
    }
    return candidates;
}

inline bool loadFirstAvailable(sf::Font& font, const std::vector<std::filesystem::path>& candidates) {
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate) && font.openFromFile(candidate.string())) {
            return true;
        }
    }
    return false;
}

inline std::vector<std::filesystem::path> displayFontCandidates() {
    return fontCandidates({"IBMPlexSerif-Regular.ttf", "Lato-Regular.ttf", "IBMPlexMono-Regular.ttf"});
}

inline std::vector<std::filesystem::path> bodyFontCandidates() {
    return fontCandidates({"Lato-Regular.ttf", "IBMPlexSerif-Regular.ttf", "IBMPlexMono-Regular.ttf"});
}

inline std::vector<std::filesystem::path> monoFontCandidates() {
    return fontCandidates({"IBMPlexMono-Regular.ttf", "Lato-Regular.ttf", "IBMPlexSerif-Regular.ttf"});
}

inline std::string doctrineLine(const NationId nation) {
    switch (nation) {
    case NationId::SwiftLeague:
        return "Fast border raids and pressure through tempo.";
    case NationId::IronLegion:
        return "Overwhelm weak fronts with brutal concentrated force.";
    case NationId::BastionDirectorate:
        return "Fortify choke points, bleed invaders, counter only on certainty.";
    case NationId::CrownConsortium:
        return "Grow territory, stabilize reserves, then strike with surplus.";
    case NationId::Neutral:
        return "Unaligned territory.";
    }
    return {};
}

inline std::string doctrineTag(const NationId nation) {
    switch (nation) {
    case NationId::SwiftLeague:
        return "flanking doctrine";
    case NationId::IronLegion:
        return "shock doctrine";
    case NationId::BastionDirectorate:
        return "attrition doctrine";
    case NationId::CrownConsortium:
        return "macro doctrine";
    case NationId::Neutral:
        return "neutral";
    }
    return {};
}

inline std::string standingStatus(const bool alive, const bool player) {
    if (!alive) {
        return "defeated";
    }
    return player ? "your command" : "contesting";
}

inline std::string selectedTileSummary(const sim::WorldState& world) {
    if (!world.selectedTile.has_value()) {
        return "Select an owned tile, then choose a friendly or enemy tile to move or attack.";
    }

    const auto coord = *world.selectedTile;
    const auto& tile = sim::tileAt(world, coord);
    return "Selected tile (" + std::to_string(coord.x) + ", " + std::to_string(coord.y) + ") | " +
           std::string(nationName(tile.owner)) + " | troops " + std::to_string(tile.troops) +
           (tile.hasCapital ? " | capital" : "");
}

inline void drawLine(sf::RenderTarget& target,
                     const sf::Vector2f start,
                     const sf::Vector2f end,
                     const sf::Color color) {
    sf::VertexArray line(sf::PrimitiveType::Lines, 2);
    line[0].position = start;
    line[0].color = color;
    line[1].position = end;
    line[1].color = color;
    target.draw(line);
}

} // namespace game::ui::detail
