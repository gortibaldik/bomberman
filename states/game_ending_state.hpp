#ifndef STATES_GAME_ENDING_HPP
#define STATES_GAME_ENDING_HPP

#include "menu_state.hpp"
#include "window_manager/def.hpp"
#include "network/client/game_client.hpp"

class GameEndingState: public MenuState {
public:
    GameEndingState( WindowManager&
                   , const sf::View&
                   , GameClient*);
    void update(float dt) override;
private:
    void handle_btn_pressed() override;
    void initialize_leaderboard(const std::vector<std::string>& scores);
    GameClient* client;
    sf::Clock connection_timer;
    bool is_restarting;
    bool leaderboard_initialized;
};

#endif
