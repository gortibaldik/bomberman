#ifndef STATES_SERVER_CREATE_HPP
#define STATES_SERVER_CREATE_HPP

#include "menu_state.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
class ServerCreateState: public MenuState {
public:
    ServerCreateState(WindowManager&, const sf::View&);
    void update(float) override;
protected:
    void handle_btn_pressed() override;
    void handle_resize_menu(unsigned int, unsigned int, float) override;
private:
    GStyle menu_btn_style, menu_txt_style;
    sf::IpAddress local_address;
};
#endif