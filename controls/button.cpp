#include "button.hpp"
#include "control_grid.hpp"

void Button::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    letter_width *= factor;

    text.setCharacterSize(letter_width);
    int text_size = text.getString().getSize();
    text.setPosition(new_x, new_y);

    auto&& fr = text.getGlobalBounds();
    shape.setSize(sf::Vector2f(fr.width, fr.height));
    shape.setPosition(fr.left, fr.top);
}

Button::Button( float x,
                float y,
                float letter_width,
                const std::string& text,
                GStyle* gstyle,
                ControlGrid* grid):  style(gstyle),
                                text(text, *style->font),
                                state(IDLE),
                                letter_width(letter_width),
                                grid(grid) {
    
    this->text.setFillColor(style->ctext);
    this->text.setPosition(x, y);
    this->text.setCharacterSize(this->letter_width);
    
    auto&& fr = this->text.getGlobalBounds();

    shape.setFillColor(style->cbackground);
    shape.setPosition(fr.left, fr.top);
    shape.setSize(sf::Vector2f(fr.width, fr.height));
    shape.setOutlineThickness(style->border_size);
    shape.setOutlineColor(style->cborder);
}

float Button::get_height() const {
    return shape.getGlobalBounds().height;
}

std::string Button::get_text() const {
    return text.getString().toAnsiString();
}

void Button::render(sf::RenderTarget* target) {
    target->draw(shape);
    target->draw(text);
}

void Button::handle_input(const sf::Vector2f& mouse_position) {
    state = IDLE;
    if (shape.getGlobalBounds().contains(mouse_position)) {
        state = HOVER;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            state = PRESSED;
            // grid->which_pressed = this;
        }
    }
}

void Button::update() {
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