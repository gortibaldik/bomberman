#include "server_create.hpp"
#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include "network/utils.hpp"
#include <iostream>
#include <unordered_map>

enum BTN {
    CREATE,
    RETURN,
    QUIT
};

static const int port_number_length = 5;
static const int name_length = 20;
static const float resizing_factor = 0.2f;

static const unsigned int mb_txt_size = 25;
static const float mb_default_width_txt = 150.f;
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Create", CREATE}, {"Return to main menu", RETURN}, {"Quit", QUIT}
};

void ServerCreateState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, resizing_factor);
}

void ServerCreateState::update(float) {
    set_validator(menu.get_named_field("PORT"), menu.get_named_field("VALID_PORT"), "Invalid port <must be ( bigger than 1024 ) or 0>");
    menu.update();
}

void ServerCreateState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case CREATE:
            if (menu.get_named_field("PORT")->is_valid() &&
                menu.get_named_field("NAME")->is_valid()) {
                    window_manager.change_state(std::make_unique<ServerCreateWaitingState>(window_manager,
                        view,
                        local_address,
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

ServerCreateState::ServerCreateState(WindowManager& mngr, const sf::View& view):
        MenuState(mngr, view),
        menu_btn_style( mngr.get_sh().get_style("button")),
        menu_txt_style( mngr.get_sh().get_style("txt")),
        local_address(sf::IpAddress::getLocalAddress()) {
    sf::Vector2f pos(mngr.window.getSize());
    pos *= resizing_factor;
    menu.initialize(pos.x, pos.y, mb_txt_size, mb_default_width_txt, &menu_btn_style, &menu_txt_style);
    menu.add_non_clickable("Your local address is " + local_address.toString());
    menu.add_non_clickable("Enter your username: ");
    menu.add_text_field("NAME", [](char a){ return std::isalnum(a) || std::ispunct(a); },
                                [](const std::string& s){ return s.size() > 0; }, name_length);
    menu.add_non_clickable("Enter port number");
    menu.add_text_field("PORT", [](char a){ return std::isdigit(a); }, 
                                &is_valid_port, port_number_length);
    menu.add_non_clickable("VALID_PORT", "");
    menu.add_button("Create");
    menu.add_non_clickable("");
    menu.add_button("Return to main menu");
    menu.add_button("Quit");
}