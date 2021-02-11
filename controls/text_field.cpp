#include "text_field.hpp"
#include "control_grid.hpp"
#include <iostream>

#define DUMMY "G"
#define BB_FACTOR 1.10f

void TextField::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    bool cnd = text.getGlobalBounds().width < default_width;
    default_width *= factor;
    default_height *= factor;
    sf::FloatRect fr;
    if (cnd) {
        fr = move_position(factor, new_x, new_y, default_width, default_height);
    } else {
        fr = move_position(factor, new_x, new_y);
    }
    cursor.setSize(sf::Vector2f(1.f, default_height));
    cursor.setPosition(text.findCharacterPos(cursor_pos).x, fr.top);
}

TextField::TextField(   float x,
                        float y,
                        float letter_width,
                        unsigned int max_length,
                        float default_width,
                        const GStyle* gstyle,
                        ControlGrid* grid,
                        std::function<bool(char)>&& in_f,
                        std::function<bool(const std::string&)>&& val_f):
                            ControlField(x, y, letter_width, gstyle, grid),
                            default_width(default_width),
                            max_length(max_length),
                            input_condition(std::move(in_f)),
                            validator(std::move(val_f)) {
    
    text.setString(DUMMY);
    auto&& bb = this->text.getGlobalBounds();
    shape.setPosition(bb.left, bb.top);
    default_height = bb.height * BB_FACTOR;
    shape.setSize(sf::Vector2f(default_width, default_height));
    bounding_box = shape.getGlobalBounds();
    text.setString("");

    cursor.setPosition(bb.left, bb.top);
    cursor.setFillColor(style->ctext);
    cursor.setSize(sf::Vector2f(1.f, bb.height));
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
    auto&& bb = text.getGlobalBounds();
    if (bb.width > default_width) {
        shape.setSize(sf::Vector2f(bb.width, default_height));
    } else {
        shape.setSize(sf::Vector2f(default_width, default_height));
    }
    bounding_box = shape.getGlobalBounds();
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
            state = CONTROL_STATE::ACTIVE;
        } else {
            state = CONTROL_STATE::IDLE;
        }
    }

    if (state == CONTROL_STATE::ACTIVE) {
        if (e.type == sf::Event::TextEntered) {
            handle_text_entered(e.text.unicode);
        } else if (e.type == sf::Event::KeyPressed) {
            handle_special_keys(e.text.unicode);
        }
    }
}

void TextField::update() {
    if (state == CONTROL_STATE::ACTIVE) {
        shape.setFillColor(style->cbackground_highlight);
    } else {
        shape.setFillColor(style->cbackground);
    }
}