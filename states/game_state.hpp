#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "abstract.hpp"
#include "controls/control_grid.hpp"
#include "texture_handling/texture_manager.hpp"
#include "game/map.hpp"
#include "network/client/game_client.hpp"
#include "network/server/game_server.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class GameState: public State {
public:
    void draw(float dt) override;
    void update(float dt) override;
    void handle_input() override;
    
    GameState( WindowManager&
             , const sf::View&
             , GameClient*
             , GameServer*);
protected:
    bool check_move(sf::Packet&);
    GameClient* client;
    GameServer* server;
    sf::View view;
    sf::Time last_update_time, c_time;
};
#endif