#include "game/ui/Renderer.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace game::ui {
namespace {

constexpr sf::Color kBackground{11, 16, 23};
constexpr sf::Color kShell{20, 26, 35, 235};
constexpr sf::Color kCore{30, 38, 51, 235};
constexpr sf::Color kBorder{102, 116, 138, 150};
constexpr sf::Color kTextPrimary{239, 242, 247};
constexpr sf::Color kTextMuted{160, 170, 185};
constexpr sf::Color kAccent{214, 189, 118};
constexpr sf::Color kNegative{223, 119, 119};
constexpr sf::Color kPositive{120, 200, 165};

sf::Color withAlpha(const sf::Color color, const std::uint8_t alpha) {
    return {color.r, color.g, color.b, alpha};
}

sf::Color brighten(const sf::Color color, const float factor) {
    return {
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(color.r * factor)), 0, 255)),
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(color.g * factor)), 0, 255)),
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(color.b * factor)), 0, 255)),
        color.a,
    };
}

sf::Color mix(const sf::Color a, const sf::Color b, const float t) {
    const auto lerp = [t](const std::uint8_t lhs, const std::uint8_t rhs) {
        return static_cast<std::uint8_t>(std::clamp(static_cast<int>(std::round(lhs + (rhs - lhs) * t)), 0, 255));
    };
    return {lerp(a.r, b.r), lerp(a.g, b.g), lerp(a.b, b.b), lerp(a.a, b.a)};
}

sf::FloatRect makeRect(const float x, const float y, const float width, const float height) {
    return {{x, y}, {width, height}};
}

