#include "game/ui/Renderer.hpp"

#include "RendererDetail.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace game::ui {

void Renderer::drawSelectionScreen(sf::RenderWindow& window, const NationId highlightedNation) const {
    drawAmbientBackdrop(window, window.getSize());
    const auto layout = selectionLayout();
    drawSelectionHero(window, layout);
    drawSelectionNationCards(window, layout, highlightedNation);
    drawSelectionFooter(window, layout);
}

void Renderer::drawSelectionHero(sf::RenderTarget& target, const SelectionLayout& layout) const {
    drawPanel(target,
              layout.heroRect,
              detail::withAlpha(detail::kShell, 245),
              detail::withAlpha(detail::kCore, 240),
              detail::kBorder,
              true);
    drawText(target,
             "TACTICAL COMMAND DECK",
             {layout.heroRect.position.x + 28.F, layout.heroRect.position.y + 38.F},
             14,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.18F);
    drawText(target,
             "Select a war doctrine\nand take the table.",
             {layout.heroRect.position.x + 28.F, layout.heroRect.position.y + 92.F},
             44,
             detail::kTextPrimary,
             FontRole::Display,
             false,
             0.92F);
    drawText(target,
             "Each nation bends the same battlefield in a different direction: speed, force, attrition, or macro control.",
             {layout.heroRect.position.x + 30.F, layout.heroRect.position.y + 212.F},
             20,
             detail::kTextMuted,
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
        dot.setFillColor(detail::kAccent);
        target.draw(dot);
        drawText(target,
                 bullet,
                 {layout.heroRect.position.x + 54.F, bulletY},
                 17,
                 detail::kTextPrimary,
                 FontRole::Body);
        bulletY += 42.F;
    }

    drawChip(target,
             detail::makeRect(layout.heroRect.position.x + 28.F,
                              layout.heroRect.position.y + layout.heroRect.size.y - 92.F,
                              144.F,
                              34.F),
             "keys 1-4",
             detail::withAlpha(detail::kAccent, 44),
             detail::kTextPrimary,
             FontRole::Mono);
    drawChip(target,
             detail::makeRect(layout.heroRect.position.x + 184.F,
                              layout.heroRect.position.y + layout.heroRect.size.y - 92.F,
                              142.F,
                              34.F),
             "mouse ready",
             detail::withAlpha(sf::Color(90, 123, 163), 44),
             detail::kTextPrimary,
             FontRole::Mono);
}

void Renderer::drawSelectionNationCards(sf::RenderTarget& target,
                                        const SelectionLayout& layout,
                                        const NationId highlightedNation) const {
    const auto nations = playableNations();
    for (std::size_t index = 0; index < nations.size(); ++index) {
        const auto nation = nations[index];
        const auto& profile = nationDefinition(nation);
        const auto rect = layout.nationCards[index];
        const bool highlighted = highlightedNation == nation;
        const auto nationAccent = detail::brighten(nationColor(nation), highlighted ? 1.18F : 1.0F);

        drawPanel(target,
                  rect,
                  detail::withAlpha(detail::mix(detail::kShell, nationAccent, 0.12F), 246),
                  detail::withAlpha(detail::mix(detail::kCore, nationAccent, 0.06F), 244),
                  detail::withAlpha(nationAccent, highlighted ? 220 : 140),
                  highlighted);

        sf::RectangleShape accentBand({6.F, rect.size.y - 24.F});
        accentBand.setPosition({rect.position.x + 12.F, rect.position.y + 12.F});
        accentBand.setFillColor(detail::withAlpha(nationAccent, 220));
        target.draw(accentBand);

        drawText(target,
                 detail::doctrineTag(nation),
                 {rect.position.x + 34.F, rect.position.y + 34.F},
                 13,
                 detail::kTextMuted,
                 FontRole::Mono,
                 false,
                 1.16F);
        drawText(target,
                 std::string(profile.name),
                 {rect.position.x + 34.F, rect.position.y + 74.F},
                 30,
                 detail::kTextPrimary,
                 FontRole::Display,
                 false,
                 0.94F);
        drawText(target,
                 detail::doctrineLine(nation),
                 {rect.position.x + 34.F, rect.position.y + 116.F},
                 16,
                 detail::kTextMuted,
                 FontRole::Body);

        drawChip(target,
                 detail::makeRect(rect.position.x + 34.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "mob " + std::to_string(profile.mobility).substr(0, 4),
                 detail::withAlpha(nationAccent, 44),
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(rect.position.x + 116.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "atk " + std::to_string(profile.attack).substr(0, 4),
                 detail::withAlpha(nationAccent, 38),
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(rect.position.x + 198.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "def " + std::to_string(profile.defense).substr(0, 4),
                 detail::withAlpha(nationAccent, 34),
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(rect.position.x + 280.F, rect.position.y + rect.size.y - 86.F, 72.F, 30.F),
                 "reg " + std::to_string(profile.regen).substr(0, 4),
                 detail::withAlpha(nationAccent, 30),
                 detail::kTextPrimary,
                 FontRole::Mono);

        drawText(target,
                 "Press " + std::to_string(index + 1) + " or click to command this doctrine.",
                 {rect.position.x + 34.F, rect.position.y + rect.size.y - 40.F},
                 15,
                 highlighted ? detail::kTextPrimary : detail::kTextMuted,
                 FontRole::Body);
    }
}

void Renderer::drawSelectionFooter(sf::RenderTarget& target, const SelectionLayout& layout) const {
    drawPanel(target,
              layout.footerRect,
              detail::withAlpha(detail::kShell, 220),
              detail::withAlpha(detail::kCore, 220),
              detail::withAlpha(detail::kBorder, 110));
    drawText(target,
             "Preserved controls: left click select / command · right click or Esc to clear · capitals regenerate with territory held.",
             {layout.footerRect.position.x + 28.F, layout.footerRect.position.y + 20.F},
             15,
             detail::kTextMuted,
             FontRole::Body);
}

} // namespace game::ui
