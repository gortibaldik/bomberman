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
    client->render_players(&window_manager.window);
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
    client->fit_entities_to_window();
}

static float client_update_constant = 0.05f; 

using Coords = std::pair<int, int>;

void GameState::check_move(sf::Packet& packet) {
    static const std::unordered_map<sf::Keyboard::Key, Coords> key_to_dir =
        { {sf::Keyboard::Left, Coords(-1,0)},
          {sf::Keyboard::Right, Coords(1,0)},
          {sf::Keyboard::Up, Coords(0,-1)},
          {sf::Keyboard::Down, Coords(0,1)} };

    Coords c(0,0);
    for (auto&& key : key_to_dir) {
        if (sf::Keyboard::isKeyPressed(key.first)) {
            c.first += key.second.first;
            c.second += key.second.second;
        }
    }
    if (c != Coords(0,0)) {
        add_type_to_packet(packet, PacketType::ClientMove);
        packet << sf::Int8(c.first) << sf::Int8(c.second);
        packet << sf::Int8(Network::Delimiter);
    }
}

void GameState::update(float dt) {
    if ((client == nullptr) || !client->is_game_started()) {
        window_manager.pop_states(1);
        client = nullptr;
        return;
    }
    c_time += sf::seconds(dt);
    float delta = (c_time - last_update_time).asSeconds();
    if (delta >= client_update_constant) {
        sf::Packet packet;
        last_update_time = c_time;
        check_move(packet);
    }
}