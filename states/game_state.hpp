#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "menu_state.hpp"
#include "controls/control_grid.hpp"
#include "network/client/game_client.hpp"
#include "network/server/game_server.hpp"
class GameState: public MenuState {
public:
    void draw(float dt) override;
    void update(float dt) override;
    
    GameState( WindowManager&
             , const sf::View&
             , GameClient*);
protected:
    void handle_btn_pressed() override;
    bool check_move(sf::Packet&);
    bool check_deploy(sf::Packet&, sf::Time&);
    void check_messages(float dt);
    bool can_deploy = true;
    GameClient* client;
    sf::View view;
    sf::Time last_update_time, c_time, message_time;
};
#endif