#include "control_field.hpp"
#include "control_grid.hpp"

sf::FloatRect ControlField::move_position(float factor, unsigned int new_x, unsigned int new_y) {
    letter_width *= factor;

    text.setCharacterSize(letter_width);
    text.setPosition(new_x, new_y);

    auto&& fr = text.getGlobalBounds();
    shape.setSize(sf::Vector2f(fr.width, fr.height));
    shape.setPosition(fr.left, fr.top);
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
                                text("", *style->font),
                                state(IDLE),
                                letter_width(letter_width),
                                grid(grid) {
    
    text.setFillColor(style->ctext);
    text.setPosition(x, y);
    text.setCharacterSize(this->letter_width);

    shape.setFillColor(style->cbackground);
    shape.setOutlineThickness(style->border_size);
    shape.setOutlineColor(style->cborder);
}

float ControlField::get_height() const {
    return shape.getGlobalBounds().height;
}

float ControlField::get_width() const {
    return shape.getGlobalBounds().width;
}

std::string ControlField::get_text() const {
    return text.getString().toAnsiString();
}

void ControlField::render(sf::RenderTarget* target) {
    target->draw(shape);
    target->draw(text);
}

void ControlField::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {
    state = IDLE;
    if (shape.getGlobalBounds().contains(mouse_position)) {
        state = HOVER;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            state = ACTIVE;
            grid->which_pressed = this;
        }
    }
}

void ControlField::update() {
    switch(state) {
    case IDLE:
        text.setFillColor(style->ctext);
        shape.setFillColor(style->cbackground);
        shape.setOutlineColor(style->cborder);
        break;
    case HOVER:
        text.setFillColor(style->ctext_highlight);
        shape.setFillColor(style->cbackground_highlight);
        shape.setOutlineColor(style->cborder_highlight);
        break;
    }
}