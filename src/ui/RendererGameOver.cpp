#include "game/ui/Renderer.hpp"

#include "RendererDetail.hpp"
#include "game/sim/ScoreSystem.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace game::ui {

void Renderer::drawGameOver(sf::RenderTarget& target, const sim::WorldState& world) const {
    drawMatch(target, world, InputController{});
    drawGameOverPanel(target, world);
}

void Renderer::drawGameOverPanel(sf::RenderTarget& target, const sim::WorldState& world) const {
    sf::RectangleShape overlay({1280.F, 800.F});
    overlay.setFillColor(sf::Color(4, 6, 10, 205));
    target.draw(overlay);

    const auto panel = detail::makeRect(260.F, 144.F, 760.F, 470.F);
    drawPanel(target,
              panel,
              detail::withAlpha(detail::kShell, 248),
              detail::withAlpha(detail::kCore, 246),
              detail::withAlpha(detail::kBorder, 160),
              true);

    drawText(target,
             "POST-BATTLE DEBRIEF",
             {panel.position.x + 28.F, panel.position.y + 30.F},
             14,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.16F);
    drawText(target,
             "Battle concluded",
             {panel.position.x + 28.F, panel.position.y + 82.F},
             46,
             detail::kTextPrimary,
             FontRole::Display,
             false,
             0.93F);

    if (world.winner.has_value()) {
        const auto winner = *world.winner;
        drawText(target,
                 std::string(nationName(winner)) +
                     (winner == world.playerNation ? " secured the field." : " outlasted every rival."),
                 {panel.position.x + 30.F, panel.position.y + 144.F},
                 24,
                 nationColor(winner),
                 FontRole::Display,
                 false,
                 0.96F);
    } else {
        drawText(target,
                 "Mutual destruction. No nation survived the final tick.",
                 {panel.position.x + 30.F, panel.position.y + 144.F},
                 24,
                 detail::kAccent,
                 FontRole::Display,
                 false,
                 0.96F);
    }

    const auto standings = sim::ScoreSystem::standings(world);
    drawText(target,
             "Final standings by score",
             {panel.position.x + 30.F, panel.position.y + 204.F},
             20,
             detail::kTextPrimary,
             FontRole::Body);

    float rowY = panel.position.y + 232.F;
    for (const auto& rowData : standings) {
        const auto nation = rowData.nation;
        const auto rowRect = detail::makeRect(panel.position.x + 28.F, rowY, panel.size.x - 56.F, 38.F);
        sf::RectangleShape row({rowRect.size.x, rowRect.size.y});
        row.setPosition(rowRect.position);
        row.setFillColor(detail::withAlpha(detail::mix(detail::kCore, nationColor(nation), 0.12F), 220));
        row.setOutlineThickness(1.F);
        row.setOutlineColor(detail::withAlpha(nationColor(nation), 120));
        target.draw(row);

        const bool alive = !world.nationStates.at(sim::nationIndex(nation)).eliminated;
        drawText(target,
                 std::string(nationName(nation)),
                 {rowRect.position.x + 14.F, rowRect.position.y + 9.F},
                 17,
                 alive ? nationColor(nation) : detail::kTextMuted,
                 FontRole::Body);
        drawText(target,
                 std::to_string(rowData.score) + " pts   grade " + rowData.grade,
                 {rowRect.position.x + 218.F, rowRect.position.y + 8.F},
                 14,
                 detail::kTextPrimary,
                 FontRole::Mono);
        drawText(target,
                 std::to_string(rowData.ownedTiles) + "t  " + std::to_string(rowData.totalTroops) + "tr  " +
                     std::to_string(rowData.stats.abilitiesUsed) + "ab",
                 {rowRect.position.x + 430.F, rowRect.position.y + 10.F},
                 11,
                 detail::kTextMuted,
                 FontRole::Mono);
        drawText(target,
                 rowData.winner ? "winner" : detail::standingStatus(alive, nation == world.playerNation),
                 {rowRect.position.x + rowRect.size.x - 128.F, rowRect.position.y + 9.F},
                 15,
                 alive ? (nation == world.playerNation ? detail::kAccent : detail::kPositive) : detail::kNegative,
                 FontRole::Mono);
        rowY += 46.F;
    }

    drawText(target,
             "Enter or left click to return to doctrine select.",
             {panel.position.x + 30.F, panel.position.y + panel.size.y - 26.F},
             17,
             detail::kTextMuted,
             FontRole::Body);
}

} // namespace game::ui
