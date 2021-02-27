#ifndef STATES_SERVER_CREATE_WAITING_HPP
#define STATES_SERVER_CREATE_WAITING_HPP
#include "menu_state.hpp"
#include "network/client/game_client.hpp"
#include "network/server/game_server.hpp"
#include "network/network_params.hpp"
#include "controls/control_grid.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class ServerCreateWaitingState: public MenuState {
public:
    ServerCreateWaitingState(WindowManager&
                            , const sf::View&
                            , sf::IpAddress
                            , PortNumber
                            , const std::string& client_name);
    void update(float dt) override;
protected:
    void handle_btn_pressed() override;
private:
    sf::Clock connection_timer;

    GameClient client;
    GameServer server;
    bool is_game_started = false;
};

#endif