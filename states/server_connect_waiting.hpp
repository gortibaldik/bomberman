#ifndef STATES_SERVER_CONNECT_WAITING_HPP
#define STATES_SERVER_CONNECT_WAITING_HPP

#include "menu_state.hpp"
#include "network/client/def.hpp"
#include "network/network_params.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
class ServerWaitingState: public MenuState {
public:
    ServerWaitingState(WindowManager&, const sf::View&, const sf::IpAddress&, PortNumber, const std::string& client_name);
    //void update(float dt) override;
protected:
    void handle_btn_pressed() override;
    void handle_resize_menu(unsigned int, unsigned int, float) override;
private:
    GStyle menu_btn_style, menu_txt_style;
    Client client;
};
#endif