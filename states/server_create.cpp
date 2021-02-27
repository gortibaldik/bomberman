#include "server_create.hpp"
#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include "network/utils.hpp"
#include <iostream>
#include <unordered_map>

enum class BTN : int {
    CREATE, MM_RETURN, QUIT, ENTER_USER, NAME, ENTER_IP, IP, VALID_IP,
    BLANK, ENTER_PORT, PORT, VALID_PORT
};

static const int port_number_length = 5;
static const int name_length = 20;

static const std::unordered_map<BTN, std::string> btn_to_str = { 
    {BTN::CREATE, "Create"}, {BTN::MM_RETURN, "Return to main menu"}, {BTN::QUIT, "Quit"},
    {BTN::ENTER_USER, "Enter your new username:"}, {BTN::NAME, "NAME"},
    {BTN::ENTER_IP, "IP address of the server"}, {BTN::IP, "IP"}, {BTN::VALID_IP, "VALID_IP"},
    {BTN::ENTER_PORT, "Port number of the server"}, {BTN::PORT, "PORT"},
    {BTN::VALID_PORT, "VALID_PORT"}, {BTN::BLANK, ""}
};
static std::unordered_map<std::string, BTN> str_to_btn;
static void initialize_maps() {
    for (auto&& p : btn_to_str) {
        str_to_btn.emplace(p.second, p.first);
    }
}

void ServerCreateState::update(float) {
    set_validator(menu.get_named_field(btn_to_str.at(BTN::PORT))
                 , menu.get_named_field(btn_to_str.at(BTN::VALID_PORT))
                 , "Invalid port <must be ( bigger than 1024 ) or 0>");
    menu.update();
}

void ServerCreateState::handle_btn_pressed() {
    if (pressed && (str_to_btn.find(pressed->get_content()) != str_to_btn.end())) {
        switch (str_to_btn[pressed->get_content()]) {
        case BTN::CREATE:
            if (menu.get_named_field(btn_to_str.at(BTN::PORT))->is_valid() &&
                menu.get_named_field(btn_to_str.at(BTN::NAME))->is_valid()) {
                    window_manager.change_state(std::make_unique<ServerCreateWaitingState>(window_manager,
                        view,
                        local_address,
                        std::stoi(menu.get_named_field(btn_to_str.at(BTN::PORT))->get_content()),
                        menu.get_named_field(btn_to_str.at(BTN::NAME))->get_content()));
                }
            break;
        case BTN::MM_RETURN:
            window_manager.pop_states(1);
            break;
        case BTN::QUIT:
            window_manager.window.close();
            break;
        }
    }
}

ServerCreateState::ServerCreateState(WindowManager& mngr
                                    , const sf::View& view)
                                    : MenuState(mngr, view, "server_create")
                                    , local_address(sf::IpAddress::getLocalAddress()) {
    initialize_maps();
    if (local_address == sf::IpAddress()) {
        // from the SFML 2.5.1 documentation, the default address is returned in case of
        // no connection to the internet
        std::cout << "SERVERCREATESTATE : couldn't get valid ip address of the computer, setting it to local host!" << std::endl;
        local_address = sf::IpAddress::LocalHost;
        menu.add_non_clickable("Just single player, nobody else can connect!");
    } else {
        menu.add_non_clickable("Your local address is " + local_address.toString());
    }
    menu.add_non_clickable(btn_to_str.at(BTN::ENTER_USER));
    menu.add_text_field(btn_to_str.at(BTN::NAME), [](char a){ return std::isalnum(a) || std::ispunct(a); },
                                [](const std::string& s){ return s.size() > 0; }, name_length);
    menu.add_non_clickable(btn_to_str.at(BTN::ENTER_PORT));
    menu.add_text_field(btn_to_str.at(BTN::PORT), [](char a){ return std::isdigit(a); },
                                &is_valid_port, port_number_length);
    menu.add_non_clickable(btn_to_str.at(BTN::VALID_PORT), btn_to_str.at(BTN::BLANK));
    menu.add_button(btn_to_str.at(BTN::CREATE));
    menu.add_non_clickable(btn_to_str.at(BTN::BLANK));
    menu.add_button(btn_to_str.at(BTN::MM_RETURN));
    menu.add_button(btn_to_str.at(BTN::QUIT));
}