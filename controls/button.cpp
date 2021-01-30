#include "button.hpp"
#include "control_grid.hpp"

Button::Button( float x,
                float y,
                float letter_width,
                const std::string& text,
                GStyle* gstyle,
                ControlGrid* grid):  ControlField(x, y, letter_width, gstyle, grid) {
    
    this->text.setString(text);
    auto&& fr = this->text.getGlobalBounds();
    shape.setPosition(fr.left, fr.top);
    shape.setSize(sf::Vector2f(fr.width, fr.height));
}

NonClickableButton::NonClickableButton( float x,
                                        float y,
                                        float letter_width,
                                        const std::string& text,
                                        GStyle* gstyle,
                                        ControlGrid* grid): ControlField(x, y, letter_width, gstyle, grid) {
    this->text.setString(text);
    auto&& fr = this->text.getGlobalBounds();
    shape.setPosition(fr.left, fr.top);
    shape.setSize(sf::Vector2f(fr.width, fr.height));
}

void NonClickableButton::update() {}

void NonClickableButton::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {}