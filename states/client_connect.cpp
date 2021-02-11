#include "client_connect.hpp"
#include "client_connect_waiting.hpp"
#include "window_manager/def.hpp"
#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <functional>
#include "network/utils.hpp"

static const int port_n_length = 5;
static const int ip_length = 15;
static const int name_length = 20;
static const float resizing_factor = 0.2f;
enum BTN {
    ENTER,
    RETURN,
    QUIT
};

static const unsigned int txt_size = 25;
static const float mb_default_width_txt = 150.f;
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Enter the server", ENTER}, {"Return to main menu", RETURN}, {"Quit", QUIT}
};

void ClientConnectState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, resizing_factor);
}

void ClientConnectState::update(float) {
    set_validator(menu.get_named_field("IP"), menu.get_named_field("VALID_IP"), "Invalid ip <example:0.0.0.0>");
    set_validator(menu.get_named_field("PORT"), menu.get_named_field("VALID_PORT"), "Invalid port <must be ( bigger than 1024 ) or 0>");

    menu.update();
}

void ClientConnectState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    std::string ip_address, port, name;
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case ENTER:
            if (menu.get_named_field("IP")->is_valid() &&
                menu.get_named_field("PORT")->is_valid() &&
                menu.get_named_field("NAME")->is_valid()) {
                    window_manager.change_state(std::make_unique<ClientConnectWaitingState>(window_manager,
                        view,
                        menu.get_named_field("IP")->get_content(),
                        std::stoi(menu.get_named_field("PORT")->get_content()),
                        menu.get_named_field("NAME")->get_content()));
                }
            break;
        case RETURN:
            window_manager.pop_states(1);
            break;
        case QUIT:
            window_manager.window.close();
            break;
        }
    }
}

ClientConnectState::ClientConnectState(WindowManager& mngr, const sf::View& view):
        MenuState(mngr, view),
        menu_btn_style( mngr.get_sh().get_style("button")),
        menu_txt_style( mngr.get_sh().get_style("txt")) {
    sf::Vector2f pos(mngr.window.getSize());
    pos *= resizing_factor;
    menu.initialize(pos.x, pos.y, txt_size, mb_default_width_txt, &menu_btn_style, &menu_txt_style);
    menu.add_non_clickable("Enter your new username:");
    menu.add_text_field("NAME", [](char a){ return std::isalnum(a) || std::ispunct(a); },
                                [](const std::string& s){ return s.size() > 0; }, name_length);
    menu.add_non_clickable("IP address of the server");
    menu.add_text_field("IP", [](char a){ return std::isdigit(a) || (a == '.');},
                              &is_valid_ip, ip_length);
    menu.add_non_clickable("VALID_IP", "");
    menu.add_non_clickable("Port number of the server");
    menu.add_text_field("PORT", [](char a){ return std::isdigit(a);},
                                &is_valid_port, port_n_length);
    menu.add_non_clickable("VALID_PORT", "");
    menu.add_button("Enter the server");
    menu.add_non_clickable("");
    menu.add_button("Return to main menu");
    menu.add_button("Quit");
}