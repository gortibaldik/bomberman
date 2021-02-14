#ifndef STATES_SERVER_CREATE_HPP
#define STATES_SERVER_CREATE_HPP

#include "menu_state.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
class ServerCreateState: public MenuState {
public:
    ServerCreateState(WindowManager&, const sf::View&);
    void update(float) override;
protected:
    void handle_btn_pressed() override;
private:
    sf::IpAddress local_address;
    bool transition_happened;
};
#endif