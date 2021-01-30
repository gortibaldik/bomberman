#ifndef STATES_SERVER_CONNECT_HPP
#define STATES_SERVER_CONNECT_HPP

#include "menu_state.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class ServerConnectState: public MenuState {
public:

    ServerConnectState(WindowManager& window_manager);
protected:
    void handle_btn_pressed() override;
    void handle_resize_menu(unsigned int, unsigned int, float) override;
private:
    GStyle menu_btn_style, menu_txt_style;
};
#endif