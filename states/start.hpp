#ifndef STATES_START_HPP
#define STATES_START_HPP

#include "abstract.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class GameStateStart: public GameState {
public:
    void draw(float dt) override;
    void update(float dt) override;
    void handle_input() override;

    GameStateStart(WindowManager& window_manager);
private:
    friend void handle_resize_menu(GameStateStart&, unsigned int, unsigned int);
    friend void handle_btn_pressed(GameStateStart&);
    sf::View view;
    ControlGrid menu;
    GStyle menu_btn_style;
};
#endif