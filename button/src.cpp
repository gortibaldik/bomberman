#include "def.hpp"

#define LETTER_WIDTH 21

void Button::move_pos(float x, float y) {
    shape.setPosition(x - text.getString().getSize() * LETTER_WIDTH / 2, y);
}

Button::Button(float x,
            float y,
            float height,
            const std::string& text,
            GStyle* gstyle):    shape(sf::Vector2f(text.size() * LETTER_WIDTH, height)),
                                style(gstyle),
                                text(text, *style->font),
                                state(IDLE) {
    shape.setPosition(x - text.size() * 21 / 2, y);
    shape.setFillColor(style->cbackground);
    this->text.setFillColor(style->ctext);
    this->text.setPosition(
        shape.getPosition().x,
        shape.getPosition().y
    );
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
        }
    }
}

void Button::update() {
    switch(state) {
    case IDLE:
        text.setFillColor(style->ctext);
        shape.setFillColor(style->cbackground);
        break;
    case HOVER:
        text.setFillColor(style->ctext_highlight);
        shape.setFillColor(style->cbackground_highlight);
        break;
    }
}