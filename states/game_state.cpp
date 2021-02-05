#include "game_state.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

void GameState::draw(float dt) {
    window_manager.window.setView(view);
    window_manager.window.draw(window_manager.background);
    window_manager.window.draw(animation.get_sprite());
}

void GameState::handle_input() {
    sf::Event event;
    while(window_manager.window.pollEvent(event)) {
        switch(event.type) {
        case sf::Event::Closed:
            window_manager.close_window();
            break;
        }
    }
}

GameState::GameState(WindowManager& mngr, const sf::View& view)
                    : State(mngr)
                    , view(view)
                    , animation(window_manager.get_tm().get_animation("p1")) {
    sf::Vector2f fr(view.getSize());
    window_manager.resize_window(fr.x, fr.y);
}

void GameState::update(float dt) {
    animation.update(dt);
}