bool loadFirstAvailable(sf::Font& font, const std::vector<std::filesystem::path>& candidates) {
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate) && font.openFromFile(candidate.string())) {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> displayFontCandidates() {
    return {
        "assets/fonts/Georgia.ttf",
        "/System/Library/Fonts/Supplemental/Georgia.ttf",
        "/System/Library/Fonts/SFGeorgian.ttf",
        "/System/Library/Fonts/Supplemental/Baskerville.ttc",
        "assets/fonts/DejaVuSans.ttf",
    };
}

std::vector<std::filesystem::path> bodyFontCandidates() {
    return {
        "assets/fonts/Avenir Next.ttc",
        "/System/Library/Fonts/Avenir Next.ttc",
        "/System/Library/Fonts/Avenir.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "assets/fonts/DejaVuSans.ttf",
    };
}

std::vector<std::filesystem::path> monoFontCandidates() {
    return {
        "assets/fonts/Menlo.ttc",
        "/System/Library/Fonts/Menlo.ttc",
        "/System/Library/Fonts/Supplemental/Menlo.ttc",
        "assets/fonts/DejaVuSans.ttf",
    };
}

std::string doctrineLine(const NationId nation) {
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

std::string doctrineTag(const NationId nation) {
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

std::string standingStatus(const bool alive, const bool player) {
    if (!alive) {
        return "defeated";
    }
    return player ? "your command" : "contesting";
}

std::string selectedTileSummary(const sim::WorldState& world) {
    if (!world.selectedTile.has_value()) {
        return "Select an owned tile, then choose a friendly or enemy tile to move or attack.";
    }

    const auto coord = *world.selectedTile;
    const auto& tile = sim::tileAt(world, coord);
    return "Selected tile (" + std::to_string(coord.x) + ", " + std::to_string(coord.y) + ") | " +
           std::string(nationName(tile.owner)) + " | troops " + std::to_string(tile.troops) +
           (tile.hasCapital ? " | capital" : "");
}

void drawLine(sf::RenderTarget& target,
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

} // namespace

Renderer::Renderer() { static_cast<void>(loadFonts()); }

bool Renderer::hasFont() const { return m_hasDisplayFont || m_hasBodyFont || m_hasMonoFont; }

bool Renderer::loadFonts() {
    m_hasDisplayFont = loadFirstAvailable(m_displayFont, displayFontCandidates());
    m_hasBodyFont = loadFirstAvailable(m_bodyFont, bodyFontCandidates());
    m_hasMonoFont = loadFirstAvailable(m_monoFont, monoFontCandidates());
    return hasFont();
}

const sf::Font* Renderer::fontFor(const FontRole role) const {
    switch (role) {
    case FontRole::Display:
        if (m_hasDisplayFont) {
            return &m_displayFont;
        }
        if (m_hasBodyFont) {
            return &m_bodyFont;
        }
        if (m_hasMonoFont) {
            return &m_monoFont;
        }
        break;
    case FontRole::Body:
        if (m_hasBodyFont) {
            return &m_bodyFont;
        }
        if (m_hasDisplayFont) {
            return &m_displayFont;
        }
        if (m_hasMonoFont) {
            return &m_monoFont;
        }
        break;
    case FontRole::Mono:
        if (m_hasMonoFont) {
            return &m_monoFont;
        }
        if (m_hasBodyFont) {
            return &m_bodyFont;
        }
        if (m_hasDisplayFont) {
            return &m_displayFont;
        }
        break;
    }
    return nullptr;
}

void Renderer::drawText(sf::RenderTarget& target,
                        const std::string& value,
                        const sf::Vector2f position,
                        const unsigned int size,
                        const sf::Color color,
                        const FontRole role,
                        const bool centered,
                        const float letterSpacing) const {
    const auto* font = fontFor(role);
    if (font == nullptr) {
        return;
    }

    sf::Text text(*font, value, size);
    text.setFillColor(color);
    text.setLetterSpacing(letterSpacing);
    if (centered) {
        const auto bounds = text.getLocalBounds();
        text.setOrigin({bounds.position.x + bounds.size.x / 2.F, bounds.position.y + bounds.size.y / 2.F});
    }
    text.setPosition(position);
    target.draw(text);
}

void Renderer::drawAmbientBackdrop(sf::RenderTarget& target, const sf::Vector2u size) const {
    sf::RectangleShape base(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));
    base.setFillColor(kBackground);
    target.draw(base);

    const std::array<sf::Vector2f, 4> centers{{
        {120.F, 90.F},
        {1040.F, 120.F},
        {180.F, 730.F},
        {1130.F, 650.F},
    }};
    const std::array<sf::Color, 4> colors{{
        withAlpha(kAccent, 28),
        withAlpha(sf::Color(81, 132, 201), 22),
        withAlpha(sf::Color(125, 102, 198), 20),
        withAlpha(sf::Color(86, 161, 140), 18),
    }};
    const std::array<float, 4> radii{{230.F, 200.F, 180.F, 210.F}};

    for (std::size_t index = 0; index < centers.size(); ++index) {
        sf::CircleShape orb(radii[index]);
        orb.setOrigin({radii[index], radii[index]});
        orb.setPosition(centers[index]);
        orb.setFillColor(colors[index]);
        target.draw(orb);
    }
}

void Renderer::drawPanel(sf::RenderTarget& target,
                         const sf::FloatRect& rect,
                         const sf::Color shellColor,
                         const sf::Color coreColor,
                         const sf::Color borderColor,
                         const bool accentTop) const {
    sf::RectangleShape shadow({rect.size.x, rect.size.y});
    shadow.setPosition({rect.position.x, rect.position.y + 8.F});
    shadow.setFillColor(sf::Color(0, 0, 0, 70));
    target.draw(shadow);

    sf::RectangleShape shell({rect.size.x, rect.size.y});
    shell.setPosition(rect.position);
    shell.setFillColor(shellColor);
    shell.setOutlineThickness(1.F);
    shell.setOutlineColor(borderColor);
    target.draw(shell);

    sf::RectangleShape core({rect.size.x - 16.F, rect.size.y - 16.F});
    core.setPosition({rect.position.x + 8.F, rect.position.y + 8.F});
    core.setFillColor(coreColor);
    core.setOutlineThickness(1.F);
    core.setOutlineColor(withAlpha(brighten(borderColor, 1.2F), 110));
    target.draw(core);

    if (accentTop) {
        sf::RectangleShape accent({core.getSize().x, 4.F});
        accent.setPosition({core.getPosition().x, core.getPosition().y});
        accent.setFillColor(withAlpha(kAccent, 210));
        target.draw(accent);
    }
}

void Renderer::drawChip(sf::RenderTarget& target,
                        const sf::FloatRect& rect,
                        const std::string& label,
                        const sf::Color fill,
                        const sf::Color textColor,
                        const FontRole role) const {
    sf::RectangleShape chip({rect.size.x, rect.size.y});
    chip.setPosition(rect.position);
    chip.setFillColor(fill);
    chip.setOutlineThickness(1.F);
    chip.setOutlineColor(withAlpha(brighten(fill, 1.25F), 120));
    target.draw(chip);
    drawText(target,
             label,
             {rect.position.x + rect.size.x / 2.F, rect.position.y + rect.size.y / 2.F - 1.F},
             13,
             textColor,
             role,
             true,
             role == FontRole::Display ? 0.94F : 1.F);
}

Renderer::SelectionLayout Renderer::selectionLayout() const {
    return {
        .heroRect = makeRect(58.F, 84.F, 360.F, 590.F),
        .nationCards = {{
            makeRect(452.F, 96.F, 326.F, 250.F),
            makeRect(806.F, 72.F, 360.F, 214.F),
            makeRect(442.F, 382.F, 350.F, 286.F),
            makeRect(822.F, 326.F, 372.F, 296.F),
        }},
        .footerRect = makeRect(58.F, 696.F, 1136.F, 58.F),
    };
}

Renderer::MatchLayout Renderer::matchLayout(const sim::WorldState& world) const {
    MatchLayout layout{};
    layout.topStrip = makeRect(34.F, 24.F, 1212.F, 74.F);
    layout.mapShell = makeRect(34.F, 118.F, 856.F, 626.F);
    layout.mapViewport = makeRect(58.F, 142.F, 808.F, 548.F);
    layout.rightRail = makeRect(910.F, 118.F, 336.F, 626.F);
    layout.commanderPanel = makeRect(926.F, 134.F, 304.F, 106.F);
    layout.hoverPanel = makeRect(926.F, 256.F, 304.F, 198.F);
    layout.frontsPanel = makeRect(926.F, 470.F, 304.F, 176.F);
    layout.objectivePanel = makeRect(926.F, 662.F, 304.F, 66.F);
    layout.bottomChip = makeRect(90.F, 650.F, 492.F, 74.F);

    const float availableTileWidth = layout.mapViewport.size.x - 28.F;
    const float availableTileHeight = layout.mapViewport.size.y - 28.F;
    layout.tileSize = std::floor(std::min(availableTileWidth / static_cast<float>(world.width),
                                          availableTileHeight / static_cast<float>(world.height)));
    const float usedWidth = layout.tileSize * static_cast<float>(world.width);
    const float usedHeight = layout.tileSize * static_cast<float>(world.height);
    layout.mapOrigin = {layout.mapViewport.position.x + (layout.mapViewport.size.x - usedWidth) / 2.F,
                        layout.mapViewport.position.y + (layout.mapViewport.size.y - usedHeight) / 2.F};
    return layout;
}

sf::Color Renderer::nationColor(const NationId nation) const {
    const auto rgb = nationDefinition(nation).color;
    return {rgb.r, rgb.g, rgb.b};
}

sf::Color Renderer::tileColor(const sim::TileState& tile) const {
    if (tile.owner == NationId::Neutral) {
        return tile.terrain == TerrainType::Road ? sf::Color(76, 86, 98) : sf::Color(56, 64, 76);
    }

    auto base = mix(nationColor(tile.owner), sf::Color(18, 22, 29), 0.48F);
    if (tile.terrain == TerrainType::Road) {
        base = brighten(base, 1.18F);
    } else if (tile.terrain == TerrainType::Highland) {
        base = mix(base, sf::Color(38, 44, 58), 0.28F);
    } else if (tile.terrain == TerrainType::Capital) {
        base = brighten(base, 1.10F);
    }
    return base;
}

void Renderer::drawSelectionScreen(sf::RenderWindow& window, const NationId highlightedNation) const {
    drawAmbientBackdrop(window, window.getSize());
    const auto layout = selectionLayout();

    drawPanel(window, layout.heroRect, withAlpha(kShell, 245), withAlpha(kCore, 240), kBorder, true);
    drawText(window, "TACTICAL COMMAND DECK", {layout.heroRect.position.x + 28.F, layout.heroRect.position.y + 38.F}, 14,
             kAccent, FontRole::Mono, false, 1.18F);
    drawText(window,
             "Select a war doctrine\nand take the table.",
             {layout.heroRect.position.x + 28.F, layout.heroRect.position.y + 92.F},
             44,
             kTextPrimary,
             FontRole::Display,
             false,
             0.92F);
    drawText(window,
             "Each nation bends the same battlefield in a different direction: speed, force, attrition, or macro control.",
             {layout.heroRect.position.x + 30.F, layout.heroRect.position.y + 212.F},
             20,
             kTextMuted,
             FontRole::Body,
             false,
             0.98F);

    const std::array<std::string, 3> bullets{{
        "Mouse-first controls with keyboard send-ratio shortcuts.",
        "Capitals regenerate faster as your territory grows.",
        "Capital loss or total force collapse ends the war immediately.",
    }};
    float bulletY = layout.heroRect.position.y + 314.F;
    for (const auto& bullet : bullets) {
        sf::CircleShape dot(4.F);
        dot.setOrigin({4.F, 4.F});
        dot.setPosition({layout.heroRect.position.x + 36.F, bulletY + 8.F});
        dot.setFillColor(kAccent);
        window.draw(dot);
        drawText(window, bullet, {layout.heroRect.position.x + 54.F, bulletY}, 17, kTextPrimary, FontRole::Body);
        bulletY += 42.F;
    }

    drawChip(window,
             makeRect(layout.heroRect.position.x + 28.F, layout.heroRect.position.y + layout.heroRect.size.y - 92.F, 144.F, 34.F),
             "keys 1-4",
             withAlpha(kAccent, 44),
             kTextPrimary,
             FontRole::Mono);
    drawChip(window,
             makeRect(layout.heroRect.position.x + 184.F, layout.heroRect.position.y + layout.heroRect.size.y - 92.F, 142.F, 34.F),
             "mouse ready",
             withAlpha(sf::Color(90, 123, 163), 44),
             kTextPrimary,
             FontRole::Mono);

    const auto nations = playableNations();
    for (std::size_t index = 0; index < nations.size(); ++index) {
        const auto nation = nations[index];
        const auto& profile = nationDefinition(nation);
        const auto rect = layout.nationCards[index];
        const bool highlighted = highlightedNation == nation;
        const auto nationAccent = brighten(nationColor(nation), highlighted ? 1.18F : 1.0F);

        drawPanel(window,
                  rect,
                  withAlpha(mix(kShell, nationAccent, 0.12F), 246),
                  withAlpha(mix(kCore, nationAccent, 0.06F), 244),
                  withAlpha(nationAccent, highlighted ? 220 : 140),
                  highlighted);

        sf::RectangleShape accentBand({6.F, rect.size.y - 24.F});
        accentBand.setPosition({rect.position.x + 12.F, rect.position.y + 12.F});
        accentBand.setFillColor(withAlpha(nationAccent, 220));
        window.draw(accentBand);

        drawText(window, doctrineTag(nation), {rect.position.x + 34.F, rect.position.y + 34.F}, 13, kTextMuted, FontRole::Mono, false, 1.16F);
        drawText(window, std::string(profile.name), {rect.position.x + 34.F, rect.position.y + 74.F}, 30, kTextPrimary, FontRole::Display, false, 0.94F);
        drawText(window, doctrineLine(nation), {rect.position.x + 34.F, rect.position.y + 116.F}, 16, kTextMuted, FontRole::Body);

        drawChip(window, makeRect(rect.position.x + 34.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "mob " + std::to_string(profile.mobility).substr(0, 4), withAlpha(nationAccent, 44), kTextPrimary, FontRole::Mono);
        drawChip(window, makeRect(rect.position.x + 116.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "atk " + std::to_string(profile.attack).substr(0, 4), withAlpha(nationAccent, 38), kTextPrimary, FontRole::Mono);
        drawChip(window, makeRect(rect.position.x + 198.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "def " + std::to_string(profile.defense).substr(0, 4), withAlpha(nationAccent, 34), kTextPrimary, FontRole::Mono);
        drawChip(window, makeRect(rect.position.x + 280.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "reg " + std::to_string(profile.regen).substr(0, 4), withAlpha(nationAccent, 30), kTextPrimary, FontRole::Mono);

        drawText(window,
                 "Press " + std::to_string(index + 1) + " or click to command this doctrine.",
                 {rect.position.x + 34.F, rect.position.y + rect.size.y - 40.F},
                 15,
                 highlighted ? kTextPrimary : kTextMuted,
                 FontRole::Body);
    }

    drawPanel(window, layout.footerRect, withAlpha(kShell, 220), withAlpha(kCore, 220), withAlpha(kBorder, 110));
    drawText(window,
             "Preserved controls: left click select / command · right click or Esc to clear · capitals regenerate with territory held.",
             {layout.footerRect.position.x + 28.F, layout.footerRect.position.y + 20.F},
             15,
             kTextMuted,
             FontRole::Body);
}

void Renderer::drawMatch(sf::RenderWindow& window,
                         const sim::WorldState& world,
                         const InputController& inputController) const {
    drawAmbientBackdrop(window, window.getSize());
    const auto layout = matchLayout(world);

    drawPanel(window, layout.topStrip, withAlpha(kShell, 236), withAlpha(kCore, 234), kBorder, true);
    drawText(window, "LIVE THEATER", {layout.topStrip.position.x + 26.F, layout.topStrip.position.y + 20.F}, 13, kAccent, FontRole::Mono, false, 1.18F);
    drawText(window, "Realtime 4-Nation Territory War", {layout.topStrip.position.x + 24.F, layout.topStrip.position.y + 44.F}, 24, kTextPrimary, FontRole::Display, false, 0.95F);
    drawChip(window, makeRect(layout.topStrip.position.x + 548.F, layout.topStrip.position.y + 20.F, 152.F, 34.F),
             std::string(nationName(world.playerNation)), withAlpha(nationColor(world.playerNation), 54), kTextPrimary, FontRole::Body);
    drawChip(window, makeRect(layout.topStrip.position.x + 714.F, layout.topStrip.position.y + 20.F, 150.F, 34.F),
             "ratio " + inputController.sendRatioLabel(), withAlpha(kAccent, 44), kTextPrimary, FontRole::Mono);
    drawChip(window, makeRect(layout.topStrip.position.x + 878.F, layout.topStrip.position.y + 20.F, 338.F, 34.F),
             "objective: hold your capital or erase every rival", withAlpha(sf::Color(79, 100, 128), 42), kTextPrimary, FontRole::Body);

    drawPanel(window, layout.mapShell, withAlpha(kShell, 248), withAlpha(kCore, 246), withAlpha(kBorder, 150));

    sf::RectangleShape mapSurface({layout.mapViewport.size.x, layout.mapViewport.size.y});
    mapSurface.setPosition(layout.mapViewport.position);
    mapSurface.setFillColor(sf::Color(17, 22, 30));
    mapSurface.setOutlineThickness(1.F);
    mapSurface.setOutlineColor(withAlpha(kBorder, 90));
    window.draw(mapSurface);

    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            const sim::TileCoord coord{x, y};
            const auto& tile = sim::tileAt(world, coord);
            const sf::Vector2f tilePos{layout.mapOrigin.x + x * layout.tileSize, layout.mapOrigin.y + y * layout.tileSize};
            const sf::Vector2f tileSize{layout.tileSize - 3.F, layout.tileSize - 3.F};

            sf::RectangleShape tileShadow(tileSize);
            tileShadow.setPosition({tilePos.x, tilePos.y + 4.F});
            tileShadow.setFillColor(sf::Color(0, 0, 0, 50));
            window.draw(tileShadow);

            const bool selected = world.selectedTile && *world.selectedTile == coord;
            const bool hovered = world.hoveredTile && *world.hoveredTile == coord;

            sf::RectangleShape outer(tileSize);
            outer.setPosition(tilePos);
            outer.setFillColor(brighten(tileColor(tile), selected ? 1.08F : 1.0F));
            outer.setOutlineThickness(selected ? 3.F : (hovered ? 2.F : 1.F));
            outer.setOutlineColor(selected ? withAlpha(kAccent, 240) : (hovered ? withAlpha(sf::Color::White, 180) : withAlpha(kBorder, 95)));
            window.draw(outer);

            sf::RectangleShape inner({tileSize.x - 10.F, tileSize.y - 10.F});
            inner.setPosition({tilePos.x + 5.F, tilePos.y + 5.F});
            inner.setFillColor(withAlpha(brighten(tileColor(tile), tile.hasCapital ? 1.12F : 0.9F), 220));
            window.draw(inner);

            if (tile.terrain == TerrainType::Road) {
                sf::RectangleShape road({tileSize.x - 12.F, 4.F});
                road.setPosition({tilePos.x + 6.F, tilePos.y + tileSize.y / 2.F - 2.F});
                road.setFillColor(withAlpha(sf::Color(204, 210, 222), 90));
                window.draw(road);
            }
            if (tile.terrain == TerrainType::Highland) {
                sf::RectangleShape ridge({tileSize.x - 14.F, 3.F});
                ridge.setPosition({tilePos.x + 7.F, tilePos.y + 10.F});
                ridge.setFillColor(withAlpha(sf::Color(236, 211, 150), 110));
                window.draw(ridge);
            }
            if (tile.hasCapital) {
                sf::CircleShape halo(layout.tileSize * 0.28F);
                halo.setOrigin({layout.tileSize * 0.28F, layout.tileSize * 0.28F});
                halo.setPosition({tilePos.x + tileSize.x / 2.F, tilePos.y + tileSize.y / 2.F});
                halo.setFillColor(withAlpha(nationColor(tile.owner), 55));
                window.draw(halo);
                sf::RectangleShape crest({14.F, 14.F});
                crest.setPosition({tilePos.x + tileSize.x - 20.F, tilePos.y + 6.F});
                crest.setFillColor(withAlpha(kAccent, 200));
                crest.setOutlineThickness(1.F);
                crest.setOutlineColor(withAlpha(sf::Color::White, 120));
                window.draw(crest);
            }

            const auto troopChipRect = makeRect(tilePos.x + 6.F, tilePos.y + tileSize.y - 24.F, tileSize.x - 12.F, 18.F);
            sf::RectangleShape troopChip({troopChipRect.size.x, troopChipRect.size.y});
            troopChip.setPosition(troopChipRect.position);
            troopChip.setFillColor(withAlpha(sf::Color(8, 11, 16), 175));
            troopChip.setOutlineThickness(1.F);
            troopChip.setOutlineColor(withAlpha(sf::Color::White, 40));
            window.draw(troopChip);
            drawText(window,
                     std::to_string(tile.troops),
                     {troopChipRect.position.x + troopChipRect.size.x / 2.F, troopChipRect.position.y + troopChipRect.size.y / 2.F - 1.F},
                     13,
                     kTextPrimary,
                     FontRole::Mono,
                     true,
                     0.96F);
        }
    }

    for (const auto& transit : world.activeTransits) {
        sim::TileCoord nextCoord = transit.destination;
        sim::TileCoord prevCoord = transit.origin;
        if (transit.nextWaypoint > 0 && transit.nextWaypoint - 1 < transit.path.size()) {
            prevCoord = transit.path[transit.nextWaypoint - 1];
        }
        if (transit.nextWaypoint < transit.path.size()) {
            nextCoord = transit.path[transit.nextWaypoint];
        }

        const sf::Vector2f start{layout.mapOrigin.x + prevCoord.x * layout.tileSize + layout.tileSize / 2.F,
                                 layout.mapOrigin.y + prevCoord.y * layout.tileSize + layout.tileSize / 2.F};
        const sf::Vector2f end{layout.mapOrigin.x + nextCoord.x * layout.tileSize + layout.tileSize / 2.F,
                               layout.mapOrigin.y + nextCoord.y * layout.tileSize + layout.tileSize / 2.F};
        const sf::Vector2f current{start.x + (end.x - start.x) * transit.progressToNext,
                                   start.y + (end.y - start.y) * transit.progressToNext};

        drawLine(window, start, current, withAlpha(nationColor(transit.owner), 120));

        sf::CircleShape token(10.F);
        token.setOrigin({10.F, 10.F});
        token.setPosition(current);
        token.setFillColor(brighten(nationColor(transit.owner), 1.08F));
        token.setOutlineThickness(2.F);
        token.setOutlineColor(withAlpha(sf::Color::Black, 170));
        window.draw(token);
        drawText(window,
                 std::to_string(transit.troops),
                 {current.x, current.y - 20.F},
                 13,
                 kTextPrimary,
                 FontRole::Mono,
                 true,
                 0.96F);
    }

    drawPanel(window, layout.commanderPanel, withAlpha(kShell, 244), withAlpha(kCore, 242), withAlpha(kBorder, 140), true);
    drawText(window, "COMMAND", {layout.commanderPanel.position.x + 18.F, layout.commanderPanel.position.y + 18.F}, 13, kAccent, FontRole::Mono, false, 1.16F);
    drawText(window,
             std::string(nationName(world.playerNation)),
             {layout.commanderPanel.position.x + 18.F, layout.commanderPanel.position.y + 46.F},
             22,
             nationColor(world.playerNation),
             FontRole::Display,
             false,
             0.95F);
    drawText(window,
             "Send ratio  " + inputController.sendRatioLabel() + "   |   right click to clear",
             {layout.commanderPanel.position.x + 18.F, layout.commanderPanel.position.y + 78.F},
             15,
             kTextMuted,
             FontRole::Body);

    drawPanel(window, layout.hoverPanel, withAlpha(kShell, 244), withAlpha(kCore, 242), withAlpha(kBorder, 140));
    drawText(window, "HOVER INTEL", {layout.hoverPanel.position.x + 18.F, layout.hoverPanel.position.y + 18.F}, 13, kAccent, FontRole::Mono, false, 1.16F);
    const auto hoverLines = world.hoveredTile ? buildHoverLines(world, *world.hoveredTile) : std::vector<std::string>{"Hover a tile to inspect owner, troops, terrain, and capital regen."};
    float hoverY = layout.hoverPanel.position.y + 54.F;
    for (const auto& line : hoverLines) {
        drawText(window, line, {layout.hoverPanel.position.x + 18.F, hoverY}, 15, kTextPrimary, FontRole::Body);
        hoverY += 28.F;
    }

    drawPanel(window, layout.frontsPanel, withAlpha(kShell, 244), withAlpha(kCore, 242), withAlpha(kBorder, 140));
    drawText(window, "LIVE FRONTS", {layout.frontsPanel.position.x + 18.F, layout.frontsPanel.position.y + 18.F}, 13, kAccent, FontRole::Mono, false, 1.16F);
    float frontY = layout.frontsPanel.position.y + 54.F;
    for (const auto nation : playableNations()) {
        const bool alive = !world.nationStates.at(sim::nationIndex(nation)).eliminated;
        const bool player = nation == world.playerNation;
        const auto panelRect = makeRect(layout.frontsPanel.position.x + 14.F, frontY, layout.frontsPanel.size.x - 28.F, 28.F);
        sf::RectangleShape strip({panelRect.size.x, panelRect.size.y});
        strip.setPosition(panelRect.position);
        strip.setFillColor(withAlpha(mix(kCore, nationColor(nation), alive ? 0.18F : 0.05F), 210));
        strip.setOutlineThickness(1.F);
        strip.setOutlineColor(withAlpha(nationColor(nation), alive ? 120 : 50));
        window.draw(strip);
        drawText(window, std::string(nationName(nation)), {panelRect.position.x + 12.F, panelRect.position.y + 7.F}, 14,
                 alive ? kTextPrimary : kTextMuted, FontRole::Body);
        drawText(window,
                 std::to_string(sim::ownedTileCount(world, nation)) + " tiles  •  " + std::to_string(sim::totalTroops(world, nation)) + " troops",
                 {panelRect.position.x + 124.F, panelRect.position.y + 7.F},
                 13,
                 kTextMuted,
                 FontRole::Mono);
        drawText(window,
                 standingStatus(alive, player),
                 {panelRect.position.x + panelRect.size.x - 12.F, panelRect.position.y + 7.F},
                 13,
                 alive ? (player ? kAccent : kPositive) : kNegative,
                 FontRole::Mono,
                 false,
                 1.02F);
        frontY += 34.F;
    }

    drawPanel(window, layout.objectivePanel, withAlpha(kShell, 244), withAlpha(kCore, 242), withAlpha(kBorder, 140));
    drawText(window, "WIN / LOSE", {layout.objectivePanel.position.x + 18.F, layout.objectivePanel.position.y + 18.F}, 13, kAccent, FontRole::Mono, false, 1.16F);
    drawText(window, "Capture capitals or erase total enemy force. Lose your capital and the table is gone.",
             {layout.objectivePanel.position.x + 18.F, layout.objectivePanel.position.y + 40.F}, 14, kTextMuted, FontRole::Body);

    drawPanel(window, layout.bottomChip, withAlpha(kShell, 240), withAlpha(kCore, 238), withAlpha(kBorder, 135));
    drawText(window, "COMMAND PREVIEW", {layout.bottomChip.position.x + 18.F, layout.bottomChip.position.y + 18.F}, 13, kAccent, FontRole::Mono, false, 1.16F);
    drawText(window, selectedTileSummary(world), {layout.bottomChip.position.x + 18.F, layout.bottomChip.position.y + 42.F}, 15, kTextPrimary, FontRole::Body);
}

void Renderer::drawGameOver(sf::RenderWindow& window, const sim::WorldState& world) const {
    drawMatch(window, world, InputController{});

    sf::RectangleShape overlay({1280.F, 800.F});
    overlay.setFillColor(sf::Color(4, 6, 10, 205));
    window.draw(overlay);

    const auto panel = makeRect(260.F, 144.F, 760.F, 470.F);
    drawPanel(window, panel, withAlpha(kShell, 248), withAlpha(kCore, 246), withAlpha(kBorder, 160), true);

    drawText(window, "POST-BATTLE DEBRIEF", {panel.position.x + 28.F, panel.position.y + 30.F}, 14, kAccent, FontRole::Mono, false, 1.16F);
    drawText(window, "Battle concluded", {panel.position.x + 28.F, panel.position.y + 82.F}, 46, kTextPrimary, FontRole::Display, false, 0.93F);

    if (world.winner.has_value()) {
        const auto winner = *world.winner;
        drawText(window,
                 std::string(nationName(winner)) + (winner == world.playerNation ? " secured the field." : " outlasted every rival."),
                 {panel.position.x + 30.F, panel.position.y + 144.F},
                 24,
                 nationColor(winner),
                 FontRole::Display,
                 false,
                 0.96F);
    } else {
        drawText(window,
                 "Mutual destruction. No nation survived the final tick.",
                 {panel.position.x + 30.F, panel.position.y + 144.F},
                 24,
                 kAccent,
                 FontRole::Display,
                 false,
                 0.96F);
    }

    drawText(window,
             "Final standings",
             {panel.position.x + 30.F, panel.position.y + 204.F},
             20,
             kTextPrimary,
             FontRole::Body);

    float rowY = panel.position.y + 244.F;
    for (const auto nation : playableNations()) {
        const auto rowRect = makeRect(panel.position.x + 28.F, rowY, panel.size.x - 56.F, 42.F);
        sf::RectangleShape row({rowRect.size.x, rowRect.size.y});
        row.setPosition(rowRect.position);
        row.setFillColor(withAlpha(mix(kCore, nationColor(nation), 0.12F), 220));
        row.setOutlineThickness(1.F);
        row.setOutlineColor(withAlpha(nationColor(nation), 120));
        window.draw(row);

        const bool alive = !world.nationStates.at(sim::nationIndex(nation)).eliminated;
        drawText(window, std::string(nationName(nation)), {rowRect.position.x + 14.F, rowRect.position.y + 11.F}, 17,
                 alive ? nationColor(nation) : kTextMuted, FontRole::Body);
        drawText(window,
                 std::to_string(sim::ownedTileCount(world, nation)) + " tiles   " + std::to_string(sim::totalTroops(world, nation)) + " troops",
                 {rowRect.position.x + 262.F, rowRect.position.y + 11.F},
                 15,
                 kTextMuted,
                 FontRole::Mono);
        drawText(window,
                 standingStatus(alive, nation == world.playerNation),
                 {rowRect.position.x + rowRect.size.x - 128.F, rowRect.position.y + 11.F},
                 15,
                 alive ? (nation == world.playerNation ? kAccent : kPositive) : kNegative,
                 FontRole::Mono);
        rowY += 52.F;
    }

    drawText(window,
             "Enter or left click to return to doctrine select.",
             {panel.position.x + 30.F, panel.position.y + panel.size.y - 44.F},
             17,
             kTextMuted,
             FontRole::Body);
}

std::optional<NationId> Renderer::selectionFromPixel(const sf::Vector2i pixel) const {
    const auto layout = selectionLayout();
    const sf::Vector2f point(static_cast<float>(pixel.x), static_cast<float>(pixel.y));
    const auto nations = playableNations();
    for (std::size_t index = 0; index < layout.nationCards.size(); ++index) {
        if (layout.nationCards[index].contains(point)) {
            return nations[index];
        }
    }
    return std::nullopt;
}

std::optional<sim::TileCoord> Renderer::tileFromPixel(const sim::WorldState& world, const sf::Vector2i pixel) const {
    const auto layout = matchLayout(world);
    const float localX = static_cast<float>(pixel.x) - layout.mapOrigin.x;
    const float localY = static_cast<float>(pixel.y) - layout.mapOrigin.y;
    if (localX < 0.F || localY < 0.F) {
        return std::nullopt;
    }

    const int tileX = static_cast<int>(localX / layout.tileSize);
    const int tileY = static_cast<int>(localY / layout.tileSize);
    const sim::TileCoord coord{tileX, tileY};
    if (!sim::inBounds(world, coord)) {
        return std::nullopt;
    }
    return coord;
}

} // namespace game::ui
