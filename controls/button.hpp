#ifndef CF_BUTTON_HPP
#define CF_BUTTON_HPP

#include "control_field.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Button : public ControlField {
public:
    Button( float x,
            float y,
            float letter_width,
            const std::string& text,
            const GStyle* style,
            ControlGrid* grid);
};

class NonClickableButton : public ControlField {
public:
    NonClickableButton( float x,
                        float y,
                        float letter_width,
                        const std::string& text,
                        const GStyle* style,
                        ControlGrid* grid);
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) override;
    void update() override;
};

#endif