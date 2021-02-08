#include "server_create_waiting.hpp"
#include "window_manager/def.hpp"
#include "states/game_state.hpp"
#include <iostream>
#include <unordered_map>

static const int port_n_length = 4;
static const int ip_length = 15;
static const int name_length = 20;
static const float resizing_factor = 0.2f;
static const int max_players = 4;
enum BTN {
    START_GAME,
    MM_RETURN,
    QUIT
};

static const unsigned int txt_size = 25;
static const float mb_default_width_txt = 150.f;
static const std::unordered_map<std::string, BTN> mb_actions = { 
    {"Start game", START_GAME}, {"Return to main menu", MM_RETURN}, {"Quit", QUIT}
};

void ServerCreateWaitingState::handle_resize_menu(unsigned int width, unsigned int height, float factor) {
    MenuState::handle_resize_menu(width, height, resizing_factor);
}

static int last_dt = Network::ConnectionInterval;

void ServerCreateWaitingState::update(float) {
    if (!server.is_in_waiting_room()) {
        if (connection_timer.getElapsedTime().asMilliseconds() >= Network::ConnectionInterval) {
            server.start_game();
        } else {
            int dt = Network::ConnectionInterval - connection_timer.getElapsedTime().asMilliseconds();
            menu.get_named_field("START_TIME")->set_content("Starting in " + std::to_string(dt / 1000) + " seconds.");
            if (last_dt - dt > 200) {
                server.set_ready_game();
                last_dt = dt;
            }
        }
        if (client.is_game_started()) {
            window_manager.push_state(std::make_unique<GameState>(window_manager, view, &client, &server));
            return;
        }
    }
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
    if (!server.is_in_waiting_room()) {
        return; // after starting the server don't accept any other input 
    }
    auto&& btn = menu.get_pressed_btn();
    if (btn) {
        auto it = mb_actions.find(btn->get_content());
        if (it == mb_actions.end()) {
            return;
        }
        switch (it->second) {
        case START_GAME:
            server.set_ready_game(map_name);
            connection_timer.restart();
            break;
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
        client(name, mngr.get_tm()),
        server(max_players),
        run_server(true),
        run_client(true),
        map_name("media/map_basic.cfg") {
    sf::Vector2f pos(view.getSize());
    pos *= resizing_factor;
    menu.initialize(pos.x, pos.y, txt_size, mb_default_width_txt, &menu_btn_style, &menu_txt_style);
    using namespace std::chrono_literals;
    server_runner = std::thread([this, port]() {
                                    server.start(port);
                                    sf::Clock c;
                                    c.restart();
                                    // server update handles only heartbeating
                                    // we can keep the service running in this
                                    // manner even after the game was started
                                    // since listener loop as well as sending one
                                    // are completely independent of this thread
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
                                    // the same as above, we can keep the thread running
                                    // for whole duration of the game
                                    while(run_client && client.is_connected()) {
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
    menu.add_non_clickable("START_TIME", "");
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