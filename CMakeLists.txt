cmake_minimum_required(VERSION 3.24)
project(realtime_4nation_territory_war LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
include(GNUInstallDirs)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.2
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SFML)

enable_testing()

set(CORE_SOURCES
  src/GameConfig.cpp
  src/sim/WorldState.cpp
  src/sim/ArmyTransit.cpp
  src/sim/CommandQueue.cpp
  src/sim/Pathfinder.cpp
  src/sim/AbilitySystem.cpp
  src/sim/ScoreSystem.cpp
  src/sim/MovementSystem.cpp
  src/sim/CombatSystem.cpp
  src/sim/RegenSystem.cpp
  src/sim/VictorySystem.cpp
  src/ai/AiDirector.cpp
  src/ui/InputController.cpp
  src/ui/HoverPanel.cpp
  src/ui/VisualTuning.cpp
)

set(RENDERER_SOURCES
  src/ui/Renderer.cpp
  src/ui/RendererSelection.cpp
  src/ui/RendererMatch.cpp
  src/ui/RendererGameOver.cpp
)

add_library(territory_war_core STATIC ${CORE_SOURCES})
target_include_directories(territory_war_core PUBLIC include)
target_compile_features(territory_war_core PUBLIC cxx_std_20)

add_executable(territory_war
  src/main.cpp
  src/App.cpp
  ${RENDERER_SOURCES}
)
target_include_directories(territory_war PRIVATE include)
target_compile_definitions(territory_war PRIVATE TERRITORY_WAR_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(territory_war PRIVATE territory_war_core SFML::Graphics SFML::Window SFML::System)

add_executable(territory_war_preview
  tools/render_preview.cpp
  ${RENDERER_SOURCES}
)
target_include_directories(territory_war_preview PRIVATE include)
target_compile_definitions(territory_war_preview PRIVATE TERRITORY_WAR_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(territory_war_preview PRIVATE territory_war_core SFML::Graphics SFML::Window SFML::System)

add_executable(territory_war_tests
  tests/test_main.cpp
  tests/sim_rules_test.cpp
  tests/ai_behavior_test.cpp
  tests/victory_conditions_test.cpp
  tests/ability_system_test.cpp
  tests/score_system_test.cpp
  tests/input_command_test.cpp
  tests/renderer_layout_test.cpp
  tests/ui_visual_tuning_test.cpp
  ${RENDERER_SOURCES}
)
target_include_directories(territory_war_tests PRIVATE include tests)
target_compile_definitions(territory_war_tests PRIVATE TERRITORY_WAR_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(territory_war_tests PRIVATE territory_war_core SFML::Graphics SFML::Window SFML::System)
add_test(NAME territory_war_tests COMMAND territory_war_tests)

install(TARGETS territory_war
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  BUNDLE DESTINATION .
)
install(DIRECTORY assets DESTINATION .)
install(FILES README.md LICENSE DESTINATION .)
