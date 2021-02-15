#include "game_state.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

enum class BTN : int {
    QUIT, INFO, DISCONNECT, BLANK
};

static const std::unordered_map<BTN, std::string> btn_to_str = { 
    {BTN::QUIT, "QUIT"}, {BTN::INFO, "INFO"}, {BTN::DISCONNECT, "Disconnect"},
    {BTN::BLANK, ""}
};
static std::unordered_map<std::string, BTN> str_to_btn;
static void initialize_maps() {
    for (auto&& p : btn_to_str) {
        str_to_btn.emplace(p.second, p.first);
    }
}

void GameState::draw(float dt) {
    window_manager.window.setView(view);
    if (client == nullptr) {
        return;
    }
    client->get_game_map().render(&window_manager.window);
    client->render_entities(&window_manager.window);
    menu.render(&window_manager.window);
}

void GameState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    
}

static float client_update_constant = 0.05f; 

using Coords = std::pair<int, int>;

bool GameState::check_move(sf::Packet& packet) {
    using coords_dir = std::pair<Coords, EntityDirection>;
    static const std::unordered_map<sf::Keyboard::Key, coords_dir> key_to_dir =
        { {sf::Keyboard::Left, {Coords(0,-1), EntityDirection::LEFT}},
          {sf::Keyboard::Right, {Coords(0,1), EntityDirection::RIGHT}},
          {sf::Keyboard::Up, {Coords(-1,0), EntityDirection::UP}},
          {sf::Keyboard::Down, {Coords(1,0), EntityDirection::DOWN}} };

    Coords c(0,0);
    auto d = client->me->direction;
    
    for (auto&& key : key_to_dir) {
        if (sf::Keyboard::isKeyPressed(key.first)) {
            c.first += key.second.first.first;
            c.second += key.second.first.second;
            d = key.second.second;
            break;
        }
    }
    if (c != Coords(0,0)) {
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ClientMove);
        packet << sf::Int8(c.first) << sf::Int8(c.second) << sf::Int8(d);
        can_deploy = true;
        return true;
    }
    return false;
}

#define WAIT_PERIOD 0.5f

bool GameState::check_deploy(sf::Packet& packet, sf::Time& c_time) {
    static sf::Time last_time = sf::seconds(0.f);
    if (!can_deploy || ((c_time - last_time).asSeconds() <= WAIT_PERIOD)) { return false; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        last_time = c_time;
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ClientDeployBomb);
        can_deploy = false;
        return true;
    }
    return false;
}

void GameState::update(float dt) {
    menu.update();
    if ((client == nullptr) || !client->is_game_started()) {
        window_manager.pop_states(1);
        client = nullptr;
        return;
    }
    client->update(dt);
    c_time += sf::seconds(dt);
    float delta = (c_time - last_update_time).asSeconds();
    if (delta >= client_update_constant) {
        sf::Packet packet;
        last_update_time = c_time;
        add_type_to_packet(packet, PacketType::Update);
        bool send = false;
        send = send || check_move(packet);
        send = check_deploy(packet, c_time) || send;
        if (send) {
            client->send(packet);
        }
    }
}

GameState::GameState(WindowManager& mngr
                    , const sf::View& view
                    , GameClient* client)
                    : MenuState(mngr, "game_menu")
                    , view(view)
                    , client(client) {
    menu.add_button(btn_to_str.at(BTN::QUIT));
    menu.add_button(btn_to_str.at(BTN::DISCONNECT));
    menu.add_non_clickable(btn_to_str.at(BTN::INFO), btn_to_str.at(BTN::BLANK));


    window_manager.window.setSize(sf::Vector2u(mngr.get_window_size().x, menu.get_top() + menu.get_height()));
    auto size = mngr.get_window_size();
    this->view.setSize(size.x, size.y);
    this->view.setCenter(size.x / 2.f, size.y / 2.f);

    client->get_game_map().fit_to_window(size.x, menu.get_top());
    client->fit_entities_to_window();
}