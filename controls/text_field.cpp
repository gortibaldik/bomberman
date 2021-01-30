#include "text_field.hpp"
#include "control_grid.hpp"
#include <iostream>

#define DUMMY_FILL "GGGGGGGGGGGG"
#define DUMMY_FILL_SIZE 10

void TextField::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    sf::FloatRect fr;
    if (text.getString().getSize() <= DUMMY_FILL_SIZE) {
        sf::String tmp = text.getString();
        text.setString(DUMMY_FILL);
        fr = move_position(factor, new_x, new_y);
        text.setString(tmp);
    } else {
        fr = move_position(factor, new_x, new_y);
    }
    cursor.setSize(sf::Vector2f(1.f, fr.height));
    cursor.setPosition(fr.left, fr.top);
}

TextField::TextField(   float x,
                        float y,
                        float letter_width,
                        unsigned int max_length,
                        GStyle* gstyle,
                        ControlGrid* grid):  ControlField(x, y, letter_width, gstyle, grid),
                                             max_length(max_length) {
    
    text.setString(DUMMY_FILL);
    auto&& fr = this->text.getGlobalBounds();
    shape.setPosition(fr.left, fr.top);
    shape.setSize(sf::Vector2f(max_length * fr.width, fr.height));
    text.setString("");
    cursor.setPosition(fr.left, fr.top);
    cursor.setFillColor(style->ctext);
    cursor.setSize(sf::Vector2f(1.f, fr.height));
}

void TextField::render(sf::RenderTarget* target) {
    ControlField::render(target);
    target->draw(cursor);
}

void TextField::set_cursor(unsigned int index) {
    cursor.setPosition(text.findCharacterPos(cursor_pos).x, cursor.getGlobalBounds().top);
    if (cursor_pos >= DUMMY_FILL_SIZE) {
        auto&& fr = text.getGlobalBounds();
        shape.setSize(sf::Vector2f(fr.width, fr.height));
    }
}

void TextField::handle_text_entered(sf::Uint32 c) {
    if (std::isalnum(c) || std::ispunct(c) || (c == ' ')) {
        if (cursor_pos > max_length) {
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