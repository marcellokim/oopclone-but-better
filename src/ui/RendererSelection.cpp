#include "game/ui/Renderer.hpp"

#include "RendererDetail.hpp"
#include "game/ui/VisualTuning.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>

namespace game::ui {

void Renderer::drawSelectionScreen(sf::RenderTarget& target, const NationId highlightedNation) const {
    drawAmbientBackdrop(target, target.getSize());
    const auto layout = selectionLayout();
    drawSelectionHero(target, layout);
    drawSelectionNationCards(target, layout, highlightedNation);
    drawSelectionFooter(target, layout);
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
             "Choose\nyour doctrine\nand seize the table.",
             {layout.heroRect.position.x + 28.F, layout.heroRect.position.y + 92.F},
             34,
             detail::kTextPrimary,
             FontRole::Display,
             false,
             0.92F);
    drawText(target,
             "Each nation bends the same battlefield toward\nspeed, force, attrition, or macro control.",
             {layout.heroRect.position.x + 30.F, layout.heroRect.position.y + 212.F},
             16,
             detail::kTextMuted,
             FontRole::Body,
             false,
             0.98F);

    const std::array<std::string, 3> bullets{{
        "Mouse-first commands. Ratios on 1 / 2 / 3.",
        "Capitals regenerate faster as territory grows.",
        "Lose your capital or your entire force and the table is gone.",
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
    static sf::Clock animationClock;
    const float timeSeconds = animationClock.getElapsedTime().asSeconds();
    const auto nations = playableNations();
    for (std::size_t index = 0; index < nations.size(); ++index) {
        const auto nation = nations[index];
        const auto& profile = nationDefinition(nation);
        const auto rect = layout.nationCards[index];
        const bool highlighted = highlightedNation == nation;
        const float pulse = nationCardAccentPulse(highlighted, timeSeconds);
        const auto nationAccent = detail::brighten(nationColor(nation), highlighted ? (1.12F + pulse * 0.12F) : 1.0F);

        drawPanel(target,
                  rect,
                  detail::withAlpha(detail::mix(detail::kShell, nationAccent, 0.12F), 246),
                  detail::withAlpha(detail::mix(detail::kCore, nationAccent, 0.06F), 244),
                  detail::withAlpha(nationAccent, highlighted ? static_cast<std::uint8_t>(208 + pulse * 34.F) : 140),
                  highlighted);

        if (highlighted) {
            sf::RectangleShape glow({rect.size.x + 12.F, rect.size.y + 12.F});
            glow.setPosition({rect.position.x - 6.F, rect.position.y - 6.F});
            glow.setFillColor(detail::withAlpha(nationAccent, static_cast<std::uint8_t>(20 + pulse * 28.F)));
            target.draw(glow);
        }

        sf::RectangleShape accentBand({6.F, rect.size.y - 24.F});
        accentBand.setPosition({rect.position.x + 12.F, rect.position.y + 12.F});
        accentBand.setFillColor(detail::withAlpha(nationAccent, highlighted ? static_cast<std::uint8_t>(196 + pulse * 48.F) : 220));
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
                 highlighted ? detail::brighten(detail::kTextPrimary, 1.03F + pulse * 0.02F) : detail::kTextPrimary,
                 FontRole::Display,
                 false,
                 0.94F);
        drawText(target,
                 detail::doctrineLine(nation),
                 {rect.position.x + 34.F, rect.position.y + 116.F},
                 15,
                 detail::kTextMuted,
                 FontRole::Body);

        drawChip(target,
                 detail::makeRect(rect.position.x + 34.F, rect.position.y + rect.size.y - 98.F, 94.F, 24.F),
                 "mob " + std::to_string(profile.mobility).substr(0, 4),
                 detail::withAlpha(nationAccent, 44),
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(rect.position.x + 142.F, rect.position.y + rect.size.y - 98.F, 94.F, 24.F),
                 "atk " + std::to_string(profile.attack).substr(0, 4),
                 detail::withAlpha(nationAccent, 38),
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(rect.position.x + 34.F, rect.position.y + rect.size.y - 66.F, 94.F, 24.F),
                 "def " + std::to_string(profile.defense).substr(0, 4),
                 detail::withAlpha(nationAccent, 34),
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(rect.position.x + 142.F, rect.position.y + rect.size.y - 66.F, 94.F, 24.F),
                 "reg " + std::to_string(profile.regen).substr(0, 4),
                 detail::withAlpha(nationAccent, 30),
                 detail::kTextPrimary,
                 FontRole::Mono);

        drawText(target,
                 "Press " + std::to_string(index + 1) + " or click to deploy this doctrine.",
                 {rect.position.x + 34.F, rect.position.y + rect.size.y - 34.F},
                 13,
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
             "Controls: left click select/command | right click or Esc to clear | capitals regenerate with territory held.",
             {layout.footerRect.position.x + 28.F, layout.footerRect.position.y + 20.F},
             14,
             detail::kTextMuted,
             FontRole::Body);
}

} // namespace game::ui
