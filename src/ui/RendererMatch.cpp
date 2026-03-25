#include "game/ui/Renderer.hpp"

#include "RendererDetail.hpp"
#include "game/ui/VisualTuning.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>

namespace game::ui {

void Renderer::drawMatch(sf::RenderTarget& target,
                         const sim::WorldState& world,
                         const InputController& inputController) const {
    drawAmbientBackdrop(target, target.getSize());
    const auto layout = matchLayout(world);
    drawMatchTopStrip(target, layout, world, inputController);
    drawMatchMap(target, layout, world);
    drawActiveTransits(target, layout, world);
    drawCommanderPanel(target, layout, world, inputController);
    drawHoverPanel(target, layout, world);
    drawFrontsPanel(target, layout, world);
    drawObjectivePanel(target, layout);
    drawCommandPreview(target, layout, world);
}

void Renderer::drawMatchTopStrip(sf::RenderTarget& target,
                                 const MatchLayout& layout,
                                 const sim::WorldState& world,
                                 const InputController& inputController) const {
    drawPanel(target,
              layout.topStrip,
              detail::withAlpha(detail::kShell, 236),
              detail::withAlpha(detail::kCore, 234),
              detail::kBorder,
              true);
    drawText(target,
             "LIVE THEATER",
             {layout.topStrip.position.x + 26.F, layout.topStrip.position.y + 20.F},
             13,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.18F);
    drawText(target,
             "Realtime 4-Nation Territory War",
             {layout.topStrip.position.x + 24.F, layout.topStrip.position.y + 44.F},
             24,
             detail::kTextPrimary,
             FontRole::Display,
             false,
             0.95F);
    drawChip(target,
             detail::makeRect(layout.topStrip.position.x + 548.F, layout.topStrip.position.y + 20.F, 152.F, 34.F),
             std::string(nationName(world.playerNation)),
             detail::withAlpha(nationColor(world.playerNation), 54),
             detail::kTextPrimary,
             FontRole::Body);
    drawChip(target,
             detail::makeRect(layout.topStrip.position.x + 714.F, layout.topStrip.position.y + 20.F, 150.F, 34.F),
             "ratio " + inputController.sendRatioLabel(),
             detail::withAlpha(detail::kAccent, 44),
             detail::kTextPrimary,
             FontRole::Mono);
    drawChip(target,
             detail::makeRect(layout.topStrip.position.x + 878.F, layout.topStrip.position.y + 20.F, 338.F, 34.F),
             "objective: defend your capital / erase every rival",
             detail::withAlpha(sf::Color(79, 100, 128), 42),
             detail::kTextPrimary,
             FontRole::Body);
}

void Renderer::drawMatchMap(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const {
    static sf::Clock animationClock;
    const float timeSeconds = animationClock.getElapsedTime().asSeconds();
    drawPanel(target,
              layout.mapShell,
              detail::withAlpha(detail::kShell, 248),
              detail::withAlpha(detail::kCore, 246),
              detail::withAlpha(detail::kBorder, 150));

    sf::RectangleShape mapSurface({layout.mapViewport.size.x, layout.mapViewport.size.y});
    mapSurface.setPosition(layout.mapViewport.position);
    mapSurface.setFillColor(sf::Color(17, 22, 30));
    mapSurface.setOutlineThickness(1.F);
    mapSurface.setOutlineColor(detail::withAlpha(detail::kBorder, 90));
    target.draw(mapSurface);

    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            const sim::TileCoord coord{x, y};
            const auto& tile = sim::tileAt(world, coord);
            const sf::Vector2f tilePos{layout.mapOrigin.x + x * layout.tileSize, layout.mapOrigin.y + y * layout.tileSize};
            const sf::Vector2f tileSize{layout.tileSize - 3.F, layout.tileSize - 3.F};

            sf::RectangleShape tileShadow(tileSize);
            tileShadow.setPosition({tilePos.x, tilePos.y + 4.F});
            tileShadow.setFillColor(sf::Color(0, 0, 0, 50));
            target.draw(tileShadow);

            const bool selected = world.selectedTile && *world.selectedTile == coord;
            const bool hovered = world.hoveredTile && *world.hoveredTile == coord;
            const auto tuning = tileVisualTuning(selected, hovered, tile.hasCapital, timeSeconds);

            if (tuning.glowAlpha > 0) {
                sf::RectangleShape glow({tileSize.x + tuning.glowPadding * 2.F, tileSize.y + tuning.glowPadding * 2.F});
                glow.setPosition({tilePos.x - tuning.glowPadding, tilePos.y - tuning.glowPadding});
                glow.setFillColor(detail::withAlpha(nationColor(tile.owner), tuning.glowAlpha));
                target.draw(glow);
            }

            sf::RectangleShape outer(tileSize);
            outer.setPosition(tilePos);
            outer.setFillColor(detail::brighten(tileColor(tile), tuning.fillBoost));
            outer.setOutlineThickness(tuning.outlineThickness);
            outer.setOutlineColor(selected ? detail::withAlpha(detail::kAccent, tuning.outlineAlpha)
                                          : (hovered ? detail::withAlpha(sf::Color::White, tuning.outlineAlpha)
                                                     : detail::withAlpha(detail::kBorder, tuning.outlineAlpha)));
            target.draw(outer);

            sf::RectangleShape inner({tileSize.x - 10.F, tileSize.y - 10.F});
            inner.setPosition({tilePos.x + 5.F, tilePos.y + 5.F});
            inner.setFillColor(detail::withAlpha(detail::brighten(tileColor(tile), tuning.innerBoost), tuning.innerAlpha));
            target.draw(inner);

            if (tile.terrain == TerrainType::Road) {
                sf::RectangleShape road({tileSize.x - 12.F, 4.F});
                road.setPosition({tilePos.x + 6.F, tilePos.y + tileSize.y / 2.F - 2.F});
                road.setFillColor(detail::withAlpha(sf::Color(204, 210, 222), 90));
                target.draw(road);
            }
            if (tile.terrain == TerrainType::Highland) {
                sf::RectangleShape ridge({tileSize.x - 14.F, 3.F});
                ridge.setPosition({tilePos.x + 7.F, tilePos.y + 10.F});
                ridge.setFillColor(detail::withAlpha(sf::Color(236, 211, 150), 110));
                target.draw(ridge);
            }
            if (tile.hasCapital) {
                sf::CircleShape halo(layout.tileSize * tuning.capitalHaloScale);
                halo.setOrigin({layout.tileSize * tuning.capitalHaloScale, layout.tileSize * tuning.capitalHaloScale});
                halo.setPosition({tilePos.x + tileSize.x / 2.F, tilePos.y + tileSize.y / 2.F});
                halo.setFillColor(detail::withAlpha(nationColor(tile.owner), tuning.capitalHaloAlpha));
                target.draw(halo);
                sf::RectangleShape beacon({4.F, tuning.capitalBeaconHeight});
                beacon.setPosition({tilePos.x + tileSize.x / 2.F - 2.F, tilePos.y - tuning.capitalBeaconHeight + 4.F});
                beacon.setFillColor(detail::withAlpha(detail::kAccent, detail::withAlpha(detail::kAccent, tuning.capitalHaloAlpha).a));
                target.draw(beacon);
                const float crestSize = 14.F * tuning.capitalCrestScale;
                sf::RectangleShape crest({crestSize, crestSize});
                crest.setPosition({tilePos.x + tileSize.x - crestSize - 6.F, tilePos.y + 6.F});
                crest.setFillColor(detail::withAlpha(detail::kAccent, 200));
                crest.setOutlineThickness(1.F);
                crest.setOutlineColor(detail::withAlpha(sf::Color::White, 120));
                target.draw(crest);
            }

            const auto troopChipRect = detail::makeRect(tilePos.x + 6.F, tilePos.y + tileSize.y - 24.F, tileSize.x - 12.F, 18.F);
            sf::RectangleShape troopChip({troopChipRect.size.x, troopChipRect.size.y});
            troopChip.setPosition(troopChipRect.position);
            troopChip.setFillColor(detail::withAlpha(sf::Color(8, 11, 16), tuning.troopChipAlpha));
            troopChip.setOutlineThickness(1.F);
            troopChip.setOutlineColor(detail::withAlpha(sf::Color::White, tuning.troopChipOutlineAlpha));
            target.draw(troopChip);
            drawText(target,
                     std::to_string(tile.troops),
                     {troopChipRect.position.x + troopChipRect.size.x / 2.F,
                      troopChipRect.position.y + troopChipRect.size.y / 2.F - 1.F},
                     13,
                     detail::kTextPrimary,
                     FontRole::Mono,
                     true,
                     0.96F);
        }
    }
}

