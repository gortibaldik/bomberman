#ifndef STATES_STARTING_GAME_HPP
#define STATES_STARTING_GAME_HPP

#include "menu_state.hpp"
#include "network/server/game_server.hpp"
#include "network/client/game_client.hpp"
class GameStartingState: public MenuState {
public:
    GameStartingState( WindowManager&
                     , const sf::View&);
    void update(float dt) override;
protected:
    void handle_btn_pressed() override;
    GameClient client;
    GameServer server;
    sf::Clock connection_timer;
};

#endif
