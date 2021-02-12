#ifndef STATES_CLIENT_CONNECT_WAITING_HPP
#define STATES_CLIENT_CONNECT_WAITING_HPP

#include "menu_state.hpp"
#include "network/client/game_client.hpp"
#include "network/network_params.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
class ClientConnectWaitingState: public MenuState {
public:
    ClientConnectWaitingState(WindowManager&, const sf::View&, const sf::IpAddress&, PortNumber, const std::string& client_name);
    ~ClientConnectWaitingState();
    void update(float dt) override;
protected:
    void handle_btn_pressed() override;
private:
    GameClient client;
};
#endif