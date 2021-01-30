#include "server_connect.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

static const int port_size = 4;
static const int ip_size = 15; 
enum BTN {
    SUBMIT,
    RETURN,
    QUIT
};

static const unsigned int mb_txt_size = 30;
static const float mb_default_width_txt = 150.f;
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Submit", SUBMIT}, {"Return to main menu", RETURN}, {"Quit", QUIT}
};

void ServerConnectState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, 0.33f);
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
    pos *= 0.33f;
    menu.initialize(pos.x, pos.y, mb_txt_size, mb_default_width_txt, &menu_btn_style, &menu_txt_style);
    menu.add_non_clickable("Enter IP address");
    menu.add_text_field("IP", [](char a){ return std::isdigit(a) || (a == '.');}, ip_size);
    menu.add_non_clickable("Enter port number");
    menu.add_text_field("PORT", [](char a){ return std::isdigit(a);}, port_size);
    menu.add_button("Submit");
    menu.add_non_clickable("");
    menu.add_button("Return to main menu");
    menu.add_button("Quit");
}