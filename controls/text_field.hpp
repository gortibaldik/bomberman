#ifndef CF_TEXT_FIELD_HPP
#define CF_TEXT_FIELD_HPP

#include "control_field.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class TextField {
public:
    TextField(  float x,
                float y,
                float letter_width,
                float default_width,
                unsigned int max_length,
                GStyle* gstyle,
                ControlGrid* grid);
    void render(sf::RenderTarget* target);
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e);
    void update();
    void move_pos(float, unsigned int, unsigned int);

    float get_height() const;
    std::string get_text() const;
private:
    sf::RectangleShape shape;
    GStyle* style;
    ControlGrid* grid;
    CONTROL_STATE state;
    float letter_width;
    sf::Text text;
};

#endif