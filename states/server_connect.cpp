#include "server_connect.hpp"
#include "server_connect_waiting.hpp"
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
    SUBMIT,
    RETURN,
    QUIT
};

static const unsigned int txt_size = 25;
static const float mb_default_width_txt = 150.f;
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Enter the server", SUBMIT}, {"Return to main menu", RETURN}, {"Quit", QUIT}
};

void ServerConnectState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, resizing_factor);
}

static void set_validator(ControlField* field_to_check, 
                         ControlField* field_to_modify,
                         const std::string& new_value) {
    std::string new_content = "";
    if (!field_to_check->is_valid()) {
        new_content = new_value;
    }
    field_to_modify->set_content(new_content);
}

void ServerConnectState::update(float) {
    set_validator(menu.get_named_field("IP"), menu.get_named_field("VALID_IP"), "Invalid ip <example:0.0.0.0>");
    set_validator(menu.get_named_field("PORT"), menu.get_named_field("VALID_PORT"), "Invalid port <must be bigger than 1025>");

    menu.update();
}

void ServerConnectState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    std::string ip_address, port, name;
    if (btn) {
        auto it = mb_actions.find(btn->get_text());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case SUBMIT:
            if (menu.get_named_field("IP")->is_valid() &&
                menu.get_named_field("PORT")->is_valid() &&
                menu.get_named_field("NAME")->is_valid()) {
                    window_manager.change_state(std::make_unique<ServerWaitingState>(window_manager,
                        view,
                        menu.get_named_field("IP")->get_text(),
                        std::stoi(menu.get_named_field("PORT")->get_text()),
                        menu.get_named_field("NAME")->get_text()));
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

ServerConnectState::ServerConnectState(WindowManager& mngr, const sf::View& view):
        MenuState(mngr, view),
        menu_btn_style( sf::Color::Transparent,
                        sf::Color::Transparent,
                        sf::Color::Transparent,
                        sf::Color::Transparent,
                        sf::Color::Black,
                        sf::Color::Blue,
                        mngr.get_font("main_font"),
                        1.f),
        menu_txt_style( sf::Color::Transparent,
                        sf::Color(250, 188, 188, 50),
                        sf::Color::Black,
                        sf::Color::Black,
                        sf::Color::Black,
                        sf::Color::Black,
                        mngr.get_font("main_font"),
                        1.f) {
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