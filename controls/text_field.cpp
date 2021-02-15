#include "text_field.hpp"
#include "control_grid.hpp"

TextField::TextField( float x
                    , float y
                    , float letter_width
                    , unsigned int max_length
                    , const GStyle* gstyle
                    , ControlGrid* grid
                    , std::function<bool(char)>&& in_f
                    , std::function<bool(const std::string&)>&& val_f)
                    : ControlField(x, y, letter_width, gstyle, grid)
                    , max_length(max_length)
                    , input_condition(std::move(in_f))
                    , validator(std::move(val_f)) {
    
    cursor.setPosition(x + gstyle->spacing_x, y + gstyle->spacing_y);
    cursor.setFillColor(style->ctext);
    cursor.setSize(sf::Vector2f(1.f, bounding_box.height - 2 * gstyle->spacing_y));

    shape.setPosition(bounding_box.left, bounding_box.top);
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
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
    bounding_box.width = 2.f * style->spacing_x;
    if (bb.width > style->default_width) {
        bounding_box.width += bb.width;
    } else {
        bounding_box.width += style->default_width;
    }
    shape.setSize(sf::Vector2f(bounding_box.width, bounding_box.height));
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