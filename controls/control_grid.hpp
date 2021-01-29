#ifndef CONTROL_GRID_HPP
#define CONTROL_GRID_HPP
#include "control_field.hpp"
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

class ControlGrid {
public:
    ControlGrid() {}

    void initialize(    float x,
                        float y,
                        unsigned int letter_size,
                        GStyle* gstyle);
    void add_button(const std::string& button_title);
    
    void render(sf::RenderTarget* target);
    void handle_input(const sf::Vector2f& mouse_position);
    void update();
    void move_pos(float, unsigned int, unsigned int);

    const ControlField* get_pressed_btn();
private:
    float last_x, last_y;
    unsigned int letter_size;
    GStyle* style;
    std::vector<CFPtr> buttons;
    ControlField* which_pressed;
    friend ControlField;
};
#endif