void Renderer::drawActiveTransits(sf::RenderTarget& target,
                                  const MatchLayout& layout,
                                  const sim::WorldState& world) const {
    static sf::Clock animationClock;
    const auto transitTuning = transitVisualTuning(animationClock.getElapsedTime().asSeconds());
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

        detail::drawBeam(target, start, current, transitTuning.haloThickness, detail::withAlpha(nationColor(transit.owner), transitTuning.haloAlpha));
        detail::drawBeam(target, start, current, transitTuning.beamThickness, detail::withAlpha(nationColor(transit.owner), transitTuning.beamAlpha));

        sf::CircleShape token(transitTuning.tokenRadius);
        token.setOrigin({transitTuning.tokenRadius, transitTuning.tokenRadius});
        token.setPosition(current);
        token.setFillColor(detail::brighten(nationColor(transit.owner), 1.08F));
        token.setOutlineThickness(2.F);
        token.setOutlineColor(detail::withAlpha(sf::Color::Black, 170));
        target.draw(token);
        sf::RectangleShape labelChip({36.F, 18.F});
        labelChip.setPosition({current.x - 18.F, current.y - 34.F});
        labelChip.setFillColor(detail::withAlpha(sf::Color(8, 11, 16), transitTuning.labelChipAlpha));
        labelChip.setOutlineThickness(1.F);
        labelChip.setOutlineColor(detail::withAlpha(sf::Color::White, 44));
        target.draw(labelChip);
        drawText(target,
                 std::to_string(transit.troops),
                 {current.x, current.y - 20.F},
                 13,
                 detail::kTextPrimary,
                 FontRole::Mono,
                 true,
                 0.96F);
    }
}

