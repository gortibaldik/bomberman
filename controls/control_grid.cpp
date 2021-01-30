#include "control_grid.hpp"
#include "button.hpp"
#include "text_field.hpp"
#include <stdexcept>

#define SPACING_FACTOR 1.3f

void ControlGrid::initialize(float x,
                    float y,
                    unsigned int letter_size,
                    unsigned int max_text_field_size,
                    GStyle* button_style,
                    GStyle* text_field_style) {
    buttons.clear();
    last_x = x;
    last_y = y;
    this->letter_size = letter_size;
    this->max_text_field_size = max_text_field_size;
    this->button_style = button_style;
    this->text_field_style = text_field_style;
}

void ControlGrid::initialize(float x,
                    float y,
                    unsigned int letter_size,
                    unsigned int max_text_field_size,
                    GStyle* text_field_style) {
    if (max_text_field_size == 0) {
        throw std::runtime_error("Cannot initialize text_field with size less than or equal to zero!");
    }
    initialize(x, y, letter_size, max_text_field_size, nullptr, text_field_style);
}

void ControlGrid::initialize(float x,
                float y,
                unsigned int letter_size,
                GStyle* button_style) {
    initialize(x, y, letter_size, 0, button_style, nullptr);
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

const ControlField* ControlGrid::get_named_field(const std::string& name_of_field) {
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

void ControlGrid::add_text_field(const std::string& name_of_field) {
    if (text_field_style == nullptr) {
        throw std::runtime_error("Non initialized text_field style before adding text_field to the grid!");
    }
    add_entry(name_of_field, std::make_unique<TextField>(last_x, last_y, letter_size, max_text_field_size,text_field_style, this));
}

void ControlGrid::add_non_clickable(const std::string& title) {
    if (button_style == nullptr) {
        throw std::runtime_error("Non initialized button style before adding non_clickable to the grid!");
    }
    add_entry(std::make_unique<NonClickableButton>(last_x, last_y, letter_size, title, button_style, this));
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
        new_y += button_height * SPACING_FACTOR;
    }
}