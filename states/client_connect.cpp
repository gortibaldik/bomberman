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
enum class BTN : int {
    ENTER_SERVER, MM_RETURN, QUIT, ENTER_USER, NAME, ENTER_IP, IP, VALID_IP,
    BLANK, ENTER_PORT, PORT, VALID_PORT
};

static const std::unordered_map<BTN, std::string> btn_to_str = { 
    {BTN::ENTER_SERVER, "Enter the server"}, {BTN::MM_RETURN, "Return to main menu"}, {BTN::QUIT, "Quit"},
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

void ClientConnectState::update(float) {
    set_validator(menu.get_named_field(btn_to_str.at(BTN::IP))
                 , menu.get_named_field(btn_to_str.at(BTN::VALID_IP))
                 , "Invalid ip <example:0.0.0.0>");
    set_validator(menu.get_named_field(btn_to_str.at(BTN::PORT))
                 , menu.get_named_field(btn_to_str.at(BTN::VALID_PORT))
                 , "Invalid port <must be bigger than 1024>");

    menu.update();
}

void ClientConnectState::handle_btn_pressed() {
    std::string ip_address, port, name;
    if (pressed && (str_to_btn.find(pressed->get_content()) != str_to_btn.end())) {
        switch (str_to_btn[pressed->get_content()]) {
        case BTN::ENTER_SERVER:
            if (menu.get_named_field(btn_to_str.at(BTN::IP))->is_valid() &&
                menu.get_named_field(btn_to_str.at(BTN::PORT))->is_valid() &&
                menu.get_named_field(btn_to_str.at(BTN::NAME))->is_valid()) {
                    window_manager.change_state(std::make_unique<ClientConnectWaitingState>(window_manager,
                        view,
                        menu.get_named_field(btn_to_str.at(BTN::IP))->get_content(),
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

ClientConnectState::ClientConnectState(WindowManager& mngr
                                      , const sf::View& view)
                                      : MenuState(mngr, view, "client_connect") {
    initialize_maps();
    menu.add_non_clickable(btn_to_str.at(BTN::ENTER_USER));
    menu.add_text_field(btn_to_str.at(BTN::NAME), [](char a){ return std::isalnum(a) || std::ispunct(a); },
                                [](const std::string& s){ return s.size() > 0; }, name_length);
    menu.add_non_clickable(btn_to_str.at(BTN::ENTER_IP));
    menu.add_text_field(btn_to_str.at(BTN::IP), [](char a){ return std::isdigit(a) || (a == '.');},
                              &is_valid_ip, ip_length);
    menu.add_non_clickable(btn_to_str.at(BTN::VALID_IP), "");
    menu.add_non_clickable(btn_to_str.at(BTN::ENTER_PORT));
    menu.add_text_field(btn_to_str.at(BTN::PORT), [](char a){ return std::isdigit(a);},
                                &is_valid_port_nz, port_n_length);
    menu.add_non_clickable(btn_to_str.at(BTN::VALID_PORT), btn_to_str.at(BTN::BLANK));
    menu.add_button(btn_to_str.at(BTN::ENTER_SERVER));
    menu.add_non_clickable(btn_to_str.at(BTN::BLANK));
    menu.add_button(btn_to_str.at(BTN::MM_RETURN));
    menu.add_button(btn_to_str.at(BTN::QUIT));
}