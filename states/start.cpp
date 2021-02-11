#include "start.hpp"
#include "server_create.hpp"
#include "client_connect.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

enum BTN {
    NEW_GAME,
    CONNECT,
    CREATE,
    QUIT
};

static const std::string button_style = "media/button_style.cfg";
static const unsigned int mb_txt_size = 30;
static const std::vector<std::string> mb_entries = { "Create a server", "Connect to a server", "Quit" };
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Quit", QUIT}, {"Connect to a server", CONNECT}, {"Create a server", CREATE}
};

void StartState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case CREATE:
            window_manager.push_state(std::make_unique<ServerCreateState>(window_manager, view));
            break;
        case CONNECT:
            window_manager.push_state(std::make_unique<ClientConnectState>(window_manager, view));
            break;
        case QUIT:
            window_manager.window.close();
            break;
        }
    }
}

StartState::StartState(WindowManager& mngr):
        MenuState(mngr),
        menu_btn_style(mngr.get_sh().get_style("button")){
    sf::Vector2f pos(mngr.window.getSize());
    pos *= 0.38f;
    menu.initialize(pos.x, pos.y, mb_txt_size, &menu_btn_style);
    for (auto&& it : mb_entries) {
        menu.add_button(it);
    }
}