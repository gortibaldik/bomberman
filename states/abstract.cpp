#include "abstract.hpp"
#include "window_manager/def.hpp"

void GameState::update_mouse_pos() {
    mouse_pos = window_manager.window.mapPixelToCoords(sf::Mouse::getPosition(window_manager.window));
}