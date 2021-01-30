#ifndef CF_TEXT_FIELD_HPP
#define CF_TEXT_FIELD_HPP

#include "control_field.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class TextField : public ControlField {
public:
    TextField(  float x,
                float y,
                float letter_width,
                unsigned int max_length,
                GStyle* gstyle,
                ControlGrid* grid);
    void move_pos(float, unsigned int, unsigned int) override;
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) override;
    void render(sf::RenderTarget* target) override;
    void update() override;
private:
    void handle_text_entered(sf::Uint32);
    void handle_special_keys(sf::Uint32);
    void set_cursor(unsigned int);
    sf::RectangleShape cursor;
    unsigned int max_length, cursor_pos = 0;
};

#endif