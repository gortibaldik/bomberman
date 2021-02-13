#include "client_connect_waiting.hpp"
#include "window_manager/def.hpp"
#include "game_state.hpp"
#include <iostream>
#include <unordered_map>

static const int port_n_length = 4;
static const int ip_length = 15;
static const int name_length = 20;
enum BTN {
    SUBMIT,
    MM_RETURN,
    QUIT
};

static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Submit", SUBMIT}, {"Return to main menu", MM_RETURN}, {"Quit", QUIT}
};

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
            new_value = "";
            break;
        }
    }
    menu.get_named_field("CONNECTION_STATUS")->set_content(new_value);
    menu.update();
}

void ClientConnectWaitingState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case MM_RETURN:
            window_manager.pop_states(1);
            break;
        case QUIT:
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
    client.connect(ip, port);
    
    menu.add_non_clickable("CONNECTION_STATUS", "Not started yet");
    menu.add_non_clickable("");
    menu.add_button("Return to main menu");
    menu.add_button("Quit");
}

ClientConnectWaitingState::~ClientConnectWaitingState() {}