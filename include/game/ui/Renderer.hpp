#pragma once

#include "game/GameConfig.hpp"
#include "game/ui/HoverPanel.hpp"
#include "game/ui/InputController.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <string>

namespace game::ui {

class Renderer {
  public:
    Renderer();
    [[nodiscard]] bool hasFont() const;

    void drawSelectionScreen(sf::RenderWindow& window, NationId highlightedNation) const;
    void drawMatch(sf::RenderWindow& window,
                   const sim::WorldState& world,
                   const InputController& inputController) const;
    void drawGameOver(sf::RenderWindow& window, const sim::WorldState& world) const;

    [[nodiscard]] std::optional<NationId> selectionFromPixel(sf::Vector2i pixel) const;
    [[nodiscard]] std::optional<sim::TileCoord> tileFromPixel(const sim::WorldState& world, sf::Vector2i pixel) const;

  private:
    enum class FontRole {
        Display,
        Body,
        Mono,
    };

    struct SelectionLayout {
        sf::FloatRect heroRect{};
        std::array<sf::FloatRect, 4> nationCards{};
        sf::FloatRect footerRect{};
    };

    struct MatchLayout {
        sf::FloatRect topStrip{};
        sf::FloatRect mapShell{};
        sf::FloatRect mapViewport{};
        sf::Vector2f mapOrigin{};
        float tileSize{0.F};
        sf::FloatRect rightRail{};
        sf::FloatRect commanderPanel{};
        sf::FloatRect hoverPanel{};
        sf::FloatRect frontsPanel{};
        sf::FloatRect objectivePanel{};
        sf::FloatRect bottomChip{};
    };

    [[nodiscard]] SelectionLayout selectionLayout() const;
    [[nodiscard]] MatchLayout matchLayout(const sim::WorldState& world) const;
    [[nodiscard]] sf::Color nationColor(NationId nation) const;
    [[nodiscard]] sf::Color tileColor(const sim::TileState& tile) const;
    [[nodiscard]] const sf::Font* fontFor(FontRole role) const;
    [[nodiscard]] bool loadFonts();
    void drawText(sf::RenderTarget& target,
                  const std::string& value,
                  sf::Vector2f position,
                  unsigned int size,
                  sf::Color color,
                  FontRole role = FontRole::Body,
                  bool centered = false,
                  float letterSpacing = 1.F) const;
    void drawAmbientBackdrop(sf::RenderTarget& target, sf::Vector2u size) const;
    void drawPanel(sf::RenderTarget& target,
                   const sf::FloatRect& rect,
                   sf::Color shellColor,
                   sf::Color coreColor,
                   sf::Color borderColor,
                   bool accentTop = false) const;
    void drawChip(sf::RenderTarget& target,
                  const sf::FloatRect& rect,
                  const std::string& label,
                  sf::Color fill,
                  sf::Color textColor,
                  FontRole role = FontRole::Body) const;

    void drawSelectionHero(sf::RenderTarget& target, const SelectionLayout& layout) const;
    void drawSelectionNationCards(sf::RenderTarget& target,
                                  const SelectionLayout& layout,
                                  NationId highlightedNation) const;
    void drawSelectionFooter(sf::RenderTarget& target, const SelectionLayout& layout) const;

    void drawMatchTopStrip(sf::RenderTarget& target,
                           const MatchLayout& layout,
                           const sim::WorldState& world,
                           const InputController& inputController) const;
    void drawMatchMap(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const;
    void drawActiveTransits(sf::RenderTarget& target,
                            const MatchLayout& layout,
                            const sim::WorldState& world) const;
    void drawCommanderPanel(sf::RenderTarget& target,
                            const MatchLayout& layout,
                            const sim::WorldState& world,
                            const InputController& inputController) const;
    void drawHoverPanel(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const;
    void drawFrontsPanel(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const;
    void drawObjectivePanel(sf::RenderTarget& target, const MatchLayout& layout) const;
    void drawCommandPreview(sf::RenderTarget& target, const MatchLayout& layout, const sim::WorldState& world) const;

    void drawGameOverPanel(sf::RenderTarget& target, const sim::WorldState& world) const;

    sf::Font m_displayFont{};
    sf::Font m_bodyFont{};
    sf::Font m_monoFont{};
    bool m_hasDisplayFont{false};
    bool m_hasBodyFont{false};
    bool m_hasMonoFont{false};
};

} // namespace game::ui
