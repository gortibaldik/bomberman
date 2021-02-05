#include "game_state.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

void GameState::draw(float dt) {
    window_manager.window.setView(view);
    window_manager.window.draw(window_manager.background);
}

void GameState::handle_input() {
}

GameState::GameState(WindowManager& mngr): State(mngr) {
    sf::Vector2f pos(mngr.window.getSize());
    view.setSize(pos);
    pos *= 0.5f;
    view.setCenter(pos);
}

GameState::GameState(WindowManager& mngr, const sf::View& view): State(mngr), view(view) {
    sf::Vector2f fr(view.getSize());
    window_manager.resize_window(fr.x, fr.y);
}

void GameState::update(float dt) {
}