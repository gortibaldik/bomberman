#include "server_create.hpp"
#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include "network/utils.hpp"
#include <iostream>
#include <unordered_map>

enum BTN {
    CREATE, MM_RETURN, QUIT, ENTER_USER, NAME, ENTER_IP, IP, VALID_IP,
    BLANK, ENTER_PORT, PORT, VALID_PORT
};

static const int port_number_length = 5;
static const int name_length = 20;

static const std::unordered_map<BTN, std::string> btn_to_str = { 
    {CREATE, "Create"}, {MM_RETURN, "Return to main menu"}, {QUIT, "Quit"},
    {ENTER_USER, "Enter your new username:"}, {NAME, "NAME"},
    {ENTER_IP, "IP address of the server"}, {IP, "IP"}, {VALID_IP, "VALID_IP"},
    {ENTER_PORT, "Port number of the server"}, {PORT, "PORT"},
    {VALID_PORT, "VALID_PORT"}, {BLANK, ""}
};
static std::unordered_map<std::string, BTN> str_to_btn;
static void initialize_maps() {
    for (auto&& p : btn_to_str) {
        str_to_btn.emplace(p.second, p.first);
    }
}

void ServerCreateState::update(float) {
    set_validator(menu.get_named_field(btn_to_str.at(PORT))
                 , menu.get_named_field(btn_to_str.at(VALID_PORT))
                 , "Invalid port <must be ( bigger than 1024 ) or 0>");
    menu.update();
}

void ServerCreateState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn && (str_to_btn.find(btn->get_content()) != str_to_btn.end())) {
        switch (str_to_btn[btn->get_content()]) {
        case CREATE:
            if (menu.get_named_field(btn_to_str.at(PORT))->is_valid() &&
                menu.get_named_field(btn_to_str.at(NAME))->is_valid()) {
                    std::cout << "Creating server waiting state!" << std::endl;
                    window_manager.change_state(std::make_unique<ServerCreateWaitingState>(window_manager,
                        view,
                        local_address,
                        std::stoi(menu.get_named_field(btn_to_str.at(PORT))->get_content()),
                        menu.get_named_field(btn_to_str.at(NAME))->get_content()));
                }
            break;
        case MM_RETURN:
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
    initialize_maps();
    menu.add_non_clickable("Your local address is " + local_address.toString());
    menu.add_non_clickable(btn_to_str.at(ENTER_USER));
    menu.add_text_field(btn_to_str.at(NAME), [](char a){ return std::isalnum(a) || std::ispunct(a); },
                                [](const std::string& s){ return s.size() > 0; }, name_length);
    menu.add_non_clickable(btn_to_str.at(ENTER_PORT));
    menu.add_text_field(btn_to_str.at(PORT), [](char a){ return std::isdigit(a); }, 
                                &is_valid_port, port_number_length);
    menu.add_non_clickable(btn_to_str.at(VALID_PORT), btn_to_str.at(BLANK));
    menu.add_button(btn_to_str.at(CREATE));
    menu.add_non_clickable(btn_to_str.at(BLANK));
    menu.add_button(btn_to_str.at(MM_RETURN));
    menu.add_button(btn_to_str.at(QUIT));
}