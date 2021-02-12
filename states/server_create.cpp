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

static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Create", CREATE}, {"Return to main menu", RETURN}, {"Quit", QUIT}
};

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
                    std::cout << "Creating server waiting state!" << std::endl;
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
        MenuState(mngr, view, "server_create"),
        local_address(sf::IpAddress::getLocalAddress()) {
    menu.initialize(mngr.get_cgsh().get_style("server_create"), sf::Vector2f(mngr.window.getSize()));
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