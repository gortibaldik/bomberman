#include "control_grid.hpp"
#include "button.hpp"
#include "text_field.hpp"
#include <stdexcept>
#include <iostream>

#define SPACING_FACTOR 1.3f

void ControlGrid::initialize(const CGStyle& cgstyle, const sf::Vector2f& pos) {
    buttons.clear();
    last_x = cgstyle.left_top_x;
    last_y = cgstyle.left_top_y;
    if (cgstyle.factor != 0.f) {
        last_x = cgstyle.factor * pos.x;
        last_y = cgstyle.factor * pos.y;
    }
    letter_size = cgstyle.letter_size;
    button_style = cgstyle.button_style;
    text_field_style = cgstyle.txt_style;
    default_width = cgstyle.default_width;
}

void ControlGrid::add_entry(CFPtr&& entry_ptr) {
    // x is left
    // y is upper
    // x remains unchanged
    // y grows with button_height
    buttons.push_back(std::move(entry_ptr));
    float button_height = buttons.begin()->get()->get_height();
    last_y += button_height * SPACING_FACTOR;
}

void ControlGrid::add_entry(const std::string& name_of_field, CFPtr&& entry_ptr) {
    buttons.push_back(std::move(entry_ptr));
    float button_height = buttons.begin()->get()->get_height();
    last_y += button_height * SPACING_FACTOR;

    ControlField* cptr = buttons[buttons.size() -1].get();
    auto it = named_fields.find(name_of_field);
    if (it != named_fields.end()) {
        throw std::runtime_error("Two named fields with the same name!");
    }
    named_fields[name_of_field] = cptr;
}

ControlField* ControlGrid::get_named_field(const std::string& name_of_field) {
    auto it = named_fields.find(name_of_field);
    if (it == named_fields.end()) {
        throw std::runtime_error("No named field with this name!");
    }
    return named_fields[name_of_field];
}

void ControlGrid::add_button(const std::string& button_title) {
    if (button_style == nullptr) {
        throw std::runtime_error("Non initialized button style before adding button to the grid!");
    }
    add_entry(std::make_unique<Button>(last_x, last_y, letter_size, button_title, button_style, this));
}

void ControlGrid::add_text_field(const std::string& name_of_field,
                                std::function<bool(char)>&& in_f,
                                std::function<bool(const std::string&)>&& val_f,
                                unsigned int max_length) {
    if (text_field_style == nullptr) {
        throw std::runtime_error("Non initialized text_field style before adding text_field to the grid!");
    }
    add_entry(name_of_field, std::make_unique<TextField>(last_x, last_y, letter_size,
                                                        max_length, default_width,
                                                        text_field_style, this, std::move(in_f), std::move(val_f)));
}

void ControlGrid::add_non_clickable(const std::string& title) {
    if (button_style == nullptr) {
        throw std::runtime_error("Non initialized button style before adding non_clickable to the grid!");
    }
    add_entry(std::make_unique<NonClickableButton>(last_x, last_y, letter_size, title, button_style, this));
}

void ControlGrid::add_non_clickable(const std::string& title, const std::string& content) {
    if (button_style == nullptr) {
        throw std::runtime_error("Non initialized button style before adding non_clickable to the grid!");
    }
    add_entry(title, std::make_unique<NonClickableButton>(last_x, last_y, letter_size, content, button_style, this));
}
    
void ControlGrid::render(sf::RenderTarget* target) {
    for (auto&& button : buttons) {
        button->render(target);
    }
}

void ControlGrid::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {
    for (auto&& button : buttons) {
        button->handle_input(mouse_position, e);
    }
}

void ControlGrid::update() {
    for (auto&& button : buttons) {
        button->update();
    }
    which_pressed = nullptr;
}

const ControlField* ControlGrid::get_pressed_btn() {
    return which_pressed;
}

void ControlGrid::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    float button_height = buttons.begin()->get()->get_height();
    for (auto&& button : buttons) {
        button->move_pos(factor, new_x, new_y);
        new_y += static_cast<int>(button_height * SPACING_FACTOR);
    }
}