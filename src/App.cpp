#include "game/App.hpp"

#include "game/sim/CombatSystem.hpp"
#include "game/sim/AbilitySystem.hpp"
#include "game/sim/MovementSystem.hpp"
#include "game/sim/RegenSystem.hpp"
#include "game/sim/VictorySystem.hpp"

namespace game {
App::App()
    : m_config(defaultMatchConfig()),
      m_window(sf::VideoMode({1280u, 800u}), "Realtime 4-Nation Territory War") {
    m_window.setFramerateLimit(60);
}

int App::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        processEvents();
        update(clock.restart().asSeconds());
        draw();
    }

    return 0;
}

void App::startMatch(const NationId playerNation) {
    m_selectedNation = playerNation;
    m_world = sim::createInitialWorld(playerNation, m_config);
    m_commandQueue = sim::CommandQueue{};
    m_inputController.clearSelection(m_world);
    m_simAccumulator = 0.F;
    m_scene = Scene::Match;
}

void App::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            handleKeyPressed(*keyPressed);
        }
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
            handleMouseMoved(*mouseMoved);
        }
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            handleMousePressed(*mousePressed);
        }
    }
}

void App::update(const float frameSeconds) {
    if (m_scene == Scene::Match) {
        updateMatch(frameSeconds);
    }
}

void App::updateMatch(const float frameSeconds) {
    m_simAccumulator += frameSeconds;
    while (m_simAccumulator >= m_config.simTickSeconds) {
        m_world.tickCount += 1;
        sim::AbilitySystem::update(m_world, m_config.simTickSeconds, m_config);
        m_aiDirector.update(m_world, m_commandQueue, m_config.simTickSeconds, m_config);
        sim::MovementSystem::flushOrders(m_world, m_commandQueue, m_config);
        for (const auto& arrival : sim::MovementSystem::update(m_world, m_config.simTickSeconds)) {
            sim::CombatSystem::resolveArrival(m_world, arrival);
        }
        sim::RegenSystem::update(m_world, m_config.simTickSeconds, m_config);
        sim::VictorySystem::update(m_world);
        if (m_world.matchEnded) {
            m_scene = Scene::GameOver;
            break;
        }
        m_simAccumulator -= m_config.simTickSeconds;
    }
}

void App::draw() {
    if (!m_renderer.hasFont()) {
        if (m_scene == Scene::NationSelect) {
            m_window.setTitle("Realtime 4-Nation Territory War | Font missing: press 1-4 or click buttons top-to-bottom");
        } else if (m_scene == Scene::Match) {
            std::string title = "Realtime 4-Nation Territory War | Font missing | Ratio " + m_inputController.sendRatioLabel();
            if (m_world.hoveredTile.has_value()) {
                const auto coord = *m_world.hoveredTile;
                const auto& tile = sim::tileAt(m_world, coord);
                title += " | Hover (" + std::to_string(coord.x) + "," + std::to_string(coord.y) + ") " +
                         std::string(nationCompactName(tile.owner)) + " troops=" + std::to_string(tile.troops);
            }
            m_window.setTitle(title);
        } else if (m_scene == Scene::GameOver) {
            m_window.setTitle(
                m_world.winner.has_value()
                    ? "Battle concluded | Winner: " + std::string(nationName(*m_world.winner)) + " | Enter/click to return"
                    : "Battle concluded | Stalemate | Enter/click to return");
        }
    } else {
        m_window.setTitle("Realtime 4-Nation Territory War");
    }
    m_window.clear(sf::Color(20, 24, 31));
    switch (m_scene) {
    case Scene::NationSelect:
        m_renderer.drawSelectionScreen(m_window, m_selectedNation);
        break;
    case Scene::Match:
        m_renderer.drawMatch(m_window, m_world, m_inputController);
        break;
    case Scene::GameOver:
        m_renderer.drawGameOver(m_window, m_world);
        break;
    }
    m_window.display();
}

void App::handleKeyPressed(const sf::Event::KeyPressed& keyPressed) {
    if (m_scene == Scene::NationSelect) {
        if (keyPressed.code == sf::Keyboard::Key::Num1) {
            startMatch(NationId::SwiftLeague);
        } else if (keyPressed.code == sf::Keyboard::Key::Num2) {
            startMatch(NationId::IronLegion);
        } else if (keyPressed.code == sf::Keyboard::Key::Num3) {
            startMatch(NationId::BastionDirectorate);
        } else if (keyPressed.code == sf::Keyboard::Key::Num4) {
            startMatch(NationId::CrownConsortium);
        }
        return;
    }

    if (m_scene == Scene::Match) {
        if (keyPressed.code == sf::Keyboard::Key::Escape) {
            m_inputController.clearSelection(m_world);
        } else if (keyPressed.code == sf::Keyboard::Key::Num1) {
            m_inputController.setSendRatio(sim::SendRatio::Quarter);
        } else if (keyPressed.code == sf::Keyboard::Key::Num2) {
            m_inputController.setSendRatio(sim::SendRatio::Half);
        } else if (keyPressed.code == sf::Keyboard::Key::Num3) {
            m_inputController.setSendRatio(sim::SendRatio::Full);
        } else if (keyPressed.code == sf::Keyboard::Key::Space) {
            static_cast<void>(sim::AbilitySystem::activate(m_world, m_world.playerNation, m_config));
        }
        return;
    }

    if (m_scene == Scene::GameOver && keyPressed.code == sf::Keyboard::Key::Enter) {
        m_scene = Scene::NationSelect;
    }
}

void App::handleMouseMoved(const sf::Event::MouseMoved& mouseMoved) {
    if (m_scene == Scene::NationSelect) {
        if (const auto hoveredNation = m_renderer.selectionFromPixel(mouseMoved.position)) {
            m_selectedNation = *hoveredNation;
        }
        return;
    }

    if (m_scene != Scene::Match) {
        return;
    }

    m_world.hoveredTile = m_renderer.tileFromPixel(m_world, mouseMoved.position);
}

void App::handleMousePressed(const sf::Event::MouseButtonPressed& mousePressed) {
    if (mousePressed.button == sf::Mouse::Button::Left) {
        if (m_scene == Scene::NationSelect) {
            if (const auto picked = m_renderer.selectionFromPixel(mousePressed.position)) {
                startMatch(*picked);
            }
            return;
        }

        if (m_scene == Scene::Match) {
            if (m_renderer.abilityPanelFromPixel(mousePressed.position)) {
                static_cast<void>(sim::AbilitySystem::activate(m_world, m_world.playerNation, m_config));
                return;
            }
            if (const auto tile = m_renderer.tileFromPixel(m_world, mousePressed.position)) {
                m_inputController.handleTileClick(m_world, m_world.playerNation, *tile, m_commandQueue);
            }
            return;
        }

        if (m_scene == Scene::GameOver) {
            m_scene = Scene::NationSelect;
        }
    }

    if (mousePressed.button == sf::Mouse::Button::Right && m_scene == Scene::Match) {
        m_inputController.clearSelection(m_world);
    }
}

} // namespace game
