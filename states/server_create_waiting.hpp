#ifndef STATES_SERVER_CREATE_WAITING_HPP
#define STATES_SERVER_CREATE_WAITING_HPP
#include "menu_state.hpp"
#include "network/client/game_client.hpp"
#include "network/server/game_server.hpp"
#include "network/network_params.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <atomic>
class ServerCreateWaitingState: public MenuState {
public:
    ServerCreateWaitingState(WindowManager&, const sf::View&, sf::IpAddress, PortNumber, const std::string& client_name);
    ~ServerCreateWaitingState();
    void update(float dt) override;
protected:
    void handle_btn_pressed() override;
    void handle_resize_menu(unsigned int, unsigned int, float) override;
private:
    GStyle menu_btn_style, menu_txt_style;
    sf::Clock connection_timer;

    GameClient client;
    GameServer server;
    std::atomic<bool> run_server;
    std::atomic<bool> run_client;
    bool is_game_started = false;
    std::thread client_runner;  
    std::thread server_runner;

    std::string map_name;
};

#endif