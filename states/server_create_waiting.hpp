#ifndef STATES_SERVER_CREATE_WAITING_HPP
#define STATES_SERVER_CREATE_WAITING_HPP
#include "menu_state.hpp"
#include "network/client/def.hpp"
#include "network/server/def.hpp"
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
    ControlGrid connected_players;
    Client client;
    Server server;
    std::atomic<bool> run_server;
    std::atomic<bool> run_client;
    std::thread client_runner;  
    std::thread server_runner;
};

#endif