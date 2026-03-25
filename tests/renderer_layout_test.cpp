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

std::optional<game::TerrainType> terrainNamed(const std::string_view target) {
    for (int rawValue = 0; rawValue < 8; ++rawValue) {
        const auto terrain = static_cast<game::TerrainType>(rawValue);
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
    const std::array<std::string_view, 4> expectedNames{{
        "서강대학교",
        "성균관대학교",
        "한양대학교",
        "중앙대학교",
    }};

    for (const auto expectedName : expectedNames) {
        const bool found = std::any_of(nations.begin(), nations.end(), [expectedName](const game::NationId nation) {
            return game::nationName(nation) == expectedName;
        });
        test::require(found, "playable faction roster should include each university name exactly once player-facing");
    }

    for (const auto nation : nations) {
        test::require(!game::nationDoctrineLine(nation).empty(),
                      "each university faction should expose a non-empty doctrine line");
        test::require(!game::nationDoctrineTag(nation).empty(),
                      "each university faction should expose a non-empty doctrine tag");
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
    test::require(hasLineContaining(mountainLines, "Owner: 서강대학교"),
                  "hover intel should show the renamed university owner label");
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
