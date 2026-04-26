#include "game/ui/HoverPanel.hpp"
#include "game/ui/Renderer.hpp"
#include "test_harness.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <string>
#include <string_view>

namespace {
constexpr int kMapOriginX = 75;
constexpr int kMapOriginY = 158;
constexpr int kTileSize = 43;
constexpr std::array<game::TerrainType, 6> kKnownTerrainTypes{game::TerrainType::Plains,
                                                               game::TerrainType::Road,
                                                               game::TerrainType::Highland,
                                                               game::TerrainType::Mountain,
                                                               game::TerrainType::Sea,
                                                               game::TerrainType::Capital};

std::optional<game::TerrainType> terrainNamed(const std::string_view target) {
    for (const auto terrain : kKnownTerrainTypes) {
        if (game::terrainName(terrain) == target) {
            return terrain;
        }
    }
    return std::nullopt;
}

bool hasLineContaining(const std::vector<std::string>& lines, const std::string_view needle) {
    return std::any_of(lines.begin(), lines.end(), [needle](const std::string& line) {
        return line.find(needle) != std::string::npos;
    });
}
}

TEST_CASE(renderer_loads_bundled_redistributable_fonts) {
    game::ui::Renderer renderer;
    test::require(renderer.hasFont(), "renderer should load bundled redistributable fonts");
}

TEST_CASE(renderer_selection_hit_testing_matches_doctrine_cards) {
    game::ui::Renderer renderer;
    const auto nations = game::playableNations();

    test::require(renderer.selectionFromPixel({520, 180}) == nations[0],
                  "first doctrine card should map to the first playable faction");
    test::require(renderer.selectionFromPixel({980, 180}) == nations[1],
                  "second doctrine card should map to the second playable faction");
    test::require(renderer.selectionFromPixel({520, 470}) == nations[2],
                  "third doctrine card should map to the third playable faction");
    test::require(renderer.selectionFromPixel({980, 470}) == nations[3],
                  "fourth doctrine card should map to the fourth playable faction");
    test::require(!renderer.selectionFromPixel({32, 32}).has_value(), "pixels outside cards should not select a nation");
}

TEST_CASE(renderer_player_facing_names_follow_university_retheme) {
    const auto nations = game::playableNations();
    const std::array<std::string_view, 4> expectedNames{{"Sogang Univ.", "Hanyang Univ.", "Sungkyunkwan Univ.", "Chung-Ang Univ."}};
    const std::array<std::string_view, 4> expectedCompact{{"Sogang", "Hanyang", "SKKU", "Chung-Ang"}};
    const std::array<std::string_view, 4> expectedTags{{"tempo doctrine", "infrastructure doctrine", "stability doctrine", "adaptive doctrine"}};

    for (std::size_t index = 0; index < nations.size(); ++index) {
        const auto nation = nations[index];
        test::require(game::nationName(nation) == expectedNames[index], "university full-name mapping should match the canonical faction order");
        test::require(game::nationCompactName(nation) == expectedCompact[index], "compact university-name mapping should match canonical config");
        test::require(game::nationDoctrineTag(nation) == expectedTags[index], "doctrine tag mapping should match canonical config");
        test::require(!game::nationDoctrineLine(nation).empty(), "each university faction should expose a non-empty doctrine line");
    }
}

TEST_CASE(renderer_hover_lines_surface_university_names_and_new_terrain_labels) {
    const auto mountain = terrainNamed("Mountain");
    const auto sea = terrainNamed("Sea");
    test::require(mountain.has_value(), "terrain update should expose a Mountain terrain label");
    test::require(sea.has_value(), "terrain update should expose a Sea terrain label");

    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    auto& probeTile = game::sim::tileAt(world, {5, 5});
    probeTile.owner = game::playableNations()[0];
    probeTile.troops = 37;
    probeTile.hasCapital = false;
    probeTile.terrain = *mountain;

    const auto mountainLines = game::ui::buildHoverLines(world, {5, 5});
    test::require(hasLineContaining(mountainLines, "Owner: Sogang"),
                  "hover intel should show the compact English owner label");
    test::require(hasLineContaining(mountainLines, "Mountain"),
                  "hover intel should surface Mountain in the tile summary");
    test::require(hasLineContaining(mountainLines, "Move:"),
                  "hover intel should include movement guidance for new terrain");
    test::require(hasLineContaining(mountainLines, "Launch cap:"),
                  "hover intel should include throughput guidance for new terrain");

    probeTile.terrain = *sea;
    const auto seaLines = game::ui::buildHoverLines(world, {5, 5});
    test::require(hasLineContaining(seaLines, "Sea"),
                  "hover intel should surface Sea in the tile summary");
    test::require(hasLineContaining(seaLines, "Move: blocked for land orders"),
                  "sea hover intel should explain that land movement is blocked");
    test::require(hasLineContaining(seaLines, "Launch cap: blocked"),
                  "sea hover intel should show that throughput is blocked");
}

TEST_CASE(renderer_match_hit_testing_maps_pixels_to_tiles) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ui::Renderer renderer;

    const auto firstTile = renderer.tileFromPixel(world, {kMapOriginX + 10, kMapOriginY + 10});
    test::require(firstTile.has_value() && firstTile->x == 0 && firstTile->y == 0,
                  "top-left map tile should be hittable inside its rendered bounds");

    const auto bottomRightTile = renderer.tileFromPixel(
        world,
        {kMapOriginX + (world.width - 1) * kTileSize + 20, kMapOriginY + (world.height - 1) * kTileSize + 20});
    test::require(bottomRightTile.has_value() && bottomRightTile->x == world.width - 1 &&
                      bottomRightTile->y == world.height - 1,
                  "bottom-right rendered tile should map back to the last world tile");

    test::require(!renderer.tileFromPixel(world, {kMapOriginX - 1, kMapOriginY + 20}).has_value(),
                  "pixels left of the rendered map should not resolve to a tile");
    test::require(!renderer.tileFromPixel(world,
                                          {kMapOriginX + world.width * kTileSize + 1, kMapOriginY + 20})
                       .has_value(),
                  "pixels right of the rendered map should not resolve to a tile");
}

TEST_CASE(renderer_ability_panel_hit_testing_stays_outside_map) {
    auto world = game::sim::createInitialWorld(game::NationId::SwiftLeague);
    game::ui::Renderer renderer;

    test::require(renderer.abilityPanelFromPixel({1060, 282}),
                  "ability panel center should be clickable for active ability activation");
    test::require(!renderer.abilityPanelFromPixel({420, 360}),
                  "map pixels should not be treated as ability-panel clicks");
    test::require(renderer.tileFromPixel(world, {420, 360}).has_value(),
                  "map hit-testing should remain active after adding the ability panel");
}
