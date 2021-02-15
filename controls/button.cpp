#include "button.hpp"
#include "control_grid.hpp"
#include <iostream>

Button::Button( float x,
                float y,
                float letter_width,
                const std::string& text,
                const GStyle* gstyle,
                ControlGrid* grid):  ControlField(x, y, letter_width, gstyle, grid) {
    
    this->text.setString(text);
    bounding_box.width = gstyle->spacing_x * 2 + this->text.getGlobalBounds().width;
    
    shape.setPosition(bounding_box.left, bounding_box.top);
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
}

NonClickableButton::NonClickableButton( float x,
                                        float y,
                                        float letter_width,
                                        const std::string& text,
                                        const GStyle* gstyle,
                                        ControlGrid* grid): ControlField(x, y, letter_width, gstyle, grid) {
    this->text.setString(text);
    bounding_box.width = gstyle->spacing_x * 2 + this->text.getGlobalBounds().width;
    shape.setPosition(bounding_box.left, bounding_box.top);
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
}

void NonClickableButton::update() {}

void NonClickableButton::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {}