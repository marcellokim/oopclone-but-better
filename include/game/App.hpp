#pragma once

#include "game/GameConfig.hpp"
#include "game/ai/AiDirector.hpp"
#include "game/sim/CommandQueue.hpp"
#include "game/ui/InputController.hpp"
#include "game/ui/Renderer.hpp"

#include <SFML/Graphics.hpp>

namespace game {

class App {
  public:
    App();
    int run();

  private:
    enum class Scene {
        NationSelect,
        Match,
        GameOver,
    };

    void processEvents();
    void update(float frameSeconds);
    void updateMatch(float frameSeconds);
    void draw();
    void startMatch(NationId playerNation);
    void handleKeyPressed(const sf::Event::KeyPressed& keyPressed);
    void handleMouseMoved(const sf::Event::MouseMoved& mouseMoved);
    void handleMousePressed(const sf::Event::MouseButtonPressed& mousePressed);

    MatchConfig m_config{};
    sf::RenderWindow m_window;
    Scene m_scene{Scene::NationSelect};
    NationId m_selectedNation{NationId::SwiftLeague};
    float m_simAccumulator{0.F};
    sim::WorldState m_world{};
    sim::CommandQueue m_commandQueue{};
    ai::AiDirector m_aiDirector{};
    ui::InputController m_inputController{};
    ui::Renderer m_renderer{};
};

} // namespace game
