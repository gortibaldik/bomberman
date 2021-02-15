#include "control_field.hpp"
#include "control_grid.hpp"
#include <iostream>

#define ALL_ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

ControlField::ControlField( float x
                          , float y
                          , float letter_size
                          , const GStyle* gstyle
                          , ControlGrid* grid)
                          : style(gstyle)
                          , text("", style->font)
                          , state(CONTROL_STATE::IDLE)
                          , letter_size(letter_size)
                          , grid(grid) {
    
    text.setFillColor(style->ctext);
    text.setPosition(x + gstyle->spacing_x, y + gstyle->spacing_y);
    text.setString(ALL_ALPHABET);
    text.setCharacterSize(static_cast<unsigned int>(this->letter_size));

    bounding_box = this->text.getGlobalBounds();
    bounding_box.width = gstyle->default_width + 2 * gstyle->spacing_x;
    bounding_box.height += 2 * gstyle->spacing_y;
    bounding_box.left = x;
    bounding_box.top = y;
    text.setString("");

    shape.setFillColor(style->cbackground);
    shape.setOutlineThickness(style->border_size);
    shape.setOutlineColor(style->cborder);
}

float ControlField::get_height() const {
    return bounding_box.height;
}

float ControlField::get_width() const {
    return bounding_box.width;
}

float ControlField::get_top() const {
    return bounding_box.top;
}

float ControlField::get_left() const {
    return bounding_box.left;
}

std::string ControlField::get_content() const {
    return text.getString().toAnsiString();
}

void ControlField::set_content(const std::string& new_content) {
    text.setString(new_content);
    bounding_box.width = 2 * style->spacing_x + text.getGlobalBounds().width;
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
}

void ControlField::render(sf::RenderTarget* target) {
    target->draw(shape);
    target->draw(text);
}

void ControlField::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {
    state = CONTROL_STATE::IDLE;
    if (shape.getGlobalBounds().contains(mouse_position)) {
        state = CONTROL_STATE::HOVER;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            state = CONTROL_STATE::ACTIVE;
            grid->which_pressed = this;
        }
    }
}

void ControlField::update() {
    switch(state) {
    case CONTROL_STATE::IDLE:
        text.setFillColor(style->ctext);
        shape.setFillColor(style->cbackground);
        shape.setOutlineColor(style->cborder);
        break;
    case CONTROL_STATE::HOVER:
        text.setFillColor(style->ctext_highlight);
        shape.setFillColor(style->cbackground_highlight);
        shape.setOutlineColor(style->cborder_highlight);
        break;
    }
}