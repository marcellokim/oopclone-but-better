#include "game/ui/Renderer.hpp"
#include "test_harness.hpp"

namespace {
constexpr int kMapOriginX = 75;
constexpr int kMapOriginY = 158;
constexpr int kTileSize = 43;
}

TEST_CASE(renderer_loads_bundled_redistributable_fonts) {
    game::ui::Renderer renderer;
    test::require(renderer.hasFont(), "renderer should load bundled redistributable fonts");
}

TEST_CASE(renderer_selection_hit_testing_matches_doctrine_cards) {
    game::ui::Renderer renderer;
    const auto nations = game::playableNations();

    test::require(renderer.selectionFromPixel({520, 180}) == nations[0], "first doctrine card should map to Swift League");
    test::require(renderer.selectionFromPixel({980, 180}) == nations[1], "second doctrine card should map to Iron Legion");
    test::require(renderer.selectionFromPixel({520, 470}) == nations[2], "third doctrine card should map to Bastion Directorate");
    test::require(renderer.selectionFromPixel({980, 470}) == nations[3], "fourth doctrine card should map to Crown Consortium");
    test::require(!renderer.selectionFromPixel({32, 32}).has_value(), "pixels outside cards should not select a nation");
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
