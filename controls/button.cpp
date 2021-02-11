#include "button.hpp"
#include "control_grid.hpp"

#define DUMMY "G"

Button::Button( float x,
                float y,
                float letter_width,
                const std::string& text,
                const GStyle* gstyle,
                ControlGrid* grid):  ControlField(x, y, letter_width, gstyle, grid) {
    
    this->text.setString(text);
    bounding_box = this->text.getGlobalBounds();
    shape.setPosition(bounding_box.left, bounding_box.top);
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
}

NonClickableButton::NonClickableButton( float x,
                                        float y,
                                        float letter_width,
                                        const std::string& text,
                                        const GStyle* gstyle,
                                        ControlGrid* grid): ControlField(x, y, letter_width, gstyle, grid) {
    if (text.size() == 0) {
        this->text.setString(DUMMY);
        bounding_box = this->text.getGlobalBounds();
        this->text.setString("");
    } else {
        this->text.setString(text);
        bounding_box = this->text.getGlobalBounds();
    }
    shape.setPosition(bounding_box.left, bounding_box.top);
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
}

void NonClickableButton::update() {}

void NonClickableButton::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {}