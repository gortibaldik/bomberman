#include "control_field.hpp"
#include "control_grid.hpp"
#include <iostream>

sf::FloatRect ControlField::move_position(  float factor,
                                            unsigned int new_x,
                                            unsigned int new_y,
                                            float default_width /*= 0.f*/,
                                            float default_height /*=0 .f*/) {
    letter_width *= factor;

    text.setCharacterSize(static_cast<int>(letter_width));
    text.setPosition(static_cast<float>(new_x), static_cast<float>(new_y));

    auto&& fr = text.getGlobalBounds();
    shape.setSize(sf::Vector2f( default_width == 0.f ? fr.width : default_width,
                                default_height == 0.f ? fr.height : default_height));
    shape.setPosition(fr.left, fr.top);
    bounding_box = shape.getGlobalBounds();
    return fr;
}

void ControlField::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    move_position(factor, new_x, new_y);
}

ControlField::ControlField( float x,
                float y,
                float letter_width,
                GStyle* gstyle,
                ControlGrid* grid):  style(gstyle),
                                text("", style->font),
                                state(CONTROL_STATE::IDLE),
                                letter_width(letter_width),
                                grid(grid) {
    
    text.setFillColor(style->ctext);
    text.setPosition(x, y);
    text.setCharacterSize(static_cast<unsigned int>(this->letter_width));

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

std::string ControlField::get_content() const {
    return text.getString().toAnsiString();
}

void ControlField::set_content(const std::string& new_content) {
    text.setString(new_content);
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