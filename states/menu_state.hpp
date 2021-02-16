#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "abstract.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class MenuState: public State {
public:
    void draw(float dt) override;
    void update(float dt) override;
    void handle_input() override;
    
    MenuState(WindowManager&, const std::string&);
    MenuState(WindowManager&, const sf::View&, const std::string&);
    virtual ~MenuState() = default;
protected:
    virtual void handle_btn_pressed() = 0;
    sf::View view;
    const ControlField* pressed = nullptr;
    ControlGrid menu;
    float factor;
};
#endif