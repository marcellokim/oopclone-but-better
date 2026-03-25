#include "game/ui/Renderer.hpp"

#include "RendererDetail.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <algorithm>
#include <cmath>

namespace game::ui {

Renderer::Renderer() { static_cast<void>(loadFonts()); }

bool Renderer::hasFont() const { return m_hasDisplayFont || m_hasBodyFont || m_hasMonoFont; }

bool Renderer::loadFonts() {
    m_hasDisplayFont = detail::loadFirstAvailable(m_displayFont, detail::displayFontCandidates());
    m_hasBodyFont = detail::loadFirstAvailable(m_bodyFont, detail::bodyFontCandidates());
    m_hasMonoFont = detail::loadFirstAvailable(m_monoFont, detail::monoFontCandidates());
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
    base.setFillColor(detail::kBackground);
    target.draw(base);

    const std::array<sf::Vector2f, 4> centers{{
        {120.F, 90.F},
        {1040.F, 120.F},
        {180.F, 730.F},
        {1130.F, 650.F},
    }};
    const std::array<sf::Color, 4> colors{{
        detail::withAlpha(detail::kAccent, 28),
        detail::withAlpha(sf::Color(81, 132, 201), 22),
        detail::withAlpha(sf::Color(125, 102, 198), 20),
        detail::withAlpha(sf::Color(86, 161, 140), 18),
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
    core.setOutlineColor(detail::withAlpha(detail::brighten(borderColor, 1.2F), 110));
    target.draw(core);

    if (accentTop) {
        sf::RectangleShape accent({core.getSize().x, 4.F});
        accent.setPosition({core.getPosition().x, core.getPosition().y});
        accent.setFillColor(detail::withAlpha(detail::kAccent, 210));
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
    chip.setOutlineColor(detail::withAlpha(detail::brighten(fill, 1.25F), 120));
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
        .heroRect = detail::makeRect(58.F, 84.F, 338.F, 590.F),
        .nationCards = {{
            detail::makeRect(438.F, 96.F, 336.F, 258.F),
            detail::makeRect(798.F, 74.F, 370.F, 248.F),
            detail::makeRect(432.F, 372.F, 360.F, 296.F),
            detail::makeRect(808.F, 320.F, 382.F, 308.F),
        }},
        .footerRect = detail::makeRect(58.F, 696.F, 1136.F, 58.F),
    };
}

Renderer::MatchLayout Renderer::matchLayout(const sim::WorldState& world) const {
    MatchLayout layout{};
    layout.topStrip = detail::makeRect(34.F, 24.F, 1212.F, 74.F);
    layout.mapShell = detail::makeRect(34.F, 118.F, 856.F, 626.F);
    layout.mapViewport = detail::makeRect(58.F, 142.F, 808.F, 548.F);
    layout.rightRail = detail::makeRect(910.F, 118.F, 336.F, 626.F);
    layout.commanderPanel = detail::makeRect(926.F, 134.F, 304.F, 106.F);
    layout.hoverPanel = detail::makeRect(926.F, 256.F, 304.F, 198.F);
    layout.frontsPanel = detail::makeRect(926.F, 470.F, 304.F, 198.F);
    layout.objectivePanel = detail::makeRect(926.F, 684.F, 304.F, 44.F);
    layout.bottomChip = detail::makeRect(90.F, 650.F, 492.F, 74.F);

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
    const auto terrain = terrainName(tile.terrain);
    if (tile.owner == NationId::Neutral) {
        if (terrain == "Sea") {
            return sf::Color(42, 74, 108);
        }
        if (terrain == "Mountain") {
            return sf::Color(88, 84, 80);
        }
        return tile.terrain == TerrainType::Road ? sf::Color(76, 86, 98) : sf::Color(56, 64, 76);
    }

    auto base = detail::mix(nationColor(tile.owner), sf::Color(18, 22, 29), 0.48F);
    if (terrain == "Sea") {
        base = detail::mix(base, sf::Color(36, 92, 138), 0.60F);
    } else if (tile.terrain == TerrainType::Road) {
        base = detail::brighten(base, 1.18F);
    } else if (tile.terrain == TerrainType::Highland) {
        base = detail::mix(base, sf::Color(38, 44, 58), 0.28F);
    } else if (terrain == "Mountain") {
        base = detail::mix(base, sf::Color(100, 92, 84), 0.45F);
    } else if (tile.terrain == TerrainType::Capital) {
        base = detail::brighten(base, 1.10F);
    }
    return base;
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
