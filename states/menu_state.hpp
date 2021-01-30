#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "abstract.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class MenuState: public GameState {
public:
    void draw(float dt) override;
    void update(float dt) override;
    void handle_input() override;
    
    MenuState(WindowManager&);
    MenuState(WindowManager&, const sf::View&);
protected:
    virtual void handle_resize_menu(unsigned int, unsigned int, float factor = 0.38f);
    virtual void handle_btn_pressed() = 0;
    sf::View view;
    ControlGrid menu;
};
#endif