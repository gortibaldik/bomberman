#include "game_state.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

void GameState::draw(float dt) {
    window_manager.window.setView(view);
    window_manager.window.draw(window_manager.background);
    if (client == nullptr) {
        return;
    }
    client->get_game_map().render(&window_manager.window);
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

GameState::GameState(WindowManager& mngr
                    , const sf::View& view
                    , GameClient* client
                    , GameServer* server)

                    : State(mngr)
                    , view(view)
                    , client(client)
                    , server(server) {
    sf::Vector2f fr(view.getSize());
    window_manager.resize_window(fr.x, fr.y);
    client->get_game_map().fit_to_window(fr.x, fr.y);
}

void GameState::update(float dt) {
    if ((client == nullptr) || !client->is_game_started()) {
        window_manager.pop_states(1);
        client = nullptr;
    }
}