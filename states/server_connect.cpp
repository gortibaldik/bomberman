#include "server_connect.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

static const int port_n_length = 4;
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
    {"Submit", SUBMIT}, {"Return to main menu", RETURN}, {"Quit", QUIT}
};

void ServerConnectState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, resizing_factor);
}

void ServerConnectState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_text());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case RETURN:
            window_manager.pop_state();
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
    menu.add_text_field("NAME", [](char a){ return std::isalnum(a) || std::ispunct(a); }, name_length);
    menu.add_non_clickable("Enter IP address");
    menu.add_text_field("IP", [](char a){ return std::isdigit(a) || (a == '.');}, ip_length);
    menu.add_non_clickable("Enter port number");
    menu.add_text_field("PORT", [](char a){ return std::isdigit(a);}, port_n_length);
    menu.add_button("Submit");
    menu.add_non_clickable("");
    menu.add_button("Return to main menu");
    menu.add_button("Quit");
}