#ifndef STATES_CLIENT_CONNECT_HPP
#define STATES_CLIENT_CONNECT_HPP

#include "menu_state.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class ClientConnectState: public MenuState {
public:

    ClientConnectState(WindowManager&, const sf::View&);
    void update(float) override;
protected:
    void handle_btn_pressed() override;
    bool transition_happened;
};
#endif