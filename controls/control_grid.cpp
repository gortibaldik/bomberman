#include "control_grid.hpp"
#include <stdexcept>

#define SPACING_FACTOR 1.3f

void ControlGrid::initialize(float x,
                            float y,
                            unsigned int letter_size,
                            GStyle* gstyle) {
    buttons.clear();
    last_x = x;
    last_y = y;
    this->letter_size = letter_size;
    style = gstyle;
}

void ControlGrid::add_button(const std::string& button_title) {
    // x is left
    // y is upper
    // x remains unchanged
    // y grows with button_height
    buttons.push_back(std::make_unique<ControlField>(last_x, last_y, letter_size, button_title, style, this));
    float button_height = buttons.begin()->get()->get_height();
    last_y += button_height * SPACING_FACTOR;
}
    
void ControlGrid::render(sf::RenderTarget* target) {
    for (auto&& button : buttons) {
        button->render(target);
    }
}

void ControlGrid::handle_input(const sf::Vector2f& mouse_position) {
    for (auto&& button : buttons) {
        button->handle_input(mouse_position);
    }
}

void ControlGrid::update() {
    for (auto&& button : buttons) {
        button->update();
    }
    which_pressed = nullptr;
}

const ControlField* ControlGrid::get_pressed_btn() {
    return which_pressed;
}

void ControlGrid::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    float button_height = buttons.begin()->get()->get_height();
    for (auto&& button : buttons) {
        button->move_pos(factor, new_x, new_y);
        new_y += button_height * SPACING_FACTOR;
    }
}