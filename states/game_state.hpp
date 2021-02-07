#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "abstract.hpp"
#include "controls/control_grid.hpp"
#include "texture_handling/texture_manager.hpp"
#include "game/map.hpp"
#include "network/client/def.hpp"
#include "network/server/server.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class GameState: public State {
public:
    void draw(float dt) override;
    void update(float dt) override;
    void handle_input() override;
    
    GameState( WindowManager&
             , const sf::View&
             , const std::string&
             , Client*
             , Server*);
protected:
    Client* client;
    Server* server;
    sf::View view;
    GameMap game_map;
};
#endif