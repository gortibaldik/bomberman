#include "client_connect_waiting.hpp"
#include "window_manager/def.hpp"
#include "game_state.hpp"
#include <iostream>
#include <unordered_map>

static const int port_n_length = 4;
static const int ip_length = 15;
static const int name_length = 20;
enum class BTN {
    SUBMIT, MM_RETURN, CONNECTION_STATUS, BLANK, QUIT
};

static const std::unordered_map<std::string, BTN> str_to_btn = { 
    {"Submit", BTN::SUBMIT}, {"Return to main menu", BTN::MM_RETURN}, {"Quit", BTN::QUIT},
    {"Connection status:", BTN::CONNECTION_STATUS}, {"", BTN::BLANK}
};
static std::unordered_map<BTN, std::string> btn_to_str;
static void initialize_maps() {
    for (auto&& p : str_to_btn) {
        btn_to_str.emplace(p.second, p.first);
    }
}

void ClientConnectWaitingState::update(float) {
    std::string new_value;
    if (client.is_game_started()) {
        window_manager.push_state(std::make_unique<GameState>(window_manager, view, &client));
        return;
    }

    if (client.is_approved()) {
        new_value = "Game starts soon!";   
    } else {
        switch(client.get_status()) {
        case ClientStatus::Connected:
            new_value = "Connected! Waiting for server to start game!";
            break;
        case ClientStatus::TryingToConnect:
            new_value = "Connecting...";
            break;
        case ClientStatus::Failed: case ClientStatus::Terminated:
            new_value = "Failed to connect, please return to main menu!";
            break;
        case ClientStatus::Duplicate:
            new_value = "Server already contains player with the selected name!";
            break;
        default:
            new_value = btn_to_str[BTN::BLANK];
            break;
        }
    }
    menu.get_named_field(btn_to_str[BTN::CONNECTION_STATUS])->set_content(new_value);
    menu.update();
}

void ClientConnectWaitingState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn && (str_to_btn.find(btn->get_content()) != str_to_btn.end())) {
        switch (str_to_btn.at(btn->get_content())) {
        case BTN::MM_RETURN:
            window_manager.pop_states(1);
            break;
        case BTN::QUIT:
            window_manager.window.close();
            break;
        }
    }
}

ClientConnectWaitingState::ClientConnectWaitingState(WindowManager& mngr
                                                    , const sf::View& view
                                                    , const sf::IpAddress& ip
                                                    , PortNumber port
                                                    , const std::string& name)
                                                    : MenuState(mngr, view, "client_wait")
                                                    , client( name
                                                            , mngr.get_tm()
                                                            , mngr.get_tm().get_font("game_font")) {
    initialize_maps();
    client.connect(ip, port);
    menu.add_non_clickable(btn_to_str[BTN::CONNECTION_STATUS], "Not started yet");
    menu.add_non_clickable(btn_to_str[BTN::BLANK]);
    menu.add_button(btn_to_str[BTN::MM_RETURN]);
    menu.add_button(btn_to_str[BTN::QUIT]);
}

ClientConnectWaitingState::~ClientConnectWaitingState() {}