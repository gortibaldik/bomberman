#ifndef CONTROL_GRID_HPP
#define CONTROL_GRID_HPP
#include "control_field.hpp"
#include <vector>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

class ControlGrid {
public:
    ControlGrid() {}

    void initialize(float x,
                    float y,
                    unsigned int letter_size,
                    GStyle* button_style);

    void initialize(float x,
                    float y,
                    unsigned int letter_size,
                    unsigned int max_text_field_size,
                    GStyle* text_field_style);

    void initialize(float x,
                    float y,
                    unsigned int letter_size,
                    unsigned int max_text_field_size,
                    GStyle* button_style,
                    GStyle* text_field_style);
    void add_button(const std::string& button_title);
    void add_text_field(const std::string& name_of_field);
    void add_non_clickable(const std::string& title);
    
    void render(sf::RenderTarget* target);
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e);
    void update();
    void move_pos(float, unsigned int, unsigned int);

    const ControlField* get_pressed_btn();
    const ControlField* get_named_field(const std::string& name);
private:
    void add_entry(CFPtr&& entry);
    void add_entry(const std::string& name_of_field, CFPtr&& entry);

    float last_x, last_y;
    unsigned int letter_size, max_text_field_size;

    GStyle* button_style;
    GStyle* text_field_style;

    std::vector<CFPtr> buttons;
    std::map<std::string, ControlField*> named_fields;
    ControlField* which_pressed;
    friend ControlField;
};
#endif