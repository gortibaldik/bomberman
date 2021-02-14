#include "start.hpp"
#include "server_create.hpp"
#include "client_connect.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

enum class BTN: int {
    NEW_GAME, CONNECT, CREATE, QUIT
};

static const std::vector<std::string> mb_entries = { "Create a server", "Connect to a server", "Quit" };
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Quit", BTN::QUIT}, {"Connect to a server", BTN::CONNECT}, {"Create a server", BTN::CREATE}
};

void StartState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case BTN::CREATE:
            window_manager.push_state(std::make_unique<ServerCreateState>(window_manager, view));
            break;
        case BTN::CONNECT:
            window_manager.push_state(std::make_unique<ClientConnectState>(window_manager, view));
            break;
        case BTN::QUIT:
            window_manager.window.close();
            break;
        }
    }
}

StartState::StartState( WindowManager& mngr)
                      : MenuState(mngr, "start_menu") {
    for (auto&& it : mb_entries) {
        menu.add_button(it);
    }
}