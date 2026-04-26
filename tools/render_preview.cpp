#include "game/GameConfig.hpp"
#include "game/sim/AbilitySystem.hpp"
#include "game/sim/WorldState.hpp"
#include "game/ui/InputController.hpp"
#include "game/ui/Renderer.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <filesystem>
#include <iostream>

namespace {

bool saveTexture(sf::RenderTexture& texture, const std::filesystem::path& path) {
    texture.display();
    const auto image = texture.getTexture().copyToImage();
    return image.saveToFile(path.string());
}

} // namespace

int main() {
    namespace fs = std::filesystem;

    const fs::path outDir = fs::path(".omx") / "screens";
    fs::create_directories(outDir);

    game::ui::Renderer renderer;
    game::ui::InputController input;
    input.setSendRatio(game::sim::SendRatio::Full);

    sf::RenderTexture renderTexture({1280u, 800u});

    renderTexture.clear();
    renderer.drawSelectionScreen(renderTexture, game::NationId::IronLegion);
    if (!saveTexture(renderTexture, outDir / "preview-nation-select.png")) {
        std::cerr << "failed to save nation-select preview\n";
        return 1;
    }

    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    world.selectedTile = game::sim::TileCoord{1, 1};
    world.hoveredTile = game::sim::TileCoord{9, 6};
    world.nationStates.at(game::sim::nationIndex(game::NationId::SwiftLeague)).commandPower =
        game::sim::AbilitySystem::abilityCost(game::NationId::SwiftLeague);
    static_cast<void>(game::sim::AbilitySystem::activate(world, game::NationId::SwiftLeague));

    game::sim::ArmyTransit transit{};
    transit.owner = game::NationId::IronLegion;
    transit.origin = {14, 1};
    transit.destination = {11, 3};
    transit.path = {{14, 1}, {13, 1}, {12, 1}, {11, 1}, {11, 2}, {11, 3}};
    transit.nextWaypoint = 2;
    transit.progressToNext = 0.55F;
    transit.troops = 38;
    world.activeTransits.push_back(transit);

    renderTexture.clear();
    renderer.drawMatch(renderTexture, world, input);
    if (!saveTexture(renderTexture, outDir / "preview-match.png")) {
        std::cerr << "failed to save match preview\n";
        return 1;
    }

    world.matchEnded = true;
    world.winner = game::NationId::SwiftLeague;
    auto& playerStats = world.nationStates.at(game::sim::nationIndex(game::NationId::SwiftLeague)).stats;
    playerStats.tilesCaptured = 4;
    playerStats.capitalsCaptured = 1;
    playerStats.troopsDefeated = 62;

    renderTexture.clear();
    renderer.drawGameOver(renderTexture, world);
    if (!saveTexture(renderTexture, outDir / "preview-game-over.png")) {
        std::cerr << "failed to save game-over preview\n";
        return 1;
    }

    std::cout << (outDir / "preview-nation-select.png") << '\n';
    std::cout << (outDir / "preview-match.png") << '\n';
    std::cout << (outDir / "preview-game-over.png") << '\n';
    return 0;
}
