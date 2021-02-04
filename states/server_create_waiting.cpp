#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

static const int port_n_length = 4;
static const int ip_length = 15;
static const int name_length = 20;
static const float resizing_factor = 0.3f;
enum BTN {
    SUBMIT,
    MM_RETURN,
    QUIT
};

static const unsigned int txt_size = 25;
static const float mb_default_width_txt = 150.f;
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Submit", SUBMIT}, {"Return to main menu", MM_RETURN}, {"Quit", QUIT}
};

void ServerCreateWaitingState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, resizing_factor);
}

void ServerCreateWaitingState::update(float) {
    std::string new_value;
    switch(client.get_status()) {
    case ClientStatus::Connected:
        new_value = "Connected! Waiting for server to start game!";
        break;
    case ClientStatus::TryingToConnect:
        new_value = "Connecting...";
        break;
    case ClientStatus::Failed:
        new_value = "Failed to connect, please return to main menu!";
        break;
    default:
        new_value = "";
        break;
    }
    menu.get_named_field("CONNECTION_STATUS")->set_content(new_value);
    menu.update();
}

void ServerCreateWaitingState::handle_btn_pressed() {
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case MM_RETURN:
            window_manager.pop_states(1);
            run_client = false;
            run_server = false;
            break;
        case QUIT:
            window_manager.window.close();
            break;
        }
    }
}

ServerCreateWaitingState::ServerCreateWaitingState(WindowManager& mngr, const sf::View& view, sf::IpAddress ip, PortNumber port, const std::string& name):
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
                        1.f),
        client(name),
        server(4),
        run_server(true) {
    sf::Vector2f pos(100, 100);
    menu.initialize(pos.x, pos.y, txt_size, mb_default_width_txt, &menu_btn_style, &menu_txt_style);
    server_runner = std::thread([this, port]() {
                                    server.start(port);
                                    sf::Clock c;
                                    c.restart();
                                    while(run_server && server.is_running()) {
                                        server.update(c.restart());
                                    }
                                    server.terminate();});
    client_runner = std::thread([this, ip, port](){
                                    client.connect(ip, port);
                                    while(run_client && client.is_connected()) {}
                                    client.terminate();});
    menu.add_non_clickable("CONNECTION_STATUS", "Not started yet");
    menu.add_non_clickable("");
    menu.add_button("Return to main menu");
    menu.add_button("Quit");
}

ServerCreateWaitingState::~ServerCreateWaitingState() {
    client.terminate();
    server.terminate();
    if (client_runner.joinable()) {
        client_runner.join();
    }
    if (server_runner.joinable()) {
        server_runner.join();
    }
}