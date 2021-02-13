#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include "states/game_state.hpp"
#include <iostream>
#include <unordered_map>
#include <stdexcept>

static const int port_n_length = 4;
static const int ip_length = 15;
static const int name_length = 20;

enum BTN {
    START_GAME,
    MM_RETURN,
    QUIT,
    BLANK,
    START_TIME,
    CONNECTED_PLAYERS,
    IP
};
static const std::unordered_map<std::string, BTN> str_to_btn = { 
    {"Start game", START_GAME}, {"Return to main menu", MM_RETURN}, {"Quit", QUIT},
    {"IP of the server: ", IP}, {"Connected players:", CONNECTED_PLAYERS},
    {"START_TIME", START_TIME}, {"", BLANK}
};
static std::unordered_map<BTN, std::string> btn_to_str;
static void initialize_maps() {
    for (auto&& str_btn : str_to_btn) {
        btn_to_str.emplace(str_btn.second, str_btn.first);
    }
}

static int last_dt = Network::ConnectionInterval;

void ServerCreateWaitingState::update(float) {
    if (!server.is_in_waiting_room()) {
        if (connection_timer.getElapsedTime().asMilliseconds() >= Network::ConnectionInterval) {
            server.start_game();
        } else {
            int dt = Network::ConnectionInterval - connection_timer.getElapsedTime().asMilliseconds();
            menu.get_named_field(btn_to_str[START_TIME])->set_content("Starting in " + std::to_string(dt / 1000) + " seconds.");
            if (last_dt - dt > 200) {
                server.set_ready_game();
                last_dt = dt;
            }
        }
        if (client.is_game_started()) {
            window_manager.push_state(std::make_unique<GameState>(window_manager, view, &client));
            return;
        }
    }
    int i = 0;
    for (auto&& client : server.get_connected_clients()) {
        menu.get_named_field(std::to_string(i))->set_content("   " + client);
        i++;
    }
    for (;i < server.get_max_players();i++){
        menu.get_named_field(std::to_string(i))->set_content("");
    }
    menu.update();
}

void ServerCreateWaitingState::handle_btn_pressed() {
    if (!server.is_in_waiting_room()) {
        return; // after starting the server don't accept any other input 
    }
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = str_to_btn.find(btn->get_content());
        if (it == str_to_btn.end()) {
            return;
        }
        switch (it->second) {
        case START_GAME:
            server.set_ready_game();
            connection_timer.restart();
            break;
        case MM_RETURN:
            window_manager.pop_states(1);
            server.terminate();
            break;
        case QUIT:
            window_manager.window.close();
            break;
        }
    }
}


ServerCreateWaitingState::ServerCreateWaitingState( WindowManager& mngr
                                                  , const sf::View& view
                                                  , sf::IpAddress ip
                                                  , PortNumber port
                                                  , const std::string& name)
                                                  : MenuState(mngr, view, "server_wait")
                                                  , client(name, mngr.get_tm(), mngr.get_tm().get_font("game_font"))
                                                  , server("media/map_basic.cfg") {
    auto in_port = server.start(port);
    if (in_port == -1) {
        throw std::runtime_error("SERVER_CREATE_WAITING -- Couldn't start the server!");
    }
    if (!client.connect(ip, in_port)) {
        throw std::runtime_error("SERVER_CREATE_WAITING -- Couldn't connect to the server!");
    }

    initialize_maps();
    menu.add_non_clickable(btn_to_str[IP]+ip.toString()+":"+std::to_string(in_port)); 
    menu.add_non_clickable(btn_to_str[CONNECTED_PLAYERS]);
    for (int i = 0; i < server.get_max_players(); i++) {
        menu.add_non_clickable(std::to_string(i), btn_to_str[BLANK]);
    }
    menu.add_non_clickable(btn_to_str[START_TIME], btn_to_str[BLANK]);
    menu.add_non_clickable(btn_to_str[BLANK]);
    menu.add_button(btn_to_str[START_GAME]);
    menu.add_button(btn_to_str[MM_RETURN]);
    menu.add_button(btn_to_str[QUIT]);
}