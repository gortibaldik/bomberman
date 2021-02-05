#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

static const int port_n_length = 4;
static const int ip_length = 15;
static const int name_length = 20;
static const float resizing_factor = 0.2f;
static const int max_players = 4;
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
    int i = 0;
    for (auto&& client : server.get_connected_clients()) {
        menu.get_named_field(std::to_string(i))->set_content("   " + client);
        i++;
    }
    for (;i < max_players;i++){
        menu.get_named_field(std::to_string(i))->set_content("");
    }
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
        server(max_players),
        run_server(true),
        run_client(true) {
    sf::Vector2f pos(view.getSize());
    pos *= resizing_factor;
    menu.initialize(pos.x, pos.y, txt_size, mb_default_width_txt, &menu_btn_style, &menu_txt_style);
    using namespace std::chrono_literals;
    server_runner = std::thread([this, port]() {
                                    server.start(port);
                                    sf::Clock c;
                                    c.restart();
                                    while(run_server && server.is_running()) {
                                        server.update(c.restart());
                                        std::this_thread::sleep_for(100ms);
                                    }
                                    std::cout << "Server terminating" << std::endl;
                                    server.terminate();});
    client_runner = std::thread([this, ip, port](){
                                    client.connect(ip, port);
                                    sf::Clock c;
                                    c.restart();
                                    while(client.is_connected()) {
                                        std::this_thread::sleep_for(100ms);
                                        client.update(c.restart());
                                    }
                                    std::cout << "Client terminating" << std::endl;
                                    client.terminate(); });
    menu.add_non_clickable("IP of the server: "+ip.toString()+":"+std::to_string(port)); 
    menu.add_non_clickable("Connected players:");
    for (int i = 0; i < max_players; i++) {
        menu.add_non_clickable(std::to_string(i), "");
    }
    menu.add_non_clickable("");
    menu.add_button("Start game");
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