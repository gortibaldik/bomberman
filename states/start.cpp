#include "start.hpp"
#include "server_create.hpp"
#include "client_connect.hpp"
#include "starting_game.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

enum class BTN: int {
    NEW_GAME, CONNECT, CREATE, SINGLE_GAME, QUIT
};

static const std::unordered_map<std::string, BTN> str_to_btn = { 
    {"Quit", BTN::QUIT}, {"Connect to a server", BTN::CONNECT}, {"Create a server", BTN::CREATE},
    {"Start single game", BTN::SINGLE_GAME}
};
static std::unordered_map<BTN, std::string> btn_to_str;
static void initialize_maps() {
    for (auto&& p : str_to_btn) {
        btn_to_str.emplace(p.second, p.first);
    }
}

void StartState::handle_btn_pressed() {
    if (pressed != nullptr) {
        auto it = str_to_btn.find(pressed->get_content());
        if (it == str_to_btn.end()) { return; }
        switch (it->second) {
        case BTN::CREATE:
            window_manager.push_state(std::make_unique<ServerCreateState>(window_manager, view));
            break;
        case BTN::CONNECT:
            window_manager.push_state(std::make_unique<ClientConnectState>(window_manager, view));
            break;
        case BTN::SINGLE_GAME:
            window_manager.push_state(std::make_unique<GameStartingState>(window_manager, view));
            break;
        case BTN::QUIT:
            window_manager.window.close();
            break;
        }
    }
}

StartState::StartState( WindowManager& mngr)
                      : MenuState(mngr, "start_menu") {
    initialize_maps();
    menu.add_button(btn_to_str.at(BTN::SINGLE_GAME));
    if (sf::IpAddress::getLocalAddress() != sf::IpAddress()) {
        menu.add_button(btn_to_str.at(BTN::CREATE));
        menu.add_button(btn_to_str.at(BTN::CONNECT));
    } else {
        menu.add_non_clickable("");
        menu.add_non_clickable("Multiplayer mode not allowed,");
        menu.add_non_clickable("no internet connection");
        menu.add_non_clickable("");
    }
    menu.add_button(btn_to_str.at(BTN::QUIT));
}
