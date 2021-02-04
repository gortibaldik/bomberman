#ifndef CF_TEXT_FIELD_HPP
#define CF_TEXT_FIELD_HPP

#include "control_field.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <functional>

class TextField : public ControlField {
public:
    TextField(  float x,
                float y,
                float letter_width,
                unsigned int max_length,
                float default_width,
                GStyle* gstyle,
                ControlGrid* grid,
                std::function<bool(char)>&&,
                std::function<bool(const std::string&)>&&);
    void move_pos(float, unsigned int, unsigned int) override;
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) override;
    void render(sf::RenderTarget* target) override;
    void update() override;
    bool is_valid() override;
private:
    void handle_text_entered(sf::Uint32);
    void handle_special_keys(sf::Uint32);
    void set_cursor(unsigned int);
    std::function<bool(char)> input_condition;
    std::function<bool(const std::string)> validator;
    float default_width, default_height;
    sf::RectangleShape cursor;
    unsigned int max_length, cursor_pos = 0;
};

#endif