void Renderer::drawCommanderPanel(sf::RenderTarget& target,
                                  const MatchLayout& layout,
                                  const sim::WorldState& world,
                                  const InputController& inputController) const {
    drawPanel(target,
              layout.commanderPanel,
              detail::withAlpha(detail::kShell, 244),
              detail::withAlpha(detail::kCore, 242),
              detail::withAlpha(detail::kBorder, 140),
              true);
    drawText(target,
             "COMMAND",
             {layout.commanderPanel.position.x + 18.F, layout.commanderPanel.position.y + 18.F},
             13,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.16F);
    drawText(target,
             std::string(nationName(world.playerNation)),
             {layout.commanderPanel.position.x + 18.F, layout.commanderPanel.position.y + 46.F},
             22,
             nationColor(world.playerNation),
             FontRole::Display,
             false,
             0.95F);
    drawText(target,
             "Send ratio  " + inputController.sendRatioLabel() + "   |   right click to clear",
             {layout.commanderPanel.position.x + 18.F, layout.commanderPanel.position.y + 78.F},
             15,
             detail::kTextMuted,
             FontRole::Body);
}

void Renderer::drawHoverPanel(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const {
    drawPanel(target,
              layout.hoverPanel,
              detail::withAlpha(detail::kShell, 244),
              detail::withAlpha(detail::kCore, 242),
              detail::withAlpha(detail::kBorder, 140));
    drawText(target,
             "HOVER INTEL",
             {layout.hoverPanel.position.x + 18.F, layout.hoverPanel.position.y + 18.F},
             13,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.16F);

    const auto hoverLines = world.hoveredTile
                                ? buildHoverLines(world, *world.hoveredTile)
                                : std::vector<std::string>{"Hover a tile to inspect owner, troops, terrain, and capital regen."};
    float hoverY = layout.hoverPanel.position.y + 54.F;
    for (const auto& line : hoverLines) {
        drawText(target,
                 line,
                 {layout.hoverPanel.position.x + 18.F, hoverY},
                 15,
                 detail::kTextPrimary,
                 FontRole::Body);
        hoverY += 28.F;
    }
}

void Renderer::drawFrontsPanel(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const {
    drawPanel(target,
              layout.frontsPanel,
              detail::withAlpha(detail::kShell, 244),
              detail::withAlpha(detail::kCore, 242),
              detail::withAlpha(detail::kBorder, 140));
    drawText(target,
             "LIVE FRONTS",
             {layout.frontsPanel.position.x + 18.F, layout.frontsPanel.position.y + 18.F},
             13,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.16F);

    float frontY = layout.frontsPanel.position.y + 54.F;
    for (const auto nation : playableNations()) {
        const bool alive = !world.nationStates.at(sim::nationIndex(nation)).eliminated;
        const bool player = nation == world.playerNation;
        const auto panelRect = detail::makeRect(layout.frontsPanel.position.x + 14.F,
                                                frontY,
                                                layout.frontsPanel.size.x - 28.F,
                                                32.F);
        sf::RectangleShape strip({panelRect.size.x, panelRect.size.y});
        strip.setPosition(panelRect.position);
        strip.setFillColor(detail::withAlpha(detail::mix(detail::kCore, nationColor(nation), alive ? 0.18F : 0.05F), 210));
        strip.setOutlineThickness(1.F);
        strip.setOutlineColor(detail::withAlpha(nationColor(nation), alive ? 120 : 50));
        target.draw(strip);
        drawText(target,
                 detail::compactNationName(nation),
                 {panelRect.position.x + 12.F, panelRect.position.y + 5.F},
                 13,
                 alive ? detail::kTextPrimary : detail::kTextMuted,
                 FontRole::Body);
        drawText(target,
                 "tiles " + std::to_string(sim::ownedTileCount(world, nation)) + " | troops " +
                     std::to_string(sim::totalTroops(world, nation)),
                 {panelRect.position.x + 12.F, panelRect.position.y + 17.F},
                 10,
                 detail::kTextMuted,
                 FontRole::Mono);
        drawChip(target,
                 detail::makeRect(panelRect.position.x + panelRect.size.x - 66.F, panelRect.position.y + 5.F, 56.F, 15.F),
                 detail::compactStandingStatus(alive, player),
                 detail::withAlpha(alive ? (player ? detail::kAccent : detail::kPositive) : detail::kNegative, 42),
                 alive ? (player ? detail::kAccent : detail::kPositive) : detail::kNegative,
                 FontRole::Mono);
        frontY += 36.F;
    }
}

void Renderer::drawObjectivePanel(sf::RenderTarget& target, const MatchLayout& layout) const {
    drawPanel(target,
              layout.objectivePanel,
              detail::withAlpha(detail::kShell, 244),
              detail::withAlpha(detail::kCore, 242),
              detail::withAlpha(detail::kBorder, 140));
    drawText(target,
             "OBJECTIVE",
             {layout.objectivePanel.position.x + 18.F, layout.objectivePanel.position.y + 14.F},
             12,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.16F);
    drawText(target,
             "Hold capital. Take capitals. Erase force.",
             {layout.objectivePanel.position.x + 18.F, layout.objectivePanel.position.y + 26.F},
             10,
             detail::kTextMuted,
             FontRole::Body);
}

void Renderer::drawCommandPreview(sf::RenderTarget& target,
                                  const MatchLayout& layout,
                                  const sim::WorldState& world) const {
    drawPanel(target,
              layout.bottomChip,
              detail::withAlpha(detail::kShell, 240),
              detail::withAlpha(detail::kCore, 238),
              detail::withAlpha(detail::kBorder, 135));
    drawText(target,
             "COMMAND PREVIEW",
             {layout.bottomChip.position.x + 18.F, layout.bottomChip.position.y + 18.F},
             13,
             detail::kAccent,
             FontRole::Mono,
             false,
             1.16F);
    drawText(target,
             detail::selectedTileSummary(world),
             {layout.bottomChip.position.x + 18.F, layout.bottomChip.position.y + 42.F},
             15,
             detail::kTextPrimary,
             FontRole::Body);
}

} // namespace game::ui
