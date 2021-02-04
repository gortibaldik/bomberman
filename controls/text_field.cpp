#include "text_field.hpp"
#include "control_grid.hpp"
#include <iostream>

#define DUMMY "G"
#define BB_FACTOR 1.10f

void TextField::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    sf::FloatRect fr = text.getGlobalBounds();
    bool cnd = fr.width < default_width;
    default_width *= factor;
    default_height *= factor;
    if (cnd) {
        fr = move_position(factor, new_x, new_y, default_width, default_height);
    } else {
        fr = move_position(factor, new_x, new_y);
    }
    cursor.setSize(sf::Vector2f(1.f, default_height));
    cursor.setPosition(fr.left, fr.top);
}

TextField::TextField(   float x,
                        float y,
                        float letter_width,
                        unsigned int max_length,
                        float default_width,
                        GStyle* gstyle,
                        ControlGrid* grid,
                        std::function<bool(char)>&& in_f,
                        std::function<bool(const std::string&)>&& val_f):
                            ControlField(x, y, letter_width, gstyle, grid),
                            default_width(default_width),
                            max_length(max_length),
                            input_condition(std::move(in_f)),
                            validator(std::move(val_f)) {
    
    text.setString(DUMMY);
    auto&& fr = this->text.getGlobalBounds();
    shape.setPosition(fr.left, fr.top);
    default_height = fr.height * BB_FACTOR;
    shape.setSize(sf::Vector2f(default_width, default_height));
    text.setString("");

    cursor.setPosition(fr.left, fr.top);
    cursor.setFillColor(style->ctext);
    cursor.setSize(sf::Vector2f(1.f, fr.height));
}

bool TextField::is_valid() {
    return validator(text.getString());
}

void TextField::render(sf::RenderTarget* target) {
    ControlField::render(target);
    target->draw(cursor);
}

void TextField::set_cursor(unsigned int index) {
    auto x = text.findCharacterPos(cursor_pos).x;
    cursor.setPosition(x, cursor.getGlobalBounds().top);
    auto&& fr = text.getGlobalBounds();
    if (fr.width > default_width) {
        shape.setSize(sf::Vector2f(fr.width, default_height));
    } else {
        shape.setSize(sf::Vector2f(default_width, default_height));
    }
}

void TextField::handle_text_entered(sf::Uint32 c) {
    if (input_condition(c)) {
        if (cursor_pos >= max_length) {
            return;
        }
        sf::String txt = text.getString();
        txt.insert(cursor_pos, c);
        text.setString(txt);
        cursor_pos++;
        set_cursor(cursor_pos);
        return;
    }
}

void TextField::handle_special_keys(sf::Uint32 c) {
    switch(c) {
    case sf::Keyboard::BackSpace:
        if (cursor_pos > 0) {
            sf::String txt = text.getString();
            txt.erase(cursor_pos - 1);
            text.setString(txt);
            cursor_pos--;
            set_cursor(cursor_pos);
        }
    }
}

void TextField::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (shape.getGlobalBounds().contains(mouse_position)) {
            state = ACTIVE;
        } else {
            state = IDLE;
        }
    }

    if (state == ACTIVE) {
        if (e.type == sf::Event::TextEntered) {
            handle_text_entered(e.text.unicode);
        } else if (e.type == sf::Event::KeyPressed) {
            handle_special_keys(e.text.unicode);
        }
    }
}

void TextField::update() {
    if (state == ACTIVE) {
        shape.setFillColor(style->cbackground_highlight);
    } else {
        shape.setFillColor(style->cbackground);
    }
}