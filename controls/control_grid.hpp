#ifndef CONTROL_GRID_HPP
#define CONTROL_GRID_HPP
#include "control_field.hpp"
#include <vector>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <functional>

class ControlGrid {
public:
    ControlGrid(): button_style(nullptr)
                 , text_field_style(nullptr)
                 , which_pressed(nullptr)
                 , default_width(0.f)
                 , last_x(0.f)
                 , last_y(0.f)
                 , letter_size(0) {}

    void initialize(float x,
                    float y,
                    unsigned int letter_size,
                    GStyle* button_style);

    void initialize(float x,
                    float y,
                    unsigned int letter_size,
                    float default_width,
                    GStyle* text_field_style);

    void initialize(float x,
                    float y,
                    unsigned int letter_size,
                    float default_width,
                    GStyle* button_style,
                    GStyle* text_field_style);
    void add_button(const std::string& button_title);
    void add_text_field(const std::string& name_of_field,
                        std::function<bool(char)>&& in_f,
                        std::function<bool(const std::string&)>&& val_f,
                        unsigned int max_length);
    void add_non_clickable(const std::string& title);
    void add_non_clickable(const std::string& title, const std::string& content);
    
    void render(sf::RenderTarget* target);
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e);
    void update();
    void move_pos(float, unsigned int, unsigned int);

    const ControlField* get_pressed_btn();
    ControlField* get_named_field(const std::string& name);
private:
    void add_entry(CFPtr&& entry);
    void add_entry(const std::string& name_of_field, CFPtr&& entry);

    float last_x, last_y, default_width;
    unsigned int letter_size;

    GStyle* button_style;
    GStyle* text_field_style;

    std::vector<CFPtr> buttons;
    std::map<std::string, ControlField*> named_fields;
    ControlField* which_pressed;
    friend ControlField;
};
#endif